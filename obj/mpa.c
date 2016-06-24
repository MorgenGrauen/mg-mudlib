/* Das Servicepaket der Morgengrauen-Presseagentur. Eine erweiterte Zeitung.
   Teilweise zur alten Zeitung identisch.

   unter Verwendung der Zeitung von Jof, voellig umgeschrieben.

   (C) Nov 1993 - 1996  Loco@Morgengrauen

   Verwendung ausserhalb von Morgengrauen ist gestattet unter folgenden
   Bedingungen:
   - Benutzung erfolgt auf eigene Gefahr. Jegliche Verantwortung wird
     abgelehnt. Bugs ausserhalb von MG sind nicht mein Problem.
   - Auch in veraenderten oder abgeleiteten Objekten muss ein Hinweis auf
     die Herkunft erhalten bleiben.
   - Bitte fragt mich vorher!
   Ein Update-Service besteht nicht. Ruecknahme dieser Lizenz ist jederzeit
   moeglich. Und wer nicht in der Lage ist, neue Versionen selber anzupassen,
   hat in der Administration eines Muds nichts zu suchen.


   04.11.93   Erste brauchbare Version
   (... seitdem viele Aenderungen ...)
   13.09.95 ca 02:00  erwarte mpa
   13.09.95 ca 23:00  RebuildGroupList()
   15.09.95             vorlaeufig ausgebaut
   16.09.               anders wieder eingebaut, und Kleinigkeiten
   05.12.95 ca 19:20  Spieler koennen nur noch eigene Artikel loeschen/verl.
   07.12.95 ca 23:00  Keine Anzeige der Statuszeile. Statuszeilen beginnen
                      mit ~#!, sind damit nicht mit dem Editor erzeugbar.
   20.12.95 ca 17:00  uebergehe thread/antworten
   31.01.96           wiederhole/uncatchup
   22.04.96           Aufraeumen ignorierter threads. ALPHA!
   24.04.96           + - . als Adressierungen, zeitungsmeldung
   02.10.96           Kleinigkeiten
   08.10.96           Kleinigkeiten
   05.12.96           antworte auf artikel <nr>, reply <neuer titel>,
                      versende artikel, Abkuerzungen fuer Rubrikennamen,
                      Layoutaenderungen,
                      Herkunft bei verlegten Artikeln in Statuszeile

   Letzte Aenderung: 
   04.11.06 Zesstra  Anpassung an das neue Verhalten von inherit_list() in LD

*/

#include <properties.h>
#include <language.h>
#include <news.h>
#include <wizlevels.h>
#include <ansi.h>
#include <input_to.h>
#include "/mail/post.h"

inherit "/std/thing";
inherit NEDIT;
/*
#define DEBUG(x)        if (funcall(symbol_function('find_player),"zesstra"))\
          tell_object(funcall(symbol_function('find_player),"zesstra"),\
                      "MPA: "+x+"\n")
#define DEBUGVAR(x) DEBUG(sprintf(x+":\n%O\n",x))
*/
#define DEBUGVAR(x)

// Konfiguration, ggf mudabhaengig
#define DEFAULTGROUP "allgemeines"
#define MINIHELPPAGE "p/service/loco/doc/mini-mpa.txt"
#define HELPPAGE     "/p/service/loco/doc/mpa"
#define WIZHELPPAGE  "/p/service/loco/doc/mpa.wiz"
#define SAVEMSGPATH(pl) ("/open/News/"+(geteuid(pl))+".news")
#define IS_POST(r)  (member(inherit_list(r),STDPOST+".c")>=0)
#define INITIAL_SUBSCRIPTIONS \
  ({  "bekanntmachungen", \
      "d.ebene", \
      "d.wueste", \
      "d.inseln", \
      "d.unterwelt", \
      "d.dschungel", \
      "d.gebirge", \
      "d.polar", \
      "d.wald", \
      "d.fernwest", \
      "d.vland", \
      "d.anfaenger", \
      "entwicklung", \
   })

// Makros etc.

#define TP          this_player()
#define TI          this_interactive()
#define STATUSESCAPE "~#!" 
#define IS_STATUSLINE(s) ((s[0..1]=="#!")||(s[0..2]=="~#!"))
//#define IS_STATUSLINE(s) (s[0..2]=="~#!")
// in Rubrik muell fanden sich tatsaechlich noch solche uralten notes...
#define IGNOREGROUP "*ignored*"
#define NNADWMSG    "*NNADWMSG*"
#define SYSTEMGROUPS ({IGNOREGROUP,NNADWMSG})

// Aufbau der ignoregroup:
//   ([group1:([tid1:lasttime1,tid2:lasttime2,...]),group2:...])

// Flags fuer Message2string()
#define M2S_VT100    1
#define M2S_ANSI     M2S_VT100
#define M2S_REMOTE   2

// Format von lasttitle
#define LAST_TITLE   0
#define LAST_WRITER  1
#define LAST_TIME    2
#define LAST_TID     3
#define LAST_GROUP   4
#define LAST_NR      5
#define LAST_SIZEOF  6


mapping read_until;
mixed   lasttitle; // Aufbau s.o.
mixed   message;
int     deadTID;
string  GROUP;

int GetTID(mixed message);
private void InitialSubscriptions();

create()
{
  ::create();
  seteuid(getuid());
  GROUP="bekanntmachungen";
  SetProp(P_SHORT,"Die Zeitung");
  SetProp(P_NAME,"Zeitung");
  SetProp(P_WEIGHT, 50);
  SetProp(P_SIZE, 35);
  SetProp(P_MATERIAL, ([MAT_PAPER: 100]) );
  SetProp(P_GENDER, FEMALE);
  AddId(({"nn","zeitung","servicepaket","mpa"}));
  SetProp(P_NODROP,
    "Das persoenliche Servicepaket der MPA kann man nicht wegwerfen.\n");
  SetProp(P_NEVERDROP, 1);

  if (!read_until) read_until=(["muell":-1]);
}

static mixed _query_read_msg() { return long(); }

long() {
  return "\
Dies ist das Servicepaket der MPA (Morgengrauens Presse Agentur) -\n\
die Zeitung des Morgengrauens.\n\
Du kannst damit aktuelle Artikel lesen und schreiben, mit folgenden Befehlen:\n\
 'nn [rubrik]' (Neueste Nachrichten) wuehlt die Rubrik durch und schaut,\n\
                ob was neu ist. Der erste ungelesene Artikel wird angezeigt.\n\
                Ohne Argument werden alle Rubriken durchwuehlt.\n\
 'hilfe mpa'    Ausfuehrliche Hilfsseite. Lesen dringend empfohlen!\n\
Weitere Befehle:\n\
 rubriken [neu]                       bestelle <rubrik> ab\n\
 [lies ]rubrik <rubrik>               abonniere <rubrik>\n\
 inhalt [<rubrik>|neu|suche <text>]   wiederhole ...\n\
 nn <rubriken>|<liste>                uebergehe ...\n\
 [lies ]artikel <nummer>"+
  ( (IS_LEARNER(TP)) ? "              speichere artikel <nr>" : "" )+"\n\
 schreib <titel>                      versende artikel [nr] an <adresse>\n\
 antworte [auf artikel <nr>] [titel]  verlege artikel <nr> nach <rubrik>\n\
 loesche artikel <nr>                 zeitungsmeldung [neue Meldung]\n\
 verbrenne zeitung                    wenn Dir alles zuviel wird....\n\
"+ (IS_SEER(TP) ? "\
Mit 'mail' bzw 'mail <spieler>' kannst Du Post lesen und schreiben.\n\
" : "" )+"\
Eingebaute aliase: note, catchup, reply, unsubscribe, subscribe, uncatchup.\n\
Viele Befehle geben mit '<befehl> ?' einige Syntaxhinweise.\n\
Aktuelle Rubrik: "+GROUP+"\n\
";
}


init()
{
  ::init();

  add_action("schreib","schreib");
  add_action("schreib","schreibe");
  add_action("schreib","note");
  add_action("LiesArtikel","lies");
  add_action("artikel","artikel");
  add_action("loesche","loesch");
  add_action("loesche","loesche");
  add_action("rubrik","rubrik");
  add_action("inhalt","inhalt");
  add_action("rubriken","rubriken");
  add_action("ReadNextUnread","nn");
  add_action("Catchup","catchup");
  add_action("Catchup","uebergeh");
  add_action("Catchup","uebergehe");
  add_action("Uncatchup","wiederhol");
  add_action("Uncatchup","wiederhole");
  add_action("Uncatchup","uncatchup");
  add_action("Reply","antwort");
  add_action("Reply","antworte");
  add_action("Reply2","reply");
  add_action("HelpPage","hilfe");
  add_action("HelpPage","man");
  add_action("Unsubscribe","unsubscribe");
  add_action("Bestelle","bestell");
  add_action("Bestelle","bestelle");
  add_action("Subscribe","subscribe");
  add_action("Subscribe","abonnier");
  add_action("Subscribe","abonniere");
  add_action("MoveMessage","verleg");
  add_action("MoveMessage","verlege");
  add_action("SetNNADWMSG","zeitungsmeldung");
  add_action("Ignore","ignorier");
  add_action("Ignore","ignoriere");
  add_action("MailMessage","versende");
  add_action("verbrennen","verbrenne");

  if (IS_SEER(TP)) {
    add_action("Mail","mail");
  }
  if (IS_LEARNER(TP)) {
    add_action("SaveMessage","speicher");
    add_action("SaveMessage","speichere");
    add_action("ReadNextUnread","read"); /* NF Compatibility Special */
  }
  if (IS_ELDER(TP))
  {
    add_action("MoveTrash","trash");
  }
  init_rescue();
}

verbrennen(str) {
  if (!str || !id(str)) return 0;
  write("Du verbrennst Deine Zeitung mit groesstem Vergnuegen.\n");
  say(TP->Name(WER)+" verbrennt "+
    TP->QueryPossPronoun(FEMALE, WEN, SINGULAR)+" Zeitung in einem "
    "Freudenfeuer.\n");
  remove(1);
  return 1;
}

// KillGroup - loescht eine nicht mehr benoetigte Rubrik aus der Liste.
// Aufruf aus rubriken() und ReadNextUnread(), wenn die gueltige
// Rubrikenliste sowieso schon abgerufen wurde.

static KillGroup(name) { read_until=m_copy_delete(read_until,name); }

/* RebuildGroupList() - tut doch nicht so, weil /secure/news anders arbeitet.
 * Bleibt vorerst zum Nachschlagen.
static RegisterGroup(name) { read_until[name]=1; }
static RebuildGroupList() {
  mixed groups;
  if ((pointerp(groups=NEWSSERVER->GetGroups()))&&(sizeof(groups))) {
    map(m_indices(read_until)-groups-SYSTEMGROUPS,#'KillGroup);
    map(groups-m_indices(read_until),#'RegisterGroup);
  }
  TP->SetProp(P_READ_NEWS,read_until);
}    
*/


_set_autoloadobj(mixed arg) {

  if (pointerp(arg) && sizeof(arg)>=2)
  {
    read_until=arg[1];
  }
  else {
    if (TP)
       read_until=((TP->QueryProp(P_READ_NEWS))||([]));
  }

  if (TP) TP->SetProp(P_READ_NEWS,read_until);
}

_query_autoloadobj() {
  return 1;
}

static Mail(str) {
  object mailer;
  if (this_interactive()!=this_player()) return 0;
  mailer=clone_object(MAILER);
  mailer->SetOfficeName("mpa Kurierdienst");
  mailer->do_mail( ((!str)||(str=="mail")) ? 0 : TP->_unparsed_args() );
  return 1;
}

static varargs string Message2string(mixed msg,mixed messages,int flag,string group) {
  // Aufrufe: (Nummer,Notes[,flag[,group]]) oder (Note,Notes[,flag[,group]])
  // flag: M2S_VT100 : ggf ANSI-Codes verwenden
  //       M2S_REMOTE: Rubrik und Artikelnummer ausgeben (speichern, versenden)
  // Achtung: Wenn flag&M2S_REMOTE, muss msg int sein
  // group: Name der Rubrik, wenn nicht aktuelle Rubrik. Nur bei M2S_REMOTE

  string txt,hs,s,*m,s2;
  int i,hi,thisnr,ansiflag;

  if (flag&M2S_REMOTE) txt="Rubrik: "+(group?group:GROUP)+", Artikel: "+
    (intp(msg)?to_string(msg+1):"???")+" von "+sizeof(messages)+"\n";
  else txt="";

  if (intp(msg)) {
    thisnr=msg;
    msg=messages[msg];
  } else thisnr=-1;
  if (!msg) return 0;

  ansiflag=(flag&M2S_VT100)&&(this_player()->QueryProp(P_TTY)!="dumb");

  txt += (ansiflag?ANSI_BOLD:"")+ msg[M_TITLE]+(ansiflag?ANSI_NORMAL:"")+
    " ("+msg[M_WRITER]+", "+
    dtime(msg[M_TIME])[5..26]+"):\n";
//  if (geteuid(TP)=="sitopanaki") txt+="TID="+GetTID(msg)+"\n"; // Debug
  if (!IS_STATUSLINE(msg[M_MESSAGE]))
    return txt+"\n"+msg[M_MESSAGE];
  m=explode(msg[M_MESSAGE],"\n");
  while (IS_STATUSLINE(m[0])) {
//    txt+=m[0]+"\n"; // ###
    if (sscanf(m[0],"%s rn=%s rt=%d rg=%s",hs,s,hi,hs)==4 ||
        sscanf(m[0],"%s rn=%s rt=%d",hs,s,hi)==3)
    {
      int nr,h;
      nr=-1;
      if (pointerp(messages))
      {
        for (h=((thisnr>=0) ? thisnr-1 : sizeof(messages)-1);
             (h>=0 && messages[h][M_TIME]>=hi);h--) 
          if (messages[h][M_TIME]==hi &&
              lower_case(messages[h][M_WRITER])==lower_case(s))
            nr=h;
      }
      txt+="[Bezug: Artikel "+((nr>=0) ? (nr+1) : 
         (hs==STATUSESCAPE||hs==(group?group:GROUP)?"(geloescht?)":"in "+hs))+
         " von "+capitalize(s)+" vom "+dtime(hi)[5..26]+"]\n";
    }
    if (sscanf(m[0],"%s on=%s ot=%d og=%s",hs,s,hi,hs)==4) {
      txt+="[Autor: "+s+", "+dtime(hi)[5..26]+", verlegt von "+hs+"]\n";
    }
    m=m[1..];
  }
  return txt+"\n"+implode(m,"\n");
}

static varargs lies(mixed str) {
  mixed num;
  mixed *messages;
  int tid;

  if (str=="?"||str=="-?") return 
    write("Syntax: lies <nr>\n"
          "        artikel <nr>\n"
          "Siehe auch: nn\n"),1;

  if (intp(str)) num=str;
  if ((!num && (!str || str=="" || sscanf(str,"%d",num)!=1)) || num<=0) {
    notify_fail("WELCHE Nachricht willst Du lesen?\n");
    return 0;
  }
  if (!pointerp(messages=NEWSSERVER->GetNotes(GROUP)))
    return notify_fail("Seltsam, die Rubrik '"+GROUP+
        "' gibt es nicht mehr...\n"), 0;
  num--;
  if (sizeof(messages)<=num) {
    notify_fail("So viele Artikel sind da nicht!\n");
    return 0;
  }
  
  lasttitle=({messages[num][M_TITLE],messages[num][M_WRITER],
      messages[num][M_TIME],GetTID(messages[num]),GROUP,num});
  this_player()->More(Message2string(messages[num],messages,M2S_VT100));
  if (this_player() && IS_LEARNER(this_player()))
    this_player()->save_me(1);
  return 1;
}

static varargs mixed GetGroupName(mixed g,mixed groups) {
  /* Name einer Rubrik. g ist int oder string, enthaelt Name oder Nummer
     (ab 1 numeriert) */
  mixed i,expr,gg;
  if (!g) return write("Du solltest schon die Rubrik angeben.\n"),0;
  if (!groups) groups=NEWSSERVER->GetGroups();
  if (intp(i=g) || sscanf(g,"%d",i)) {
    if (i>0 && i<=sizeof(groups)) return groups[i-1];
    write("Eine Rubrik mit der Nummer "+i+" gibt es leider nicht.\n");
    return 0;
  }
  g=lower_case(g);
  switch(g){
  case ".": return GROUP;
  case "+": return groups[(member(groups,GROUP)+1)%sizeof(groups)];
  case "-": 
    return groups[(member(groups,GROUP)-1+sizeof(groups))%sizeof(groups)];
  }

  // Existiert die Rubrik genau so?
  if (member(groups,g)>-1) return g;

  g = regreplace(g,"[[\\]\\*()?]","",1);
  // haerteres Kriterium: Alle Abschnitte angegeben
  expr="^"+implode(explode(g,"."),"[^\\.]*\\.")+"[^\\.]*$";
//  write("REGEXP="+expr+"\n");
  gg=regexp(groups,expr);
  if (sizeof(gg)==1) return gg[0];

  // weicheres Kriterium: Nicht alle Abschnitte angegeben
  expr="^(.*\\.)*"+implode(explode(g,"."),".*\\.")+".*$";
//  write("REGEXP="+expr+"\n");
  gg=regexp(groups,expr);

  if (!sizeof(gg)) {
    write("Eine Rubrik '"+g+"' gibt es hier leider nicht.\n");
    return 0;
  }

  if (sizeof(gg)==1) return gg[0];
  
  write(break_string("Die Rubrik "+g+" ist nicht eindeutig. Wahrscheinlich "
        "meinst Du eine der folgenden: "+implode(gg,", ")+".\n",78));
  return 0;
}

static rubrik(str)
{
  mixed *gruppen;
  mixed news;
  int anz,i;
  
  if (str=="?"||str=="-?") return
    write("Syntax: rubrik <rubrik>\n"
          "  wechselt die aktuelle Rubrik. Es wird die Nummer der Rubrik,\n"
          "  ihr Name oder jede eindeutige Abkuerzung akzeptiert.\n"),1;

  if (!str || str==0) {
      if (!pointerp(news=NEWSSERVER->GetNotes(GROUP))){
      GROUP=DEFAULTGROUP;
    if (!pointerp(news=NEWSSERVER->GetNotes(GROUP)))
      return notify_fail("Seltsam, irgendwie geht hier einiges schief...\n"),0;
      }
    return write("Aktuelle Rubrik: "+GROUP+" ("+sizeof(news)+" Artikel).\n"),1;
  }
  str=GetGroupName(str);
  if (!str) return 1;
  GROUP=str;
  news=NEWSSERVER->GetNotes(GROUP);
  write(break_string("Ok, Du hast die Rubrik "+GROUP+" mit "+sizeof(news)+
                     " Artikel"+(sizeof(news)==1?"":"n")+" aufgeschlagen.\n",
                     78));
  return 1;
}

LiesArtikel(str) {
  string s1;
  int i1;
  if ( !str ) return 0;
  if (sscanf(str,"rubrik %s",s1))
    return rubrik(s1);
   if (sscanf(str,"%d",i1))
    return lies(to_int(i1));
  if (sscanf(str,"artikel %s",s1))
    return lies(s1);
}


/* Ueberpruefe, ob in Rubrik tote threads ignoriert werden. Tot bedeutet,
   dass der letzte darin uebergangene Artikel bereits expired ist UND
   keine Artikel aus diesem thread mehr vorhanden sind.
   ACHTUNG: Es kann passieren, dass dazwischen noch ein neuer Artikel
   dazugekommen ist.
   Hier wird auf expired geprueft und ggf Variable deadTID gesetzt. Wird beim
   Lesen mit nn ein Artikel mit dieser TID gefunden (passiert direkt hinter
   Pruefung), wird deadTID wieder auf 0 gesetzt. Ist deadTID beim Umschalten
   auf naechste Rubrik !=0, ist der thread tot.
*/

static int CheckThreads(string rubrik,int timeout) {
  mixed tids;
  int i;

//  deadTID=0;
  tids=m_indices(read_until[IGNOREGROUP][rubrik]);
  for (i=sizeof(tids)-1;i>=0&&!deadTID;i--)
    if (read_until[IGNOREGROUP][rubrik][tids[i]]<timeout) deadTID=tids[i];
  return 1;
}  

static int rubriken(mixed arg)
{
  mixed *gruppen, *messages;
  mixed news;
  int anz,i,lasttime,timeout,foundone;
  string s;
  
  if (arg=="?"||arg=="-?") return
    write("Syntax: rubriken        listet alle Rubriken\n"
          "        rubriken neu    nur Rubriken mit ungelesenen Artikeln\n"),1;

  gruppen=NEWSSERVER->GetGroups();
  map(m_indices(read_until)-gruppen-SYSTEMGROUPS,#'KillGroup); // ');
  s="\nEs gibt zur Zeit ";
  anz=sizeof(gruppen);
  if (anz==0) {
    write(s+"keine Rubriken (wie seltsam ...)\n");
    return 1;
  }
  s+=anz+" Rubrik"+(anz==1 ? "" : "en")+".";
  if (arg=="neu") s+="\nDavon enthalten neue Artikel:\n\n";
  else s+="\n(* oder x: enthaelt neue Artikel, x oder -: abbestellt, "
    ">: aktuelle Rubrik)\n\n";
  for (i=0;i<anz;i++) {
    timeout=read_until[gruppen[i]];
    /* GetNewsTime lieferte leider manchmal was falsches :-(  */
    /* jetzt hoffentlich richtig? Wenn nicht, das if ausklammern */
    if ( arg!="neu" || (lasttime=NEWSSERVER->GetNewsTime(gruppen[i])) > timeout) {
      messages=NEWSSERVER->GetNotes(gruppen[i]);
      if (!messages || !sizeof(messages)) lasttime=0;
      else lasttime=messages[sizeof(messages)-1][M_TIME];
      foundone=1;
    }
    if (arg!="neu" || (timeout>=0 && lasttime>abs(timeout)))
      s+=sprintf("%s%s%3d\. %-39.39s: ",
                 ((gruppen[i]==GROUP)?">":" "),
                 ((lasttime>abs(timeout)) ? ((timeout<0) ? "x" : "*")
                                          : ((timeout<0) ? "-" : " ") ),
                 i+1,gruppen[i])+ 
         (lasttime ? sprintf("%3d Artikel (%s)\n",
                             sizeof(messages),
                             // ((sizeof(messages)==1) ? ".  " : "en."),
                             dtime(lasttime)[5..12]+ctime(lasttime)[<2..]) :
                             "        - leer -\n");
  }
  if (arg=="neu"&&!foundone) s+="Keine Rubrik enthaelt neue Artikel.\n";
  this_player()->More(s);
  return 1;
}

#define M_READNEXT  1
#define M_LISTNEW   2
#define M_LISTNEWGR 3
#define M_READGR    4

static ReadNextUnread(str)
{
  string *groups,group;
  mixed *messages;
  int curgr,curmsg,timeout,start,nrgroups,sog,mode;

  if (str=="mail") return Mail(0); /* NF Compatibility Special */

  if (str=="?"||str=="-?") return
    write("Syntax: nn             naechster neuer Artikel\n"
          "        nn <rubrik>    in entspr. Rubrik, wenn da was neu ist\n"
          "        nn rubriken    Liste der Rubriken mit neuen Artikeln\n"
          "        nn liste       Liste der ungelesenen Artikel\n"),1;

  groups=NEWSSERVER->GetGroups();
  deadTID=0;
  map(m_indices(read_until)-groups-SYSTEMGROUPS,#'KillGroup); //'
  nrgroups=sizeof(groups);
  if (str && str!="rubriken" && str!="liste"){
    if (!group=GetGroupName(str)) return 1;
  }
  else
    group=0;
  if (group && (curgr=member(groups,group)+1)) {
    --curgr;
    if (curgr<0 || curgr>=sizeof(groups)) 
      return notify_fail("Nee... so eine Rubrik gibts hier nicht.\n"),0;
    GROUP=group;
    start=curgr+1;
    mode=M_READGR;
    write("Rubrik "+(curgr+1)+": "+GROUP+"\n");
  } else {
    switch (str) {
    case 0: mode=M_READNEXT; break;
    case "liste": mode=M_LISTNEW; write("Du hast noch nicht gelesen:\n"); break;
    case "rubriken": return rubriken("neu");
    default:
      notify_fail("\
Diesen Parameter verstehe ich nicht. Entweder gar nichts, \"liste\"\n\
\"rubriken\", oder Name bzw. Nummer einer Rubrik.\n");
      return 0;
    }
    curgr=member(groups,GROUP);
    start=curgr+nrgroups;
  }
  if (!pointerp(messages=NEWSSERVER->GetNotes(GROUP))){
    GROUP=DEFAULTGROUP;
    if (!pointerp(messages=NEWSSERVER->GetNotes(GROUP)))
    return notify_fail("Seltsam, irgendwie geht hier einiges schief...\n"),0;
  }
  timeout=read_until[GROUP];
  curmsg=0;
  sog=sizeof(messages);
  while (curgr<start) {
    ++curmsg;
    if (curmsg>sog) {
      if (deadTID)
        read_until[IGNOREGROUP][GROUP]=
          m_copy_delete(read_until[IGNOREGROUP][GROUP],deadTID);
      ++curgr;
      deadTID=0;
      if (mode!=M_READGR) {
        GROUP=groups[curgr%nrgroups];
        timeout=read_until[GROUP];
        if (!timeout) read_until[GROUP]=1;  // Nimm neue Gruppen in Liste auf
        if (timeout<0 || timeout>=NEWSSERVER->GetNewsTime(GROUP)) {
          sog=0;    /* Ueberlistung: Gruppe hat nix neues oder */
          curmsg=1; /* ist unsubscribed */
        }
        else {
          messages=NEWSSERVER->GetNotes(GROUP);
          curmsg=0;
          sog=sizeof(messages);
        }
      }
    } else {
      if ((timeout>=0 || mode==M_READGR) && messages[curmsg-1][M_TIME] > abs(timeout)) {
        if (pointerp(this_player()->QueryProp(P_IGNORE)) &&
            member(this_player()->QueryProp(P_IGNORE),
                   lower_case(messages[curmsg-1][M_WRITER])+".news") != -1) {
          printf("Uebergehe ignorierten Artikel %d von %s in Rubrik %s.\n",
                 curmsg,messages[curmsg-1][M_WRITER],GROUP);
          read_until[GROUP]=messages[curmsg-1][M_TIME];
          if (TP) TP->SetProp(P_READ_NEWS,read_until);
        } else if 
          (read_until[IGNOREGROUP]&&
           read_until[IGNOREGROUP][GROUP]&&
           CheckThreads(GROUP,messages[0][M_TIME])&& /* Tote threads weg */
           read_until[IGNOREGROUP][GROUP][GetTID(messages[curmsg-1])]) {
            printf("Uebergehe Artikel %d aus ignoriertem Thread.\n",curmsg);
            read_until[IGNOREGROUP][GROUP][GetTID(messages[curmsg-1])]=
              messages[curmsg-1][M_TIME];
            if (deadTID&&deadTID==GetTID(messages[curmsg-1])) deadTID=0;
            read_until[GROUP]=messages[curmsg-1][M_TIME];
            if (TP) TP->SetProp(P_READ_NEWS,read_until);
        } else {
          write("\nRubrik "+(curgr%nrgroups+1)+": "+GROUP+", Artikel: "+curmsg+" von "+sog+"\n");
          if (mode==M_LISTNEW) {
            write(sprintf("  %-45s [%-11s] %s\n",messages[curmsg-1][M_TITLE],
                          messages[curmsg-1][M_WRITER],
                          dtime(messages[curmsg-1][M_TIME])[5..16]));
          } else { /* mode == M_READNEXT || mode==M_READGR */
            if (timeout>=0) read_until[GROUP]=messages[curmsg-1][M_TIME];
            else read_until[GROUP]=-messages[curmsg-1][M_TIME];
            if (TP) TP->SetProp(P_READ_NEWS,read_until);
            return (lies(""+curmsg));
          }
        }
      }
      /* sonst mach einfach garnix. Schleife laeuft weiter. */
    }
  }
  switch(mode) {
    case M_LISTNEW:  return 1;
    case M_READNEXT: write((read_until[NNADWMSG]||"Nix Neues auf der Welt.")
                           +"\n"); break;
    case M_READGR:   write("Nix Neues in dieser Rubrik.\n"); break;
  }
  return 1;
}


static SetNNADWMSG(str) {
  if (str=="?"||str=="-?") return
    write("Syntax: zeitungsmeldung <neue Meldung>    setzt Meldung\n"
          "        zeitungsmeldung                   loescht Meldung\n"),1;
  if (!read_until[NNADWMSG]) {
    write("Du hast zur Zeit keine eigene NNADW-Meldung definiert.\n");
    if (!str) return 1;
  }
  else write("Deine alte NNADW-Meldung war:\n"+read_until[NNADWMSG]+"\n");
  if (!str) {
    read_until=m_copy_delete(read_until,NNADWMSG);
    write("Meldung ist geloescht, es gilt wieder die Standardmeldung.\n");
  } else {
    read_until[NNADWMSG]=this_player()->_unparsed_args();
    write("Deine neue Meldung lautet jetzt:\n"+read_until[NNADWMSG]+"\n");
  }
  if (TP) TP->SetProp(P_READ_NEWS,read_until);
  return 1;
}    

varargs int InterpretTime(mixed a,int flag) {
  // string oder string *
  // akzeptiert folgende Formate:
  //   dd.mm.jj     (Rueckgabe: 0:00 des entsprechenden Tages)
  //   vor [<anz> d|tagen] [<anz> h|stunden] [<anz> m|minuten]
  // flag=1: "inklusive": bei dd.mm.jj-Format 23:59:59 statt 0:00 Uhr

  int i,j,k,t,nrargs;
  string s;
  if (stringp(a)) a=explode(a," ");

//  printf("%O\n",a);

  if ((nrargs=sscanf(a[0],"%d.%d.%d",i,j,k))==3 || 
      (nrargs=sscanf(a[0],"%d.%d.",i,j))==2) {
    // Datum -> Zeit: Funktioniert im Zeitraum 1973 - ca. 2090
    //                in Zeitzonen mit ganzen Stunden ggue Rechneruhr.
    if (nrargs==2) 
      k=70+time()/31536000;
    if (k<70) k+=100;
    if (k>1970) k-=1900;
    if (k<70||k>150) return
      write("Unzulaessiges Jahr (erlaubt: 70-heute).\n"),0;
    t=(k-70)*31536000;

    if (i<1||i>31) return write("Unzulaessiger Tag (erlaubt: 1-31).\n"),0;
    if (j<1||j>12) return write("Unzulaessiger Monat (erlaubt: 1-12).\n"),0;
//    printf("%d.%d.%d\n",i,j,k);
    s=ctime(t);
    if ((j>2) && !(k%4)) t+=86400;    // Schaltjahrkorrektur fuer Monate>=3
    t+=({        0,  2678400,  5097600,  7776000,
          10368000, 13046400, 15638400, 18316800,
          20995200, 23587200, 26265600, 28857600})[j-1];
    t+=86400*(i-1);
    t+=86400*(32-to_int(s[8..9]));  // Schaltjahrkorrektur
    t-=3600*to_int(s[11..12]);      // Zeitzonenkorrektur
    t-=3600*to_int(ctime(t)[11..12]);      // Sommerzeitkorrektur
//    write("Kontrolle: "+dtime(t)+"\n");
    if (nrargs==2 && t>time()) t-=31536000;
    return (flag?t+86399:t);
  }

  t=0;
  if (a[0]=="vor") for (i=sizeof(a)-1;i>0;i--) {
    switch (a[i]) {
    case "m": 
    case "minuten": 
    case "min": 
    case "minute":
      t+=60*to_int(a[i-1]);
      break;
    case "h": 
    case "stunde": 
    case "stunden": 
    case "s":
      t+=3600*to_int(a[i-1]);
      break;
    case "d": 
    case "tag": 
    case "tage": 
    case "t":
      t+=86400*to_int(a[i-1]);
      break;
    default: 
      if (!to_int(a[i]))
        write("Argumentfehler: Kann nichts mit '"+a[i]+"' anfangen.\n");
    }
    return time()-t;
  }
  else return write("Argumentfehler.\n"),0;
}

static Catchup(string str)
{
  int welche,zeit,i;
  string gr;
  mixed groups,news,args;

  if (!pointerp(NEWSSERVER->GetNotes(GROUP)))
    return notify_fail("Seltsam, die Rubrik '"+GROUP+
        "' gibt es nicht mehr...\n"), 0;

  str=this_player()->_unparsed_args();   // wg. Datum
  if (str) str=lower_case(str);
  else str=GROUP; // default: aktuelle Rubrik komplett.

  if (str=="?"|| str=="-?") return CatchupSyntax();

  // uebergehe Antworten (Thread ignorieren)
  if (str&&(str[0..6]=="antwort"||str=="thread")) {
    if (!pointerp(lasttitle)) return 
      write("Du hast bisher noch keinen Artikel gelesen, damit kann ich nicht wissen,\nwelchen Thread du uebergehen willst.\n"),1;
    if (!read_until[IGNOREGROUP]) read_until[IGNOREGROUP]=([]);
    if (!read_until[IGNOREGROUP][GROUP]) read_until[IGNOREGROUP][GROUP]=([]);
    if (read_until[IGNOREGROUP][GROUP][lasttitle[3]]) {
      read_until[IGNOREGROUP][GROUP]=m_copy_delete(read_until[IGNOREGROUP][GROUP],lasttitle[3]);
      write("Dieser Thread wird jetzt nicht mehr uebergangen.\n");
    } else {
      read_until[IGNOREGROUP][GROUP][lasttitle[3]]=lasttitle[2];
      write("Dieser Thread wird ab sofort uebergangen.\nFalls das ein Irrtum war, wiederhole den Befehl augenblicklich.\n");
    }
    if (TP) TP->SetProp(P_READ_NEWS,read_until);
    return 1;
  }    

  groups=NEWSSERVER->GetGroups();

  args=explode(str," ");

  // Uebergehe alles

  if (args[0]=="alle" || args[0]=="alles" || args[0]=="all") {
    if (sizeof(args)<=1) zeit=time()-1;
    else if (args[1]!="bis") return CatchupSyntax();
    else if (sizeof(args)<3) return CatchupSyntax();

    else zeit=InterpretTime(args[2..],1);
    if (zeit>time()) zeit=time()-1;
    write("Uebergehe alle Rubriken bis "+dtime(zeit)+".\n");
    for (welche=0;welche<sizeof(groups);++welche) {
      gr=groups[welche];
//      zeit=NEWSSERVER->GetNewsTime(gr);
      if (abs(read_until[gr])<zeit) 
        read_until[gr]=(read_until[gr]>=0)?zeit:-zeit;
      if (TP) TP->SetProp(P_READ_NEWS,read_until);
    }
    return 1;
  }

  // Anzahl Artikel
  
  if (sizeof(args)>=2 && args[1]=="artikel") {
    if (!to_int(args[0])) return CatchupSyntax();
    news=NEWSSERVER->GetNotes(GROUP);
    for (i=sizeof(news)-1;i>=0&&news[i][M_TIME]>read_until[GROUP];i--);
    welche=i+to_int(args[0]);
    if (welche>=sizeof(news)) welche=sizeof(news)-1;
    write("Uebergehe die naechsten "+(welche-i)+" Artikel in Rubrik "+
        GROUP+"\n.");
    if (welche>=0) {
      zeit=news[welche][M_TIME];
      read_until[GROUP]=(read_until[GROUP]>=0)?zeit:-zeit;
      if (TP) TP->SetProp(P_READ_NEWS,read_until);
    }
    return 1;
  }

  // einzelne Rubrik.

  if (!(gr=GetGroupName(args[0]))) return 1;
  news=NEWSSERVER->GetNotes(gr);
  if (!sizeof(news)) {
    write("Rubrik "+gr+" ist leer.\n");
    return 1;
  }
//  zeit=news[sizeof(news)-1][M_TIME];
  if (sizeof(args)<=1)
    zeit=time();
  else
    if (args[1]!="bis") return CatchupSyntax();
  else 
    zeit=InterpretTime(args[2..],1);
  if (zeit>time()) zeit=time();
  read_until[gr]=zeit;
  if (TP) TP->SetProp(P_READ_NEWS,read_until);
  write("Uebergehe "+gr+" bis "+dtime(zeit)+",\nletzter Artikel war vom "+
      dtime(NEWSSERVER->GetNewsTime(gr))+"\n");
  return 1;
}


static Ignore(str) {
  if (str=="thread"||str=="antworten") return Catchup(str);
  return 0;
}

static CatchupSyntax() {
  write("Syntax des Befehls uebergehe (oder catchup):\n"
        "  uebergehe [rubrik]              (default: aktuelle Rubrik)\n"
        "  uebergehe alles                 (in allen Rubriken)\n"
        "  uebergehe <anz> artikel         (in akt. Rubrik)\n"
        "  uebergehe [rubrik]|alles bis <tag>.<monat>.[<jahr>]\n"
        "  uebergehe [rubrik]|alles bis vor <zeit>        wobei\n"
        "      <zeit> = [<n> d|tage] [<n> h|stunden] [<n> m|min|minuten]\n"
        "  uebergehe thread|antworten      (entspr. 'ignoriere thread')\n");

  return 1;
}


static int UncatchupSyntax()
{
  notify_fail(
    "Syntax: wiederhole <anz> [artikel]\n"
    "        wiederhole [ab vor] [<anz> m|minute[n]] [<anz> h|stunde[n]] [<anz> d|tag[e]]\n"
    "        wiederhole ab tag.monat[.jahr]\n"
    "        wiederhole alles\n"
    "Der wiederhole- oder uncatchup-Befehl bezieht sich immer auf die aktuelle\n"
    "Rubrik und markiert die angegebenen Artikel wieder als ungelesen.\n"
    "Zeiten (2. Syntax) sind rueckwaerts ab aktueller Uhrzeit gerechnet.\n"),0;
  return 0;
}


static Uncatchup(string str)
{
  mixed args;
  int i,zeit;
  int mode; // 0 = nix, 1=Anzahl Artikel, 2=Zeit, 3=alles

  str=this_player()->_unparsed_args();   // wg. Datum
  if ( !TP || !str || !stringp(str) || str=="?" || str=="-?" )
    return UncatchupSyntax();

  if (!pointerp(NEWSSERVER->GetNotes(GROUP)))
    return notify_fail("Seltsam, die Rubrik '"+GROUP+
        "' gibt es nicht mehr...\n"), 0;

  args=({""})+explode(lower_case(str)," ");
  if (args[1]=="ab") {
    mode=2;
    if (sizeof(args)<3)
      return UncatchupSyntax();
    zeit=InterpretTime(args[2..]);
  } else {
    zeit=time();
    for (i=sizeof(args)-1;i>0;i--) {
      switch(args[i]){
      case "alles":
      case "alle":
      case "all":
        if (mode&&mode!=3) return
          notify_fail("Bitte nur Zeit ODER alles ODER Anzahl angeben!\n"),0;
        mode=3;
        break;
      case "minuten":
      case "minute":
      case "m":
      case "stunden":
      case "stunde":
      case "h":
      case "tage":
      case "tag":
      case "d":
        if (mode&&mode!=2) return
          notify_fail("Bitte nur Zeit/Datum ODER alles ODER "
              "Anzahl angeben!\n"),0;
        mode=2;
        zeit-=(((args[i][0]=='m') ? 60 :
                ((args[i][0]=='s' || args[i][0]=='h') ? 3600 : 86400))
               *to_int(args[i-1]));
        i--;
        break;
      case "artikel":
        if (mode&&mode!=1) return 
          notify_fail("Bitte nur Zeit/Datum ODER alles ODER "
              "Anzahl angeben!\n"),0;
        mode=1;
        zeit=to_int(args[i-1]);
        i--;
        break;
      case "ab":
        return 
          notify_fail("Bitte nur Zeit/Datum ODER alles ODER "
              "Anzahl angeben!\n"),0;
      default:
        if (!to_int(args[i])) 
          return notify_fail("Unbekanntes Argument '"+args[i]+
                             "'! Aktion abgebrochen.\n"),0;
        if (mode&&mode!=1) return 
          notify_fail("Bitte nur Zeit/Datum ODER alles ODER "
              "Anzahl angeben!\n"),0;
        mode=1;
        zeit=to_int(args[i]);
      }
    }
  }

  switch(mode){
  case 0:
    return notify_fail("Gib bitte irgendeine Einheit an "
        "(Hilfe mit wiederhole -?)!\n");
  case 2:
    read_until[GROUP]=(read_until[GROUP]>=0)?zeit:-zeit;
    write(break_string("Markiere alle Artikel in Rubrik "+GROUP+
          " seit "+dtime(zeit)+" als ungelesen.\n",78));
    break;
  case 3:
    read_until[GROUP]=(read_until[GROUP]>=0?1:-1);
    write("Markiere die komplette Rubrik "+GROUP+" als ungelesen.\n");
    break;
  case 1:
    write(break_string("Markiere die letzten "+zeit+
          " gelesenen Artikel in Rubrik "+GROUP+" als ungelesen.\n",78));
    { int h; mixed notes;
      notes=NEWSSERVER->GetNotes(GROUP);
      h=sizeof(notes)-1;
      while ( (h>=0) && (abs(read_until[GROUP]) < notes[h][M_TIME]) ) {
        h--;
      }
      if (h==-1||h<zeit)
        read_until[GROUP]=
          (read_until[GROUP]>=0)?1:-1;
      else
        read_until[GROUP]=(
          (read_until[GROUP]>=0)?notes[h-zeit][M_TIME]
            :-notes[h-zeit][M_TIME]);
    }
  }
  write("Ok. Du kannst die als ungelesen markierten Artikel "
      "mit nn nochmal lesen.\n");
  
  if (TP) TP->SetProp(P_READ_NEWS,read_until);
  return 1;
}


QueryRead() { 
  return read_until; 
}

static CatchNewsError(int err,string text4minus3) {
  switch (err) {
    case  1: return 1;
    case -1: write("Du darfst in dieser Rubrik nicht schreiben!\n"); return 0;
    case -2: write("Die Rubrik gibt es nicht mehr, sehr seltsam...\n"); return 0;
    case -3: write(text4minus3+"\n"); return 0;
    default: write("Interner Fehler "+err+", Erzmagier verstaendigen!\n"); return 0;
  }
}

static varargs schreib(str,pretext,called_by_itself,statuslines) {
  int err;

  if (str=="?"||str=="-?") {
    write("Syntax: schreib <Titel>\n"
          "  beginnt einen neuen Artikel in der Zeitung.\n");
    return 1;
  }
  
  if (!this_interactive() || !this_interactive()->query_real_name()) return 0;
  if (!called_by_itself && extern_call() && !pretext)
    str=this_interactive()->_unparsed_args()||"";
  if (called_by_itself && str=="~q") {
    write("Abgebrochen.\n");
    return 1;
  }
  if (!CatchNewsError(
                      NEWSSERVER->AskAllowedWrite(GROUP),
                      "Diese Rubrik ist leider schon randvoll!")) return 1;
  if (!called_by_itself)
    write("Neuer Artikel in Rubrik "+GROUP+":\n");
  if (!str || str=="" || str=="artikel") {
    input_to("schreib", INPUT_PROMPT, "Titel des Artikels: ", pretext,1);
    return 1;
  }
  // writer=this_interactive()->query_real_name();
  message=allocate(6);
  message[M_BOARD]=GROUP;
  message[M_TITLE]=killctrl(str);
  message[M_MESSAGE]=statuslines;
  write("Titel ist: "+str+".\n\
Gib jetzt Deinen Text ein,\n\
** oder . wenn Du fertig bist, ~q zum Abbrechen, ~h fuer eine Hilfsseite.\n");
  nedit("PostNote",pretext);
  return 1;
}

static varargs Reply(string str,string newtitle) {
  mixed dummy,replytitle,s;
  int nr;

  if ((dummy=(str||newtitle))=="?"||dummy=="-?") {
    write("Der Antworte-Befehl ist doppelt belegt.\n"
          "1. (Zeitung): Schreibe Antwort auf einen Artikel in der Zeitung.\n"
          "   Syntax: antworte\n"
          "           antworte auf artikel <nr> [neuer Titel]\n"
          "           reply [auf artikel <nr> | to note <nr>] [neuer Titel]\n"
          "2. aehnlich 'sage':\n"
          "   Du tippst zum Beispiel:\n"
          "     antworte ja, das weiss ich\n"
          "   Alle Spieler im Raum sehen dann:\n"
          "     <Dein Name> antwortet: ja, das weiss ich.\n"
          "Bitte beachte, dass jede Syntax, die auf den antworte-Befehl der "
          "Zeitung\npasst, auch von der Zeitung ausgewertet wird.\n");
    return 1;
  }

  if (str&&
      ((sscanf(lower_case(str),"auf artikel %d",dummy)==1 && 
        str=this_player()->_unparsed_args()[12..])||
       (sscanf(lower_case(str),"to note %d",dummy)==1 && 
        str=this_player()->_unparsed_args()[8..]))) {
    mixed notes;
    notes=NEWSSERVER->GetNotes(GROUP);
    if (dummy<1||dummy>sizeof(notes))
      return write("Einen Artikel mit der Nummer "+dummy+
                   " gibt es in dieser Rubrik nicht.\n"),1;
    dummy--;
    replytitle=({notes[dummy][M_TITLE],notes[dummy][M_WRITER],
                   notes[dummy][M_TIME],GetTID(notes[dummy]),GROUP});
    DEBUGVAR(str);
    if (!newtitle&&str&&sizeof(str)&&sscanf(str,"%d %s",dummy,str)==2)
      newtitle=str;
  }
  else if (!str||!sizeof(str)) {
    if (!lasttitle) return
      write("Du hast noch gar nichts gelesen, worauf Du "
          "antworten koenntest.\n"),1;
    replytitle=lasttitle;
  }
  else return 0;      

// return ComposeReply(replytitle);
//}
//
//
//ComposeReply(mixed replytitle) {

  if (!newtitle) {
    if (replytitle[0][0..7]=="Re: Re: ") newtitle="Re^3: "+replytitle[0][8..];
    else if (sscanf(replytitle[0],"Re^%d: %s",nr,newtitle))
      newtitle="Re^"+(nr+1)+": "+newtitle;
    else newtitle="Re: "+replytitle[0];
  }
  return schreib(newtitle,0,0,
                 STATUSESCAPE+" rn="+replytitle[LAST_WRITER]+
                              " rt="+replytitle[LAST_TIME]+
                              " rg="+replytitle[LAST_GROUP]+"\n"+
                 STATUSESCAPE+" tid="+replytitle[LAST_TID]+"\n");
}

static Reply2(str) {
  str = this_player()->_unparsed_args();
  if (!str||str[0..11]=="auf artikel "||str[0..7]=="to note ")
    return Reply(str);
  return Reply(0,str);
}

static void InformPlayers(string group,string player,string text)
{
  object *players;
  int i;
  mixed data;
  string ig;

  players=users();
  ig=lower_case(player)+".news";
  for (i=sizeof(players)-1;i>=0;i--) {
    data=players[i]->QueryProp(P_WAITFOR);
    if (pointerp(data)&&(member(data,"Mpa")>-1)) {
      data=players[i]->QueryProp(P_READ_NEWS);
      if (mappingp(data)&&(data[group]>0)) {
        data=players[i]->QueryProp(P_IGNORE);
        if ((!pointerp(data))||(member(data,ig)==-1))
          tell_object(players[i],text);
      }
    }
  }
}

static PostNote(text) {
  int err;
  string sig;

  if (!text) {
    write("Abbruch! Artikel landet im Reisswolf.\n");
    return 1;
  }
  if (!sizeof(old_explode(text,"\n")-
              ({"q","quit"," **","** ","ende","","exit"," "}) 
              ) )
    return write("\
ACHTUNG: Wolltest Du wirklich einen Artikel ohne Inhalt in die mpa setzen?\n\
Artikel ohne erkennbaren Inhalt werden NICHT veroeffentlicht. Bitte ueber-\n\
pruef Deine Syntax, falls Du keinen Artikel schreiben wolltest, oder schreib\n\
auch ein bisschen Text!\n\
Artikel landet im Reisswolf.\n"),1;
  if (!message[M_MESSAGE])
    message[M_MESSAGE] = text;
  else
    message[M_MESSAGE]+=text;
  if (sig=read_file("/players/"+geteuid(this_interactive())+"/.signature"))
    message[M_MESSAGE]+=sig;
  if (!CatchNewsError(NEWSSERVER->WriteNote(message),
       "Diese Rubrik ist voll. Mist, da war jemand schneller...")) {
    write("Versuch, Platz in der Rubrik zu finden, dann kannst Du mir ~r nochmal\nin den Text einsteigen und ihn anschliessend veroeffentlichen.\n(Vorschlag: Einen veralteten Artikel abhaengen oder verlegen.\n");
    return 0;
  }
  write("OK, Artikel ist veroeffentlicht.\n");
  say(capitalize(TP->name())+
      " hat einen Artikel in der Zeitung veroeffentlicht.\n");
  if (geteuid(TP)!="sitopanaki")
    InformPlayers(message[M_BOARD],geteuid(this_interactive()),
                "* MPA: Neuer Artikel von "+
                capitalize(geteuid(this_interactive()))+
                " in Rubrik \""+message[M_BOARD]+"\".\n");
  message=0; /* Platz sparen! */
  return 1;
}


//static // allowing aliasing *Rumata* 5/8/96
inhalt(str) {
  int i,endflag,timeout;
  string *gruppen,s,txt,suche;
  mixed messages;

  if (str=="?"||str=="-?") return
    write("Syntax: inhalt [<rubrik>] [ende] [suche <text>]\n"),1;

  str=(!str)?({}):explode(str," ");

  if (sizeof(str) && (str[0]=="neu" || str[0]=="neues") )
    return ReadNextUnread("liste"),1;

  if (endflag=member(str,"ende")+1) str-=({"ende"});
  if (((i=member(str,"suche")) != -1) && (sizeof(str) > i)) {
    suche=lower_case(implode(str[i+1..]," "));
    str=str[0..i-1];
  }
  else
    suche=0;
  if (!sizeof(str)) {
    str=GROUP;
    if (!pointerp(messages=NEWSSERVER->GetNotes(str))){
      str=GROUP=DEFAULTGROUP;
    if (!pointerp(messages=NEWSSERVER->GetNotes(str)))
      return notify_fail("Seltsam, irgendwie geht hier einiges schief...\n"),0;
    }
  }
  else {
    str=GetGroupName(str[0]);
    if (!str) return 1;
    messages=NEWSSERVER->GetNotes(str);
  }
  timeout=abs(read_until[str]);
  s="Inhalt der Rubrik "+str+":\n\n";
  if (!pointerp(messages) || !sizeof(messages))
    return 
      write(s+"Zur Zeit befinden sich keine Artikel in dieser Rubrik.\n"),1;
  if (suche)
    s+="Suche nach '"+suche+"' in der Rubrik ergab folgende Treffer:\n\n";
  else
    if (sizeof(messages)==1)
      s+="Zur Zeit befindet sich ein Artikel in der Rubrik:\n\n";
    else
      s+="Zur Zeit befinden sich "+sizeof(messages)+
        " Artikel in der Rubrik:\n\n";
  if (endflag&&(sizeof(messages)>16)&&
      messages[sizeof(messages)-16][M_TIME]>=timeout) timeout=-1;
  for (i=(endflag?(((endflag=sizeof(messages)-15)<0)?0:endflag):0);
       i<sizeof(messages);i++)
  {
    txt=sprintf("%2d.%s%-48s%4d (%-11s) %s\n",i+1,
                (((timeout>=0) && timeout<messages[i][M_TIME] )?
                 ( (timeout=-1),"*"):" "),messages[i][M_TITLE],
                sizeof(explode(messages[i][M_MESSAGE],"\n")),
                messages[i][M_WRITER],
                dtime(messages[i][M_TIME])[5..11]);
    if (!suche || (strstr(lower_case(txt), suche) != -1))
      s+=txt;
  }
  if (endflag) write(s);
  else this_player()->More(s);
  return 1;
}


static artikel(str) {
  return lies(str);
}


static loesche(str) {
  int num;
  mixed *messages;

  if (str=="?"||str=="-?") return 
    write("Syntax: loesche artikel <nr>\n"
          "  (bezieht sich immer auf die aktuelle Rubrik.\n"),1;

  if (!str || sscanf(str,"artikel %d",num)!=1 || num<=0) 
   return notify_fail("WELCHEN Artikel willst Du loeschen ?\n"),0;
  num--;
  messages=(NEWSSERVER->GetNotes(GROUP));
  if (sizeof(messages)<=num) return
    notify_fail("So viele Artikel sind da nicht!\n"),0;
  
  write("Rubrik "+GROUP+", Artikel "+(num+1)+
        " von "+capitalize(messages[num][M_WRITER])+
        " vom "+dtime(messages[num][M_TIME])[5..26]+
        ",\nTitel: "+messages[num][M_TITLE]+",\n\n");

  /* (ueberfluessige Abfrage, macht schon /secure/news)
  if (!IS_LEARNER(TI) && lower_case(messages[num][M_WRITER])!=geteuid(TI)) 
    return 
      write("Nicht geloescht - du darfst nur eigene Artikel loeschen.\n"),1;
          */

  switch (NEWSSERVER->RemoveNote(GROUP, num)){
  case 1: write("Artikel ist geloescht.\n");
    say(this_player()->name()+" loescht einen Artikel aus der Zeitung.\n");
    return 1;
  case -1: write("Diesen Artikel darfst Du nicht loeschen.\n");
    say(this_player()->name()+" versucht vergeblich, einen Artikel zu loeschen.\n");
    return 1;
  case -3: write("So viele Artikel sind da nicht !\n");
    return 1;
  default: write("Interner Fehler. Bitte Erzmagier verstaendigen !\n");
    return 1;
  }  
}

// Low-level Funktion zum Abonnieren/Abbestellen von Rubriken
// bestellen==0 -> abbestellen, bestellen!=0 -> bestellen
// Rueckgabe: 0, wenn der gewuenschte Zustand schon eingestellt war, sonst 1
private int _subscribe(string groupname, int bestellen) {

  int timeout = read_until[groupname];
  // gar nicht abonniert/abbestellt?
  if (!bestellen && timeout < 0)
    return 0;
  else if (bestellen && timeout > 0)
    return 0;

  // wenn noch kein timeout, erstmal auf 1 setzen
  timeout ||= 1;

  // -1 fuer abbestellen, +1 fuer bestellen
  if (bestellen)
    read_until[groupname] = abs(timeout);
  else
    read_until[groupname] = -timeout;

  if (environment())
    environment()->SetProp(P_READ_NEWS, read_until);

  return 1;
}

static Unsubscribe(str) {
  int timeout;
  if (str=="?"||str=="-?") return
    write("Syntax: unsubscribe <rubrik>"
          "  oder: bestelle <rubrik> ab\n"),1;
  str=GetGroupName(str);
  if (!str) return 1;
  if (!_subscribe(str,0)) {
    notify_fail("Die Rubrik hast Du gar nicht abonniert!\n");
    return 0;
  }
  else {
    write("Rubrik "+str+" abbestellt.\n");
  }
  return 1;
}


static Bestelle(str) { /* ab ! */
  if (!str || !sscanf(str,"%s ab",str)) return _notify_fail(
       "Die Syntax ist: 'bestelle <rubrik> ab', "
       "oder meinst Du 'abonniere'?\n"),0;
  return Unsubscribe(str);
}

static Subscribe(str) {
  int timeout;
  if (str=="?"||str=="-?") return
    write("Syntax: abonniere <rubrik>\n"
          "  oder: subscribe <rubrik>\n"),1;
  str=GetGroupName(str);
  if (!str) return 1;
  if (!_subscribe(str,1)) {
    notify_fail("Die Rubrik hast Du doch schon abonniert!\n");
    return 0;
  }
  else {
    write("Rubrik "+str+" abonniert.\n");
  }
  return 1;
}

// Legt die anfaenglichen Abonnements eines Neulesers fest und bestellt alle
// anderen ab.
private void InitialSubscriptions() {
  // alle abbestellen. ;-)
  // Alternative: fuer alle _subscribe(,0) rufen
  if (!query_once_interactive(environment()))
    return;
  string *gruppen = NEWSSERVER->GetGroups();
  int *vals = allocate(sizeof(gruppen),-1);
  read_until = mkmapping(gruppen,vals);

  // jetzt die vorausgewaehlten bestellen
  foreach(string gruppe : INITIAL_SUBSCRIPTIONS) {
    if (member(gruppen, gruppe) > -1)
      _subscribe(gruppe,1);
  }
  // und ggf. noch die eigene Gildenrubrik
  string gilde = environment()->QueryProp(P_GUILD);
  if (stringp(gilde)
      && member(gruppen, "gilden."+gilde) > -1)
    _subscribe("gilden."+gilde,1);

  environment()->SetProp(P_READ_NEWS, read_until);
}

static MoveMessage(str) {
  int num,i;
  mixed msg/*,expl*/;
  string gr;
  if (str=="?"||str=="-?") return
    write("Syntax: verlege artikel <nr> nach <rubrik>\n"
          "  Artikel und Rubrik muessen explizit angegeben werden.\n"),1;
  if (!str || sscanf(str,"artikel %d nach %s",num,gr)!=2) return (int)notify_fail(
        "verlege artikel <nr> nach <rubrik>, oder was?\n");
  if (!(gr=GetGroupName(gr))) return 1;
  if (!(CatchNewsError(NEWSSERVER->AskAllowedWrite(gr),"Die Rubrik ist leider voll.\n"))) return 1;

  if (!pointerp(msg=NEWSSERVER->GetNotes(GROUP)))
    return notify_fail("Seltsam, die Rubrik '"+GROUP+
        "' gibt es nicht mehr...\n"), 0;

  num--;
  if (sizeof(msg)<=num) return
    notify_fail("So viele Nachrichten sind da nicht !\n"),0;
  msg=msg[num];
  if (geteuid(TI) != lower_case(msg[M_WRITER])) {    
    if (!IS_LEARNER(TI)) return 
      write("Du darfst nur Deine eigenen Artikel verlegen.\n"),1;
    write("WARNUNG: Das ist nicht Dein eigener Artikel!\n");
  }
    //  return (int)notify_fail("Man darf zur Zeit nur eigene Artikel verlegen.\n");
  if (!CatchNewsError(NEWSSERVER->RemoveNote(GROUP,num),"Dieser Fehler kann eigentlich nicht auftreten"))
    write("Warnung! Konnte Artikel an alter Position nicht loeschen.\n");
  else write("Artikel von Rubrik "+GROUP+" entfernt.\n");

  msg[M_MESSAGE]=
    sprintf("%s on=%s ot=%d og=%s\n",
            STATUSESCAPE,msg[M_WRITER],msg[M_TIME],msg[M_BOARD])
      +msg[M_MESSAGE];

/*
  expl=explode(msg[M_MESSAGE],"\n");
  for (i=0;(IS_STATUSLINE(expl[i][0..2]));i++);
  msg[M_MESSAGE]=( (i) ? implode(expl[0..i-1],"\n")+"\n" : "" ) +
    "[Verlegt von "+msg[M_BOARD]+", "+msg[M_WRITER]+", "+
    dtime(msg[M_TIME])[5..26]+"]:\n"+
      implode(expl[i..],"\n")+"\n";
*/

  msg[M_BOARD]=gr;
  NEWSSERVER->WriteNote(msg);
  write("Artikel nach Rubrik "+gr+" verlegt.\n");
  return 1;
}

static MoveTrash()
{
  if (!pointerp(lasttitle)||sizeof(lasttitle)<LAST_SIZEOF) return
    write("Was denn bitte? Du hast noch gar nichts gelesen!\n"),1;
  if (lasttitle[LAST_GROUP]!=GROUP) return
    write("Nix gibts! Du hast die Rubrik gewechselt!\n"),1;
  return MoveMessage(sprintf("artikel %d nach muell",lasttitle[LAST_NR]+1));
}

static SaveMessage(str) {
  mixed num;
  mixed *messages;

  if (intp(str)) num=str;
  if ((!num && (!str || str=="" || sscanf(str,"artikel %d",num)!=1)) || num<=0) {
    notify_fail("Welchen Artikel willst Du abspeichern?\n");
    return 0;
  }
  if (!pointerp(messages=NEWSSERVER->GetNotes(GROUP)))
    return notify_fail("Seltsam, die Rubrik '"+GROUP+
        "' gibt es nicht mehr...\n"), 0;
  num--;
  if (sizeof(messages)<=num) {
    notify_fail("So viele Nachrichten sind da nicht !\n");
    return 0;
  }
  if(write_file(SAVEMSGPATH(TP),Message2string(num,messages,M2S_REMOTE)+"\n"))
    write(break_string(
      "Inhalt des Artikels wurde nach "+SAVEMSGPATH(TP)+" gespeichert.\n"));
  else
    write(break_string(
      "Fehler beim Schreiben nach "+SAVEMSGPATH(TP)+"!\n"));
  return 1;
}


static MailMessage(str) {
  mixed num,rec,group;
  mixed *messages;

//  printf("%O\n",inherit_list(environment(TP)));
//  if (member(query_actions(this_player()),"mail")<0)
//     tut nicht wegen anderer Implemtierung von AddCmd in Raeumen
  
  if (str=="?"||str=="-?") return
    write("Syntax: versende artikel <nr> an <adresse>\n"),1;

  str=TP->_unparsed_args();     // wegen Mailadressen
  if (str) str=lower_case(str);

  if (!IS_SEER(TP) && !IS_POST(environment(TP)))
    return notify_fail("Du musst in ein Postamt gehen, "
        "um etwas versenden zu koennen.\n"),0;
  
  num=0;
  
  if (!str || (sscanf(str,"artikel %d an %s",num,rec)!=2 &&
               sscanf(str,"note %d to %s",num,rec)!=2 &&
               sscanf(str,"note to %s",rec)!=1 &&
               sscanf(str,"artikel an %s",rec)!=1)){
     if (!str || str[0..6]=="artikel"||str[0..3]=="note") return 
      write("Welchen Artikel willst Du versenden, und wohin?\n"),1;
    else return 
      notify_fail("Welchen Artikel willst Du versenden, und wohin?\n"),0;
  }
  if (!num&&(!pointerp(lasttitle)||sizeof(lasttitle)<LAST_SIZEOF))
    return write("Du hast scheinbar noch nichts gelesen, worauf man sich "
        "beziehen kann.\nGib notfalls die Nummer des Artikels an.\n"),1;

  //  printf("lasttitle= %O\nnum=%d\n",lasttitle,num);

  if (!pointerp(messages=
      NEWSSERVER->GetNotes(group=(num?GROUP:lasttitle[LAST_GROUP]))))
    return notify_fail("Seltsam, die Rubrik gibt es nicht mehr...\n"), 0;

  if (!pointerp(messages)||!sizeof(messages))
    return write("Die Rubrik "+group+" ist leer.\n"),1;
  if (num<0||sizeof(messages)<num) return
    write("Einen Artikel mit Nummer "+num+" gibt es in Rubrik "+group+
        " nicht!\n"),1;

  if (num) num--;
  else {
    int h;
    num=-1;
    if (pointerp(messages)) {
      for (h=sizeof(messages)-1;
           (h>=0 && messages[h][M_TIME]>=lasttitle[LAST_TIME]);h--) 
        if (messages[h][M_TIME]==lasttitle[LAST_TIME] && 
            lower_case(messages[h][M_WRITER])==
            lower_case(lasttitle[LAST_WRITER]))
          num=h;
    }
    if (num<0)
      return notify_fail("Konnte Artikel nicht wiederfinden, "
          "bitte gib die Nummer an.\n"),0;
  }
  MAILER->do_mail( rec,
      "MPA: "+messages[num][M_TITLE]+" ("+messages[num][M_WRITER]+")",
      Message2string(num,messages,M2S_REMOTE,group)+"\n");
  return 1;
}



HelpPage(str) {
  if (str!="mpa"&&str!="zeitung") return 0;
  this_player()->More(read_file(HELPPAGE)+
      (IS_LEARNER(TP) ? read_file(WIZHELPPAGE) : ""));
  return 1;
}

/*--------------------------------------------------------------------------*/

protected void NotifyMove(object dest, object oldenv, int method) {
  ::NotifyMove(dest, oldenv, method);

  // P_READ_NEWS aus dem Spieler holen.
  if (objectp(environment()) && query_once_interactive(environment())) {
    read_until=environment()->QueryProp(P_READ_NEWS);
  }

  if (!mappingp(read_until) || !sizeof(read_until))
    InitialSubscriptions();
}

int GetTID(mixed message) {
  string dummy;
  int tid;
  return (sscanf(message[M_MESSAGE],"%s" STATUSESCAPE " tid=%d",dummy,tid)==2) 
    ? tid : message[M_TIME];
}

