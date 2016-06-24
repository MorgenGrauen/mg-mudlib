#include <language.h>
#include <properties.h>
#include <fishing.h>

inherit STD_FISCH;

protected void create() {
   if (!clonep(this_object())) return;
  ::create();
  AddId(({"schleie","schlei"}));
  SetProp(P_NAME,"Schleie" );
  SetProp(P_GENDER,FEMALE);
  SetProp(P_SHORT,"Eine Schlei" );
  SetProp(P_LONG,"Eine aeusserst schleimige Schleie.\n");
  SetProp(P_VALUE,100);
  SetProp(P_WEIGHT, 800);
}
