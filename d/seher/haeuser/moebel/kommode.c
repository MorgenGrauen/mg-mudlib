//--------------------------------------------------------------------------------
// Name des Objects:    Kommode
// Letzte Aenderung:    17.03.2001
// Magier:              Swift
//--------------------------------------------------------------------------------
#pragma strong_types,rtt_checks

#include "schrankladen.h"
inherit LADEN("swift_std_container");

#define VERSION_OBJ "3"

protected void create()
{
  if (!clonep(TO)) return;
  swift_std_container::create();
  SetProp(P_SHORT, "Eine Kommode");
  SetProp(P_LONG, BS(
     "Diese robuste Kommode aus Eichenholz passt einfach in jeden Raum. "
    +"Man kann jede Menge Zeug darin verstauen, und die vielen Schubladen "
    +"lassen es einfach verschwinden. So ein Moebelstueck gehoert einfach "
    +"in jedes Haus!")+"@@cnt_status@@");
  AddId(({"kommode"}));
  SetProp("cnt_version_obj", VERSION_OBJ);
  SetProp(P_NAME, "Kommode");
  SetProp(P_GENDER, FEMALE);
  SetProp(P_WEIGHT, 5000);         // Gewicht 5 Kg
  SetProp(P_MAX_WEIGHT, 1000000);  // Es passen fuer 1000 kg Sachen rein.
  SetProp(P_WEIGHT_PERCENT, 100);  // Dafuer macht er auch nix leichter :)
  SetProp(P_MAX_OBJECTS, 100);     // Mehr sollte nicht sein, lt. Tiamak.
  SetProp(P_VALUE, 0);             // Kein materieller Wert. Ist eh nicht verkaufbar.
  SetProp(P_NOBUY, 1);             // Wird im Laden zerstoert, falls er verkauft wird.
  SetProp(P_NOGET, "Das geht nicht. "+Name(WER,1)+" haftet wie magisch am Boden.\n");
  SetProp(P_MATERIAL, ({MAT_OAK, MAT_MISC_MAGIC}) );
  SetProp(P_INFO, "Versuchs mal mit: \"skommandos "+QueryProp(P_IDS)[1]+"\" !\n");

  AD(({"schubladen"}), BS("Es sind viele, aber zum Glueck musst Du sie nicht "
    +"einzeln ansprechen. Du kannst einfach alles in "+name(WEN,1)+" stecken."));
  AD(({"moebelstueck"}), BS(Name(WER,1)+" ist gemeint."));
}
