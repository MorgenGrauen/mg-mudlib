#include <language.h>
#include <properties.h>
#include <fishing.h>

inherit STD_FISCH;

protected void create() {
   if (!clonep(this_object())) return;
   ::create();
  AddId(({"barsch"}));
  SetProp(P_NAME,"Barsch" );
  SetProp(P_GENDER,MALE);
  SetProp(P_SHORT,"Ein Barsch" );
  SetProp(P_LONG,"Ein dunkelgruener Barsch.\n");
  SetProp(P_VALUE,60);
  SetProp(P_WEIGHT, 500);
}
