#pragma strong_types, save_types, rtt_checks
#pragma no_inherit, no_shadow

#include <language.h>
#include <properties.h>
#include <fishing.h>
#include <items/flasche.h>

inherit STD_HAKEN;

protected void create() {
  ::create();
  SetProp(P_NAME, "Haken");
  SetProp(P_SHORT, "Ein Haken");
  SetProp(P_LONG, "Ein Angelhaken aus Metall.\n");
  SetProp(P_LONG_EMPTY, "Du kannst etwas damit aufspiessen!\n");
}
