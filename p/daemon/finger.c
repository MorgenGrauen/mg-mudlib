/* a finger
 * Original (c) 14.03.1993 by Taube @Nightfall
 * Umsetzung fuer Morgengrauen 09.08.1993 by Loco

   Verwendung ausserhalb von Morgengrauen ist gestattet unter folgenden
   Bedingungen:
   - Benutzung erfolgt auf eigene Gefahr. Jegliche Verantwortung wird
     abgelehnt.
   - Auch in veraenderten oder abgeleiteten Objekten muss ein Hinweis auf
     die Herkunft erhalten bleiben.
   Ein Update-Service besteht nicht.

 * 29.Okt 1993 Seherversion.
 * spaeter auch fuer externen Aufruf verwendbar gemacht
 * 13.Okt .plan und .project koennen auch in ~loco/plans sein.
 * 15.Jan 1994 angepasst an neues Speicherformat
 * 02-05.Dez94 -n, -p
 *
 * Gelegentlich minor changes, zuletzt 04.Okt.95
 */

#pragma strong_types,save_types
#pragma no_clone, no_shadow

#include <config.h>
#include <properties.h>
#include <wizlevels.h>
#include <new_skills.h>
#include <userinfo.h>
#include <config.h>

#define HBINT    2   /* interval between two heart_beats in seconds */
#define MINIDLE 60   /* minimum idle time in seconds to be stated idle */
#define TBANISH_EXTRACT 71..  /* Der benoetigte Teil des TBanish-strings */

#define TP this_player()
#define wiz (local && IS_LEARNER(TP))
#define seer (local && IS_SEER(TP))
#define IN ((properties[P_GENDER]==2)?"in":"")


#define FLAG_NOPLAN 1
#define FLAG_LONG 2
#define FLAG_SPONSOR 4
#define FLAG_VIS_LOGOUT 8
#define FLAG_AVATAR 16


mapping properties;
int age,invis,hc_play;
int filetime;
mapping relatives;


void create()
{
  seteuid(getuid());
  filetime=0;
}


string timediff(int time)
{
  string ret;

  ret="";
  if ( time >= 86400*365 ) {
    ret+=time/(86400*365)+"a ";
    time%=(86400*365);
  }
  if(time>=86400) {
    ret+=time/86400+"d ";
    time%=86400;
  }
  if(time<36000) ret+="0";
  ret+=time/3600+":";
  time%=3600;
  if(time<600) ret+="0";
  ret+=time/60+":";
  time%=60;
  if(time<10) ret+="0";
  ret+=time+"";
  return ret;
}

string sponsoring(string name)
{
  int i,w;
  string *s,s2,s3,s4;
  // Daten einlesen, wenn die daten aelter als 1 Tag sind oder sich
  // /log/SPONSORS geaendert hat.
  if ((time() > filetime+86400) ||
      filetime!=file_time("/log/SPONSOR")) 
  {
      relatives=m_allocate(0,2);
      filetime=file_time("/log/SPONSOR");
      s=explode(read_file("/log/SPONSOR"),"\n");
      foreach(string str: s) { 
	  sscanf(str,"%s: %s macht %s zum Learner.",s2,s3,s4);
	  if (IS_LEARNER(lower_case(s3)) && IS_LEARNER(lower_case(s4)))
          {
             relatives[lower_case(s4),0]=s3;
             s3=lower_case(s3);
             s4+=" ("+query_wiz_level(lower_case(s4))+")";
             if (!relatives[s3,1]) relatives[s3,1]=({s4});
             else relatives[s3,1]+=({s4});
          }
      }  
  }
  s2="";
  if (relatives[name,0])
    s2+="Vorfahre:     "+relatives[name,0]+"\n";
  if (relatives[name,1])
    s2+="Nachfahre(n): "+break_string(implode(relatives[name,1],", "),78,14)[14..];
  return s2;
}

varargs string finger_single(string str,int local)
{
  string ip,text,ipnum,filename,away;
  int wizlevel,playerlevel,idle,pos,flags,last;
  mixed h,data,tmp;
  object player,ob;

  /*DEBUG###  tell_object((find_player("loco")||this_object()),"Finger request: '"+str+"'("+local+")\n");/**/
  str=lower_case(str);
  text="";
  away="";
  hc_play=0;
  
  h=explode(str," ");
  if (sizeof(h)==1) h=explode(str,"+");
  if (member(h,"-n")>=0) flags=FLAG_NOPLAN;
  if (member(h,"-p")>=0) flags=0;
  if (member(h,"-l")>=0) flags|=FLAG_LONG;
  if (member(h,"-s")>=0) flags|=FLAG_SPONSOR;
  if (member(h,"-v")>=0) flags|=FLAG_VIS_LOGOUT;
  if (member(h,"-a")>=0) flags|=FLAG_AVATAR;

  h=(h-({"-n","-p","-l","-s", "-v","-a"}));
  if (!sizeof(h)) {
    text="Du solltest schon sagen, wer Dich interessiert!\n";
    if (local) return write(text),0;
    else return text;
  }
  str=h[0];
  if (!sizeof(str) || str[0]<'a' || str[0]>'z') {
    text="Also, so ("+str+") heisst bestimmt kein Spieler hier!\n";
    if (local) return write(text),0;
    else return text;
  }

  /* does this player exist? */
  str=old_explode(str,".")[0];
  player=find_player(str)||find_netdead(str);

  if( (!master()->find_userinfo(str) && !player) )
  {
    text="Hmm... diesen Namen gibt es im "MUDNAME" nicht.\n";
    if (tmp="/secure/master"->QueryBanished(str)){
      text="Hoppla - dieser Name ist reserviert oder gesperrt (\"gebanisht\")!\n";
      if ( tmp != "Dieser Name ist gesperrt." )
          text += "Grund fuer die Sperrung: " + tmp +
              (member( ({'!','?','.'}), tmp[<1] ) != -1 ? "" : ".") + "\n";
    }
    if (local) return write(text),0;
    else return text;
  }

  if (player) {
    hc_play=player->query_hc_play();
    properties=player->QueryProperties();
    properties[P_RACE]=player->QueryProp(P_RACE);
    properties[P_VISIBLE_GUILD]=player->QueryProp(P_VISIBLE_GUILD);
    properties[P_TITLE]=player->QueryProp(P_TITLE);
    tmp = player->QueryProp(P_PRESAY);
    properties[P_PRESAY]=(stringp(tmp) && sizeof(tmp)>1) ? tmp[0..<2] : 0;
  }
  else
    restore_object("/save/"+str[0..0]+"/"+str);
  if (!properties)
  {
    text+="Mist!!! Das Einlesen der Daten klappt nicht wie es soll :-(\n";
    properties=0;
    if (!local)
      return text;
    write(text);
    return "";
  }
  if ( player && interactive(player) )
      ipnum = query_ip_number(player);
  else
      ipnum = properties[P_CALLED_FROM_IP];
  // frueher stand in P_CALLED_FROM_IP evtl. auch der Name direkt drin
  // anstelle der numerischen IP
  ip=query_ip_name(ipnum)||properties[P_CALLED_FROM_IP];
  if(player) {
      if (!interactive(player) || (idle=query_idle(player))<MINIDLE)
          idle=0;
      if (!(invis=age=player->QueryProp(P_INVIS)))
          age=player->QueryProp(P_AGE);
  } else {
    if (properties[P_INVIS]) age=properties[P_INVIS];
    idle=properties[P_LAST_LOGOUT];
  }

   wizlevel=query_wiz_level(str);
   if ( (tmp = file_time("/save/"+str[0..0]+"/"+str+".o")) <= 0 )
       // Hack, um bei ganz "frischen" Spielern (noch kein Savefile vorhanden)
       // die Ausgabe von 1.1.1970 zu verhindern
       tmp = time();
   last=properties[P_LAST_LOGOUT];
   if ( last <= 0 || (!(flags&FLAG_VIS_LOGOUT) && wiz && wizlevel > 10
                  && properties[P_INVIS] && tmp - last > 300) )
       last = tmp;

  /* output routine for all */
  if(player) {
    h=player->QueryProp(P_RACE);
    if(interactive(player) && (wiz || !invis)) {
      text+=capitalize(str)+" ist anwesend,\n"+
        "und zwar von: "+
          (wiz ? (ip+(ipnum?" ("+ipnum+")":"")):"")+
          (stringp(properties[P_LOCATION]) ? (wiz ? "\n              [" : "")
           +capitalize(properties[P_LOCATION])+
           ((properties[P_LOCATION] != country(ip, ipnum))
            ? " (ueber "+country(ip, ipnum)+")" : "" )
           : (wiz?" [":"")+country(ip, ipnum))+(wiz ? "]":"")+".\n";
      if(idle)
        text+="Passiv seit: "+timediff(idle)+"\n";
     if (local)
      text+="Eingeloggt seit: "+dtime(properties[P_LAST_LOGIN])+"\n";
     if (properties[P_AWAY])
       away="z.Z. abwesend, Grund  : "+properties[P_AWAY]+"\n";
    } else
      text+=capitalize(str)+" ist nicht anwesend.\nZuletzt eingeloggt von: "+
        (wiz ? (ip+(ipnum&&ipnum!=ip?" ("+ipnum+")":"")):"")+
        (stringp(properties[P_LOCATION]) ?
         (wiz ? "\n                        [": "")
         +capitalize(properties[P_LOCATION])+" (ueber "+country(ip, ipnum)+")":
         (wiz?" [":"")+country(ip, ipnum))+(wiz ? "]":"")+".\n"+
        "Zuletzt ausgeloggt: "+dtime(last)+" ("+timediff(time()-last)+").\n";
  }
  else {
    text+=capitalize(str)+" ist nicht anwesend.\nZuletzt eingeloggt von: "+
        (wiz ? (ip+(ipnum&&ipnum!=ip?" ("+ipnum+")":"")):"")+
        (stringp(properties[P_LOCATION]) ?
         (wiz ? "\n                        [": "")
         +capitalize(properties[P_LOCATION])+" (ueber "+country(ip, ipnum)+")":
         (wiz?" [":"")+country(ip, ipnum))+(wiz ? "]":"")+".\n"+
        "Zuletzt ausgeloggt: "+dtime(last)+" ("+timediff(time()-last)+").\n";
  }
  text+="Voller Name: "+(((h=properties[P_PRESAY]) && h!="") ? h+" " : "")+
    capitalize((h=properties[P_NAME]) ? h : str)+" "+
     ((h=properties[P_TITLE]) ? h :
    ((mappingp(h=properties["guild_title"]) && (h=(h[properties[P_GUILD]?properties[P_GUILD]:"abenteurer"])) ) ? h : "") )
       +"\n";

  if (properties[P_GHOST]) text+="Hoppla, ein Geist!\n";
  if ((flags&FLAG_LONG)&&properties[P_LONG])
    text+="Beschreibung: \n"+break_string(properties[P_LONG],78,2);

  if(wiz ||
     (properties[P_SHOWEMAIL]=="alle") ||
     ( (properties[P_SHOWEMAIL]=="freunde") &&
       objectp(player) &&
       this_player() &&
       (ob=present("\n\bfband",player)) &&
       pointerp(tmp=ob->QueryProp(P_AUTOLOADOBJ)) &&
       pointerp(tmp=tmp[1]) &&
       (member(tmp,getuid(this_player()))!=-1))) tmp = 1;
  else tmp=0;

  string shell=properties[P_RACE];
  if (!stringp(shell))
  {
    shell = master()->query_userlist(str, USER_OBJECT);
    shell = capitalize(explode(shell, "/")[3]);
    shell =(["Human":"Mensch","Dwarf":"Zwerg","Darkelf":"Dunkelelf",
             "Orc":"Ork"])[shell] || shell;
  }

  if (!stringp(shell)) shell="<keine>";
  int creation = master()->query_userlist(str, USER_CREATION_DATE);
  text+="Rasse: "+shell+",  Gilde: "+
                     ((h=properties[P_VISIBLE_GUILD])?capitalize(h):((h=properties[P_GUILD])?capitalize(h):"Abenteurer"))+
                     ((h=properties[P_VISIBLE_SUBGUILD_TITLE])?" ("+capitalize(h)+")":((h=properties[P_SUBGUILD_TITLE])?" ("+capitalize(h)+")":""))+
                     ",  Geschlecht: "+({"neutral ?!","maennlich","weiblich","<verdammt seltsam>"})[properties[P_GENDER]]+"\n"+
                      (seer ? "Alter: "+timediff(age*HBINT)+",   " : "")+
                      (wizlevel>=10?"Magierlevel: "+wizlevel+
          (wizlevel>=GOD_LVL?" (Mudgott)":str=="boing"?" (Mudgott a.D.)":str=="muadib"?" (Apostolischer Visitator)":wizlevel>=ARCH_LVL?" (Erzmagier)":IS_DEPUTY(str)?" (Hilfssheriff)":wizlevel>=ELDER_LVL?" (Weiser)":wizlevel>=LORD_LVL?" (Regionsmagier)":wizlevel>=SPECIAL_LVL?" (Hilfsmagier)":wizlevel>=DOMAINMEMBER_LVL?" (Regionsmitarbeiter)":wizlevel>WIZARD_LVL?" (Vollmagier)":" (Lehrling)"):
                       ("Spielerlevel: "+properties[P_LEVEL]+( wizlevel ? " (Seher"+IN+")" : "" )+
                        (((h=properties[P_GUILD_LEVEL]) && h=h[properties[P_GUILD]]) ?
                         (",   Gildenlevel: "+h) : "" )
                        )) + ((sprintf("\nDatum des ersten Login: %s",
                                 (creation > 0) ? dtime(creation) : "vor dem 10. Nov 1995")))+
                         (tmp ? ("\nE-Mail-Adresse: "+((h=properties[P_MAILADDR]) ? h : "keine")+"\n") : "\n");

  if (properties[P_HOMEPAGE])
    text+="Homepage: "+properties[P_HOMEPAGE]+"\n";

  if (stringp(data=properties[P_MESSENGER])) {
    text+=sprintf("Messenger: %s", data);
    if (intp(data=properties[P_ICQ])) {
      if (data<0 && IS_WIZARD(this_player())) data*=-1;
      if (data>0) text += sprintf(", ICQ: %d", data);
    }
    text+="\n";
  } else 
    if (intp(data=properties[P_ICQ])) {
      if (data<0 && IS_WIZARD(this_player()))
        data*=-1;
      if (data>0)
        text+=sprintf("ICQ: %d\n",data);
    }

  if (properties[P_MARRIED])
    text+="Verheiratet mit: "+capitalize(properties[P_MARRIED])+"\n";

  if ( pointerp(properties[P_SIBLINGS]) )
      text += ({ "Es", "Er", "Sie" })[properties[P_GENDER]] + " ist Bluts" +
          ({ "verwandt mit ", "bruder von ", "schwester von " })
          [properties[P_GENDER]] +
          CountUp(properties[P_SIBLINGS]) + ".\n";

  text+=away;
  
  if(MASTER->check_late_player(str))
  {
    text+=capitalize(str)+" hat uns leider fuer immer verlassen.\n";
  }
  else
  {
    if (h=MASTER->QueryTBanished(str))
      text+=capitalize(str)+" will fruehestens "+h[TBANISH_EXTRACT];
  }
  
  if (h=properties[P_TESTPLAYER])
  {
    text+=capitalize(str)+" ist Testspieler"+IN;
    if (stringp(h)) text+=" ("+h+")";
    text+=".\n";
  }
  if ( h=properties[P_SECOND])
  {
    if (IS_WIZARD(this_player())) {
       text+=capitalize(str)+" ist";
       switch((int)properties[P_SECOND_MARK]) {
         case -1: text+=" unsichtbar markierte"
                       +((int)properties[P_GENDER]!=FEMALE ? "r": "");
                  break;
         case  0: text+=" nicht namentlich markierte"
                       +((int)properties[P_GENDER]!=FEMALE ? "r": "");
                  break;
         default:
       }
       text+=" Zweitspieler"+IN;
       if (stringp(h))
         text+=" ("+capitalize(h)+")";
       text+=".\n";
    }
    else if ((int)properties[P_SECOND_MARK]>0)
    {
      text+=capitalize(str)+" ist Zweitspieler"+IN;
      if (stringp(h))
        text+=" ("+capitalize(h)+")";
      text+=".\n";
    }
    else if ((int)properties[P_SECOND_MARK]>-1)
      text+=capitalize(str)+" ist Zweitspieler"+IN+".\n";
  }
  if (properties[P_DEADS])
  {
    text+="Bisher bereits "+properties[P_DEADS]+" mal gestorben\n";
    // Bezieht sich nur auf den Zeitraum ab dem 30.11.'94
  }
  if(hc_play==1)
  {
    text+=capitalize(str)+" ist sehr mutig.\n";
  }
  if(hc_play>1)
  {
    text+=capitalize(str)+" ist am "+dtime(hc_play)+" in das Nirvana eingegangen.\n";
  }

  data=master()->query_userlist(str, USER_DOMAIN);
  if (sizeof(data))
    text+="Regionsmagier"+IN+" von     : "
          +implode(map(data,#'capitalize),", ")+".\n";
  data="/secure/master"->get_domain_homes(str);
  data=filter(data-({"erzmagier"}),#'stringp);
  if ((wizlevel>=DOMAINMEMBER_LVL) && (sizeof(data)))
    text+="Regionsmitarbeiter"+IN+" von: "
          +implode(map(data,#'capitalize),", ")+".\n";

    data=master()->query_userlist(str, USER_GUILD);
    if (sizeof(data))
      text += "Gildenmagier"+IN+" von      : "
              +implode(map(data, #'capitalize), ", ")+".\n";

  // ggf. Avatar-URI mit ausgeben.
  if (flags & FLAG_AVATAR
      && stringp(properties[P_AVATAR_URI]))
    text += "Avatar-URI: " + properties[P_AVATAR_URI] + "\n";

  if (flags & FLAG_SPONSOR)
    text+=sponsoring(str);

  filename="/players/"+str+"/.project";
  if(file_size(filename)>=0)
    text+="Projekt: "+explode(read_file(filename), "\n")[0]+"\n";
  else {
    filename="/p/service/loco/plans/"+str+".project";
    if(file_size(filename)>=0)
      text+="Projekt: "+explode(read_file(filename), "\n")[0]+"\n";
  }
  if (seer && !(flags&FLAG_NOPLAN)) {
    filename="/players/"+str+"/.plan";
    if(file_size(filename)>=0)
      text+="Plan:\n"+read_file(filename);
    else {
      filename="/p/service/loco/plans/"+str+".plan";
      if(file_size(filename)>=0)
        text+="Plan:\n"+read_file(filename);
//      else
//      text+="Keine .plan-Datei.\n";
    }
  }
  properties=0;
  text=break_string(text,78,0,BS_LEAVE_MY_LFS);
  if (!local)
    return text;
  this_player()->More(text);
  return "";
}

string Finger(string str)
{
  if(!str || str=="") { notify_fail("Wen denn?\n"); return 0; }
  else
    return finger_single(str,1);
}
