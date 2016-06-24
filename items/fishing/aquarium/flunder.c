#include <language.h>
#include <properties.h>
#include <fishing.h>

inherit STD_FISCH;

protected void create() {
   if (!clonep(this_object())) return;
  ::create();
  AddId(({"flunder"}));
  SetProp(P_NAME,"Flunder" );
  SetProp(P_GENDER,FEMALE);
  SetProp(P_SHORT,"Eine Flunder" );
  SetProp(P_LONG,"Ein ziemlich flache Flunder.\n");
  SetProp(P_VALUE,60);
  SetProp(P_WEIGHT, 300);
}
