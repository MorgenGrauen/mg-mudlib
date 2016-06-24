#include <properties.h>

inherit "/std/thing";

create() {
  if (!clonep(this_object())) return;
  ::create();
  SetProp(P_SHORT, "Ein Klumpen Eisen");
  SetProp(P_LONG, "Sieht aus, als haette da schon jemand dran rumgenagt. Derjenige muss\naber sehr stabile Zaehne gehabt haben.\n");
  SetProp(P_NAME, "Klumpen");
  SetProp(P_GENDER, 1);
  AddId(({"klumpen", "eisen", "klumpen eisen"}));
  SetProp(P_MATERIAL, MAT_IRON);
  SetProp(P_WEIGHT, 2438);
  SetProp(P_VALUE, random(750));
}
