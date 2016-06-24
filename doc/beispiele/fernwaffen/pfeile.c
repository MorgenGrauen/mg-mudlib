inherit "/std/unit";

#include "./path.h"
#include <properties.h>
#include <class.h>

void create() {
  if (!clonep(this_object())) return;
  ::create();

  SetProp(P_NAME, ({"Pfeil", "Pfeile"}) );
  SetProp(P_LONG, break_string(
    "Ein paar einfache Holzpfeile.", 78));
  SetProp(P_GENDER, MALE);
  SetProp(P_AMOUNT, 1);
  SetProp(P_SHOOTING_WC, 40);
  SetProp(P_DAM_TYPE, ({DT_PIERCE}));
  SetProp(P_WEAPON_TYPE, WT_AMMU);
  SetProp(P_MATERIAL, MAT_MISC_WOOD);

  SetGramsPerUnits(120,1);
  SetCoinsPerUnits(25,1);

  AddId(MUN_ARROW);
  AddSingularId("pfeil");
  AddPluralId("pfeile");
  AddClass(CL_AMMUNITION);

  SetProp(P_HIT_FUNC, this_object());
}

int HitFunc(object enemy) {
  if(!random(5)) {
    tell_object(environment(), break_string(
      "Der Pfeil schnurrt vom Bogen und dabei splittert das Holz "
      "etwas. Gratiswiderhaken, das ist ja toll!", 78));
    return 20;
  }

  return 0;
}