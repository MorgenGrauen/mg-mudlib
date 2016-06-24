#include <language.h>
#include <properties.h>
#include <fishing.h>

inherit STD_FISCH;

protected void create() {
   if (!clonep(this_object())) return;
  ::create();
  AddId(({"lachs"}));
  SetProp(P_NAME,"Lachs" );
  SetProp(P_SHORT,"Ein Lachs" );
  SetProp(P_LONG,"Ein schoener, grosser Lachs.\n");
  SetProp(P_VALUE,60);
  SetProp(P_WEIGHT, 400);
}
