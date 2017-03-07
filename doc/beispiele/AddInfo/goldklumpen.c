#pragma strong_types, save_types, rtt_checks, pedantic
inherit "/std/thing";

#include <properties.h>

protected void create() {
  if(!clonep(this_object())) return;
  ::create();

  AddId(({"klumpen", "goldklumpen"}));

  SetProp(P_NAME, "Goldklumpen");
  SetProp(P_SHORT, "Ein Goldklumpen");
  SetProp(P_LONG, break_string(
    "Ein Klumpen Gold. Heb ihn doch auf und frage ein Monster "
    "nach Gold.", 78));

  SetProp(P_GENDER, MALE);
  SetProp(P_WEIGHT, 200);
  SetProp(P_VALUE, 0);
  SetProp(P_MATERIAL, MAT_GOLD);
}
