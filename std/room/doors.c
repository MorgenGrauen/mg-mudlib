// MorgenGrauen MUDlib
//
// room/doors.c -- new doors, managed by doormaster
//
// $Id: doors.c 9134 2015-02-02 19:26:03Z Zesstra $

#pragma strong_types
#pragma save_types
#pragma pedantic
#pragma range_check
#pragma no_clone

#include <config.h>
#include <properties.h>
#include <defines.h>
#include <language.h>
#include <doorroom.h>
#define NEED_PROTOTYPES
#include <thing/properties.h>
#include <room/exits.h>

protected void create()
{
  if (object_name(this_object()) == __FILE__[0..<3])
  {
    set_next_reset(-1);
    return;
  }
  SetProp(P_DOOR_INFOS,0);
}

protected void create_super() {
  set_next_reset(-1);
}

varargs int NewDoor(string|string* cmds, string dest, string|string* ids,
                    mapping|<int|string|string*>* props)
{
/*
  cmds: Befehl(e), um durch die Tuer zu gehen (String oder Array von Strings)
  dest: Zielraum
  ids:  Id(s) der Tuer, default "tuer" (String, Array von Strings oder 0)
  props: besondere Eigenschaften der Tuer (optional)
  Array mit Paaren Nummer der Eigenschaft, Inhalt
  definierte Properties sind:
  D_FLAGS:  Flags wie bei Sir's Tueren
  default: DOOR_CLOSED | DOOR_RESET_CL
  Bei Schluesseln wird getestet, ob der String, den
  QueryDoorKey zurueckliefert, gleich
  "raumname1:raumname2" ist, wobei raumname1,2 die
  kompletten Filenamen der beiden Raeume in sortierter
  Reihenfolge sind.
  D_LONG:   lange Beschreibung der Tuer
  default: "Eine Tuer.\n"
  D_SHORT:  kurze Beschreibung der Tuer, wird an die Raumbeschreibung
  angefuegt, wobei %s durch geoeffnet bzw. geschlossen
  ersetzt wird.
  default: "Eine %se Tuer. "
  D_NAME:   Name, der beim Oeffnen/Schliessen und bei Fehlermeldungen
  angezeigt wird.
  default: "Tuer"
  D_GENDER: default: FEMALE
  D_FUNC:   Funktion, die im Raum aufgerufen werden soll, wenn die
  Tuer erfolgreich durchschritten wird.
  default: 0
  D_MSGS:   Falls String: ausgegebene Richtung fuer move
  Falls Array: ({direction, textout, textin}) fuer move
  default: 0

  Beispiel:
  NewDoor("norden","/players/rochus/room/test2","portal",
  ({D_NAME,"Portal",
  D_GENDER,NEUTER,
  D_SHORT,"Im Norden siehst Du ein %ses Portal. ",
  D_LONG,"Das Portal ist einfach nur gigantisch.\n"
  }));

*/

  if (!cmds || !dest) return 0;
  return call_other(DOOR_MASTER,"NewDoor",cmds,dest,ids,props);
}

void init()
{
  mixed *info;
  string *cmds;
  int i,j;

  if (!pointerp(info=(mixed *)QueryProp(P_DOOR_INFOS))) return;
  add_action("oeffnen","oeffne");
  add_action("schliessen","schliesse");
  add_action("schliessen","schliess");
  for (i=sizeof(info)-1;i>=0;i--) {
    cmds=(string *)(info[i][D_CMDS]);
    for (j=sizeof(cmds)-1;j>=0;j--)
      add_action("go_door",cmds[j]);
    // Befehle IMMER anfuegen, gechecked wird sowieso erst beim Durchgehen.
  }
}

void reset()
{
  if (QueryProp(P_DOOR_INFOS))
    call_other(DOOR_MASTER,"reset_doors");
}

int oeffnen (string str)
{
  if (!str || !QueryProp(P_DOOR_INFOS))
    return 0;
  return (int) call_other(DOOR_MASTER,"oeffnen",str);
}

int schliessen (string str)
{
  if (!str || !QueryProp(P_DOOR_INFOS))
    return 0;
  return (int) call_other(DOOR_MASTER,"schliessen",str);
}

varargs int
go_door (string str)
{
  if (!QueryProp(P_DOOR_INFOS))
    return 0;
  if (call_other(DOOR_MASTER,"go_door",query_verb()))
    return 1;
  return 0;
}

int set_doors(string *cmds,int open)
{
  int j;
  
  if (!previous_object())
    return 0;
  if (object_name(previous_object())!=DOOR_MASTER)
    return 0;
  // Andere sollen nicht rumpfuschen.
  if (!this_player()) return 0;
  if (environment(this_player())!=this_object())
    return 0;
  // Ist sowieso keiner da...
  if (!pointerp(cmds))
    return 0;
  if (open)
    AddSpecialExit(cmds,"go_door");
  else
    RemoveSpecialExit(cmds);
  for (j=sizeof(cmds)-1;j>=0;j--)
    add_action("go_door",cmds[j]);
  return 1;
}

/* Fuer Tueren, die flexible Langbeschreibungen haben, wird ein 
 * SpecialDetail eingefuehrt.
 */

string special_detail_doors(string key){
  return DOOR_MASTER->special_detail_doors(key);
}
