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

//#include <player.h>
#include <properties.h>
#include <rooms.h>
#include <wizlevels.h>
#include <defines.h>
#include <moving.h>
#include <new_skills.h>
#include <ansi.h>
#include <notify_fail.h>
#include <sys_debug.h>

#define NEED_PROTOTYPES
#include <container.h>
#include <thing/properties.h>
#include <living/put_and_get.h>
#include <living/description.h>
#include <living/put_and_get.h>
#include <player/command.h>

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

private int select_sense(string str, closure transparent_check)
{
  int sense = -1;
  if(member(({"riech","rieche","schnupper","schnuppere"}),query_verb())!=-1)
  {
    _notify_fail("Du kannst nichts Besonderes riechen.\n");
    sense = SENSE_SMELL;
  }
  else if(member(({"lausche","lausch","hoer","hoere"}),query_verb())!=-1)
  {
    if(QueryProp(P_DEAF))
    {
      notify_fail("Du bist taub!\n");
      return -1;
    }
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
      return -1;
    if (CannotSee(1)) {
      notify_fail("Du kannst nichts sehen!\n", NF_NL_MAX);
      return -1;
    }
    sense = SENSE_READ;
  }
  // else ist normales Sehen/untersuchen
  else
  {
    if (CannotSee(1)) {
      notify_fail("Du kannst nichts sehen!\n", NF_NL_MAX);
      return -1;
    }
    _notify_fail("Was willst Du denn untersuchen?\n");
    if (!str) return -1;
    sense = SENSE_VIEW;
  }
  // "rieche an ..." etc.: das an rausschneiden, wird nicht gebraucht.
  if (sizeof(str))
  {
    if (sense!=SENSE_VIEW)
    {
      string what;
      if (sscanf(str,"an %s",what)==1) str=what;
    }
  }

  switch(sense)
  {
    // Fuer diese Sinne muss der Container offen sein.
    case SENSE_SMELL:
    case SENSE_TOUCH:
    case SENSE_SOUND:
      transparent_check = function int (object o)
         { return o->QueryProp(P_CNT_STATUS) == CNT_STATUS_OPEN;};
      break;
    // Fuer alle anderen reicht, wenn er transparent ist.
    default:
      transparent_check = function int (object o)
         { return o->QueryProp(P_TRANSPARENT);};
  }

  return sense;
}

private object get_ref_object(string *tokens, closure transparent_check,
                              int ref_given)
{
  object ref_object;

  // im Kommando speziell beschrieben ist, wo die Suche losgehen soll, spielt
  // das Referenzobjekt keine Rolle.
  if (sizeof(tokens) > 1 && tokens[<1] == "hier") {
      tokens = tokens[..<2];
      ref_object = environment();
      ref_given=1;
  }
  else if (sizeof(tokens) > 2 && tokens[<2] == "in")
  {
      if (tokens[<1] == "mir" ||
          tokens[<1] == "dir")
      {
          tokens = tokens[..<3];
          ref_object = this_object();
          ref_given=1;
      }
      else if (tokens[<1] == "raum")
      {
          tokens = tokens[..<3];
          ref_object = environment();
          ref_given=1;
      }
  }
  else
  {
    // ansonsten bleibt tokens unmodifiziert und wir nehmen das
    // Referenzobjekt (so existent).
    ref_object = QueryProp(P_REFERENCE_OBJECT);
    if (objectp(ref_object))
    {
      // Dieses muss aber "irgendwo" in unserem Umfeld sein.
      if (ref_object == environment() || ref_object==this_object())
      {
        // Umgebung oder Spieler selber sind als Bezugsobjekt immer in
        // Ordnung, nichts machen.
      }
      // Wenn es nicht wir oder environment ist, darf das Referenzobjekt
      // nicht unsichtbar sein.
      else if (!ref_object->short())
        ref_object = 0;
      // Und nur wenn das Ref-Objekt irgendwie in unserer Umgebung ist
      // (direkt, in uns, in einem Container etc.) kommt es in Frage. Es
      // wird daher geprueft, ob all_environments() des Ref-Objekts auch
      // unser environment enthaelt.
      else
      {
        object *all_envs=all_environment(ref_object) || ({});
        int i=member(all_envs, environment());
        if (i >= 0)
        {
          // dann von dem Environment an entlanglaufen und alle
          // intermediaeren Objekt pruefen, ob sie einsehbar und sichtbar
          // sind.
          foreach(object env : all_envs[..i]) {
            // Ist eine Umgebung Living oder intransparenter Container oder
            // unsichtbar?
            if (living(env) || !funcall(transparent_check, env)
                || !env->short())
            {
              // Kette zum Ref-Objekt unterbrochen. Wenn das unterbrechende
              // Env nicht dieses Living selber oder sein Env ist, wird das
              // Referenzobjekt zusaetzlich genullt.
              if (env != this_object() && env != environment())
                ref_object = 0;
              break;
            }
          }
        }
        else
          ref_object = 0; // nicht im Raum oder Inventory
      }
    } // ref_object exists
  }
  return ref_object;
}

// Versucht ein Basisobjekt (ggf. in env) finden, was durch <tokens>
// beschrieben wird.
// Das Basisobjekt ist das, was bei einem "detail an parent" durch parent
// gekennzeichnet wird oder notfalls das Ref-Objekt selber. Zu beachten ist,
// dass find_base() ggf. rekursiv arbeitet, um sowas wie "riss an kerze in
// kerzenstaender auf tisch" zu finden. detail ist dann der letzte Teil der
// tokens (der Anfang!), was keinen Container mehr bezeichnet, das gefundene
// Objekt ist der letzte Container/Item auf dem Weg zu detail.
// detail muss als Referenz uebergeben werden.
// transparent_check ist die Closure, die prueft, ob man in einen
// Container reinforschen (sehen, lesen, riechen, tasten etc.) kann. Sie
// bekommt den Container uebergeben.
private object find_base(object env, string *tokens, string detail,
                         closure transparent_check)
{
  // zuerst pruefen, ob man in env reinsehen/reinforschen kann (in
  // this_object() und environment() kann man immer reinsehen).
  if  (env && env != this_object() && env != environment())
  {
    // Nicht in Lebewesen reingucken und nicht in intransparente/geschlossene
    // Container
    if (living(env) || !funcall(transparent_check, env))
    {
      // suche nach Objekten in env ist jetzt zuende, weil nicht in env
      // reinsehbar.
      // Aber tokens koennte noch ein detail beschreiben, was der Aufrufer an
      // env findet. Also tokens nach detail und env zurueckgeben.
      detail = implode(tokens, " ");
      return env;
    }
  }
  // ansonsten schauen, ob in tokens noch ein (weiterer) Container beschrieben
  // wird.
  for (int i = sizeof(tokens)-1; i > 1; i--)
  {
    object ob, *objs;
    // In env oder mir oder umgebung gucken, ob ein passendes Objekt da ist.
    // Hierfuer werden mit jedem Schleifendurchlauf von hinten mehr worte aus
    // tokens benutzt. (Beispiel "muenze in boerse 2")
    // present waere schneller und die Komplexitaet von present_objects waere
    // hier nicht noetig. Allerdings beruecksichtigt es die Sichtbarkeit und
    // ggf. virtuell anwesende Objekte, was present nicht tut.
    string suchid=implode(tokens[i..], " ");
    if (env)
        objs = env->present_objects(suchid) || ({});
    else
        objs = environment()->present_objects(suchid) ||
               this_object()->present_objects(suchid);
    // wenn nicht genau ein Objekt gefunden wird, wird weitergesucht, im
    // naechsten Durchlauf mit einem Wort mehr aus tokens
    if (!sizeof(objs))
    {
      continue;
    }
    // falls mehr als ein Objekt gefunden wurden: das erste nehmen, wie
    // ueberall sonst auch.
    ob = objs[0];

    // an dieser Stelle wird (noch) nicht geprueft, ob man in das gefundene
    // Objekt hineinforschen kann.

    // Wenn passende Praeposition davor (z.B. "in" (beutel), "auf" (tisch)),
    // rekursiv weitersuchen, ob der restliche Teil von tokens evtl. nochmal
    // nen container in dem gefundenen Objekt bezeichnet.
    if (tokens[i-1] == ob->QueryProp(P_PREPOSITION))
        return find_base(ob, tokens[..i-2], &detail, transparent_check);

    // Wenn tokens[i-1] "an" oder "am" ist, wird zwar nicht weiter verzweigt,
    // aber vermutlich wird ein Detail an diesem Objekt gesucht, d.h. das
    // Objekt wird env, tokens entsprechend gekuerzt und die Schleife beendet.
    // Bemerkung: haette ob tatsaechlich eine Praeposition an|am gehabt,
    // waeren wir vorher in find_base() verzweigt zum Suchen in dem Container.
    if (tokens[i-1] == "an" || tokens[i-1] == "am") {
      env = ob;
      tokens=tokens[..i-2];
      break;
    }
  }
  // an diesem Punkt sind in tokens nur noch worte drin, die keinen Container
  // mehr beschreiben. Die Resttokens kommen nach <detail>, sie beschreiben
  // dann entweder ein Objekt oder ein Detail.
  detail = implode(tokens, " ");

  // Wenn es ein env gibt, beschreibt detail nun ein Objekt in env oder ein
  // Detail an env, was der Aufrufer prueft.
  // Wenn es aber kein env gibt (das passiert, wenn der Aufrufer dieser
  // Funktion schon kein env (->Referenzobjekt) hereingegeben hat, wurde auch
  // kein Container gefunden, der durch die tokens beschrieben wurde. Dann
  // kann der Aufrufer noch an/in this_object() oder environment() gucken, ob
  // was zu finden ist.
  return env;
}

private nosave int exa_cnt;
private nosave int exa_time;
private nosave string *exa;
private void unt_script_dings(string str)
{
  // unt-script-sucher
  if (!sizeof(str)) return;
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
}

private int _examine_rec(object *ref_objects, string *tokens, closure
                         transparent_check, int sense, int mode,
                         int ref_given)
{
  // ref_objects sind alle in Frage kommenden Kandidaten, an denen wir suchen.

  // Sodann ein Basisobjekt finden. Das Basisobjekt ist das, was bei einem
  // "detail an parent" durch parent gekennzeichnet wird oder notfalls das
  // Ref-Objekt von oben. Zu beachten ist, dass find_base() ggf. rekursiv
  // arbeitet, um sowas wie "riss an kerze in kerzenstaender auf tisch" zu
  // finden. detail ist dann das letzte Teil der tokens, der keinen Container
  // mehr bezeichnet, das gefundene Objekt ist der letzte Container/Item auf
  // dem Weg zu detail. (detail==riss, base==kerze)
  string detail; // detail an parent
  // das fuer parent gefundene objekt
  object base = find_base(ref_objects[0], tokens, &detail, transparent_check);
  // BTW: find_base liefert immer was zurueck - wenn nix gefunden wird, das
  // ref_object, was als Start der Suche uebergeben wurde.
  // Jetzt wird versucht, ein Objekt mit der ID "detail" in base oder
  // ein Detail an base zu finden.

  object *objs;
  // Wenn wir in base reingucken koennen, ermitteln wir alle zu
  // detail in Frage kommenanden Objekte in seinem Inventar
  if  (base == this_object() || base == environment() ||
      (funcall(transparent_check, base) && !living(base)))
  {
    objs = base->locate_objects(detail, 1) || ({});
  }
  else
  {
    // Basisobjekt da, aber nicht reinguckbar, also keine Objekte
    // gefunden. base aber nicht nullen, denn es ist ja noch gueltig fuer
    // Detailsuchen an base...
    objs = ({});
  }
  // Und in jedem Fall werden alle Objekt raussortiert, die unsichtbar sind.
  // ;-)
  objs = filter_objects(objs, "short"); // nur sichtbare...

  // Wenn es sichtbare gibt, werden die ggf. angeguckt.
  if (sizeof(objs))
  {
    // Objekte gefunden, mal schauen, ob die taugen (d.h. fuer den jew. Sinn
    // Infos haben. Wenn nicht, muessen wir weitersuchen.
    // Aber erstmal die Objekte durchlaufen.
    foreach(object ob: objs)
    {
      string out;
      if (sense == SENSE_VIEW)
      {
        out = ob->long(mode);
      }
      else if (sense == SENSE_READ)
      {
        // Extrawurst: P_READ_MSG auch noch abfragen.
        out = ob->QueryProp(P_READ_MSG);
        if (!stringp(out))
          out = ob->GetDetail(SENSE_DEFAULT,QueryProp(P_REAL_RACE),SENSE_READ);
      }
      else 
        out=ob->GetDetail(SENSE_DEFAULT,QueryProp(P_REAL_RACE),sense);
      // Wenn was gefunden wurde, sind wir fertig.
      if (stringp(out))
      {
        SetProp(P_REFERENCE_OBJECT, ob);
        tell_object(ME, out);
        return 1;
      }
    }
  }
  // keine Objekte gefunden oder die hatten nix fuer unseren Sinn
  // dabei. Also nach ordinaeren Details an base/parent suchen.
  string out = base->GetDetail(detail, QueryProp(P_REAL_RACE), sense);
  // Achja, Tueren gibt es ja auch noch. :-o (TODO: die solten vItems werden)
  if (!out && sense==SENSE_VIEW)
    out = base->GetDoorDesc(detail);
  if (out)
  {
    // Detail gefunden, base darf neues Referenzobjekt werden und nach
    // Ausgabe sind wir fertig.
    SetProp(P_REFERENCE_OBJECT, base);
    write(out);
    return 1;
  }
  else
  {
    // wenn auch keine Details gefunden, dann schauen, unser Env evtl.
    // noch nen passendes Detail oder Objekt hat. Das ist aber natuerlich
    // nur der Fall, wenn ref_object nicht eh schon das Environment war.
    // Ausserdem nicht machen, wenn der Nutzer ein spezifisches Referenzobjekt
    // angegeben hat (z.B. "in raum").
    if (sizeof(ref_objects) > 1)
    {
      // Wir schauen uns ggf. weitere Kandidaten an. Bei den anderen
      // Sinnen kein neues notify_fail, weil sonst staendig Meldungen kommen,
      // dass x nicht da ist, nur weil es keine Beschreibung fuer den Sinn hat.
      if (sense==SENSE_VIEW)
      {
        // Ausgabe, was gerade angeguckt hat.
        if (ref_objects[0] == environment())
          out="Sowas siehst Du hier nicht. ";
        else if (ref_objects[0] == this_object())
          out="Sowas siehst Du an Dir nicht. ";
        else
          out="Sowas siehst Du an/in "+ref_objects[0]->name(WEM) +" nicht. ";
        // Ausgabe, was man als naechstes anguckt.
        if (ref_objects[1] == environment())
          out+="Du schaust Dich um.";
        else
          out+="Du schaust Dich an.";

        write(break_string(out,78));
      }
      return _examine_rec(ref_objects[1..], tokens, transparent_check,
                          sense, mode, 0);
    }
    // Leider ist nix mehr uebrig zum angucken und in jedem Fall Ende.
    else
    {
      // Wenn spezifisches ref_object vom Nutzer angegeben (ref_given!=0),
      // gibt es eine leicht andere Meldung und das Bezugsobjekt muss weg.
      if (ref_given) {
        SetProp(P_REFERENCE_OBJECT, 0);
        notify_fail("\'" + capitalize(detail) + "\'"
                    + (sense==SENSE_VIEW ? " siehst " : " findest ")
                    + "Du da nicht!\n");
      }
      // Ansonsten gibt es nur nen passendes notify_fail und Ende.
      else if (sense==SENSE_VIEW) {
        notify_fail("\'" + capitalize(detail) + "\'"
                    + " siehst Du auch da nicht!\n");
      }
      else {
        notify_fail("\'" + capitalize(detail) + "\'"
                    + " findest Du da nicht!\n");
      }
      return 0;
    }
  }
  // Nie erreicht.
  return 0;
}

varargs int _examine(string str, int mode)
{
  // Sinn auswaehlen, der benutzt wird. Wenn -1, abbrechen (kein Sinn bzw.
  // Sinn erlaubt ggf. kein leeren <str> fuer Defaulmeldung). Je nach Sinn
  // wird auch <str> modifiziert.
  // transparent_check enthaelt spaeter die Closure, die entscheidet,
  // ob man mit dem Sinn in einen Container reinforschen kann.
  closure transparent_check;
  int sense=select_sense(&str, &transparent_check);
  if (sense<0) return 0;

  unt_script_dings(str);

  // Wenn kein str, dann SENSE_DEFAULT vom Environment ausgeben. Bei
  // SENSE_VIEW wird das bereits bei den Aufrufern dieser Funktion gemacht und
  // die Beschreibung der Umgebung ausgegeben.
  if (sense!=SENSE_VIEW)
  {
    if (!str)
    {
      string detail = environment()->GetDetail(SENSE_DEFAULT,
                                               QueryProp(P_REAL_RACE),sense);
      if(!detail)
        return 0;
      write(detail);
      return 1;
    }
  }

  string *tokens = explode(str, " ");
  int ref_given;
  // Das Ref-Objekt finden, das kann dann auch in tokens angegeben sein. In dem
  // Fall wird die Liste an tokens um die Angabe des Ref-Objekts gekuerzt und
  // ref_given gesetzt.
  object ref_ob = get_ref_object(&tokens, transparent_check, &ref_given);
  // Bemerkung: Das Referenzobjekt ist garantiert sichtbar, aber nicht
  // unbedingt einsehbar. Wird weiter unten geprueft.

  // Falls nicht explizit ein Objekt als Ref-Objekt angegeben wurde, werden
  // max. 3 Objekte angeguckt: das eigentliche Referenzobjekt, das
  // Environment und das eigene Objekt. Da ref_object auch eines der letzten
  // beiden sein kann, muss die Dublette ggf. raus.
  object *ref_objects;
  if (ref_ob)
  {
    ref_objects=({ref_ob});
    if (!ref_given)
      ref_objects += ({environment(), this_object()}) - ref_objects;

  }
  else
    ref_objects = ({environment(), this_object()});

  return _examine_rec(ref_objects, tokens, transparent_check, sense, mode,
                      ref_given);
}

// Funktion fuer "schau in ..."
varargs int look_into(string str,int mode)
{
  object *found_obs;

  if( CannotSee() ) return 1;
  _notify_fail("Wo willst Du denn reinschauen?\n");
  found_obs=find_objects(str, 0, 0);
  if (!sizeof(found_obs))
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

// Kommandofunktion fuer schau.
// Verzweigt ggf. in _examine() fuers normale Untersuchen.
int _look(string str)
{
  string s;
  int flag;

  if(CannotSee()) return 1;

  // nur schau mit ggf. Flags entsorgt das Ref-Objekt und schaut die
  // Raumbeschreibung an.
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
  // Ansonsten wird in _examine() weitergemacht, wobei Flags vorher
  // rausgeschnitten werden. _examine() benutzt die Flags, falls es eine
  // Langbeschreibung eines Objektes ausgeben wird.
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
  // "in mir", "in dir" soll in _examine rein, aber "in ..." in look_into().
  if (sscanf(str,"%s in mir",s)||sscanf(str,"%s in dir",s)) return _examine(str,flag);
  if (sscanf(str,"in %s",s)) return look_into(s,flag);
  // Alles andere weiter an _examine().
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
      ({"lausche","_examine",0,0}),
      ({"lausch","_examine",0,0}),
      ({"hoer","_examine",0,0}),
      ({"hoere","_examine",0,0}),
      ({"lies","_examine",0,0}),
      ({"lese","_examine",0,0}),
      ({"les","_examine",0,0}),
      ({"schnupper","_examine",0,0}),
      ({"schnuppere","_examine",0,0}),
      ({"riech","_examine",0,0}),
      ({"rieche","_examine",0,0}),
      ({"taste","_examine",0,0}),
      ({"beruehre","_examine",0,0}),
      ({"beruehr","_examine",0,0}),
      ({"kurz","_toggle_brief",0,0}),
      ({"lang","_toggle_brief",0,0}),
      ({"ultrakurz","_toggle_brief",0,0}) 
    });
}
