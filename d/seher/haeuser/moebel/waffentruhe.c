//--------------------------------------------------------------------------------
// Name des Objects:    Waffentruhe
// Letzte Aenderung:    22.03.2001
// Magier:              Swift
//--------------------------------------------------------------------------------
/* Changelog
   * 21.05.2007, Zesstra
     Da Waffenschrank und Waffentruhe sich praktisch nicht unterscheiden, bis
     auf 10 Zeilen im Create, erbt nun die Waffentruhe den Waffenschrank und
     ueberschreibt einfach die passenden Props.
*/
#pragma strong_types,rtt_checks

#include "schrankladen.h"
#include <ansi.h>
#include <class.h>
//inherit LADEN("swift_std_container");
inherit LADEN("waffenschrank");

#define VERSION_OBJ "7"

protected void create() {
  if (!clonep(TO)) return;
  ::create();
  SetProp(P_SHORT, "Eine Waffentruhe");
  SetProp(P_LONG, BS(
     "Diese Stahltruhe ist speziell zur Aufnahme von grossen Waffenmengen "
     "gefertigt. Verschiedene Faecher fuer die einzelnen Waffentypen erleichtern "
     "die Suche nach Deinem bevorzugten Waffentyp.")+"@@cnt_status@@");
  RemoveId(({"waffenschrank", "holzschrank", "schrank"}));
  AddId(({"waffentruhe", "stahltruhe", "truhe"}));
  SetProp("cnt_version_obj", VERSION_OBJ);
  SetProp(P_NAME, "Waffentruhe");
  SetProp(P_GENDER, FEMALE);  
  SetProp(P_MATERIAL, ({MAT_MISC_METAL, MAT_MISC_MAGIC}) );
  SetProp(P_INFO, "Versuchs mal mit: \"skommandos "+QueryProp(P_IDS)[1]+
    "\" !\n");
  AD(({"faecher"}), BS("Fuer jeden Waffentyp gibt es ein eigenes Fach, so dass "
    "Du eine bessere Uebersicht hast."));
  AD(({"platz"}), BS("Die Waffentruhe bietet Platz fuer jede erdenkliche "
	"Waffengattung."));
}

