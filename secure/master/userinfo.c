// MorgenGrauen MUDlib
//
// master/userinfo.c -- Cache mit Spielerinfos
//
// $Id: userinfo.c 9467 2016-02-19 19:48:24Z Zesstra $

#pragma strict_types

#include "/secure/master.h"
#include "/sys/files.h"
#include "/sys/object_info.h"

// Makro aus wizlevels.h ersetzen, da secure_level ne sefun ist. *seufz*
#undef ARCH_SECURITY
#define ARCH_SECURITY (call_sefun("secure_level") >= ARCH_LVL)

private string*  ExpandUIDAlias(string alias, int rec);

nosave mapping userlist;
string name, password;
string ektips;
string fptips;
int level;
string shell;
int creation_date;
string ep, ek, mq;
string *domains,*guilds, *uidstotakecare;
nosave string* lateplayers;
// hier wird ein Mapping von UID-zu-Magier-Zuordnungen gespeichert. Als Keys
// werden UIDs verwendet, der Value ist jeweils ein Array mit den magiern, die
// dafuer zustaendig sind.
nosave private mapping userids = ([]);
// Ein Cache fuer UID-Aliase, damit ExpandUIDAlias() nicht staendig ganze
// Verzeichnisse einlesen muss, sondern der Master das nur im Reset machen
// muss.
nosave private mapping uidaliase = ([]);

#ifdef _PUREFTPD_
// Liste von FTP-berechtigten Usern
nosave private mapping ftpuser = ([]);

public void read_ftp_users() {
  string str = read_file("/"SECUREDIR"/ARCH/pureftpd.passwd") || "";
  string *data = explode(str, "\n");
  if (!sizeof(data)) return;
  ftpuser = m_allocate(sizeof(data), 1);
  foreach(str : data) {
    string *tmp=explode(str, ":");
    if(sizeof(tmp) >= 2)
      m_add(ftpuser, tmp[0], tmp[1]);
  }
}

public int write_ftp_users() {
  string *record = allocate(18,"");
  mapping tmp = m_allocate(sizeof(ftpuser));
  // set UID/GID of system user
  record[2] = "1000";
  record[3] = "1000";
  foreach(string u, string pwhash : ftpuser) {
    record[0] = u;  // UID
    record[1] = pwhash;
    record[5] = "/home/mud/mudlib/"WIZARDDIR"/" + u + "/./";
    m_add(tmp, implode(record, ":"));
  }
  write_file("/"SECUREDIR"/ARCH/pureftpd.passwd", implode(m_indices(tmp),"\n"),1);
  return sizeof(tmp);
}

// geloeschte Magier oder Magier, die 2 Wochen nicht da waren, expiren
public void expire_ftp_user() {
  foreach(string u : ftpuser) {
    mixed uinfo = get_full_userinfo(u);
    if (!uinfo) {
      m_delete(ftpuser,u);
      continue;
    }
    int zeit = call_sefun("file_time",secure_savefile(u)+".o");
    if (zeit < time()-1209600)
    {
      m_delete(ftpuser,u);
      continue;
    }
  }
  call_out(#'write_ftp_users, 2);
}

#endif // _PUREFTPD_

// ********** oeffentliche Funktionen ***********************

//Verantwortliche Magier fuer eine UID ausgeben
public varargs string* QueryWizardsForUID(string uid, int recursive) {

    if (!stringp(uid) || !sizeof(uid))
        return(({}));

    string *tolookup=({uid}); //diese spaeter in userids nachgucken.
    string *wizards=({});
    // Schauen, was automatisch ermittelt werden kann.
    string *parts=explode(uid,".");
    switch(sizeof(parts)) {
         case 3:
          // z.B. d.region.magier und p.service.magier
          if (find_userinfo(parts[2]))
              //Magier existiert, reinschreiben.
              wizards+=({parts[2]});
          if (parts[0]=="d")
              // d.region noch nachgucken (RMs)
              tolookup=({implode(parts[0..1],".")});
          break;
        //case 2:
          // GUILD.gilde, p.project, d.region
          // koennen nur in userids nachgeguckt werden (s.u. tolookup)
          // muessen da als GUILD.*, d.* und p.* drinstehen!
        case 1:
          // kein Punkt drin. Entweder Magier-ID oder spezielle ID
          // (letztere wird unten noch per tolookup nachgeguckt)
          if (find_userinfo(parts[0]))
              wizards+=({parts[0]});
          break;
    }
    // jetzt in userids nachschlagen
    foreach(uid: tolookup) {
        if (member(userids,uid))
            wizards+=userids[uid];
    }
    // so. Nun kann es aber noch sein, dass in userids sowas wie
    // "d.wald.atamur":({atamur}) und "atamur":({"rumata"}) drinsteht, also
    // ein Magier sozusagen fuer Kram eines anderen verantwortlich ist. D.h.
    // nochmal durch QueryWizardsForUID() schicken (das ist dann allerdings nicht
    // weiter rekursiv).
    if (!recursive) {
      foreach(uid: wizards) {
        //es ist moeglich, in der Schleife wizards zu vergroessern, ohne dass
        //es Einfluss auf die Schleife hat.        
        wizards += QueryWizardsForUID(uid, 1) - ({uid});
      }
    }
    return(m_indices(mkmapping(wizards)));
}

//das Define ist nicht sonderlich elegant, aber ich kann hier nicht das
//newskills.h hier includen (1. zu viel, 2. ists nen Sicherheitsproblem)
#define P_GUILD_DEFAULT_SPELLBOOK       "guild_sb"
// dito fuer den Gildenmaster
#define GUILDMASTER                 "/secure/gildenmaster"

// Den Alias-Cache loeschen (normalerweise einmal am Tag)
public void ResetUIDAliase() {
  // RM+ duerfen den Cache loeschen (wenn sie z.B. nen neues Verzeichnis
  // angelegt haben.)
  if (extern_call() 
      && call_sefun("secure_level") < LORD_LVL)
      return;

  uidaliase=([]);
}

// expandiert einige 'Aliase' in ein Array von allen UIDs, fuer die sie
// stehen. Bsp: "region" -> d.region.* + region + d.region,
// "zauberer" -> GUILD.zauberer, "p.service" -> p.service.*
// Nutzt Eintrag aus dem uidalias-Cache, sofern vorhanden. 
public varargs string* QueryUIDAlias(string alias, int rec) {
  string *uids;
  if (!stringp(alias) || !sizeof(alias))
      return ({});
  // Wen im cache, gehts schnell.
  if (member(uidaliase, alias))
      uids = uidaliase[alias];
  else
      uids = ExpandUIDAlias(alias, rec);

  if (extern_call())
    return copy(uids);
  
  return(uids);
}

// Fuer welche UIDs ist ein Magier verantwortlich? (Ist er RM,
// Gildenmagier, in welchen Regionen hat er ein Verzeichnis, etc.)
// recursive != 0 bedeutet, dass der Aufruf indirekt aus einem laufenden
// QueryUIDsForWizard() erfolgt. In dem Fall soll QueryUIDAlias() keine
// weitere Rekursion durchfuehren. Wird einfach durchgereicht.
public varargs string* QueryUIDsForWizard(string wizuid, int recursive) {
    string *uids, *tmp, *uidstoadd;
    int i;

    if (!stringp(wizuid) || !sizeof(wizuid) || !IS_LEARNER(wizuid))
        return(({}));

    if (!find_userinfo(wizuid))
        return(({}));

    uidstoadd=({}); //diese werden hinterher in userids gespeichert.

    // als erstes die ratebaren UIDs. ;-)
    uids=({wizuid});
    // Regionen ermitteln, wo wizuid ein Verzeichnis hat und entsprechende
    // UIDs anhaengen:
    foreach(string region: get_domain_homes(wizuid)) {
        uids+=({ sprintf(DOMAINDIR".%s.%s",region,wizuid) });
    }
    // Verzeichnis in /p/service?
    if (file_size(PROJECTDIR"/service/"+wizuid) == FSIZE_DIR)
        uids+=({PROJECTDIR".service."+wizuid});

    // Gildenchef?
    if (pointerp(userlist[wizuid,USER_GUILD])) {
        foreach(string gilde: userlist[wizuid,USER_GUILD]) {
          uidstoadd += QueryUIDAlias(gilde);
        }
    }
    // Regionsmagier?
    if (pointerp(userlist[wizuid,USER_DOMAIN])) {
        foreach(string domain: userlist[wizuid,USER_DOMAIN]) {
            //generelle Pseudo-UID 'd.region' und 'region' fuer geloeschte
            //Magier dieser Region vormerken, um sie hinterher fuers
            //Reverse-Lookup ins uid-Mapping zu schreiben.
            string *pseudo=({DOMAINDIR"."+domain, domain});
            uidstoadd += pseudo;
            // Rest macht QueryUIDAlias, dabei aber die von der Funktion
            // ebenfalls gelieferten Pseudo-UIDs wieder abziehen.
            uids += QueryUIDAlias(domain) - pseudo;
        }
    }
    // jetzt noch nachgucken, fuer welche UIDs dieser Magier explizit noch
    // zustaendig ist.
    if (pointerp(userlist[wizuid,USER_UIDS_TO_TAKE_CARE])) {
    // dies koennte etwas a la "region" oder "anderermagier" sein, d.h. dieser
    // Magier ist fuer alle UIDs von 'andermagier' auch zustaendig. Daher muss
    // jedes davon durch QueryUIDAlias() (was im Falle von Spielern wiederum
    // QueryUIDsForWizard() ruft, aber die Rekursion im Falle von Spielern ist
    // auf 1 begrenzt).
        foreach(string uid2: userlist[wizuid,USER_UIDS_TO_TAKE_CARE]) {        
            uidstoadd += QueryUIDAlias(uid2, recursive);
        }
    }

    // so, in uidstoadd stehen UIDs drin, die nicht Magiername selber,
    // d.region.magier oder p.service.magier sind und bei welchen folglich das
    // Mapping UIDs-nach-Magier nur mit einer Liste moeglich ist. In die
    // werden die uids nun eingetragen. (z.B. d.region)
    if (sizeof(uidstoadd)) {
        // genug Platz in userids? Sonst welche rauswerfen. :-/ 
        // (besser als bug) TODO: Auf 10k begrenzen -> max Arraygroesse!
        if ( sizeof(userids)+(i=sizeof(uidstoadd))               
            >= __MAX_MAPPING_KEYS__) {
            foreach(string tmpuid: m_indices(userids)[0..i])        
              m_delete(userids,tmpuid);  
        }
        foreach(string tmpuid: uidstoadd) {  
            if (member(userids,tmpuid)) {  
                //User dem Array hinzufuegen, wenn noch nicht drin.
                if (member(userids[tmpuid],wizuid)==-1)
                    userids[tmpuid]=userids[tmpuid]+({wizuid});              
            }
            //sonst neuen Eintragen hinzufuegen            
            else
                m_add(userids,tmpuid,({wizuid}));          
        }
    } // Ende spez. uids speichern
    
    return(uids+uidstoadd);
}

//Einen Magier als verantwortlich fuer eine bestimmte UID eintragen
public string* AddWizardForUID(string uid, string wizuid) {
    if (!stringp(wizuid) || !sizeof(wizuid)
        || !IS_LEARNER(wizuid))
        return(({}));

    //Zugriff nur als EM oder jemand, der fuer die UID zustaendig ist.
    if ( !ARCH_SECURITY
        && member(
            QueryUIDsForWizard(call_sefun("secure_euid")),
            uid) == -1)
        return(userlist[wizuid,USER_UIDS_TO_TAKE_CARE]);

    if (!pointerp(userlist[wizuid,USER_UIDS_TO_TAKE_CARE]))
        //Array neu anlegen
        userlist[wizuid,USER_UIDS_TO_TAKE_CARE]=({uid});
    else {
        //Ein Array schon vorhanden
        if (member(userlist[wizuid,USER_UIDS_TO_TAKE_CARE],uid)==-1)
            //uid nicht drin
            userlist[wizuid,USER_UIDS_TO_TAKE_CARE]=
              userlist[wizuid,USER_UIDS_TO_TAKE_CARE]+({uid});
    }
    save_userinfo(wizuid);
    // aus dem UID-Alias-Cache werfen
    m_delete(uidaliase, wizuid);
    // Aufruf, um userids und uidaliase zu aktualisieren
    QueryUIDsForWizard(wizuid);
    return(userlist[wizuid,USER_UIDS_TO_TAKE_CARE]);
}

// Einen Magier wieder austragen, wenn er nicht mehr zustaendig ist.
public string* RemoveWizardFromUID(string uid, string wizuid) {
    if (!stringp(wizuid) || !sizeof(wizuid)
        || !find_userinfo(wizuid))
        return(({}));

    //Zugriff nur als EM oder jemand, der fuer die UID zustaendig ist.
    if ( !ARCH_SECURITY
        && member(
            QueryUIDsForWizard(call_sefun("secure_euid")),
            uid)==-1)
        return copy(userlist[wizuid,USER_UIDS_TO_TAKE_CARE]);

    // jetzt muss diese wizuid aus allen UIDs in userids geloescht werden, die
    // sie bisher enthalten. Hierzu sollte QueryUIDAlias die potentiell
    // drinstehenden UIDs liefern.
    foreach(string tuid: QueryUIDAlias(wizuid,0)) {
        if (member(userids, tuid) &&
            member(userids[tuid],wizuid)!=-1 )
            userids[tuid] -= ({wizuid});
    }
    // wenn es eine UID war, fuer die der Magier explizit zustaendig war,
    // entsprechend loeschen. Sonst ist hier Ende.
    if (!pointerp(userlist[wizuid,USER_UIDS_TO_TAKE_CARE]))
        return ({});
    if (member(userlist[wizuid,USER_UIDS_TO_TAKE_CARE],uid)==-1)
        return copy(userlist[wizuid,USER_UIDS_TO_TAKE_CARE]);

    // Jetzt aus userlist loeschen.
    userlist[wizuid,USER_UIDS_TO_TAKE_CARE] -= ({uid});
    save_userinfo(wizuid);
    // und userids/uidaliase aktualisieren.
    QueryUIDsForWizard(wizuid);
    return copy(userlist[wizuid,USER_UIDS_TO_TAKE_CARE]);
}

//entfernt einen user aus dem usercache
void RemoveFromCache(string user) {
  m_delete(userlist,user);
}

//loescht den gesamten Usercache
int clear_cache() {
  userlist=m_allocate(0,widthof(userlist));
  update_late_players();
  return 1;
}

// Loescht UID->Magier Lookuptabelle. Sollte nicht gerufen werden, wenn man 
// nicht genau weiss, was man damit kaputtmacht.
// als Nebeneffekt wird clear_cache() gerufen.
int ResetUIDCache() {
  // da diese Funktion auch das UID<->Magier-Lookup loescht, darf das nicht
  // jeder rufen.
  if (extern_call() &&
      call_sefun("secure_level") < ELDER_LVL)
      return -1;
  userids=([]);
  clear_cache();
  return 1;
}

//verstorbene Spieler auflisten
string list_late_players()
{
  string ret;
  int size,i;
  
  ret= "************************************\n";
  ret+="*       Verstorbene Spieler        *\n";
  ret+="************************************\n";
  
  if(!lateplayers || (size=sizeof(lateplayers))==0)
  {
    return ret;
  }
  
  for(i=0;i<size;i++)
  {
    ret+=lateplayers[i]+"\n";
  }
  
  return ret;
}

// ist der Spieler gestorben?
int check_late_player(string str)
{
  if(!lateplayers || !str || str=="")
  {
    return 0;
  }
  
  if(member(lateplayers,str)!=-1)
  {
    return 1;
  }
  
  return 0;
}

// alle Eintraege im usercache ausgeben
public mixed *show_cache() {
    return m_indices(userlist);
}

// gibt es so einen User? Anfrage ausserdem immer in den Cache eintragen
public int find_userinfo(string user) {
  string file;
  int i;
  if (!stringp(user) || !sizeof(user) 
      || member(user,' ')!=-1 || member(user,':')!=-1)
    return 0;
  if (!member(userlist,user)) {
      //erstmal schauen, ob wir mit einem neuen Eintrag nicht die max.
      //Mapping-Groessen ueberschreiten, wenn ja, einen Eintrag loeschen
      //BTW: widthof()+1 ist richtig so.
      //BTW2: Ich hoffe, die max. Arraygroesse ist immer gross genug, sollte 
      //sie aber sein bei ner Mappingbreite von 10.
      // BTW3: Dieses Rausloeschen von einem Eintrag bei Erreichen der Grenze
      // erhoeht die benoetigten Ticks fuer diese Funktion um das 5-6fache und
      // die noetige Zeit um das 70fache. Daher wird der Cache bei Erreichen
      // der Grenze jetzt einfach geloescht.
      if ( ( (i=sizeof(userlist)+1) >= __MAX_MAPPING_KEYS__) 
          || (( i * (widthof(userlist)+1)) >
                 __MAX_MAPPING_SIZE__))    
          //m_delete(userlist,m_indices(userlist)[0]);
          userlist=m_allocate(1,widthof(userlist));

      // Usersavefile finden
      if ((file=secure_savefile(user))=="") {
        // User gibt es nicht, aber Anfrage cachen
          userlist+=([user: "NP"; -1; ({}); "LOCKED"; -1; time(); ""; ""; "";
          ({});"";""; 0 ]);
          return 0;
      }
      password="";
      ep="";
      ek="";
      mq="";
      guilds=({});
      ektips="";
      fptips="";
      uidstotakecare=0;
      if (!restore_object(file)) return 0;
      userlist+=([user: password; level; domains; shell; creation_date; 
          time();ep; ek; mq; guilds; ektips; fptips; uidstotakecare]);
      // die speziellen UIDs, fuer die dieser Magier zustaendig ist, ermitten
      // und gleichzeitig im entsprechenden Mapping fuers Reverse-Loopup
      // speichern.
      if (level >= LEARNER_LVL)
        QueryUIDsForWizard(user);
  }
  userlist[user,USER_TOUCH]=time();
  if (userlist[user,USER_LEVEL]==-1) return 0;
  return 1;
}

// Daten aus der Userlist fuer diesen User liefern. Macht ggf. find_userinfo()
public mixed *get_userinfo(string user) {
  if(!user||user=="")
    return 0;
  user=explode(user,".")[0];
  if (!member(userlist,user) && !find_userinfo(user))
    return 0;

  if (userlist[user,USER_LEVEL]==-1) return 0;

  return({user,"##"+user,userlist[user,USER_LEVEL],
      userlist[user,USER_DOMAIN], userlist[user,USER_OBJECT],
      userlist[user,USER_CREATION_DATE], 0, 0,
      userlist[user,USER_GUILD],
      userlist[user,USER_EKTIPS],userlist[user,USER_FPTIPS],
      userlist[user,USER_UIDS_TO_TAKE_CARE]});
}

// liefert das Objekt des users aus der Userlist zurueck.
public string query_player_object( string name )
{
  mixed *userentry;
  if( !find_userinfo(name) ) return "";
  return userlist[name,USER_OBJECT];
}

// Passwort ok?
public int good_password( string str, string name )
{
    string rts;
    int i, n;

    if ( str[0] == '!' ){
        tell_object( this_player() || this_object(), "Ein Ausrufungszeichen "
                     "('!') als erster Buchstabe des Passwortes wuerde zu\n"
                     "Problemen fuehren. Such Dir bitte ein anderes Passwort "
                     "aus.\n" );
        return 0;
    }

    if ( sizeof(str) < 6 ){
        tell_object( this_player() || this_object(),
                     "Das Passwort muss wenigstens 6 Zeichen lang sein.\n" );
        return 0;
    }

    str = lower_case(str);
    rts = "";

    // Zahlen/Sonderzeichen am Anfang oder Ende des Passwortes
    // (z.B. "merlin99") sind nicht wirklich einfallsreich.
    while ( sizeof(str) && (str[0] > 'z' || str[0] < 'a') ){
        rts += str[0..0];
        str = str[1..];
    }

    while ( sizeof(str) && (str[<1] > 'z' || str[<1] < 'a') ){
        rts += str[<1..];
        str = str[0..<2];
    }

    // Anzahl unterschiedlicher Zeichen merken, die herausgeschnitten wurden
    n = sizeof( mkmapping(efun::explode( rts, "" )) );
    rts = "";

    for ( i = sizeof(str); i--; )
        rts += str[i..i];

    // Eigener Name als Passwort bzw. eigener Name rueckwaerts
    if ( str == lower_case(name) || rts == lower_case(name) ||
         // Name eines anderen Mudders (Erstie?)
         secure_savefile(str) != "" || secure_savefile(rts) != "" ||
         // Name von der Banish-Liste
         QueryBanished(str) || QueryBanished(rts) ||
         // Name eines vorhandenen NPC o.ae.
         call_sefun("find_living", str ) ||
         call_sefun("find_living", rts ) ||
         // Zuwenig verschiedene Zeichen
         sizeof( mkmapping(efun::explode( str, "" )) ) + n < 4 ){
        tell_object( this_player() || this_object(),
                     "Das Passwort waere zu einfach zu erraten. Nimm bitte "
                     "ein anderes.\n" );
        return 0;
    }

    return 1;
}

// User-ID fuer ein File ermitteln.
public string get_wiz_name(string file) {
    return creator_file(file);
}

// Wizlevel aus dem Userinfo-Cache holen
public int get_wiz_level(string user) {
  if (user && find_userinfo(user)) 
    return userlist[user,USER_LEVEL];
  //return 0 if no user given (return type needed)
  return(0);
}

// Wizlevel fuer eine UID ermitteln.
public int query_wiz_level( mixed player )
{
    if ( objectp(player) && efun::object_info(player, OI_ONCE_INTERACTIVE) )
        return get_wiz_level( getuid(player) );
    else {
        // erstmal UID ermitteln, falls Objekt
        //if (objectp(player))
        //    player=getuid(player);
        if ( stringp(player) ) {
            if( player[0..1]==DOMAINDIR"." ) return 25;
            if( player[0..5]==GUILDID"." ) 
                return WIZLVLS[GUILDID];
            if( player[0..1]==PROJECTDIR"." ) return 21;
            // die alte Loesung mit || verhaelt sich falsch, wenn ein UID ne
            // spezielle ist, der Level 0 zugeordnet wurde und es einen
            // Spieler mit diesem namen gibt.
            if (member(WIZLVLS,player)) 
                return(WIZLVLS[player]);
            return get_wiz_level(player);
        }
    }
    return 0;
}

// Savefile aus /secure/save* zurueckliefern
public string secure_savefile(string name)
{
  if(!name||name=="")
    return "";
  name=explode(name,".")[0];
  if (file_size(SECURESAVEPATH+name[0..0]+"/"+name+".o")>=0)
    return SECURESAVEPATH+name[0..0]+"/"+name;
  else if (file_size("/"SECUREDIR"/save/"+name[0..0]+"/"+name+".o")>=0)
    return "/"SECUREDIR"/save/"+name[0..0]+"/"+name;

  return "";
}

// *************** 'halb-oeffentliche Funktionen *********************
#ifdef _PUREFTPD_
// FTP-Berechtigung fuer Magier
int update_ftp_access(string user, int state) {
  // wenn nicht EM+ oder ROOT, darf nur fuer this_interactive geaendert
  // werden.
  if (getuid(PO) != ROOTID && extern_call()
      && !ARCH_SECURITY) {
    if (this_interactive())
      user = getuid(this_interactive());
    else
      user = 0;
  }
  if (!user || !sizeof(user))
    return -1;

  if (!find_userinfo(user))
    return 0;

  // Passwort muss manuell vom Magier neu gesetzt werden, da es keine
  // Moeglichkeit gibt, an das aktuelle im Klartext heranzukommen.
  if (state) {
    if (!member(ftpuser,user))
      m_add(ftpuser, user, "*");
  }
  else
    m_delete(ftpuser, user);
  
  call_out(#'write_ftp_users, 4);
  return state;
}
#endif

// Spieler ein neues Wizlevel verpassen.
int update_wiz_level(string user,int lev) {
  object ob;

  if (getuid(PO) != ROOTID && extern_call()) return 0;
  if (!find_userinfo(user)) return 0;
  userlist[user,USER_LEVEL] = lev;
  save_userinfo(user);
  return 1;
}

// neue Forscherpunkte fuer den User abspeichern.
int update_ep(string user,string ep_neu) {
  if (getuid(PO) != ROOTID) return 0;
  if (!find_userinfo(user)) return 0;
  userlist[user,USER_EP] = ep_neu;
  save_userinfo(user);
  return 1;
}

// neue Erstkills fuer den User abspeichern.
int update_ek(string user,string ek_neu) {
  if (getuid(PO) != ROOTID) return 0;
  if (!find_userinfo(user)) return 0;
  userlist[user,USER_EK] = ek_neu;
  save_userinfo(user);
  return 1;
}

// Miniquestdaten speichern.
int update_mq(string user,string mq_neu) {
  if (getuid(PO) != ROOTID) return 0;
  if (!find_userinfo(user)) return 0;
  userlist[user,USER_MQ] = mq_neu;
  save_userinfo(user);
  return 1;
}

// Erstkillpunkt-Tips speichern.
int update_ektips(string user,string ek_neu) {
  if (getuid(PO) != ROOTID) return 0;
  if (!find_userinfo(user)) return 0;
  userlist[user,USER_EKTIPS] = ek_neu;
  save_userinfo(user);
  return 1;
}

// Forscherpunkttips abspeichern.
int update_fptips(string user,string fp_neu) {
  if (getuid(PO) != ROOTID) return 0;
  if (!find_userinfo(user)) return 0;
  userlist[user,USER_FPTIPS] = fp_neu;
  save_userinfo(user);
  return 1;
}

// forscherpunkte abfragen.
string query_ep(string user) {
  if (getuid(PO) != ROOTID) return 0;
  if (!find_userinfo(user)) return 0;
  return userlist[user,USER_EP];
}

// Erstkills abfragen
string query_ek(string user) {
  if (getuid(PO) != ROOTID) return 0;
  if (!find_userinfo(user)) return 0;
  return userlist[user,USER_EK];
}

// Miniquests abfragen.
string query_mq(string user) {
  if (getuid(PO) != ROOTID) return 0;
  if (!find_userinfo(user)) return 0;
  return userlist[user,USER_MQ];
}

// EK-Tips abfragen.
string query_ektips(string user) {
  if (getuid(PO) != ROOTID) return 0;
  if (!find_userinfo(user)) return 0;
  return userlist[user,USER_EKTIPS];
}

// FP-Tips abfragen.
string query_fptips(string user) {
  if (getuid(PO) != ROOTID) return 0;
  if (!find_userinfo(user)) return 0;
  return userlist[user,USER_FPTIPS];
}

// Aendert die Shells eines Users.
int set_player_object( string user, string objectname )
{
    mixed *path;
    string prev;

    // nur EM und ROOT duerfen die Shell eines Charakters aendern
    if ( !ARCH_SECURITY &&
         (!previous_object() || getuid(previous_object()) != ROOTID) ) {
        return -1;
    }

    if ( objectname == "" )
        objectname = 0;

    if ( !stringp(user) || user == "" )
        return -6;

    if ( !stringp(objectname) ){
        if ( !find_userinfo(user) )
            return -4;

        userlist[user, USER_OBJECT] = 0;
        save_userinfo(user);
        return 1;
    }

    if ( catch(load_object(objectname);publish) ) {
        write( "Fehler in " + objectname + "!\n" );
        return -2;
    }

    objectname = _get_path( objectname, 0 );
    path = (efun::explode( objectname, "/" ) - ({ "", 0 }));

    if ( sizeof(path) < 3 || path[0] != "std" || path[1] != "shells" )
        return -3;

    if ( !find_userinfo(user) )
        return -4;

    prev = userlist[user, USER_OBJECT];
    userlist[user, USER_OBJECT] = objectname;
    save_userinfo(user);

    // Loggen, falls die Aenderung nicht von Login beim Anlegen des Chars
    // erfolgt.
    if (load_name(this_interactive()) != "/secure/login"
        || prev != "") {
      if (prev == "") prev ="<keine>";
      call_sefun("log_file", "ARCH/SHELL_AENDERUNGEN",
        sprintf( "%s: %O aendert die Shell von %s von %s auf %s (PO: %O)\n",
          strftime("%Y%m%d-%H%M%S"), 
          this_interactive(), capitalize(user), prev, objectname,
          previous_object()) );
    }

    return 1;
}

// Passwort aktualisieren.
int update_password( string old, string new )
{
    string user;

    // nanu, kein user?
    if ( !find_userinfo(user = getuid(PO)) )
        return 0;

    // wenn das neue PW unterschiedlich ist, schauen, ob das neue PW ok ist.
    if ( old != new && !good_password( new, user ) )
        return 0;

    string pwhash = userlist[user, USER_PASSWORD];
    string oldpwhash;
    if (sizeof(pwhash) > 13) {
        // MD5-Hash
        oldpwhash = md5_crypt(old, pwhash);
    }
    else if (sizeof(pwhash) > 2) {
        // Crypt-Hash
        oldpwhash = crypt(old, pwhash[0..1]);
    }

    // wenn es einen PW-hash gibt, also ein PW gesetzt wird, muss der Hash von
    // old mit dem pwhash uebereinstimmen. Leerer Hash oder gar kein Hash
    // erlaubt jedes beliebige PW.
    if ( stringp(pwhash) && sizeof(pwhash) && pwhash != oldpwhash)
        return 0;
    // an dieser Stelle stimmt 'old' wohl mit dem gesetzten Passwort ueberein.
    // Wenn allerdings die Funktion mit old==new aufgerufen wurde, wird hier
    // nur 1 zurueckgeben und sonst an sich nix gemacht. Kann nicht weiter
    // oben schon gemacht werden, die Shells dies als PW-Pruefung nutzen.
    // *seufz*
    if (old == new) return 1;

    // dann mal neu setzen
    userlist[ user, USER_PASSWORD ] = md5_crypt( new, 0 );
    save_userinfo(user);
#ifdef _PUREFTPD_
    // Bedauerlicherweise versteht pureftpd unser md5_crypt nicht. :-(
    if (member(ftpuser,user)) {
      ftpuser[user] = crypt(new);
      if (find_call_out(#'write_ftp_users) == -1)
        call_out(#'write_ftp_users,4);
    }
#endif
    return 1;
}

// Spieler loeschen.
int delete_player(string passwd, string real_name)
{
  int wlevel;
  string part_filename;

  if (!PO || PO!=TP || PO!=TI || real_name != getuid(PO) ||
      !find_userinfo(real_name))
    return 0;
  mixed erstie=(mixed)this_interactive()->QueryProp(P_SECOND);
  password = userlist[real_name,USER_PASSWORD];
  wlevel = get_wiz_level(real_name);
  if (!update_password(passwd, passwd)) return 0;

  // Spielpausen aufheben (sonst kann man als Spieler nen Namen sperren).
  TBanishName(real_name, 0);

  part_filename="/"+real_name[0..0]+"/"+real_name+".o";
  rm("/"SECUREDIR"/save"+part_filename);
  rm("/"LIBSAVEDIR"/"+part_filename);
  rm("/"MAILDIR"/"+part_filename);
  
  m_delete(userlist,real_name);
  
  if (wlevel >= LEARNER_LVL)
    TO->BanishName(real_name, "So hiess mal ein Magier hier");
  else if (wlevel >= SEER_LVL)
    TO->BanishName(real_name, "So hiess mal ein Seher hier");

#ifdef _PUREFTPD_
    if (member(ftpuser,real_name)) {
      m_delete(ftpuser,real_name);
      call_out(#'write_ftp_users,4);
    }
#endif

  call_sefun("log_file", "USERDELETE",
           sprintf("%s: %s %s(%s)\n",
                   ctime(time()),real_name,
                   (stringp(erstie)?sprintf("[Erstie: %s] ",erstie):""),
                   call_sefun("query_ip_number",TI)));

  return 1;
}

// ermittelt alle existierenden Spieler, die ein Savefile in /secure/save
// haben.
// ([ "a": ({...namen...}), "b": ({...namen...}), ... ]) 
public mapping get_all_players() {
  string *dirs=({"a","b","c","d","e","f","g","h","i","j","k","l","m","n","o",
      "p","q","r","s","t","u","v","w","x","y","z"});
  mapping allplayer=([]);
  string *tmp;
  foreach(string dir: dirs) {
    tmp=get_dir("/secure/save/"+dir+"/*")
      - ({".","..",".KEEP",".svn"});
    allplayer[dir] = map(tmp,function string (string fn) 
                     { return explode(fn,".")[0]; } );
  }
  return allplayer;
}

// *************** interne Funktionen ********************************
protected void create()
{
  userlist=m_allocate(0,12);
  update_late_players();
#ifdef _PUREFTPD_
  read_ftp_users();
#endif
}

//verstorbene Spieler einlesen
void update_late_players() {
  string read;
  string *tmp;
  
  lateplayers=0;
  
  read=read_file("/"SECUREDIR"/LATE_PLAYERS");
  if(!read || read=="")
  {
    return;
  }
  
  tmp=explode(read,"\n");
  if(!sizeof(tmp))
  {
    return;
  }         
  
  lateplayers=tmp; 
}


// Daten aus der Userlist inkl. Passworthash zurueckliefern. Macht ggf.
// find_userinfo(). Nur masterintern aufrufbar.
protected mixed *get_full_userinfo(string user) {
  if(!user||user=="")
    return 0;
  user=explode(user,".")[0];
  if (!member(userlist,user) && !find_userinfo(user))
    return 0;

  return({user,userlist[user,USER_PASSWORD],userlist[user,USER_LEVEL],
      userlist[user,USER_DOMAIN], userlist[user,USER_OBJECT],
      userlist[user,USER_CREATION_DATE], 0, 0, userlist[user,USER_GUILD],
      userlist[user,USER_EKTIPS],userlist[user,USER_FPTIPS],
      userlist[user,USER_UIDS_TO_TAKE_CARE]});
}

// Userdaten aus der Userlist im Savefile in /secure/save/ speichern.
protected void save_userinfo(string user) {
  if(!user||user=="")
    return;
  user=explode(user,".")[0];
  if (!member(userlist,user)) return;
  name = user;
  level = userlist[name,USER_LEVEL];
  domains = userlist[name,USER_DOMAIN];
  shell = userlist[name,USER_OBJECT];
  password = userlist[name,USER_PASSWORD];
  creation_date = userlist[name,USER_CREATION_DATE];
  if (!creation_date) creation_date = -1;
  ep = userlist[name,USER_EP];
  if (!ep) ep="";
  ek = userlist[name,USER_EK];
  if (!ek) ek="";
  mq = userlist[name,USER_MQ];
  if (!mq) mq="";
  guilds = userlist[name,USER_GUILD];
  ektips=userlist[name,USER_EKTIPS];
  if(!ektips) ektips="";
  fptips=userlist[name,USER_FPTIPS];
  if(!fptips) fptips="";
  uidstotakecare=userlist[name,USER_UIDS_TO_TAKE_CARE];

  string ssavef=secure_savefile(name);
  if (save_object(ssavef) != 0) {
    // autsch. Buggen damit dieser moeglichst schnell auffaellt, dass hier
    // Savefiles in /secure/save/ nicht geschrieben wurden.
    raise_error(sprintf(
          "Savefile %O konnte nicht erstellt werden!\n",
          ssavef));
  }
}

// In welchen Regionen ist der Spieler Regionsmagier?
protected void set_domains(string player, string *domains)
{
  // wenn der Magier jetzt Domains nicht mehr hat, muessen die aus 'userids'
  // entfernt werden, das uebernimmt RemoveWizardFromUID().
  if (pointerp(userlist[player, USER_DOMAIN])) {
    string *removeduids=
      ((string*)userlist[player, USER_DOMAIN] | domains) - domains;
    foreach(string uid: removeduids)
      RemoveWizardFromUID(uid, player);
  }
  // gecachtes Alias fuer player loeschen
  m_delete(uidaliase,player);
  userlist[player, USER_DOMAIN]=domains;
  save_userinfo(player);
  // UID-zu-Magier-Mapping aktualisieren
  QueryUIDsForWizard(player);
}

// In welche Gilden ist der Spieler Gildenmagier?
protected void set_guilds(string player, string *guilds)
{
  // wenn der Magier jetzt Gilden nicht mehr hat, muessen die aus 'userids'
  // entfernt werden, das uebernimmt RemoveWizardFromUID().
  if (pointerp(userlist[player, USER_GUILD])) {
    string *removeduids=
      ((string*)userlist[player, USER_GUILD] | guilds) - guilds;
    foreach(string uid: removeduids)
      RemoveWizardFromUID(uid, player);
  }
  // gecachtes Alias fuer player loeschen
  m_delete(uidaliase,player);
  userlist[player, USER_GUILD]=guilds;
  save_userinfo(player);
  // UID-zu-Magier-Mapping aktualisieren
  QueryUIDsForWizard(player);
}

// Userinfo-Cache expiren...
protected void _cleanup_uinfo()
{
    foreach(string user: userlist) {
        if ((time() - userlist[user,USER_TOUCH]) > 1800
            && !call_sefun("find_player",user))
          m_delete(userlist,user);
    }
}

// expandiert einige 'Aliase' in ein Array von allen UIDs, fuer die sie
// stehen. Bsp: "region" -> d.region.* + region + d.region,
// "zauberer" -> GUILD.zauberer, "p.service" -> p.service.* oder auch
// "magier" -> QueryUIDsForWizard().
// Das erfolgt ueber Lookup der entsprechenden Verzeichnisse auf der PLatte.
// Da zusaetzlich auch noch jede Menge find_userinfo() dazu kommen, ist das
// recht aufwaendig. Damit das ganze nicht jedesmal erfolgen muss, wird das
// Ergebnis gecacht und QueryUIDAlias() nutzt den Cache.
private string* ExpandUIDAlias(string alias, int rec) {

  string *uids=({});

  // Regionsname?
  if (file_size("/"DOMAINDIR"/"+alias) == FSIZE_DIR) {
    //generelle Pseudo-UID 'd.region' und 'region' fuer geloeschte
    //Magier dieser Region
    uids += ({DOMAINDIR"."+alias, alias});
    //alle Magier-Verzeichnisse ermitteln:
    string tmpdir="/"DOMAINDIR"/"+alias+"/";
    foreach(string dir: (get_dir(tmpdir+"*") || ({}))
                          - ({".","..",".svn"})) {
      // schauen, obs nen (sichtbares) Verzeichnis ist und ob der Magier
      // existiert. Letzteres aber nur, falls die Rekursionstiefe min. 100
      // ist, da beim Nachschauen eines Magiers mit (mehreren)
      // RM-Posten, der in der Region RMs aus anderen Regionen hat, leicht
      // Rekursionstiefen von 20 (*4) auftreten koennen, wenn noch gar
      // keine UIDs im Cache sind (find_userinfo() ruft indirekt diese
      // Funktion).
      if (dir[0]!='.' 
          && file_size(tmpdir+dir) == FSIZE_DIR
#if __MAX_RECURSION__ > 99
          && find_userinfo(dir)
#endif
          )
          uids += ({DOMAINDIR"."+alias+"."+dir});
    }
  }
  // Gildenname?
  else if (GUILDMASTER->ValidGuild(alias)) {
      uids += ({GUILDID"."+alias});
      //hat die Gilde ein Projektverzeichnis?
      if (file_size("/"PROJECTDIR"/"+alias) == FSIZE_DIR) {
          uids += ({PROJECTDIR"."+alias});
      }
      // jetzt haben dummerweise die Spellbooks meist nicht den gleichen
      // Namen wie die Gilde. D.h. die Gilde muss nun noch nach dem
      // Namen des Spellbooks gefragt werden, weil dessen UID nicht
      // unbedingt gleich dem der Gilde ist. *seufz*
      string spbook;
      object guild;
      catch(guild=load_object(GUILDDIR"/"+alias));
      if (objectp(guild) 
          && (sizeof(spbook=(string)
                 guild->QueryProp(P_GUILD_DEFAULT_SPELLBOOK)))
                && spbook!=alias)
          uids += ({GUILDID"."+spbook});
  }
  // Spieler/Magier-UID?
  else if (find_userinfo(alias)) {
    // wenn rec > 0, wird eine Spieler-UID als Alias aufgefasst und zu seinen
    // UIDs expandiert. Hierbei erfolgt aber nur eine Rekursion.
    if (!rec) uids = QueryUIDsForWizard(alias, 1);
    else uids = ({alias});
  }
  // Projektkrams? -> alle Subdirs von /p/ ausser /p/service selber.
  else if (alias==PROJECTDIR) {
    foreach(string dir: (get_dir("/"PROJECTDIR"/*") || ({}))
                         - ({".","..",".svn","service"})) {
      if (dir[0]!='.' &&
          file_size("/"PROJECTDIR"/"+dir) == FSIZE_DIR)
          uids += ({PROJECTDIR"."+dir});
    }
  }
  // p.service? -> Alle Subdirs von /p/service.
  else if (alias==PROJECTDIR".service") {
    foreach(string dir: (get_dir("/"PROJECTDIR"/service/*") || ({}))
                         - ({".","..",".svn"})) {
      if (dir[0]!='.' &&
          file_size("/"PROJECTDIR"/service/"+dir) == FSIZE_DIR)
          uids += ({PROJECTDIR".service."+dir});
    }
  }
  // wenn nix zutrifft -> unexpandiert zurueckgeben
  else
    uids = ({alias});

  // im Cache vermerken
  if (sizeof(uidaliase) >= __MAX_MAPPING_KEYS__)
      uidaliase=m_allocate(1);
  // auch vermerken, wenn keine UIDs ermittelt wurden.
  uidaliase += ([alias: uids]);
  return uids;
}

