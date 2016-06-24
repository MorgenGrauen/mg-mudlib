#include <language.h>
#include <properties.h>
#include <fishing.h>

inherit STD_FISCH;

protected void create() {
   if (!clonep(this_object())) return;
  ::create();
  AddId(({"forelle","regenbogenforelle"}));
  SetProp(P_NAME,"Forelle" );
  SetProp(P_GENDER,FEMALE);
  SetProp(P_SHORT,"Eine Forelle" );
  SetProp(P_LONG,"Eine grosse, bunte Regenbogenforelle.\n");
  SetProp(P_VALUE,200);
  SetProp(P_WEIGHT, 1000);
}
