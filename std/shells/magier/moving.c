// MorgenGrauen MUDlib
//
// moving.c
//
// $Id: moving.c 9142 2015-02-04 22:17:29Z Zesstra $
#pragma strict_types
#pragma save_types
#pragma range_check
#pragma no_clone

protected functions virtual inherit "/std/util/path";

#define NEED_PROTOTYPES
#include <magier.h>
#include <thing/properties.h>
#include <living/moving.h>
#include <player.h>
#undef NEED_PROTOTYPES
#include <wizlevels.h>
#include <moving.h>
#include <properties.h>
#include <files.h>

private mixed verfolger()
{
  mixed *pur;

  if (!pointerp(pur=QueryProp(P_PURSUERS))) return 0;
  else return pur[0];
}

//                         #########
//########################## IN+AT ############################
//                         #########

static int _move_base(object target, object old_room, string cmd)
{
  if (environment()!=target)
    if (IS_ARCH(this_object()))
    {
      __set_environment(this_object(),target);
      printf("%s: Bewegung hat nicht geklappt. Versuche es mit "
             "set_environment... %s.\n.",query_verb(),
             environment()==target?"hat geklappt":"erfolglos");
    }
  else
    printf("%s: Bewegung hat nicht geklappt.\n",query_verb());
  if (environment()!=target) return 1;
  command(cmd);
  if (old_room) move_object(old_room);
  else
    return printf("%s: Ursprungsraum wurde zerstoert.\n",query_verb()),1;  
  if (environment()!=old_room)
  {
    if (IS_ARCH(this_object()))
    {
      __set_environment(this_object(),old_room);
      printf("%s: Zurueckbewegen hat nicht geklappt. Versuche es mit "
             "set_environment ... %s.\n",query_verb(),
             environment()==old_room?"hat geklappt":"erfolglos");
    }
    else
      printf("at: Zurueckbewegen hat nicht geklappt.\n");
  }
  return 1;
}

static int _in_room(string str)
{
  string room;int size;
  object old_room;
  string cmd,err;

  if (!sizeof(str=_unparsed_args()) ||
      !sizeof(str=regreplace(str,"^ *","",1)) ||
      sscanf(str, "%s %s", room, cmd) != 2)
    return USAGE("in <raum> <befehl>\n");
  old_room = environment();
  room=normalize_path(room, getuid(), 1);
  if (err=catch(move_object(room)))
  {
    if (catch(size=file_size(room+".c"))||size < 0)
      printf("%s: %s.c: Datei nicht vorhanden.\n",query_verb(),room);
    else
      printf("%s: Bewegung nach %s hat nicht funktioniert: %s\n",
             query_verb(),room,err);
    return 1;
  }
  return _move_base(find_object(room),old_room,cmd);
}

static int _at_player(string dest)
{
  object ob,old_room;
  mixed tmp;
  string cmd;

  if (!sizeof(dest=_unparsed_args()) ||
      !sizeof(dest=regreplace(dest,"^ *","",1)) ||
      sscanf(dest, "%s %s", dest, cmd) != 2)
    return USAGE("at <lebewesen> <befehl>\n");
  if (!(ob=find_living(dest)))
  {
    tmp=match_living(dest,1);
    if (stringp(tmp)) ob = find_living(tmp);
  }
  if (!ob||!(ob=environment(ob)))
    return _notify_fail(sprintf("at: Lebewesen \'%s\' nicht gefunden.\n",
                                dest)),0;
  old_room=environment();
  move_object(ob);
  return _move_base(ob,old_room,cmd);
}

//                           ########
//############################ GOTO ############################
//                           ########

static object find_living_nr(string str)
{ string name,check;
  int nr;
  object*livings;
  if(sscanf(str,"%s %d%s",name,nr,check)<2||sizeof(check))
    return find_living(str);
  if(!sizeof(livings=filter((find_livings(name)||({})),#'environment))
     ||nr<1||sizeof(livings)<nr)
    return 0;
  return livings[nr-1];
}

static int _goto(string dest)
{
  string|object target;

  dest = _unparsed_args();
  if(!sizeof(dest))
    return USAGE("goto [lebewesen|filename]\n");
  int flags;
  dest = parseargs(dest, &flags, GOTO_OPTS, 0)[0];
  if((flags & GOTO_L) && (flags & GOTO_R))
  {
    notify_fail(
      "goto: -r und -l koennen nicht gleichzeitig verwendet werden.\n");
    return 0;
  }
  if(!(flags & GOTO_R))
  {
    // Zuerst nur Spieler suchen, wenn keine existieren auch andere Livings.
    object lv = (find_player(dest) || find_living_nr(dest));
    if(objectp(lv) && environment(lv))
    {
      target = environment(lv);
    }
    else
    {
      notify_fail(
        "goto: Kein Lebewesen mit dem Namen " + dest + " gefunden.\n");
    }
  }
  if(!(flags & GOTO_L) && !target)
  {
    // Kein passendes Living, jetzt Raeume suchen.
    string path = normalize_path(dest, getuid(), 1);
    target = find_object(path);
    if(!target)
    {
      notify_fail(sprintf("goto: Datei %O nicht vorhanden.\n",path));
      // ggf. .c dranhaengen
      if (path[<2..<1]!=".c") path+=".c";
      // Erst schauen, ob die Datei existiert, falls nicht pruefen, ob im
      // gleichen Verzeichnis ein VC vorhanden ist, der sie liefern koennte.
      // Die Ueberpruefung ob das File moeglicherweise Ladbar sein koennte
      // vor load_object() dient einer spezifischeren Fehlermeldung.
      // Die Variable vc dient nur dazu, weniger Zeilenumbrueche in
      // der if-Abfrage zu haben und sie damit besser lesbar zu machen.
      string vc;
      vc = implode(explode(path, "/")[0..<2], "/") + "/virtual_compiler.c";
      if(file_size(path) > FSIZE_NOFILE ||
        (file_size(vc) > FSIZE_NOFILE) &&
        ({int})vc->QueryValidObject(path))
      {
        string err = catch(target = load_object(path));
        if(stringp(err))
        {
           notify_fail(sprintf("goto: Fehler beim Teleport nach %O:\n%s\n",
                    dest,implode(explode(err,"\n")," ")));
        }
      }
      else
      {
        // Kein Ziel gefunden.
        return 0;
      }
    }
  }
  if(!target)
    return 0;
  if (verfolger()) _verfolge("");
  if (move(target,M_TPORT|M_NOCHECK) != MOVE_OK)
    printf("Bewegung fehlgeschlagen!\n");
  return 1;
}

//                           ########
//############################ HOME ############################
//                           ########

static int _home()
{
  string dest;
  if (verfolger()) _verfolge("");
  dest="/players/" + getuid() + "/workroom";
  if (file_size(dest+".c")<0||catch(load_object(dest)))
  {
    printf("Fehler beim Laden Deines Workrooms.\n"
           "Gehe zum Magiertreff.\n");
    dest="/secure/merlin";
  }

  if (move(dest,M_TPORT|M_NOCHECK)<0)
    printf("Bewegung fehlgeschlagen!\n");
  return 1;
}

//                        ###############
//######################### +MAGIERNAME ##########################
//                        ###############

static int _go_wiz_home(string str)
{
  _notify_fail("Syntax: '+magiername'\n");
  if(sizeof(query_verb())>1) str=query_verb()[1..];
  if(!sizeof(str)) return 0;
  if(query_verb()[0]!='+') return 0;
  str=(old_explode(str," ")-({0}))[0];
  if(!sizeof(str)) return 0;
  str=lower_case(str);
  if (str=="merlin")
  {
    move("/secure/merlin",M_TPORT|M_NOCHECK);
    return 1;
  }
  if (!IS_LEARNER(str))
  {
    printf("Es gibt keinen Magier namens %s.\n",
           capitalize(str));
    return 1;
  }
  if (file_size("/players/"+str+"/workroom.c")<0)
  {
    printf("%s hat keinen Workroom.\n",capitalize(str));
    return 1;
  }
  if (catch(load_object("/players/"+str+"/workroom")))
  {
    printf("Der Workroom von %s hat Fehler.\n",capitalize(str));
    return 1;
  }
  move("/players/"+str+"/workroom",M_TPORT|M_NOCHECK);
  return 1;
}


static mixed _query_localcmds()
{
  return
    ({({"goto","_goto",0,LEARNER_LVL}),
      ({"in","_in_room",0,LEARNER_LVL}),
      ({"at","_at_player",0,LEARNER_LVL}),
      ({"home","_home",0,WIZARD_LVL}),
      ({"+","_go_wiz_home",1,LEARNER_LVL})});
}
