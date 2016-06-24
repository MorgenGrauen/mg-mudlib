#include <language.h>
#include <properties.h>
#include <fishing.h>

inherit STD_FISCH;

protected void create() {
   if (!clonep(this_object())) return;
  ::create();
  AddId(({"seesaibling","saibling"}));
  SetProp(P_NAME,"Seesaibling" );
  SetProp(P_SHORT,"Ein Seesaibling" );
  SetProp(P_LONG,"Ein blaugrauer Seesaibling mit einem roten Bauch.\n");
  SetProp(P_VALUE,80);
  SetProp(P_WEIGHT, 400);
}
