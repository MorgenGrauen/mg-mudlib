#include <language.h>
#include <properties.h>
#include <fishing.h>

inherit STD_FISCH;

protected void create() {
  if (!clonep(this_object())) return;
  ::create();
  SetProp(P_NAME,"FISCH" );
  SetProp(P_GENDER,MALE);
  SetProp(P_SHORT,"Ein Fisch" );
  SetProp(P_LONG,"Ein ziemlich gewoehnlicher Fisch.\n");
  SetProp(P_VALUE,10);
  SetProp(P_WEIGHT, 100);
}
