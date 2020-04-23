// MorgenGrauen MUDlib
//
// merlin.c -- Unser geliebter Merlin
//
// $Id: merlin.c 9405 2015-12-13 00:22:01Z Zesstra $
#pragma strict_types
#pragma no_clone
#pragma no_shadow
#pragma no_inherit
#pragma verbose_errors
#pragma combine_strings
//#pragma pedantic
//#pragma range_check
#pragma warn_deprecated

// 2014-Mai-10, Arathorn: Er soll ja weiterhin auf tm reagieren.
inherit "/std/npc/comm.c";

#include <config.h>
#include <properties.h>
#include <language.h>
#include <moving.h>
#include <defines.h>
#include <exploration.h>
#include <news.h>
#include <exploration.h>
#include <events.h>
#include <daemon/channel.h>
#include "/secure/wizlevels.h"
#include "/secure/config.h"
#include "/secure/questmaster.h"
#include "/secure/lepmaster.h"
#include <userinfo.h>

#ifndef DEBUG
#define DEBUG(x) if (find_player("zook")) tell_object(find_player("zook"),x)
#endif

#define SAVEFILE "/secure/ARCH/merlin"
#define BS(x) break_string((x),78)
#define FEHLERTEUFEL "/obj/tools/fehlerteufel"

mixed QueryProp(string prop);
int move(mixed dest, int method);
string query_g_suffix(int gen, int casus, int anzahl);
string make_invlist(object viewer,object *inv);
static int determine_action(string mess, string name);
static int create_wizard(mixed who, mixed promoter);
static int create_seer(object who);
static void give_help(mixed who);
int goto(mixed dest);
public string wiztree_html();

nosave string *whitespaces=({",",".","?",";",":","-","!","\n","\t"});
nosave string prev_room;
nosave int delay,maxusers,busy;
nosave string flag;

mapping MBanishListe;

void create()
{
  string s;
  seteuid(ROOTID);
  if (file_size("/etc/maxusers")<=0)
    maxusers=0;
  else
  {
    s=read_file("/etc/maxusers",0,1);
    sscanf(s,"%d",maxusers);
  }
  set_living_name("merlin");
  enable_commands();
  call_out("wandern",25);
  move("/gilden/abenteurer",0);
  MBanishListe = m_allocate(0,2);
  restore_object(SAVEFILE);
  ({int})EVENTD->RegisterEvent(EVT_LIB_PLAYER_CREATION, "player_change", ME);
  ({int})EVENTD->RegisterEvent(EVT_LIB_LOGIN, "player_change", ME);
  // absichtlich kein EVT_LIB_LOGOUT, s. Kommentar zu notify_player_leave().
}

string _query_kill_name()
{
  return "Ein genervter Merlin";
}

int move(mixed ob,int methods)
{
  object tmp;

  if (methods&M_GET)
    return ME_CANT_BE_TAKEN;

  if (stringp(ob))
  {
    if (!(tmp=find_object(ob)))
    {
      call_other(ob,"?");
      tmp=find_object(ob);
    }
    ob=tmp;
  }
  if (!objectp(ob))
    return 0;
  if (environment())
    tell_room(environment(),"Merlin zieht weiter.\n");
  tell_room(ob,"Merlin kommt an.\n");
  move_object(ob);
  return 1;
}

mixed QueryProp(string str)
{
  if (!stringp(str)||str=="")
    return 0;
  return ({mixed})call_other(this_object(),"_query_"+str);
}

varargs int id(string str)
{
  return (str=="merlin");
}

varargs string name(int casus, int demon)
{
  if (casus!=WESSEN)
    return "Merlin";
  return "Merlins";
}

varargs string Name(int casus, int daemon)
{
  return name(casus,daemon);
}

string QueryDu(int casus, int gender, int zahl)
{
  return
    ({ ({ ({ "du", "ihr"}), ({ "du", "ihr"}), ({ "du", "ihr"}) }),
         ({({"deines","deiner"}),({"deines","deiner"}),({"deiner","deiner"})}),
         ({({"dir","euch"}),({"dir","euch"}),({"dir","euch"})}),
         ({({"dich","euch"}),({"dich","euch"}),({"dich","euch"})})
       })[casus][gender][zahl];
}

string long()
{
  ({int})EPMASTER->GiveExplorationPoint("merlin");
  return break_string(
      "Merlin - der maechtige Urvater aller Magier - ist ein hagerer "
      "Mann, der einen blauen, mit Monden und Sternen uebersaeten Umhang "
      "traegt und sich auf einen knorrigen Stock stuetzt. Sein langer weisser "
      "Bart reicht ihm fast bis zur Guertelschnalle, sein Haupthaar ist fast "
      "ebenso lang. Auf seinem Kopf traegt er einen spitzen Hut, der ihn noch "
      "groesser macht, als er ohnehin schon ist.\nEr ist unangreifbar, denn "
      "er steht ueber solchen Dingen.",78,BS_LEAVE_MY_LFS);
}

string GetDetail(string key,mixed race,int sense) {
  switch( sense ) {
  case SENSE_SMELL: return 0;
  case SENSE_SOUND: return 0;
  }
  switch( key ) {
  case "nase": case "nasenhaare": case "hakennase":
          return BS("Merlin hat eine lange, krumme Hakennase, aus deren Nasenloechern weisse Haare spriessen. Auf seiner Nase sitzt eine Brille mit Glaesern in Form eines Halbmondes.");
  case "brille": case "glaeser": case "form": case "halbmond":
          return "Merlins Brille besitzt Glaeser in Form eines Halbmondes.\n";
  case "bart":
          return "Merlins Bart ist sehr lang - Er reicht fast bis ueber die Guertelschnalle.\n";
  case "haare" :
          return "Sie reichen ihm bis zwischen die Schulterblaetter.\n";
  case "schulterblaetter": case "schulterblatt":
          return "Seine langen Haare bedecken die Schulterblaetter.\n";
  case "gewand": case "robe":
          return BS("Das Gewand ist blau und mit silbernen Monden und Sternen bestickt. Es ist uralt und schon etwas abgetragen, aber es erfuellt immer noch seinen Zweck.");
  case "zweck":
          return BS("Der Zweck des Gewandes besteht darn, Merlin standesgemaess zu bekleiden. Diesem Zweck kommt das Gewand in Vollendung nach.");
  case "vollendung":
          return "Nichts ist vollkommen.\n";
  case "hut":
          return BS("Ein langer, spitzer Hut, der in der gleichen Weise wie seine Robe mit Monden und Sternen bestickt ist.");
  case "mond": case "monde": case "stern": case "sterne":
          return BS("Die Monde und die Sterne sind mit silbrigen Faeden gestickt, die das Licht funkelnd zurueckwerfen.");
  case "faden": case "faeden":
          return BS("Schwer zu sagen, woraus die Faeden sind. Mithril? "
              "Mondsilber?");
  case "licht": case "mondsilber": case "mithril":
          return BS("Du streckt schon Deine Hand aus, um Merlins Gewand naeher zu untersuchen, aber da trifft Dich sein strenger Blick. Schuechtern nimmst Du von Deinem Vorhaben Abstand.");
  case "abstand":
          return BS("Von so einem Magier sollte man aus Respekt einen Schritt Abstand halten.");
  case "respekt":
          return "Du hast sicher mehr Respekt vor Merlin, als er vor Dir.\n";
  case "schritt":
          return "Wessen Schritt??\n";
  case "hand": case "haende":
          return BS("Merlins Haende haben lange, gichtgeplagte Finger und gelbe lange Fingernaegel. Seine linke Hand stuetzt sich auf seinen knorrigen Gehstock. Die rechte Hand ist unter seinem Gewand verborgen.");
  case "gichtknoten": case "gicht":
          return BS("Durch die Gichtknoten sehen seine Haende fast so knorrig wie sein Gehstock aus.");
  case "finger": case "fingernaegel": case "fingernagel":
          return BS("Seine Finger sind voller Gichtknoten und seine gelben Fingernaegel koennte Merlin auch mal wieder schneiden.");
  case "gehstock": case "stock":
          return BS("Merlin stuetzt sich auf einen Gehstock. Der knorrige Stock scheint schon fast Eins mit seiner gichtknotigen Hand geworden zu sein. Ob es sich bei dem Stock um einen Magierstab handelt?");
  case "zauberstab": case "magierstab":
          return BS("Merlin stuetzt sich auf einen knorrigen Gehstock. Das koennte sein Zauberstab sein - aber vielleicht ist es auch nur ein Gehstock.");
  case "guertel": case "guertelschnalle":
          return BS("Sein Gewand wird von einem weissen Guertel gehalten. Vorne, unter seinem Bart, kannst Du die Guertelschnalle sehen. Auf der Guertelschnalle steht: 'Ich koennte jederzeit aufhoeren.'");
  }
  return 0;
}


string short()
{
  return "Merlin, der Urvater aller Magier.\n";
}

string _query_race()
{
  return "Magier";
}

int _query_gender()
{
  return 1;
}

string QueryPossPronoun( mixed what, int casus, int number )
{
  int gen2;
  gen2 = ( intp( what ) ) ? what : ({int})what->QueryProp(P_GENDER);
  return "sein" + query_g_suffix( gen2, casus, number );
}

string query_g_suffix( int gen, int casus, int anzahl )
{
  return ({ ({ ({"","e"}), ({"es","er"}), ({"em","en"}), ({"","e"}) }),
              ({ ({"","e"}), ({"es","er"}), ({"em","en"}), ({"en","e"}) }),
              ({ ({"e","e"}), ({"er","er"}), ({"er","en"}), ({"e","e"}) }) })
    [gen][casus][anzahl];
}

string QueryPronoun(int casus)
{
  switch(casus) {
    case WER: return "er";  break;
    case WEM: return "ihm";
  }
  return "ihn";
}

string _query_short()
{
  return short();
}

string _query_long()
{
  return long();
}

string _query_name()
{
  return "Merlin";
}

int _query_weight()
{
  return 75000;
}

static void PostSeher(string who, int g)
{
  mixed *art;

  art=({"stufen","Merlin",0,0,sprintf("%s ist jetzt ein%s Seher%s!",who,(g==2?"e":""),(g==2?"in":"")),sprintf("\nAm %s hat %s die noetigen Bedingungen\nerfuellt und ist vom Spielerstatus zum Seherstatus aufgestiegen. \n\n Herzlichen Glueckwunsch, %s!\n",dtime(time()),who,who)});
  ({int})"/secure/news"->WriteNote(art,1);
}

static void PostMagier(string who, string prom, int gender)
{
  mixed *art;

  art=({"stufen","Merlin",0,0,sprintf("%s ist jetzt ein%s Magier%s!",who,(gender==2?"e":""),(gender==2?"in":"")),sprintf("\nAm %s hat %s %s zu%s Magier%s\nberufen. Herzlichen Glueckwunsch, %s. Glueckwuensche auch an Dich,\n %s, zu Deine%s neuen %s!\n",dtime(time()),prom,who,(gender==2?"r":"m"),(gender==2?"in":""),who,prom,
(gender==2?"r":"m"),(gender==2?"Tochter":"Sohn"))});
  ({int})"/secure/news"->WriteNote(art,1);
}


private void GibFehlerteufel(object wiz) {
  if (!objectp(wiz))
      return;

  ({int})clone_object(FEHLERTEUFEL)->move(wiz, M_NOCHECK|M_GET);
  tell_object(wiz, break_string(
    "Huhu "+({string})wiz->Name(WER) + "! Ich habe gesehen, dass Du keinen "
    "Fehlerteufel hast. Dieses Tool ist zum Debuggen von Fehlern im "
    "Mud sehr praktisch. Bitte schau ihn Dir mal an. :-) Die Hilfe zum "
    "Fehlerteufel kannst Du mit 'man fehlerteufel' lesen. BTW: Wenn "
    "Du den Fehlerteufel nicht willst, kannst Du ihn zerstoeren, ich "
    "werde Dich kein zweites Mal damit belaestigen.",78,
    "Merlin teilt Dir mit: ")
    + "Merlin ueberreicht Dir aus der Ferne einen knusprig gebratenen "
    "Fehlerteufel.\n\n");
}

private void notify_player_change(string who,int rein)
{
  if (!stringp(who))
    return;

  // Max-user festhalten
  if (file_size("/etc/maxusers")<=0)
    maxusers=0;
  if (sizeof(users())>=maxusers)
  {
    maxusers=sizeof(users());
    write_file("etc/maxusers",sprintf("%d user: %s (%s)\n",
          maxusers,dtime(time()),query_load_average()),1);
  }

  // allen Magiern ab Level 20, die keinen Fehlerteufel haben und deren
  // letzter Logout vor "Die, 17. Mar 2009, 23:40:04" => 1237329604 war,
  // einen Fehlerteufel clonen und nen Hiwneis mitteilen.
  object ob = find_player(who);
  if (ob && rein && IS_WIZARD(ob)
      && !present_clone(FEHLERTEUFEL,ob)
      && ({int})ob->QueryProp(P_LAST_LOGOUT) < 1237329604 )
    GibFehlerteufel(ob);
}

private void AnnounceNewPlayer(object pl) {

  if (!pl || !interactive(pl))
    return;

  // verzoegern, bis der Spieler wirklich fertig und in /d/ angekommen ist,
  // weil dann der initiale Scroll abgeklungen ist und der Spieler die
  // Willkommensmeldungen von Merlin und Spielern hoffentlich auch wahrnimmt.
  if (!environment(pl)
      || strstr(object_name(environment(pl)), "/"DOMAINDIR"/") != 0)
  {
    call_out(#'AnnounceNewPlayer,20,pl);
    return;
  }

  if (!({int})pl->QueryGuest()) {
  string plname = ({string})pl->Name(WER);
    CHMASTER->send("Anfaenger", this_object(),
      sprintf("%s macht nun ebenfalls das Morgengrauen unsicher. "
              "Herzlich Willkommen %s!", plname, plname)
      );
  }
}

// Kann bei reboot wieder raus... zesstra, 16.03.2007
#if __BOOT_TIME__ < 1318110723
#define BUGTIME 1318003200 
#define BUGFIXTIME 1318109421
void renew_player(string plname, int force) {
    object pl;
    mixed err;

    pl = find_player(plname);
    if (!objectp(pl) || !query_once_interactive(pl)) return;

    // jung oder alt genug, Abbruch. ;-)
    if ((object_time(pl) > BUGFIXTIME)
        || object_time(pl) < BUGTIME)
        return;

    if (get_eval_cost() < 1000000 || !force ) {
        call_out("renew_player",8,plname,1);
        tell_object(pl,break_string(sprintf("Huhu %s, um einen Bug in "
                "Deinem Spielerobjekt zu beheben, wirst Du in ca. 8s "
                "neugeladen. Bitte warte bis dahin ab und bleibe, wo "
                "Du gerade bist.",({string})pl->Name()),78,"Merlin teilt Dir mit: "));
        return;
    }

    plname=getuid(pl);
    tell_object(pl,break_string(sprintf("Huhu %s, um einen Bug in "
            "Deinem Spielerobjekt zu beheben, wirst Du jetzt "
            "neugeladen.",({string})pl->Name()),78,"Merlin teilt Dir mit: "));

    if (err=catch(({int})master()->renew_player_object(pl);publish)) {
        log_file("zesstra/failed_rewew.log",sprintf("%s: Fehler %O beim "
              "Renew von %s\n",dtime(time()),err,plname));
        tell_object(pl,break_string("Beim Neuladen Deines Spielerobjekts "
              "gab es einen Fehler. Bitte wende Dich an einen EM.",78,
          "Merlin teilt Dir mit: "));
    }
    else {
      log_file("zesstra/renewed.log",sprintf("%s: %s wurde renewed.\n",
            dtime(time()),plname));
      if (objectp(find_player(plname)))
          tell_object(find_player(plname),
            break_string("Dein Spielerobjekt wurde neugeladen. Du "
          "kannst jetzt normal weiterspielen. Falls Du etwas ungewoehnliches "
          "bemerkst, melde Dich bitte bei einem Erzmagier. Viel Spass im "
          "Spiel!",78, "Merlin teilt Dir mit: "));
    }
}
#endif

void player_change(string eid, object trigob, mixed data) {

  if (!trigob || !interactive(trigob))
    return;

  switch(eid) {
    case EVT_LIB_PLAYER_CREATION:
      call_out(#'AnnounceNewPlayer, 5, trigob);
      break;
    case EVT_LIB_LOGIN:
      notify_player_change(data[E_PLNAME],1);
#if __BOOT_TIME__ < 1318110723
      // Spieler evtl. renewen? Bei reboot raus (08.10.2011)
      renew_player(data[E_PLNAME],0);
#endif
      break;
  }
}


//besser P_NO_ATTACK als Defend()...
mixed _query_no_attack() {
  if (!living(PL) || !objectp(environment(PL)) ) return(1);

  tell_room(environment(), "Merlin schimpft: Lass das!!!\n",({PL}));
  tell_room(environment(),sprintf(
        "Merlin gibt %s eine schallende Ohrfeige.\n",      
        capitalize(({string})this_player()->name(WEM))), ({this_player()}));
  return("Merlin schimpft: Lass das!!!\n"
      "Merlin gibt Dir eine schallende Ohrfeige.\n");
}

/*
int Defend(int dam, string dam_type, int spell, object enemy)
{
  object en;
  if (!en=previous_object() || !living(en))
    en = this_player();
  tell_room(environment(), "Merlin schimpft: Lass das!!!\n");
  write("Merlin gibt Dir eine schallende Ohrfeige.\n");
  say(sprintf("Merlin gibt %s eine schallende Ohrfeige.\n",
              capitalize((string)this_player()->name(WEM))), this_player());
  ({int})en->StopHuntFor(this_object(), 1);
}
*/
void shoutansw()
{
  object env,ti;
  string *path;
  string ans;

  if (!(env=environment()))
    return;
  if (!(ti=this_interactive())||({int})ti->QueryProp(P_LEVEL)<19||({int})ti->QueryProp(P_GHOST))
    return;
  tell_object(ti,"Du spuerst einen sengenden Schmerz.\n");
  ({int})ti->do_damage( ({int})ti->QueryProp(P_HP) / 2 + 5);
  if(({int})ti->QueryProp(P_GHOST)) return;
  ans="ich hab nicht die leiseste Idee, wo ich hier bin ...\n";
  path=old_explode(object_name(env),"/");
  if (path[0]=="d")
  {
    ans="werweisswo ..";
    switch(path[1])
    {
      case "gebirge":
      case "wald":
      case "dschungel":
      ans="im "+capitalize(path[1]);
      break;
      case "inseln":
      ans="auf den Inseln";
      break;
      case "polar":
      ans="im Polargebiet";
      break;
      case "unterwelt":
      case "ebene":
      case "wueste":
      ans="in der "+capitalize(path[1]);
      break;
      case "vland":
      ans="im Verlorenen Land";
      break;
      case "fernwest":
      ans="in Fernwest";
      break;
    }
    ans="ich bin "+ans+".\n";
  }
  if (path[0]=="players")
    ans="ich hab mich verlaufen - ich schaetze, "+capitalize(path[1])+
      " hat was damit zu tun...\n";
  if (path[0]=="gilden") {
    switch(path[1]) {
      case "abenteurer":
      ans="der Abenteurergilde";
      break;
      case "bierschuettler":
      ans="der Bierschuettlergilde";
      break;
      case "chaos":
      ans="der Gilde der Chaoten";
      break;
      case "kaempfer":
      ans="der Kaempfergilde";
      break;
      case "karate":
      ans="einem Karatedojo";
      break;
      case "klerus":
      ans="einem Tempel der Kleriker";
      break;
      case "zauberer":
      ans="der Akademie der Zauberer";
      break;
      default:
      ans="irgendeiner Gilde";
    }
    ans="ich stehe in "+ans+".\n";
  }
                if( path[0]=="p" && path[1]=="verein" ) {
                        ans="ich stehe in Port Vain.";
                }
  if (!({int})this_interactive()->QueryProp(P_INVIS))
    ans=({string})this_interactive()->Name(WER)+", "+ans;
  else ans=capitalize(ans);
  call_out("myshout",0,break_string(ans, 78, "Merlin ruft: "));
}

void myshout(string s)
{
  write(s);
  shout(s);
}

void catch_tell(string str)
{
  string name, dummy; mixed message;
  int i,s;

  if (!this_player())
    return;

  if (environment()==environment(this_player()) && previous_object()!=ME)
    tell_room(this_object(),"*"+str);

  if (this_player() != this_interactive())
    return;

  if (busy) {
    tell_object(this_player(), 
       "Merlin teilt Dir mit: Ich bin beschaeftigt.\n");
    return;
  }
    
  str=lower_case(str);

  if ((strstr(str, "teilt dir mit:") != -1) 
      && (strstr(str, "wo bist du") !=-1)) {
    shoutansw();
    return;
  }

  if ((strstr(str, "teilt dir mit:") != -1) && 
      ((strstr(str, "seher") != -1 || strstr(str, "seherin") != -1)))
    {
      create_seer(this_player());
      return;
    }

  if (sscanf(str,"%s %s sagt: %s",dummy,name,message)!=3)
    if (sscanf(str,"%s sagt: %s",name,message)!=2)
      return;

  if (!name || !sizeof(name) || !({int})master()->find_userinfo(name))
    return;

  if (name!=getuid(this_interactive()) && !ARCH_SECURITY)
  {
    if (flag==getuid(this_interactive())+"##"+name)
      return;
    call_out("do_say",0,"Merlin sagt: "+capitalize(getuid(this_interactive()))+" hat gerade so getan, als wuerde "+capitalize(name)+" etwas sagen!\n");
    flag=getuid(this_interactive())+"##"+name;
    call_out("clear_flag",0);
    return;
  }
  flag=0;
  for (i=0;i<sizeof(whitespaces);i++)
  {
    message=old_explode(message,whitespaces[i]);
    if (!pointerp(message)||sizeof(message)==0)
      return;
    message=implode(message," ");
  }

  message=old_explode(message," ");

  if (!pointerp(message) || !sizeof(message))
    return;

  message-=({0})-({""});

  if (!message || sizeof(message)==0)
    return;

  if (member(message, "merlin")==-1)
    return;

  message-=({"merlin"});

  message=implode(message," ");

  if (!message || message=="")
    return;

  determine_action(message, name);
}

static int determine_action(string mess, string name)
{
  string str;

  if (mess=="mach mich zum magier" || mess=="mach mich zur magierin")
    return create_wizard(name,name);

  if (sscanf(mess,"mach %s zum magier",str)==1||sscanf(mess,"mach %s zur magierin",str)==1)
    return create_wizard(lower_case(str),name);

  give_help(name);
  return 0;
}

#define Say(str) say(str,who)
#define Write(str) tell_object(who,str)

int QueryBedingungen(object pl,string was)
{
  int ret,i,gen;
  string s;

  if (IS_SEER(pl))
    return 1;
  
  if (({int})LEPMASTER->QueryReadyForWiz(pl)==1)
    return 1;

  s=({string})LEPMASTER->QueryReadyForWizText(pl); 

  tell_object(pl, break_string("Merlin gruebelt ein wenig und teilt Dir "
                               "nach einigem Ueberlegen mit:\n\n", 78,0,1));
  
  tell_object(pl, s+"\n");

  tell_object(pl, break_string("Da ist wohl fuer Dich noch etwas zu tun...",
                               78, "Merlin teilt Dir mit: "));

  if (environment() == environment(pl))
    say(({string})pl->name(WER)
        +" erfuellt noch nicht alle Anforderungen fuer den "+was+
        "status.\n",pl);
  
  return 0;
}

static int create_wizard(mixed who, mixed promoter)
{
  mixed *domains;
  object vertrag;
  int ret;
  string gen;

  who=find_player(who);
  promoter=find_player(promoter);
  if (!who || !promoter)
    return 0;
  //
  // player not interactive?
  //
  if (!interactive(who))
  {
    write(capitalize(getuid(who))+" ist nicht aktiv.\n");
    return 0;
  }
  //
  // player is second?
  //
  if (({string})who->QueryProp(P_SECOND))
  {
     tell_object(who,"Du bist ein Zweitspieler und kannst als solcher kein Magier werden.\n"
          +"Versuchs Doch mal mit Deinem Erstie.\n");
     write("Du kannst "+({string})who->name()+" nicht zum Magier machen. Das geht bei "
          +"Zweities\nleider nicht. Frag doch mal nach dem Erstie.\n");
     return 1;
  }
  //
  // wants to advance himself?
  //
  if (who==promoter)
  {
    write("Du kannst Dich nicht selber befoerdern. Such Dir dazu bitte einen "+
              "Vollmagier,\nder Dir einen Vertrag gibt.\n");
    return 1;
  }
  //
  // banished?
  //
  if (MBanishListe[who,1])
  {
    tell_object(promoter,capitalize(who)+" darf nur von einem Erzmagier zum "+
                    "Magier gemacht werden.\n");
    say(({string})promoter->name(WER)+" ist kein Erzmagier und darf "+capitalize(who)+
            " nicht zum Magier machen.\n");
    return 1;
  }
  //
  // player has PK?
  //
  if (({string})who->QueryProp("playerkills")) {
     tell_room(environment(who),({string})who->name()+" hat noch einen Playerkill und kann somit kein Magier werden.\n"
                           "Am besten den Sheriff oder einen Erzmagier verstaendigen.\n");
         return 1;
  }
  //
  // already dom_member?
  //
  if (IS_DOMAINMEMBER(who))
  {
    Write("Fuer Dich kann ich nichts mehr tun. Du bist doch schon Magier!\n");
    return 1;
  }

  //
  // "advanced learner"?
  //
  if (query_wiz_level(who) == 20)
  {
    if (!ARCH_SECURITY) {
      Write("Dich kann nur ein Erzmagier zum \"richtigen\" Magier machen!\n");
      Say("Nur ein Erzmagier kann "+({string})who->name(WEN)+" zum \"richtigen\" Magier machen!\n");
      return 0;
    }
    ret=({int})"secure/master"->advance_wizlevel(geteuid(who),21);
    if (ret<=0)
    {
      say("Merlin: error "+ret+"\n");
      write("Merlin: error "+ret+"\n");
    }
    write("Merlin ruft: "+({string})who->name(WER)+" ist in den Kreis der Magier "+
              "aufgenommen worden!\n");
    shout("Merlin ruft: "+({string})who->name(WER)+" ist in den Kreis der Magier "+
              "aufgenommen worden!\n");
    ({int})"secure/master"->renew_player_object(who);
    return 1;
  }

  //
  // already wizard?
  //
  if (IS_WIZARD(who))
  {
    domains=({string*})"secure/master"->get_domain_homes(getuid(who));
    if (!domains || !pointerp(domains) || !sizeof(domains))
    {
      Say(({string})who->name(WER)+" gehoert noch keiner Region an und kann daher noch "+
              "kein Voll-\nmagier werden.\n");
      Write("Du gehoerst noch keiner Region an. Schliess Dich zunaechst einer "+
                "an und komm\ndann wieder!\n");
      return 0;
    }
    ret=({int})"secure/master"->advance_wizlevel(geteuid(who),25);
    if(ret>0)
    {
      shout(({string})who->name(WER)+" arbeitet jetzt an einer Region mit!\n");
      ({int})"secure/master"->renew_player_object(who);
      return 1;
    }
    write("RETURNVALUE "+ret+"\n");
    say("RETURNVALUE "+ret+"\n");
    return 0;
  }

  //
  // still learner?
  //
  if (IS_LEARNER(who))
  {
    if (!ARCH_SECURITY) {
      Write("Dich kann nur ein Erzmagier zum \"richtigen\" Magier machen!\n");
      Say("Nur ein Erzmagier kann "+({string})who->name(WEN)+" zum \"richtigen\" Magier machen!\n");
      return 0;
    }
    ret=({int})"secure/master"->advance_wizlevel(geteuid(who),20);
    if (ret<=0)
    {
      say("Merlin: error "+ret+"\n");
      write("Merlin: error "+ret+"\n");
    }
    string wizname = ({string})who->query_real_name();
    ({int})"secure/master"->renew_player_object(who);
    // Fehlerteufel geben
    call_out(#'GibFehlerteufel,4,find_player(wizname));
    return 1;
  }

  //
  // promoter's level is not >DOMAINMEMBER_LVL+1
  //
  if (secure_level()<=DOMAINMEMBER_LVL)
  {
    tell_object(promoter,"Du musst derzeit mindestens Level "+
                    (DOMAINMEMBER_LVL+1)+" haben, um jemanden befoerden zu "+
                                "koennen.\n");
    say(({string})promoter->name(WER)+" ist kein Vollmagier und darf daher niemanden "+
            "zum Vollmagier machen.\n");
    return 1;
  }

  // der Check auf den Blutprintnamen ist fast ueberfluessig, aber nur fast.
  // Er schlaegt fehl, wenn der Vertrag der Clone einer alten BP ist.
  if (!(vertrag=present_clone("/obj/vertrag",who)) ||
      object_name(blueprint(vertrag))!="/obj/vertrag" ||
      !(({int})vertrag->is_unterschrieben()) )
  {
    Write("Du hast ja gar keinen unterschriebenen Vertrag bei Dir. Besorg Dir "+
              "einen Ver-\ntrag, unterschreibe ihn und komm dann wieder!\n");
    Say(({string})who->name(WER)+" hat keinen unterschriebenen Vertrag.\n");
    return 1;
  }
  if (geteuid(vertrag)!=secure_euid())
  {
    tell_object(promoter,"Das geht nicht, er hat einen Vertrag von "+
                    capitalize(geteuid(vertrag))+" bei sich.\n");
    say(({string})promoter->name(WER)+" versucht, "+({string})who->name(WER)+" zum Magier zu "+
            "machen, doch "+({string})who->name(WER)+" hat einen Vertrag\nvon "+
                capitalize(geteuid(vertrag))+" bei sich.\n",promoter);
    return 0;
  }
  if (!QueryBedingungen(who,"Magier"))
    return 0;
  ret=({int})"secure/master"->advance_wizlevel(geteuid(who),15);
  if (ret>0)
  {
    // Log ("Magierstammbuch") nach /data/etc loggen, weil SPONSOR ein
    // langfristlog.
    write_data("/etc/SPONSOR",dtime(time())+": "+capitalize(getuid(promoter))+" macht "+
             ({string})who->name(WER)+" zum Learner.\n");
    write(({string})who->name(WER)+" ist in den Kreis der Magier aufgenommen worden!\n");
    shout(({string})who->name(WER)+" ist in den Kreis der Magier aufgenommen worden!\n");
    PostMagier(capitalize(getuid(who)),
               capitalize(secure_euid()),
               ({int})who->QueryProp(P_GENDER));
    if (({int})"secure/master"->set_player_object(geteuid(who),"/std/shells/magier")
            <=0)
    {
      say("MERLIN: konnte Magiershell nicht einstellen.\n");
      write("MERLIN: konnte Magiershell nicht einstellen.\n");
    }
    ({int})"secure/master"->renew_player_object(who);
    call_out(function void ()
        { write_data("/etc/SPONSOR.html",wiztree_html(), 1); }, 2);
    return 1;
  }
  write(" --RETURNVALUE IS "+ret+"\n");
  say(" --RETURNVALUE IS "+ret+"\n");

  return(ret); //non-void funktion
}

void clear_flag()
{
  flag=0;
}

void do_say(string str)
{
  say(str);
}



void seer_sequenz3(object player, string plname)
{
  string playername; 
  object faq;
  string text;

  if (!objectp(player) || !interactive(player)) {
    tell_room(environment(), sprintf("Merlin sagt: Na, wo ist denn %s "
                                     "hin?\n", plname));
    busy = 0;
    return;
  }
  if (environment() != environment(player))
    move(environment(player), M_TPORT);
  
  tell_object(player,break_string("\n\nEs macht *PUFF* und eine Schwefelwolke "
                                  +"steigt um Dich herum auf.\n\nDie Aura "
                                  +"verschwindet langsam und Du fuehlst Dich "
                                  +"nun viel erfahrener.", 78,0,1));
  tell_room(environment(),
           break_string(sprintf(
                       "\n\nEs macht *PUFF* und eine Schwefelwolke steigt "
                       +"um %s herum auf.\n\nDie Aura verschwindet langsam.\n",
                       plname),78,0,1), ({player}));
 
  if (({int})"secure/master"->advance_wizlevel(geteuid(player),1)<=0)
  {
    write("Merlin sagt: Da geht was schief ... sag mal Zook Bescheid.\n");
    say("Merlin sagt: Da geht was schief ... sag mal Zook Bescheid.\n");
    busy = 0;
    return;
  }
  playername = geteuid(player);
  ({int})"secure/master"->renew_player_object(player);
  player = find_player(playername);
  text = sprintf(" ist jetzt ein%s Seher%s!\n",
    ((({int})player->QueryProp(P_GENDER))==2?"e":""),
    ((({int})player->QueryProp(P_GENDER))==2?"in":""));
  write("Merlin ruft: "+capitalize(playername)+text);
  shout("Merlin ruft: "+capitalize(playername)+text);
  PostSeher(capitalize(playername),({int})player->QueryProp(P_GENDER));
  if(!catch(
        faq=clone_object("/d/seher/haeuser/special/seherfaqmobil") ;publish))
  {
    ({int})faq->move(player,M_NOCHECK);
    tell_object(player, "Aus dem Nichts erscheint eine kleine Belohnung fuer Dich.\n");
  }
  
  tell_object(player, "\n\nDu laechelst gluecklich in die Runde.\n\n");
  tell_room(environment(), sprintf("\n\n%s laechelt gluecklich in die Runde.\n\n", plname), ({player}));

 busy=0;
}

void seer_sequenz2(object player, string plname)
{
  if (!objectp(player) || !interactive(player)) {
    tell_room(environment(), sprintf(
                                     "Merlin sagt: Na, wo ist denn %s hin?\n", 
                                     plname));
    busy = 0;
    return;
  }
  if (environment() != environment(player))
    move(environment(player), M_TPORT);

  tell_object(player, break_string("\nMerlin hebt die Arme "
       +"und macht einige beschwoerende Gesten. Um Dich herum erscheint "
       +"eine gelbliche Aura, die immer heller wird, bis Du Deine "
                                   +"Umwelt kaum mehr erkennen kannst.\n\n",
                                   78,0,1));
  tell_room(environment(),
            break_string(sprintf("\nMerlin hebt die Arme und macht einige "
                         +"beschwoerende Gesten. Um %s erscheint eine "
                         +"gelbliche Aura, die immer heller wird, bis "
                                 +"man %s kaum mehr erkennen kann.\n\n", 
                                 plname, plname),78,0,1), ({player}));

  call_out("seer_sequenz3", 7, player, plname);
}

void seer_sequenz1(object player, string plname)
{
  if (!objectp(player) || !interactive(player))
    return;

  move(environment(player), M_TPORT);

  tell_room(environment(),
            break_string(sprintf("Das ist eine grosse Freude. %s hat sich "
     "tapfer durch das "MUDNAME" geschlagen und mittlerweile alle "
     "Anforderungen erfuellt, um %s zu werden.",                                  
     plname, (({int})player->QueryProp(P_GENDER))==1?"Seher":"Seherin"),
              78, "Merlin sagt: "));

  call_out("seer_sequenz2", 4, player, plname);
}

  
static int create_seer(object player)
{
  if (!objectp(player) || !interactive(player))
    return 0;
  if (IS_SEER(player))
  {
    write(break_string("Du bist doch schon kein normaler Spieler mehr, "
                       +"was soll es also?", 78, "Merlin teilt Dir mit: "));
    return 1;
  }

  if (!QueryBedingungen(player,"Seher"))
    return -1;


  write(break_string("Grosse Freude! Du hast alle Anforderungen "
                                   "erfuellt. Warte einen Moment...", 78,
                                   "Merlin teilt Dir mit: "));
  busy=1;
  call_out("seer_sequenz1",4,player, ({string})player->Name());
  return(0); //non-void funktion
}

static void give_help(mixed player)
{
  if (!objectp(player))
    return;

  tell_object(player,
              break_string("Ich weiss nicht, was ich fuer Dich tun kann. "
                           +"Ich kann Dich zum Seher oder zum Magier "
                           +"machen, wenn Du mich darum bittest.", 78,
                           "Merlin teilt Dir mit: "));
}
              

void reset()
{
}

public int remove(int silent) {
  ({int})EVENTD->UnregisterEvent(EVT_LIB_LOGIN, ME);
  ({int})EVENTD->UnregisterEvent(EVT_LIB_PLAYER_CREATION, ME);
  destruct(this_object());
  return 1;
}

void wandern()
{
  while (remove_call_out("wandern")>=0);
  call_out("wandern",45+random(100));
  if (busy) return;
  if (delay)
  {
    delay=0;
    return;
  }
  if (!environment())
  {
    move("/gilden/abenteurer",0);
  }
  //ueber Inv iterieren, ob da ein nicht-idelnder Spieler mit genug XP
  //rumhaengt. In diesem Raum bleiben, wenn ja.
  foreach(object ob: all_inventory(environment()))
  {
    if (interactive(ob) && query_idle(ob)<180
        && ({int})ob->QueryProp(P_XP)>999999)
      return;
  }
  // Ausgaenge durchsuchen
  mapping ex=({mapping})environment()->QueryProp(P_EXITS);
  if (!mappingp(ex))
    return;
  mapping rooms = m_allocate(sizeof(ex));
  foreach(string cmd, string|closure dest, string msg : ex)
  {
    object ob;
    // nur normale Ausgaenge benutzen
    if (!stringp(dest)
        || dest = object_name(environment()))
      continue;
    rooms += ([dest]);
  }
  if (!sizeof(rooms))
    return;

  // Und nicht zuruecklatschen, wenn moeglich.
  if (prev_room && sizeof(m_indices(rooms))>1)
    rooms-=([prev_room]);
  prev_room=object_name(environment());

  string *raeume=m_indices(rooms);
  move(raeume[sizeof(raeume)],0);
}

string int_short()
{
  return "Du bist im allseits beliebten Wizclub \"Zum lustigen Merlin\".\n";
}

string int_long(object who,mixed where)
{
  string descr;

  if( IS_LEARNER(who) && ({int})who->QueryProp( P_WANTS_TO_LEARN ) )
    descr = "[" + object_name(ME) + "]\n";
  else
    descr = "";
  descr+="Du bist im Wizclub 'Zum lustigen Merlin'. Dieser Wizclub ist etwas besonderes -\ner hat keinen festen Standort, sondern wandert im Mud herum. Ausserdem kann er\nSpieler zu Magiern machen - aber nur, wenn man ihn von aussen aus darauf\nanspricht.\n";
  if ( ({int})who->QueryProp(P_SHOW_EXITS) )
    descr += "Es gibt 2 sichtbare Ausgaenge: raus und gilde.\n";
  descr += make_invlist(who, all_inventory(ME) - ({ who }));
  if( environment())
    descr += "\nAusserhalb siehst Du:\n"
      + ({string})environment()->int_short(who,ME);
  return descr;
}

string make_invlist(object viewer,object *inv)
{
  int i, iswiz;
  string descr;

  descr = "";
  iswiz = IS_LEARNER( viewer ) && ({int})viewer->QueryProp(P_WANTS_TO_LEARN);

  for (i = 0; i < sizeof(inv); i++)
  {
    string sh;
    sh = ({string})inv[i]->short();
    if (iswiz)
    {
      if (sh) sh = sh[0..<2] + " ["+object_name(inv[i])+"]\n";
      else sh = "["+object_name(inv[i])+"]\n";
    }
    if (sh) descr += sh;
  }
  return descr;
}

void init()
{
  if (interactive(this_player()) && environment(this_player())==ME)
    add_action("gehen","",1);
  add_action("befoerdere","befoerder",1);
}

int befoerdere(string str)
{
  string wen;
  int ret,stufe,l;

  if (!LORD_SECURITY) return 0;
  if (!str||sscanf(str,"%s auf stufe %d",wen,stufe)<2)
  {
    notify_fail("Syntax: befoerdere <name> auf stufe <stufe>\n");
        return 0;
  }
  if (stufe>29)
  {
    write("Maximum ist Stufe 29!\n");
        return 1;
  }
  l=query_wiz_level(wen);
  if (l<25)
  {
    printf("%s muss mindestens zwecks Weiterbefoerderung mindestens Stufe 25 haben.\n",
        capitalize(wen));        
    return 1;
  }
  if (l>=stufe)
  {
    printf("%s ist schon Stufe %d.\n",capitalize(wen),l);
        return 1;
  }
  ret=({int})"secure/master"->advance_wizlevel(wen,stufe);
  if (ret<=0)
    printf("Errorcode %d\n",ret);
  else
    write("ok\n");
  return 1;
}

int sag(mixed str)
{
  int i;

  if (str==0)
    str="";
  str = old_explode(break_string(str, 60 ), "\n");
  if (busy) {
    tell_room(ME, "Merlin mault: Ich bin beschaeftigt.\n");
    return 0;
  }
  for (i = 0; i < sizeof(str); i++)
  {
    if (!str[i])
      str[i]="";
    tell_room(environment(),"Merlin sagt: "+str[i]+"\n");
    tell_room(ME,"*Merlin sagt: "+str[i]+"\n");
  }
  return 1;
}

void show_exits()
{
  printf("%s",({string})environment()->GetExits());
}

int gehen(string str)
{
  if (busy) {
    write("Merlin mault: Ich bin beschaeftigt.\n");
    return 0;
  }

  str=({string})this_interactive()->_unparsed_args();
  switch (query_verb())
  {
    case "gilde":
      ({int})this_player()->move("/gilden/abenteurer",M_GO,"in die Gilde");
      return 1;
    case "raus":
      ({int})this_player()->move(environment(),M_GO,"raus");
      return 1;
  }
  if (!IS_WIZARD(this_interactive()))
    return 0;
  if (query_verb()!="merlin")
    return 0;
  delay=1;
  mapping exits=({mapping})environment()->QueryProp(P_EXITS);
  if (!str||str=="")
  {
    printf(({string})environment()->int_short(ME,ME));
    show_exits();
    return 1;
  }
  if (old_explode(str," ")[0]=="sag")
    return sag(implode(old_explode(str," ")[1..]," "));
  if (str[0]==';')
  {
    tell_room(environment(ME),"Merlins "+str[1..]+"\n");
    tell_room(ME,"*Merlins "+str[1..]+"\n");
    return 1;
  }
  if (str[0]==':')
  {
    tell_room(environment(ME),"Merlin "+str[1..]+"\n");
    tell_room(ME,"*Merlin "+str[1..]+"\n");
    return 1;
  }
  if (str[0]=='*')
  {
    goto(str[1..]);
    printf(({string})environment()->int_short(ME,ME));
    show_exits();
    return 1;
  }
  if (!exits[str])
    return 0;
  move(exits[str],0);
  write("Merlin sagt: Jawoll, Chef.\n");
  printf(({string})environment()->int_short(ME,ME));
  show_exits();
  return 1;
}

int goto(mixed dest)
{
  object ob;
  string path;
  mixed ret;

  if (busy) {
    write("Merlin mault: Ich bin beschaeftigt.\n");
    return 1;
  }

  if (!dest) {
    write("Wohin moechtest Du Merlin bringen ?\n");
    return 1;
  }
  ob = find_living(dest);
  if (ob) {
    ob = environment(ob);
    move(object_name(ob),0);
    return 1;
  }
  path = ({string})master()->make_path_absolute(dest);
  ret=catch(load_object(path);publish);
  if (ret && file_size(path)<0)
  {
    dest=match_living(dest,1);
    if (stringp(dest))
    {
      ob = environment(find_living(dest));
      move(object_name(ob),M_TPORT|M_NOCHECK);
      return 1;
    }
    printf("Fehler beim Teleport nach %O: No such file.\n",path);
    return 1;
  }
  if (ret)
    printf("Fehler beim Teleport nach %O: %O\n",dest,ret);
  ret=catch(move(path,M_TPORT|M_NOCHECK);publish);
  if (stringp(ret))
    ret=implode(old_explode(ret,"\n"),"");
  if (ret)
    printf("Fehler beim Teleport nach %O: %O\n",dest,ret);
  return 1;
}

mixed locate_objects(string complex_desc)
{
  object ob;

  if (complex_desc=="alles" || complex_desc=="alle")
    return filter_objects(all_inventory(this_object()),"short");
  if (ob = present(complex_desc,this_object()) )
    return ({ ob });
  return ({}) ;
}

void MBanishInsert(string name, string grund, object wer)
{
  if ( !name || !stringp(name) || !sizeof(name) ||
       !grund || !stringp(grund) || !sizeof(grund) ||
       !wer || !objectp(wer) || getuid(wer) != secure_euid() ||
       !IS_DEPUTY(wer) )
    return;

  MBanishListe += ([name:grund;getuid(wer)]);
  save_object(SAVEFILE);
}

mapping MBanishList()
{
    if ( !IS_DEPUTY(secure_euid()) )
        return 0;

    return deep_copy(MBanishListe);
}

void MBanishDelete(string name)
{
  if ( !name || !stringp(name) || !sizeof(name) || !ARCH_SECURITY )
    return 0;

  MBanishListe = m_copy_delete( MBanishListe, name );
  save_object(SAVEFILE);
}

string _query_noget()
{
  return "Merlin mag das nicht!\n";
}

private mapping create_wizard_tree()
{
  // Jof als root-elemente des Baums wird per Hand angelegt. Erstes Element
  // ist der Magierlevel, zweites das Mapping mit den Kindern (der Ast mit den
  // Kindern)
  mapping mtree = ([ "Jof": 111; m_allocate(100,2) ]);
  // Der Cache dient nur dazu, das zu einem Magier gehoerende Mapping der
  // Kinder schneller zu finden und verweist auf das Mapping im zweiten
  // Value in mtree.
  mapping cacheptr = m_allocate(500,1);
  cacheptr["Jof"] = mtree["Jof",1];

  // Dann Log lesen und parsen.
  foreach(string line: explode(read_file("/data/etc/SPONSOR"),"\n"))
  {
    string parent, child;
    if (sscanf(line,"%!s: %s macht %s zum Learner.",parent,child) != 2)
      continue;

    parent = capitalize(lower_case(parent));
    child = capitalize(lower_case(child));

    // Das Kind sollte noch nicht existieren... (Tut es aber leider
    // gelegentlich.)
    if (member(cacheptr, child))
//      raise_error(sprintf("Neu berufene Magierin ist schon Magierin: "
//                          "%s\n",child));
      continue;
    // Der Elter muss existieren...
    if (!member(cacheptr, parent))
      raise_error(sprintf("Sponsor ist keine Magierin: %s\n", parent));

    // Mapping mit allen Kindern des Elter holen. Das Mapping in cacheptr ist
    // dasselbe Mapping wie in mtree, d.h. wir muessen den Elter nicht im
    // verschachtelten mtree suchen.
    mapping parent_children = cacheptr[parent];
    // neues Kind anlegen
    m_add(parent_children, child,
          ({int})master()->query_userlist(lower_case(child), USER_LEVEL),
          m_allocate(2,2) );
    // und im cacheptr das (noch leere) Mapping fuer die Kinder vom child hinterlegen
    m_add(cacheptr, child, parent_children[child, 1]);
  }
  //printf("mtree: %O\n",mtree);
  //printf("cache: %O\n",cacheptr);
  return mtree;
}

// Starte mit dem Magier <node> in <mtree> und laeuft rekursiv durch den
// ganzen (Sub-)Baum. Erzeugt eine geschachtelte Struktur von <ul>-Elementen,
// bei denen Magier ein <li> sind und die jeweiligen Kinder wieder in ein
// tiefer liegendes <ul> kommen. Letztendlich wieder eine Baumstruktur.
private string print_mtree_node(mapping mtree, string node, int indent)
{
  mapping branch = mtree[node, 1];
  if (!branch)
    raise_error(sprintf("Magier %s existiert nicht.\n",node));

  indent += 2;
  string res;
  // Geloeschte Magier werden durchgestrichen, Vollmagier kriegen ein
  // "class=magier" Attribut, was dafuer sorgt, dass die fettgedruckt werden.
  if (mtree[node, 0] == 0)
    res = sprintf("%s<li><a href=\"/cgi-bin/mudwww.pl?REQ=finger&USER=%s\">"
                       "<del>%s</del></a></li>\n", " "*indent, node, node);
  else if (mtree[node, 0] < DOMAINMEMBER_LVL)
    res = sprintf("%s<li><a href=\"/cgi-bin/mudwww.pl?REQ=finger&USER=%s\">"
                       "%s</a></li>\n", " "*indent, node, node);
  else
    res = sprintf("%s<li><a href=\"/cgi-bin/mudwww.pl?REQ=finger&USER=%s\""
                       " class=\"wiz\">%s</a></li>\n", " "*indent, node, node);

  // Wenn ein Magier Kinder hat, kommt nun ein neues <ul> dran, welches wieder
  // alle Kinder enthaelt.
  if (sizeof(branch))
  {
    // Das root-ul von Jof bekommt das Attribute "id=jof", alle anderen nicht.
    res += sprintf("%s<ul%s>\n", " "*indent,
                   (node == "Jof" ? " id=\"jof\"" : ""));
    foreach(string child, int lvl, mapping grandchildren : branch)
    {
      res += print_mtree_node(branch, child, indent);
    }
    res += sprintf("%s</ul>\n", " "*indent);
  }
  return res;
}

public string wiztree_html()
{
  mapping mtree = create_wizard_tree();
  return sprintf("<ul>\n%s</ul>\n",print_mtree_node(mtree, "Jof", 2));
}

