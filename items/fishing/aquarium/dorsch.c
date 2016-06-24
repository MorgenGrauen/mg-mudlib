#include <language.h>
#include <properties.h>
#include <fishing.h>

inherit STD_FISCH;

protected void create() {
  if (!clonep(this_object())) return;
  ::create();
  AddId(({"dorsch","kabeljau"}));
  SetProp(P_NAME,"Dorsch" );
  SetProp(P_GENDER,MALE);
  SetProp(P_SHORT,"Ein Dorsch" );
  SetProp(P_LONG,"Ein kleinerer Dorsch.\n");
  SetProp(P_VALUE,100);
  SetProp(P_WEIGHT, 200);
}
