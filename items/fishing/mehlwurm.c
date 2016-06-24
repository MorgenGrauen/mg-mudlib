#pragma strong_types, save_types, rtt_checks
#pragma no_inherit, no_shadow

#include <language.h>
#include <properties.h>
#include <fishing.h>

inherit STD_KOEDER;

protected void create() {
  ::create();
  AddId(({"wurm","mehlwurm"}));
  SetProp(P_NAME, "Mehlwurm");
  SetProp(P_FISH, 10);
  SetProp(P_WATER, W_SWEET);
  SetProp(P_SHORT, "Ein Mehlwurm");
  SetProp(P_LONG, "Ein kleiner Mehlwurm.\n");
}
