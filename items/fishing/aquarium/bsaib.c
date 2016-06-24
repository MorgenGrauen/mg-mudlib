#include <language.h>
#include <properties.h>
#include <fishing.h>

inherit STD_FISCH;

protected void create() {
  if (!clonep(this_object())) return;
  ::create();
  AddId(({"bachsaibling","saibling"}));
  SetProp(P_NAME,"Bachsaibling" );
  SetProp(P_GENDER,MALE);
  SetProp(P_SHORT,"Ein Bachsaibling" );
  SetProp(P_LONG,"Ein oliv-brauner Bachsaibling.\n");
  SetProp(P_VALUE,80);
  SetProp(P_WEIGHT, 400);
}
