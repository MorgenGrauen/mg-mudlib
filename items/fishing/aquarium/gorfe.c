#include <language.h>
#include <properties.h>
#include <fishing.h>

inherit STD_FISCH;

protected void create() {
   if (!clonep(this_object())) return;
  ::create();
  AddId(({"goldorfe","orfe"}));
  SetProp(P_NAME,"Goldorfe" );
  SetProp(P_GENDER,FEMALE);
  SetProp(P_SHORT,"Ein Goldorfe" );
  SetProp(P_LONG,
  "Eine rote Goldorfe. Sie hat ein wenig Aehnlichkeit mit einem Goldfisch,\n"
  +"ist jedoch wesentlich schlanker, schneller und gefraessiger.\n");
  SetProp(P_VALUE, 150);
  SetProp(P_WEIGHT, 80);
}
