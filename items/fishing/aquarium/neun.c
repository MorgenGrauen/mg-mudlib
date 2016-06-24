#include <language.h>
#include <properties.h>
#include <fishing.h>

inherit STD_FISCH;

protected void create() {
   if (!clonep(this_object())) return;
  ::create();
  AddId(({"neunauge","bachneunauge"}));
  SetProp(P_NAME,"Bachneunauge" );
  SetProp(P_GENDER,NEUTER);
  SetProp(P_SHORT,"Ein Neunauge" );
  SetProp(P_LONG,
  "Ein Bachneunauge mit einer kleinen Mundscheibe. Damit kann es sich\n"
  +"an Steinen festhalten und in der Stroemung treiben.\n");
  SetProp(P_VALUE,60);
  SetProp(P_WEIGHT, 300);
}
