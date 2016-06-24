#include <language.h>
#include <properties.h>
#include <fishing.h>

inherit STD_FISCH;


protected void create() {
  if (!clonep(this_object()))
    return;
  ::create();
  corpseobject = 0;
  AddId(({"alligator"}));
  SetProp(P_NAME,"Alligator" );
  SetProp(P_SHORT,"Ein Alligator" );
  SetProp(P_LONG,"Ein original Kanalligator (TM).\n");
  SetProp(P_VALUE,600);
  SetProp(P_WEIGHT, 1000);
}
