#include <language.h>
#include <properties.h>
#include <fishing.h>

inherit STD_FISCH;

protected void create() {
   if (!clonep(this_object())) return;
  ::create();
  AddId(({"seezunge"}));
  SetProp(P_NAME,"Seezunge" );
  SetProp(P_GENDER,FEMALE);
  SetProp(P_SHORT,"Eine Seezunge" );
  SetProp(P_LONG,"Eine kleine braune Seezunge.\n");
  SetProp(P_VALUE,60);
  SetProp(P_WEIGHT, 300);
}
