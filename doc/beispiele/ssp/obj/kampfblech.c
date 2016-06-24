#include <properties.h>
#include <combat.h>
inherit "std/armour";
 
create()
{
  if (!clonep(this_object())) return;
  ::create();
  SetProp(P_SHORT, "Ein totes Kampfblech");
  SetProp(P_LONG, "Sieht aus wie ein normales Kampfblech, nur tot. Ausserdem ueberrascht Dich der merkwuerdige Griff.\n");
  SetProp(P_NAME, "Kampfblech");
  SetProp(P_NAME_ADJ, "tot");
  SetProp(P_GENDER, 0);
  SetProp(P_ARMOUR_TYPE, AT_SHIELD);
  SetProp(P_AC, 12);
  SetProp(P_WEIGHT, 850);
  SetProp(P_VALUE, 420);
  SetProp(P_MATERIAL, MAT_MISC_METAL);
  SetProp(P_INFO, "Ziemlicher Schwachsinn dieses Objekt, oder nicht?\n");
  AddId(({"schild", "blech", "kampfblech", "totes kampfblech"}));
  AddDetail("griff", "Mit Hilfe des Griffs kannst Du das tote Kampfblech als Schild benutzen.\n");
}
