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

  SetProp(P_NAME, "Bootsrute");
  SetProp(P_GENDER, FEMALE);
  SetProp(P_SHORT, "Eine kurze, starke Angel");
  SetProp(P_LONG,"Eine Angel speziell fuer die Hochsee-Fischerei.\n"
    "Sie ist von Fachleuten auf Hai-Festigkeit getestet!\n");
  SetProp(P_TRANSPARENT,1);
  SetProp(P_MAX_WEIGHT, 10000); //Hai-fest!!!!!!
  SetProp(P_WATER,W_SHORT);     //siehe fishing.h
}
