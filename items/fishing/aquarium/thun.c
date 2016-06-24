#include <language.h>
#include <properties.h>
#include <fishing.h>

inherit STD_FISCH;

protected void create() {
   if (!clonep(this_object())) return;
  ::create();
  AddId(({"thunfisch"}));
  SetProp(P_NAME,"Thunfisch" );
  SetProp(P_GENDER,MALE);
  SetProp(P_SHORT,"Ein Thunfisch" );
  SetProp(P_LONG,"Ein grosser Thunfisch.\n");
  SetProp(P_VALUE,75);
  SetProp(P_WEIGHT, 500);
}
