#include <language.h>
#include <properties.h>
#include <fishing.h>

inherit STD_FISCH;

protected void create() {
   if (!clonep(this_object())) return;
  ::create();
  AddId(({"piranha"}));
  SetProp(P_NAME,"Piranha" );
  SetProp(P_GENDER,MALE);
  SetProp(P_SHORT,"Ein Piranha" );
  SetProp(P_LONG,"Ein boesartiger kleiner Piranha mit sehr scharfen "
    "Zaehnen.\n");
  SetProp(P_VALUE,40);
  SetProp(P_WEIGHT, 100);
}
