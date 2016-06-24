#include <language.h>
#include <properties.h>
#include <fishing.h>

inherit STD_FISCH;

protected void create() {
  if (!clonep(this_object())) return;
  ::create();
  AddId(({"seeteufel","teufel"}));
  SetProp(P_NAME,"Seeteufel" );
  SetProp(P_SHORT,"Ein Seeteufel" );
  SetProp(P_LONG,
  "Ein grimmiger Seeteufel mit vielen scharfen Zaehnen. Er hat eine kleine,\n"
  +"auffaellige Angel oben an seinem Kopf, mit der er nach Beute koedert.\n");
  SetProp(P_VALUE,200);
  SetProp(P_WEIGHT, 800);
}
