#include <language.h>
#include <properties.h>
#include <fishing.h>

inherit STD_FISCH;

protected void create() {
  if (!clonep(this_object())) return;
  ::create();
  AddId(({"dorsch","kabeljau"}));
  SetProp(P_NAME,"Kabeljau" );
  SetProp(P_GENDER,MALE);
  SetProp(P_SHORT,"Ein Kabeljau" );
  SetProp(P_LONG,"Ein praechtiger, grosser Kabeljau.\n");
  SetProp(P_VALUE,500);
  SetProp(P_WEIGHT, 500);
}
