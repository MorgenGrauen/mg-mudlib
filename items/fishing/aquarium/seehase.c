#include <language.h>
#include <properties.h>
#include <fishing.h>

inherit STD_FISCH;

protected void create() {
   if (!clonep(this_object())) return;
  ::create();
  AddId(({"seehase","hase"}));
  SetProp(P_NAME,"Seehase" );
  SetProp(P_GENDER,MALE);
  SetProp(P_SHORT,"Ein Seehase" );
  SetProp(P_LONG,"Ein klobiger, dunkelgraublauer Seehase.\n");
  SetProp(P_VALUE,30);
  SetProp(P_WEIGHT, 200);
}
