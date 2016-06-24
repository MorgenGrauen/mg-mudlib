#include <language.h>
#include <properties.h>
#include <fishing.h>

inherit STD_FISCH;

protected void create() {
   if (!clonep(this_object())) return;
  ::create();
  AddId(({"rochen","nagelrochen"}));
  SetProp(P_NAME,"Rochen" );
  SetProp(P_GENDER,MALE);
  SetProp(P_SHORT,"Ein Rochen" );
  SetProp(P_LONG,"Ein kleiner Nagelrochen.\n");
  SetProp(P_VALUE,60);
  SetProp(P_WEIGHT, 300);
}
