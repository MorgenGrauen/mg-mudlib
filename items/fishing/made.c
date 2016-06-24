#pragma strong_types, save_types, rtt_checks
#pragma no_inherit, no_shadow

#include <language.h>
#include <properties.h>
#include <fishing.h>

inherit STD_KOEDER;

protected void create() {
  ::create();
  AddId(({WURM_ID,"made"}));
  SetProp(P_NAME, "Made");
  SetProp(P_GENDER , FEMALE);
  SetProp(P_WATER, W_SWEET);
  SetProp(P_FISH, 30);
  SetProp(P_SHORT, "Eine Made");
  SetProp(P_LONG, "Eine kleine eklige Made.\n");
}
