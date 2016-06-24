#include <language.h>
#include <properties.h>
#include <fishing.h>

inherit STD_FISCH;

protected void create() {
  if (!clonep(this_object())) return;
  ::create();
  AddId(({"forelle","bachforelle"}));
  SetProp(P_NAME,"Forelle" );
  SetProp(P_GENDER,FEMALE);
  SetProp(P_SHORT,"Eine Forelle" );
  SetProp(P_LONG,"Eine kleine rotgefleckte Bachforelle.\n");
  SetProp(P_VALUE,100);
  SetProp(P_WEIGHT, 500);
}
