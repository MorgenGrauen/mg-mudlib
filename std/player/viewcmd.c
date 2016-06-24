// MorgenGrauen MUDlib
//
// player/viewcmd.c -- player view command handling
//
// $Id: viewcmd.c 9548 2016-04-17 19:28:22Z Zesstra $

#pragma strong_types
#pragma save_types
#pragma range_check
#pragma no_clone
#pragma pedantic

#define NEED_PROTOTYPES
#include "/sys/thing/properties.h"
#include "/sys/living/put_and_get.h"
#include "/sys/living/description.h"

#include <container.h>
#include <player.h>
#include <properties.h>
#include <rooms.h>
#include <wizlevels.h>
#include <defines.h>
#include <moving.h>
#include <new_skills.h>
#include <ansi.h>

#include <sys_debug.h>

varargs mixed More(string str, int fflag, string returnto);

void create()
{
  Set(P_BRIEF, SAVE, F_MODE);
  Set(P_BLIND, SAVE, F_MODE);
}

int _toggle_brief()
{
  int brief;

  if (query_verb()=="kurz")
    brief=1;
  else if (query_verb()=="ultrakurz")
    brief=2;
  else brief=0;
  SetProp(P_BRIEF, brief);
  write("Du bist nun im \""+
        (brief?(brief==1?"Kurz":"Ultrakurz"):"Lang")+"\"modus.\n");
  return 1;
}

private int sortinv(mixed a, mixed b) { return a[0] > b[0]; }

private string collectinv(mixed obj)
{
  if(obj[0]=="") return 0;
  return (obj[2] ? " " : "")
       + obj[0]
       + (obj[1] > 1 ? " ("+obj[1]+")" : "");
}

#define I_AUTOLOAD      1
#define I_KEEP          4
#define I_FORMATTED     16
#define I_ARMOUR        64
#define I_SORT          256
#define I_WEAPON        1024
#define I_FORCE_SORT    4096
#define I_NO_TABLE      16384

private string getflags(string arg, int flags)
{
  int no, i;
  if(sizeof(arg) < 2) return 0;
  no = (arg[0] == '-');

  for(i = 1; i < sizeof(arg); i++)
  {
    switch(arg[i])
    {
    case 'a': flags |= I_AUTOLOAD << no; break;
    case 'b': flags |= I_KEEP << no; break;
    case 'f': flags |= I_FORMATTED << no; break;
    case 'r': flags |= I_ARMOUR << no; break;
    case 's': flags |= I_SORT << no; break;
    case 'w': flags |= I_WEAPON << no; break;
    case 'v': flags |= (I_ARMOUR | I_WEAPON) << !no; break;
    case '1': flags |= I_NO_TABLE; break;
        // Die Option macht nur Aerger und kommentiert ist sie eh nicht.
        // Wer das dringend braucht, soll Wargons Schiebepuzzle benutzen.
        //
        // Tiamak, 15.10.2000
        //    case 'S': flags |= I_FORCE_SORT << no; break;
    default : return arg[i..i]; // wird ausgegeben an Spieler als unbekannt.
    }
  }
  return 0;
}

static int _check_keep(object ob)
{
  return (ob->QueryProp(P_KEEP_ON_SELL))==geteuid(ME);
}

int _inventory(string str)
{
  mixed *args, output;
  int ansi, i, flags, minv;
  mixed inventory, weapons, armours, misc;
  string format;

  if(CannotSee()) return 1;

  if((str = _unparsed_args()) && str!="")
  {
    string error;
    error = "Benutzung: i[nventar] [-/+1abfrsvw]\n";
    args = regexp(regexplode(str, "[-+][1abfrswv][1abfrswv]*"),
                  "[-+][1abfrswv][1abfrswv]*");
    if(!sizeof(args)) return (_notify_fail(error), 0);
    if(sizeof(args = map(args, #'getflags/*'*/, &flags) - ({ 0 })))
    {
      printf("%s: Unbekanntes Argument.\n"+error, implode(args, ", "));
      return 1;
    }
  }
  // Fuer Spieler gehen nur sichtbare Objekte in den Algorithmus
  if (IS_LEARNING(ME))  
    inventory = all_inventory(ME);
  else
    inventory = filter_objects(all_inventory(ME), "short");
  
  ansi = member(({"vt100", "ansi"}), QueryProp(P_TTY)) != -1;
  minv = 1 | (flags & (I_FORMATTED | (I_FORMATTED << 1)) ? 2 : 0);
  format = (flags & I_NO_TABLE) ? "=" : "#";

//  if(flags & (I_FORCE_SORT | I_FORCE_SORT << 1))
//  {
//    closure sf;
//    sf = flags & I_FORCE_SORT ? #'> : #'<;
//    s = sort_array(s, lambda(({'a, 'b}),
//                             ({#'funcall, sf,
//                                   ({#'||,({#'call_other,'a,"short"}),""}),
//                                   ({#'||,({#'call_other,'b,"short"}),""})})));
//    map_objects(s, "move", this_object());
//    s = all_inventory(ME);
//  }

  if (flags & I_AUTOLOAD)
    inventory = filter_objects(inventory,"QueryProp",P_AUTOLOADOBJ);
  else if (flags & (I_AUTOLOAD << 1))
    inventory -= filter_objects(inventory,"QueryProp",P_AUTOLOADOBJ);

  if(flags & I_KEEP)
    inventory = filter(inventory,#'_check_keep);
  else if(flags & (I_KEEP << 1))
    inventory -= filter(inventory,#'_check_keep);

  armours = filter_objects(inventory, "QueryProp", P_ARMOUR_TYPE);
  // Kleidung dazu addieren, vorher die erkannten Ruestungen abziehen, die
  // muessen nicht nochmal durchiteriert werden.
  armours += filter_objects(inventory-armours, "IsClothing");
  // Ruestungen werden hier nicht abgezogen, weil es Kram gibt, welche sowohl
  // Ruestung als auch Waffe ist.
  weapons = filter_objects(inventory, "QueryProp", P_WEAPON_TYPE);
  misc = inventory - weapons - armours; // rest ;-)

  if(flags & I_WEAPON)
  {
    inventory = weapons; misc = ({});
    if(!(flags & (I_ARMOUR))) armours = ({});
  }
  if(flags & I_ARMOUR)
  {
     inventory = armours; misc = ({});
     if(!(flags & I_WEAPON)) weapons = ({});
  }
  if(flags & (I_WEAPON << 1)) { weapons = ({}); inventory = armours + misc; }
  if(flags & (I_ARMOUR << 1)) { armours = ({}); inventory = weapons + misc; }

  output = "";
  if(flags & (I_FORMATTED | (I_FORMATTED << 1)))
  {
    inventory = make_invlist(this_player(), inventory, minv);
    if(flags & (I_SORT | (I_SORT << 1)))
    inventory = sort_array(inventory, #'sortinv/*'*/);
    output += sprintf("%"+format+"-78s\n",
                      implode(map(inventory,#'collectinv/*'*/),"\n"));
  }
  else
  {
    if(weapons && sizeof(weapons))
    {
      weapons = make_invlist(this_player(), weapons, minv);
      if(flags & (I_SORT | (I_SORT << 1)))
        weapons = sort_array(weapons, #'sortinv/*'*/);
      output += (ansi?ANSI_BOLD:"") + "Waffen:" + (ansi?ANSI_NORMAL:"")+"\n"
              + sprintf("%"+format+"-78s\n",
                        implode(map(weapons, #'collectinv/*'*/), "\n"));
    }
    if(armours && sizeof(armours))
    {
      armours = make_invlist(this_player(), armours, minv);
      if(flags & (I_SORT | (I_SORT << 1)))
        armours = sort_array(armours, #'sortinv/*'*/);
      output += (ansi?ANSI_BOLD:"") 
              + "Kleidung & Ruestungen:" + (ansi?ANSI_NORMAL:"")+"\n"
              + sprintf("%"+format+"-78s\n",
                        implode(map(armours, #'collectinv/*'*/), "\n"));
    }
    if(misc && sizeof(misc))
    {
      misc = make_invlist(this_player(), misc, minv);
      if(flags & (I_SORT | (I_SORT << 1)))
        misc = sort_array(misc, #'sortinv/*'*/);
      output += (ansi?ANSI_BOLD:"") + "Verschiedenes:" + (ansi?ANSI_NORMAL:"")+"\n"
              + sprintf("%"+format+"-78s\n",
                        implode(map(misc, #'collectinv/*'*/), "\n"));
    }
  }

  // Spielerwunsch: 'inventar' sollte doch das Bezugsobjekt auf den Spieler
  // aendern.
  SetProp(P_REFERENCE_OBJECT, this_object());

  if (output=="") 
    output += (ansi?ANSI_BOLD:"")+"Die Liste ist leer."+(ansi?ANSI_NORMAL:"");
  More(output);
  return 1;
}

private nosave int exa_cnt;
private nosave int exa_time;
private nosave string *exa;

varargs int _examine(string str, int mode)
{
  object base, *objs, env;
  string what, detail, parent, out, error;
  int i, size, done;

  if(CannotSee()) return 1;

  _notify_fail("Was willst Du denn untersuchen?\n");
  if (!str) return 0;

  if (member(({"boden","decke","wand","waende"}),old_explode(str," ")[0]) == -1) {
    exa_cnt -= (time() - exa_time)/2;
    exa_time = time();
    exa_cnt++;
    if (!exa)
      exa = ({ str });
    else
      exa += ({ str });
    if (exa_cnt > 10) {
      log_file("ARCH/LOOK", 
          sprintf("%s: %s in %s\n%@O\n",dtime(time()),getuid(this_object()), 
            environment() ? object_name(environment()) : "???",exa), 150000);
      exa_cnt = 0;
      exa = ({});
    }
    else if (exa_cnt < 0) {
      exa_cnt = 0;
      exa = ({});
    }
  }
  // do we look at an object in our environment ?
  if (sscanf(str,"%s in raum", what) || sscanf(str,"%s im raum", what))
      base = environment();
  // is the object inside of me (inventory)
  else if (sscanf(str,"%s in mir", what) || sscanf(str,"%s in dir", what))
      base = this_object();
  else {
      what = str;
      // get the last object we looked at
      base = QueryProp(P_REFERENCE_OBJECT);
      
      // if a reference object exists, test for its existance in the room
      // or in our inventory
      if (objectp(base))
      {
       if (base == environment() || base==this_object())
        {
          // Umgebung oder Spieler selber sind als Bezugsobjekt immer in
          // Ordnung, nichts machen.
        }
        // Das Referenzobjekt darf nicht unsichtbar sein.
        else if (!base->short())
          base = 0;
        else if(member(deep_inventory(environment()), base) != -1)
        {
          foreach(env : all_environment(base)) {
            // Ist eine Umgebung Living oder intransparenter Container oder
            // unsichtbar?
            if (living(env) || !env->QueryProp(P_TRANSPARENT)
                || !env->short())
            {
              // in dem Fall ist ende, aber wenn das gefundene Env nicht
              // dieses Living selber oder sein Env ist, wird das
              // Referenzobjekt zusaetzlich genullt.
              if (env != this_object() && env != environment())
                base = 0;
              break;
            }
          }
        }
        else
          base = 0; // nicht im Raum oder Inventory
      }
  }

  // scan input if we want a specific object to look at
  if(sscanf(what, "%s an %s", detail, parent) == 2 ||
     sscanf(what, "%s am %s", detail, parent) == 2 ||
     sscanf(what, "%s in %s", detail, parent) == 2 ||
     sscanf(what, "%s im %s", detail, parent) == 2)
  {
    // if an ref object exists get its inventory. (Oben wurde sichergestellt,
    // dass das Referenzobjekt einsehbar ist)
    if(base)
      objs = base->locate_objects(parent, 1) || ({});
    else {
      // else get our inv and env
      objs = environment()->locate_objects(parent, 1)
           + locate_objects(parent, 1);
    }
    objs = filter_objects(objs, "short"); // nur sichtbare...
    if(sizeof(objs) > 1)
      return (notify_fail("Es gibt mehr als eine(n) "+capitalize(parent)+".\n"), 0);
    else
    {
      if (sizeof(objs))
        base = objs[0];
      else
        return (notify_fail("Hier ist kein(e) "+capitalize(parent)+".\n"), 0);
    }
    objs = 0;
  }
  else detail = what;

  int base_was_env = 1;
  do {
    // if a base exists get its inventory, else get our inv and env
    if (base)
    {
      if  (base == this_object() || base == environment() ||
          (base->QueryProp(P_TRANSPARENT) && !living(base)))
      {
        // ich kann in base reingucken...
        objs = base->locate_objects(detail, 1) || ({});
      }
      else
      {
        // Referenzobjekt da, aber nicht reinguckbar. base aber nicht nullen,
        // denn es ist ja noch gueltig fuer Detailsuchen an base...
        objs = ({});
      }
    }
    else
    {
      objs = environment()->locate_objects(detail, 1)
           + locate_objects(detail, 1);
      base = environment();
    }
    objs = filter_objects(objs, "short"); // nur sichtbare...

    if(!sizeof(objs))
    {
      // wenn keine Objekte gefunden wurden, wird nach Details gesucht...
      if((out = base->GetDetail(detail, QueryProp(P_REAL_RACE),SENSE_VIEW)) ||
         (out = base->GetDoorDesc(detail)))
      {
        SetProp(P_REFERENCE_OBJECT, base);
        return (write(out), 1);
      }
      else
      {
        // wenn auch keine Details gefunden, dann schauen, ob Ende ist
        // (base==env) oder wir evtl. noch im env suchen koennen.
        if (base == environment())
        {
          if (base_was_env) {
            // in diesem Fall war das Env das Bezugsobjekt - daher wegwerfen.
            SetProp(P_REFERENCE_OBJECT, 0);
            _notify_fail("Sowas siehst Du da nicht!\n");
          }
          else {
            _notify_fail("Sowas siehst Du auch da nicht!\n");
            // in diesem Fall war nicht das Env das Bezugsobjekt - es soll
            // behalten und nicht geloescht werden.
          }
          return 0;
        }
        else {
          base_was_env=0;
          write(break_string("Du findest an "+base->name(WEM)
                +" kein \"" + capitalize(detail) + "\"."
                 " Dein Blick wendet sich der Umgebung zu.",78));
          base = 0;
        }
      }
    }
    else  // Objekte gefunden!
      done = 1;
  } while(!done);

  // Es muss min. ein (sichtbares) Objekt geben, sonst waere man hier nicht
  // hingekommen.
  object ob = objs[0];
  SetProp(P_REFERENCE_OBJECT, ob);
  tell_object(ME, ob->long(mode));
  return 1;
}

varargs int _sense_exa(string str)
{
  object base, *objs, env;
  string what, detail, parent, out, error;
  int sense;

  if(member(({"riech","rieche","schnupper","schnuppere"}),query_verb())!=-1)
  {
    _notify_fail("Du kannst nichts Besonderes riechen.\n");
    sense = SENSE_SMELL;
  }
  else if(member(({"lausche","lausch","hoer","hoere"}),query_verb())!=-1)
  {
    if(QueryProp(P_DEAF))
      return notify_fail("Du bist taub!\n"), 0;

    _notify_fail("Du kannst nichts Besonderes hoeren.\n");
    sense = SENSE_SOUND;
  }
  else if(member(({"taste","beruehre","beruehr"}),query_verb())!=-1)
  {
    sense = SENSE_TOUCH;
    // ein "ab" ganz am Ende von str wird abgeschnitten, es soll sowohl "taste
    // x ab" als auch "taste x" funktionieren.
    if (str) {
      _notify_fail("Sowas kannst Du hier nicht ertasten!\n");
      string *tmp = explode(str," ");
      if (sizeof(tmp) > 1 && tmp[<1] == "ab")
        str = implode(tmp[0..<2], " ");
    }
    else
      _notify_fail("Was willst Du denn abtasten?\n");
  }
  else if (member(({"lies","lese","les"}), query_verb()) > -1)
  {
    _notify_fail("Was willst Du lesen?\n");
    if ( !str ) // Kein SENSE_DEFAULT zulassen.
      return 0;
    if (this_object()->CannotSee()) {
      notify_fail("Du kannst nichts sehen!\n");
      return 0;
    }
    sense = SENSE_READ;
  }

  if (!str) {
    if(!detail =
        environment()->GetDetail(SENSE_DEFAULT,QueryProp(P_REAL_RACE),sense))
      return 0;
    write(detail);
    return 1;
  }
  else if(sscanf(str,"an %s",what)==1)
    str=what;

  // do we look at an object in our environment ?
  if (sscanf(str,"%s in raum", what) || sscanf(str,"%s im raum", what))
      base = environment();
  // is the object inside of me (inventory)
  else if (sscanf(str,"%s in mir", what) || sscanf(str,"%s in dir", what))
      base = this_object();
  else {
      what = str;
      // get the last object we looked at
      base = QueryProp(P_REFERENCE_OBJECT);
      
      // if a reference object exists, test for its existance in the room
      // or in our inventory
      if (objectp(base))
      {
        if (base == environment() || base==this_object())
        {
          // Umgebung oder Spieler selber sind als Bezugsobjekt immer in
          // Ordnung, nichts machen.
        }
        // Das Referenzobjekt darf nicht unsichtbar sein.
        else if (!base->short())
          base = 0;
        else if(member(deep_inventory(environment()), base) != -1)
        {
          foreach(env : all_environment(base)) {
            // Ist eine Umgebung Living oder intransparenter Container oder
            // unsichtbar?
            if (living(env) || !env->QueryProp(P_TRANSPARENT)
                || !env->short())
            {
              // in dem Fall ist ende, aber wenn das gefundene Env nicht
              // dieses Living selber oder sein Env ist, wird das
              // Referenzobjekt zusaetzlich genullt.
              if (env != this_object() && env != environment())
                base = 0;
              break;
            }
          }
        }
        else
          base = 0; // nicht im Raum oder Inventory
      }
  }

  // scan input if we want a specific object to look at
  if(sscanf(what, "%s an %s", detail, parent) == 2 ||
     sscanf(what, "%s am %s", detail, parent) == 2 ||
     sscanf(what, "%s in %s", detail, parent) == 2 ||
     sscanf(what, "%s im %s", detail, parent) == 2)
  {
    // if an ref object exists get its inventory. (Oben wurde sichergestellt,
    // dass das Referenzobjekt einsehbar ist)

    if(base)
      objs = base->locate_objects(parent, 1) || ({});
    else
    {
      // else get our inv and env
      objs = environment()->locate_objects(parent, 1)
           + locate_objects(parent, 1);
    }
    objs = filter_objects(objs, "short"); // nur sichtbare...
    if(sizeof(objs) > 1)
      return (notify_fail("Es gibt mehr als eine(n) "+capitalize(parent)+".\n"), 0);
    else
    {
      if(sizeof(objs))
          base = objs[0];
      else
          return (notify_fail("Hier ist kein(e) "+capitalize(parent)+".\n"), 0);
    }
    objs = 0;
  }
  else detail = what;

  // wie auch immer haben wir jetzt ein Bezugsobjekt.
  int maxtries=3;
  do {
    int base_was_env=1;
    // als ersten werden in Frage kommende Objekte gesucht. Wenn base
    // existiert (idR nur im ersten Durchlauf), wird dort gesucht, sonst in
    // Env und Inv.
    if (base)
    {
      if  (base == this_object() || base == environment() ||
          (base->QueryProp(P_TRANSPARENT) && !living(base)))
      {
        // ich kann in base reingucken...
        objs = base->locate_objects(detail, 1) || ({});
      }
      else
      {
        // Referenzobjekt da, aber nicht reinguckbar. base aber nicht nullen,
        // denn es ist ja noch gueltig fuer Detailsuchen an base...
        objs = ({});
      }
    }
    else
    {
      objs = environment()->locate_objects(detail, 1)
           + locate_objects(detail, 1);
      base = environment();
    }
    objs = filter_objects(objs, "short"); // nur sichtbare...

    if (sizeof(objs))
    {
      // Objekte gefunden, mal schauen, ob die taugen (d.h. fuer den jew. Sinn
      // Infos haben. Wenn nicht, muessen wir weitersuchen.
      // Aber erstmal die Objekte durchlaufen.
      foreach(object ob: objs)
      {
        if (sense == SENSE_READ)
        {
          // Extrawurst: P_READ_MSG auch noch abfragen.
          out = ob->QueryProp(P_READ_MSG);
          if (!stringp(out))
            out = ob->GetDetail(SENSE_DEFAULT,QueryProp(P_REAL_RACE),SENSE_READ);
        }
        else 
          out=ob->GetDetail(SENSE_DEFAULT,QueryProp(P_REAL_RACE),sense);
        if (stringp(out))
        {
          SetProp(P_REFERENCE_OBJECT, ob);
          tell_object(ME, out);
          return 1;
        }
      }
    }

    // Keine Objekte gefunden, die in Frage kommen. Nach Details suchen.
    if(out = base->GetDetail(detail, QueryProp(P_REAL_RACE),sense))
    {
      SetProp(P_REFERENCE_OBJECT, base);
      return (write(out), 1);
    }
    else
    {
      // Auch keine Details gefunden... Wenn wir uns noch das Env angucken
      // koennen (weil base != env), dann machen wir das, ansonsten ist
      // jetzt hier leider Ende...
      if(base == environment())
      {
        if (base_was_env)
          SetProp(P_REFERENCE_OBJECT, 0);
        return 0;
      }
      else
      {
        // nochmal im naechsten Schleifendurchlauf ohne base probieren.
        base = 0;
        base_was_env = 0; 
      }
    }
  } while(--maxtries);

  // nach dieser Schleife sollte man nie ankommen...
  raise_error(sprintf("_sense_exa(): zuviele Versuche, etwas zu finden."));

  return 0;
}

varargs int look_into(string str,int mode)
{
  object *found_obs;

  if( CannotSee() ) return 1;
  _notify_fail("Wo willst Du denn reinschauen ?\n");
  found_obs=find_obs(str,PUT_GET_NONE);
  if (!found_obs)
  {
    if (environment() &&
        (environment()->GetDetail(str,QueryProp(P_REAL_RACE))||
         environment()->GetDoorDesc(str)))
      _notify_fail("Da kannst Du so nicht reinsehen.\n");
    return 0;
  }
  return _examine(str, mode);
}

/* Gebe die Umgebung des aktiven Spielers zurueck, lasse dabei  */
/* rekursiv geschachtelte Raeume zu.                            */
/* Wenn allow_short 0 ist, so wird immer die long-descr benutzt */
varargs string env_descr(int allow_short,int flags, int force_short )
{
  object env;
  int brief;

  env = environment(ME);

  if(!env)
    return "Du schwebst im Nichts ... Du siehst nichts, rein gar nichts ...\n";

  if (!force_short && (!allow_short || !QueryProp(P_BRIEF)))
      return env->int_long(ME,ME,flags);

  if (!flags && ((brief=QueryProp(P_BRIEF))>=2))
      return "";

  return env->int_short(ME,ME);
}

int _look(string str)
{
  string s;
  int flag;

  if(CannotSee()) return 1;

  if(!str)
  {
    SetProp(P_REFERENCE_OBJECT, 0);
    write( env_descr() );
    return 1;
  }
  if(str=="-f" || str=="genau")
  {
    SetProp(P_REFERENCE_OBJECT, 0);
    write( env_descr(0,2) );
    return 1;
  }
  if(str=="-k" || str=="kurz")
  {
    SetProp(P_REFERENCE_OBJECT, 0);
    write( env_descr(1,2,1) );
    return 1;
  }
  if(str[0..2]=="-f "){
    flag=2;
    str=str[3..];
  }
  else if(str[0..5]=="genau "){
    flag=2;
    str=str[6..];
  }
  else flag = 0;
  if (sscanf(str,"%s an",s)) str=s;
  if (sscanf(str,"%s in mir",s)||sscanf(str,"%s in dir",s)) return _examine(str,flag);
  if (sscanf(str,"in %s",s)) return look_into(s,flag);
  return _examine(str,flag);
}

int _equipment(string arg)
{
  if (CannotSee()) return 1;
  call_other("/std/player/invmaster/invmaster", "ShowInv", ME, arg);
  return 1;
}

static mixed _query_localcmds()
{
  return
    ({({"ausruestung", "_equipment",0,0}),
      ({"i","_inventory",0,0}),
      ({"inv","_inventory",0,0}),
      ({"inventur","_inventory",0,0}),
      ({"schau","_look",0,0}),
      ({"schaue","_look",0,0}),
      ({"unt","_examine",0,0}),
      ({"untersuch","_examine",0,0}),
      ({"betracht","_examine",0,0}),
      ({"untersuche","_examine",0,0}),
      ({"betrachte","_examine",0,0}),
      ({"betr","_examine",0,0}),
      ({"lausche","_sense_exa",0,0}),
      ({"lausch","_sense_exa",0,0}),
      ({"hoer","_sense_exa",0,0}),
      ({"hoere","_sense_exa",0,0}),
      ({"lies","_sense_exa",0,0}),
      ({"lese","_sense_exa",0,0}),
      ({"les","_sense_exa",0,0}),
      ({"schnupper","_sense_exa",0,0}),
      ({"schnuppere","_sense_exa",0,0}),
      ({"riech","_sense_exa",0,0}),
      ({"rieche","_sense_exa",0,0}),
      ({"taste","_sense_exa",0,0}),
      ({"beruehre","_sense_exa",0,0}),
      ({"beruehr","_sense_exa",0,0}),
      ({"kurz","_toggle_brief",0,0}),
      ({"lang","_toggle_brief",0,0}),
      ({"ultrakurz","_toggle_brief",0,0}) 
    });
}
