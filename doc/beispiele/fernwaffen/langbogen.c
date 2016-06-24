inherit "/std/ranged_weapon";

#include "./path.h"
#include <properties.h>

void create() {
  if (!clonep(this_object())) return;
  ::create();

  SetProp(P_SHORT, "Ein Langbogen");
  SetProp(P_INFO,
    "Die Syntax lautet: schiesse <geschoss> auf <ziel>\n");
  SetProp(P_NAME, "Langbogen");
  SetProp(P_LONG, break_string(
    "Mit diesem Langbogen kann man bestimmt hervorragend weit schiessen."));
  SetProp(P_MATERIAL, MAT_MISC_WOOD);

  AddId("langbogen");
  SetProp(P_GENDER, MALE);
  SetProp(P_WEIGHT, 800);
  SetProp(P_VALUE, 300);

  SetProp(P_WC, 30);
  SetProp(P_DAM_TYPE, DT_BLUDGEON);

  SetProp(P_SHOOTING_WC, 120);
  SetProp(P_NR_HANDS, 2);
  SetProp(P_WEAPON_TYPE, WT_RANGED_WEAPON);
  SetProp(P_AMMUNITION, MUN_ARROW);
  SetProp(P_STRETCH_TIME, 2);
  SetProp(P_RANGE, 30);
}