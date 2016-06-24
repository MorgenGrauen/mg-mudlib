// MorgenGrauen MUDlib
//
// shells/magier.c -- magier shell
//
// $Id: magier.c 9231 2015-05-27 21:53:32Z Zesstra $

//
// Magiershell Basisfile
//
// Ueberarbeitung abgeschlossen am 18.12.2002
//
// Dank an Zwirch@PK, Rikus@MG, Zoran@PK, Vanion@MG
// und viele andere, die ich vergessen habe.
//
// Fragen und Bughinweise an Mandragon@MG oder einen
// Erzmagier Deiner Wahl.
//
// Zur Shell gehoeren ausser dieser Datei:
// admin.c:      Administrative Befehle
// comm.c:       Kommunikationsbefehle
// fileedit.c:   Befehle zum Veraendern von Dateien
// fileview.c:   Befehle zum Lesen von Dateien
// magier_ext.c: Generelle Magierbefehle
// moving.c:     Bewegungsbefehle
// objects.c:    Erzeugen und zerstoeren von Objekten
// parsing.c     Auswertung von Pfadangaben und Wildcards
// players.c:    Befehle zur Beeinflussung von Spielern
// todo.c:       Implementation der Todoliste
// upd.c:        Der Befehl upd
// magier.h      Generelle Header-Datei
//

#pragma strict_types,save_types

inherit "/std/player/base";
inherit "/std/shells/magier/magier_ext";

#include <wizlevels.h>
#include <moving.h>
#include <properties.h>
#include <new_skills.h>
#include <config.h>

protected void create()
{
  if (!clonep() || object_name(this_object()) == __FILE__[0..<3]) {
      set_next_reset(-1);
      return;
  }

  base::create();

  Set(P_RACE, SAVE, F_MODE);
  Set(P_ZAP_MSG, SAVE, F_MODE);
  Set(P_TRANK_FINDEN, SAVE, F_MODE);
  Set(P_HANDS, SAVE, F_MODE);
  Set(P_RACESTRING, SAVE, F_MODE);
  SetDefaultHome("/gilden/abenteurer");
  SetProp(P_ENEMY_DEATH_SEQUENCE,
    ([17:"Der Tod schuettelt verstaendnislos den Kopf.\n\n",
      18:"Der Tod sagt: WIESO MUSSTEST DU DICH AUCH UNBEDINGT "
      "MIT EINEM MAGIER ANLEGEN?\n\n"]));
  SetProp(P_ATTRIBUTES_OFFSETS,([]));
  SetProp(P_AVERAGE_SIZE,185);
  if(!QueryProp(P_DEFAULT_GUILD)) SetProp(P_DEFAULT_GUILD,"abenteurer");
}

protected void create_super() {
  set_next_reset(-1);
}


protected void heart_beat()
{
  mixed *en;

  if (!QueryProp(P_WANTS_TO_LEARN)||((en=QueryEnemies())&&sizeof(en[0])))
    base::heart_beat();
  else if (!CheckTelnetKeepAlive()) {
    // Wenn der Magier kein Telnet Keep-Alive wuenscht, kann der HB ganz
    // abgeschaltet werden. Sonst muss er aber weiterlaufen, damit
    // CheckTelnetKeepAlive() regelmaessig gerufen wird.
    set_heart_beat(0);
  }
}


public varargs int remove(int silent)
{
  string workroom;

  if (IS_WIZARD(this_object()))
    workroom = "/players/"+getuid()+"/workroom";
  else
    workroom = "/secure/merlin";
  if( !environment() || object_name(environment()) != workroom )
    catch(move(workroom, M_GO, "nach Hause"));
  return base::remove(silent);
}

public string NotifyDestruct(object caller) {

  if (previous_object() != master()
      || object_name(this_object()) == __FILE__[..<3])
    return 0;
  
  // Nicht-EMs sollen keine EMs zerstoeren koennen, woraufhin auch evtl.
  // EM-Tools rumliegen koennten.
  if ( IS_ARCH(this_object()) && caller != this_object() 
      && getuid(caller) != ROOTID
      && (process_call() || !ARCH_SECURITY) )
    return "Das Zerstoeren von EMs ist ein Fehler. ;-)\n";

  return ::NotifyDestruct(caller);
}

void reset()
{
  if (!interactive(this_object()))
  {
    quit();
    if (this_object())
      remove();
    if (this_object())
        destruct(this_object());
    return;
  }
}

//                ####################
//################# Query-Funktionen ##################
//                ####################

varargs int id (string str) {
  if (QueryProp(P_INVIS) &&
      (!this_interactive() ||!IS_LEARNER(this_interactive())))
    return 0;
  return ::id(str);
}


static string *_query_racestring()
{
  if (pointerp(Query(P_RACESTRING)))
    return Query(P_RACESTRING);
  else
    return
    ({QueryProp(P_RACE),QueryProp(P_RACE),
      QueryProp(P_RACE),QueryProp(P_RACE)});
}


static string _query_default_guild()
{
    return (Query(P_DEFAULT_GUILD)||"abenteurer");
}


static string _query_racedescr()
{
  return "Magier koennen einfach alles. Aber manche Magier koennen mehr.\n";
}


static string _query_race()
{
  if (previous_object() && previous_object()->query_login_object())
    return 0;

  return Query(P_RACE) ? Query(P_RACE) : Set(P_RACE, "Magier");
}


static mixed _query_localcmds()
{
  return
    base::_query_localcmds()
    +magier_ext::_query_localcmds();
}


static void upd_my_age()
{
  age=_age+absolute_hb_count()-_hbstop;
  _age=age;
  _hbstop=absolute_hb_count();
  return;
}


static int _query_age()
{
  upd_my_age();
  return age;
}

static int _set_earmuffs(int level)
{
  int maxl=1+query_wiz_level(this_object());
  maxl = max(maxl,99);
  return Set(P_EARMUFFS,min(maxl,level));
}


//                   ############################
//#################### Interne Shell-Funktionen ####################
//                   ############################

int MayAddWeight(int w) { return 0;}
int MayAddObject(object ob) { return 1; }


static void initialize()
{
  magier_ext::initialize();
  return;
}


static void FinalSetup()
{
  SetProp(P_CURRENTDIR,"/players/"+getuid());
  initialize();
  if (IS_LEARNER(this_player())) cat("/etc/WIZNEWS");
  _age=age;
  _hbstop=absolute_hb_count();
  return;
}


void save_me(int i)
{
  upd_my_age();
  base::save_me(i);
  return;
}


varargs void Reconnect(int silent,string my_ip)
{
  base::Reconnect(silent,my_ip);
  magier_ext::reconnect();
  return;
}


void notify_player_change(string who, int rein, int invis)
{
  string *list,name;
  mixed mlist;
  int vis_change;

  if (invis) name="("+who+")";
    else name=who;

  if(query_verb() && (query_verb()=="vis" || query_verb()=="invis"))
    vis_change=1;

  if (Query(P_INFORMME) && !vis_change)
  {
    if (rein) 
      tell_object(this_object(),
                  sprintf("%s ist gerade ins "MUDNAME" gekommen.\n",name));
    else
      tell_object(this_object(),
                  sprintf("%s hat gerade das "MUDNAME" verlassen.\n",name));
  }

  if(Query(P_WAITFOR_FLAGS) & (0x01))return ;

  if (pointerp(list=Query(P_WAITFOR))&&sizeof(list))
    if (member(list,who)!=-1)
      delayed_write(
         ({
           ({sprintf("%s%s   I S T   J E T Z T   %s !!!\n",
                     (QueryProp(P_VISUALBELL) ? "" : sprintf("%c",7)),
                     name,
                     (vis_change?
                       (rein?"S I C H T B A R":"U N S I C H T B A R"):
                       (rein?"D A":"N I C H T   M E H R   D A"))),
           0})
         }));

  if (rein && (sizeof(mlist=QueryProp(P_WAITFOR_REASON))) &&
     (mappingp(mlist)) && (mlist[who]))
        Show_WaitFor_Reason(who,invis);
  return;
}

mixed modify_command(string str) {
  if (previous_object() &&
      (previous_object()!=this_object() || process_call()) )
  {
    if (IS_ARCH(this_object()))
      tell_object(this_object(),
        sprintf("Illegal modify_command(%s) from %O\n",
        str, previous_object()));
    return 0;
  }
  //////////////////////////////////////////////////////////////////////
  // Magier-Escape-Kommandos werden behandelt 
  if (str=="\\ESCAPE" && IS_LORD(this_object()))
  {
    __set_environment(this_object(),"/room/void");
    environment()->init();
    printf("You escaped.\n");
    return "";
  }
  if (str[0..2]=="\\\\\\" && IS_LORD(this_object()))
  {
    str = _return_args(str);
    string* input = explode(str[3..]," ");
    string verb = input[0];
    if (verb && verb!="")
    {
      string cmd = implode(input[1..]," ");
      if (!__auswerten(cmd,verb))
        SoulComm(cmd,verb);
    }
    return 1;
  }
  //////////////////////////////////////////////////////////////////////

  return ::modify_command(str);
}

