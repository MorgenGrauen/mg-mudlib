//--------------------------------------------------------------------------------
// Name des Objects:    Kuehlschrank
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
  SetProp(P_SHORT, "Ein Kuehlschrank");
  SetProp(P_LONG, BS(
     "Dieser strahlendweisse Kuehlschrank der Firma Brosch ueberzeugt durch "
    +"ein leises Summen von seiner Funktion, auch wenn Du nicht weisst, woher "
    +"er den Strom dafuer nimmt. Vermutlich auf magische Weise. Aber Du kennst "
    +"das ja, mit Magie laesst sich fast alles erklaeren. Gut dass der "
    +"Kuehlschrank so gross ist, man koennte einen ausgewachsenen Eisbaeren "
    +"darin unterbringen - aber der duerfte wohl was dagegen haben.")
    +"@@cnt_status@@");
  AddId(({"kuehlschrank", "schrank"}));
  SetProp("cnt_version_obj", VERSION_OBJ);
  SetProp(P_NAME, "Kuehlschrank");
  SetProp(P_GENDER, MALE);
  SetProp(P_WEIGHT, 5000);         // Gewicht 5 Kg
  SetProp(P_MAX_WEIGHT, 1000000);  // Es passen fuer 1000 kg Sachen rein.
  SetProp(P_WEIGHT_PERCENT, 100);  // Dafuer macht er auch nix leichter :)
  SetProp(P_MAX_OBJECTS, 100);     // Mehr sollte nicht sein, lt. Tiamak.
  SetProp(P_VALUE, 0);             // Kein materieller Wert. Ist eh nicht verkaufbar.
  SetProp(P_NOBUY, 1);             // Wird im Laden zerstoert, falls er verkauft wird.
  SetProp(P_NOGET, "Das geht nicht. "+Name(WER,1)+" haftet wie magisch am Boden.\n");
  SetProp(P_MATERIAL, ({MAT_MISC_METAL, MAT_MISC_MAGIC}) );
  SetProp(P_INFO, "Versuchs mal mit: \"skommandos "+QueryProp(P_IDS)[1]+"\" !\n");

  AD(({"strom"}), BS("Keine Ahnung, wo "+name(WER,1)+" den hernimmt, aber "
    +"wie gesagt, bestimmt ist Magie im Spiel."));

  AddSounds(({"summen"}), BS("Das Summen des Kuehlschranks ueberzeugt Dich davon, "
    +"dass er an ist."));
}
