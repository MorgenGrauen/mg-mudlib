#include <language.h>
#include <properties.h>
#include <fishing.h>

inherit STD_FISCH;

protected void create() {
   if (!clonep(this_object())) return;
  ::create();
  AddId(({"goldfisch"}));
  SetProp(P_NAME,"Goldfisch" );
  SetProp(P_GENDER,MALE);
  SetProp(P_SHORT,"Ein Goldfisch" );
  SetProp(P_LONG,"Ein wunderschoen roter Goldfisch.\n");
  SetProp(P_VALUE,100);
  SetProp(P_WEIGHT, 80);
}
