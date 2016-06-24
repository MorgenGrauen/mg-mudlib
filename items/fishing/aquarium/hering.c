#include <language.h>
#include <properties.h>
#include <fishing.h>

inherit STD_FISCH;

protected void create() {
   if (!clonep(this_object())) return;
  ::create();
  AddId(({"hering"}));
  SetProp(P_NAME,"Hering" );
  SetProp(P_GENDER,MALE);
  SetProp(P_SHORT,"Ein Hering" );
  SetProp(P_LONG,
  "Ein kleiner Hering.\n");
  SetProp(P_VALUE,30);
  SetProp(P_WEIGHT, 100);
}
