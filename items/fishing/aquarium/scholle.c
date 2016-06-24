#include <language.h>
#include <properties.h>
#include <fishing.h>

inherit STD_FISCH;

protected void create() {
   if (!clonep(this_object())) return;
  ::create();
  AddId(({"scholle"}));
  SetProp(P_NAME,"Scholle" );
  SetProp(P_GENDER,FEMALE);
  SetProp(P_SHORT,"Eine Scholle" );
  SetProp(P_LONG,"Ein platte, flache Scholle.\n");
  SetProp(P_VALUE,60);
  SetProp(P_WEIGHT, 500);
}
