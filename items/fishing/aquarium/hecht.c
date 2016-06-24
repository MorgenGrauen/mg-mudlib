#include <language.h>
#include <properties.h>
#include <fishing.h>

inherit STD_FISCH;

protected void create() {
   if (!clonep(this_object())) return;
  ::create();
  AddId(({"hecht"}));
  SetProp(P_NAME,"Hecht" );
  SetProp(P_GENDER,MALE);
  SetProp(P_SHORT,"Ein Hecht" );
  SetProp(P_LONG,"Ein torpedofoermiger, gruengestreifter Hecht.\n");
  SetProp(P_VALUE,90);
  SetProp(P_WEIGHT, 700);
}
