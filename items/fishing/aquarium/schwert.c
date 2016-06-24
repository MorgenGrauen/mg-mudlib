#include <language.h>
#include <properties.h>
#include <fishing.h>

inherit STD_FISCH;

protected void create() {
   if (!clonep(this_object())) return;
  ::create();
  AddId(({"schwertfisch"}));
  SetProp(P_NAME,"Schwertfisch" );
  SetProp(P_GENDER,MALE);
  SetProp(P_SHORT,"Ein Schwertfisch" );
  SetProp(P_LONG,"Ein Schwertfisch mit einem sehr langen Schwert.\n");
  SetProp(P_VALUE,700);
  SetProp(P_WEIGHT, 1000);
}
