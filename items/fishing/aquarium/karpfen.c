#include <language.h>
#include <properties.h>
#include <fishing.h>

inherit STD_FISCH;

protected void create() {
   if (!clonep(this_object())) return;
  ::create();
  AddId(({"karpfen"}));
  SetProp(P_NAME,"Karpfen" );
  SetProp(P_GENDER,MALE);
  SetProp(P_SHORT,"Ein Karpfen" );
  SetProp(P_LONG,"Ein grosser blaugrauer Karpfen.\n");
  SetProp(P_VALUE,200);
  SetProp(P_WEIGHT, 1000);
}
