#include <language.h>
#include <properties.h>
#include <fishing.h>

inherit STD_FISCH;

protected void create() {
  if (!clonep(this_object())) return;
  ::create();
  AddId(({"blauhai","hai"}));
  SetProp(P_NAME,"Hai" );
  SetProp(P_GENDER,MALE);
  SetProp(P_SHORT,"Ein Hai" );
  SetProp(P_LONG,"Ein grosser, bissiger Blauhai.\n");
  SetProp(P_VALUE,800);
  SetProp(P_WEIGHT, 1200);
}
