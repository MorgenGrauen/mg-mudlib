//--------------------------------------------------------------------------------
// Name des Objects:    Zauberkiste
// Letzte Aenderung:    10.05.2002
// Magier:              Vanion
// Was:                 Die ID fuer die Kompos der Zauberer umgestellt.
//--------------------------------------------------------------------------------
#pragma strong_types,rtt_checks

#include "schrankladen.h"

// Fuer die IDs
#include "/p/zauberer/zauberer.h"

inherit LADEN("swift_std_container");

#define VERSION_OBJ "3"

protected void create()
{
  if (!clonep(TO)) return;
  swift_std_container::create();
  SetProp(P_SHORT, "Eine Zauberkiste");
  SetProp(P_LONG, BS(
     "Eine kleine Kiste aus Birnbaumholz. Da diesem seltenen Holz selbst "
    +"magische Faehigkeiten nachgesagt werden, ist es wohl das ideale Material, "
    +"um die teilweise gefaehrlichen Komponenten eines Zauberers darin "
    +"unterzubringen.")+"@@cnt_status@@");
  AddId(({"zauberkiste", "kiste"}));
  SetProp("cnt_version_obj", VERSION_OBJ);
  SetProp(P_NAME, "Zauberkiste");
  SetProp(P_GENDER, FEMALE);
  SetProp(P_WEIGHT, 5000);         // Gewicht 5 Kg
  SetProp(P_MAX_WEIGHT, 1000000);  // Es passen fuer 1000 kg Sachen rein.
  SetProp(P_WEIGHT_PERCENT, 100);  // Dafuer macht er auch nix leichter :)
  SetProp(P_MAX_OBJECTS, 100);     // Mehr sollte nicht sein, lt. Tiamak.
  SetProp(P_VALUE, 0);             // Kein materieller Wert. Ist eh nicht verkaufbar.
  SetProp(P_NOBUY, 1);             // Wird im Laden zerstoert, falls er verkauft wird.
  SetProp(P_NOGET, "Das geht nicht. "+Name(WER,1)+" haftet wie magisch am Boden.\n");
  SetProp(P_MATERIAL, ({MAT_PEAR_WOOD, MAT_MISC_MAGIC}) );
  SetProp(P_INFO, "Versuchs mal mit: \"skommandos "+QueryProp(P_IDS)[1]+"\" !\n");

  AD(({"holz", "birnbaumholz"}), BS("Fast koenntest Du meinen, dass "+name(WER,1)
    +" lebt!"));
}

varargs int PreventInsert(object ob)
{
  // Keine Zauberkompo? Hat in diesem Container nix verloren!
  if( !ob->id(Z_ID_ZUTAT) )
  {
    write( BS("In "+name(WEN,1)+" kannst Du nur Zauberer-Komponenten legen!"));
    return 1;
  }
  else
    return ::PreventInsert(ob);
}
