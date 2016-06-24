inherit "/std/thing";

#include <properties.h>

protected void create() {
  if ( !clonep(this_object()) ) {
    set_next_reset(-1);
    return;
  }
  ::create();

  SetProp(P_SHORT, "Ein paar Graeten");
  SetProp(P_LONG, "Einige Graeten, die von einem Fisch uebriggeblieben "
    "sind.\n");
  SetProp(P_PLURAL, 1),
  SetProp(P_NAME, "Graeten");
  AddId("graeten");
  SetProp(P_WEIGHT, 3);
  SetProp(P_VALUE, 1);
  SetProp(P_NOBUY, 1);
  SetProp(P_GENDER, FEMALE);
}
