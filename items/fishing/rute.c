#pragma strong_types, save_types, rtt_checks
#pragma no_inherit, no_shadow

#include <language.h>
#include <properties.h>
#include <fishing.h>

inherit STD_ANGEL;

protected void create() {
  if (!clonep(this_object())) 
    return;
  ::create();

  SetProp(P_NAME, "Angel");
  SetProp(P_GENDER, FEMALE);
  SetProp(P_SHORT, "Eine Angel");
  SetProp(P_LONG, "Eine normale Angel.\n");
  SetProp(P_TRANSPARENT, 1);
  // 2014-Mai-10, Arathorn. von 1000 auf 1050 erhoeht, damit Haken+Koeder
  // aufgrund ihres Gewichts von wenigen Gramm nicht mehr verhindern, dass
  // einer der standardmaessig 1000 g schweren Fische gefangen werden kann.
  SetProp(P_MAX_WEIGHT, 1050);
  SetProp(P_WATER,W_SHORT);    //siehe fishing.h
}

