#include <language.h>
#include <properties.h>
#include <fishing.h>

inherit STD_FISCH;

protected void create() {
   if (!clonep(this_object())) return;
  ::create();
  AddId(({"stichling"}));
  SetProp(P_NAME,"Stichling" );
  SetProp(P_SHORT,"Ein Stichling" );
  SetProp(P_LONG,"Ein suesser kleiner Stichling.\n");
  SetProp(P_VALUE,6);
  SetProp(P_WEIGHT, 30);
}
