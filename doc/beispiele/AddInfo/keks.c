#pragma strong_types, save_types, rtt_checks, pedantic
inherit "/std/thing";

#include <properties.h>

protected void create() {
  if(!clonep(this_object())) return;
  ::create();

  AddId(({"keks"}));

  SetProp(P_NAME, "Keks");
  SetProp(P_SHORT, "Ein Keks");
  SetProp(P_LONG, break_string(
    "Ein leckerer Keks. Gib ihn doch einem Monster.",78));

  SetProp(P_GENDER, MALE);
  SetProp(P_WEIGHT, 20);
  SetProp(P_VALUE, 0);
  SetProp(P_MATERIAL, MAT_MISC_PLANT);

  AddCmd("iss|mampf|friss&@ID&\n", 0,
         "Was willst du essen?|Sieht nicht lecker genug aus.^");
}
