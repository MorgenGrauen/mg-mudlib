#pragma strong_types, save_types, rtt_checks
#pragma no_clone, no_shadow

inherit "/std/thing";

#include <language.h>
#include <properties.h>
#include <items/fishing/fishing.h>

protected void create(){
  ::create();
  AddId(({WURM_ID,"koeder", "wurm","regenwurm"}));
  SetProp(P_NAME, "Wurm");
  SetProp(P_GENDER , MALE);
  SetProp(P_ARTICLE, 1);
  SetProp(P_FISH, 0);
  SetProp(P_WATER, 0);
  SetProp(P_SHORT, "Ein kleiner Wurm");
  SetProp(P_LONG, "Ein kleiner Regenwurm.\n");
  SetProp(P_MATERIAL, MAT_MISC_LIVING);
  SetProp(P_VALUE, 1);
  SetProp(P_WEIGHT, 5);
}

protected void create_super() {
  set_next_reset(-1);
}

