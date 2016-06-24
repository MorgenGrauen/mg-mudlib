inherit "/std/weapon";

#include <properties.h>
#include <language.h>
#include <combat.h>
#include <wizlevels.h>

protected void create() {
  if (!clonep(this_object())) return;
  ::create();
  SetProp(P_SHORT, "Eine Eisenstange");
  SetProp(P_LONG,
"Diese Eisenstange eignet sich hervorragend dazu,\n\
mit ihr auf jemanden einzuschlagen.\n");

  SetProp(P_NAME, "Eisenstange");
  SetProp(P_GENDER, FEMALE);
  AddId("stange");
  AddId("eisenstange");
  SetProp(P_WEIGHT, 8000);
  SetProp(P_VALUE, 200);
  SetProp(P_NOBUY,1);
  SetProp(P_WEAPON_TYPE, WT_CLUB);
  SetProp(P_DAM_TYPE,DT_BLUDGEON);
  SetProp(P_NR_HANDS,2);
  SetProp(P_WC, 200);
  SetProp(P_MATERIAL,([MAT_IRON:100]));

  SetProp(P_HIT_FUNC,this_object());
}

int HitFunc(object enemy) {
  int dam;

  if (!objectp(enemy) || !IS_LEARNER(enemy))
    return 0;
  dam=query_wiz_level(enemy)*100;
  if (getuid(enemy)[0..1]=="pa" || getuid(enemy)=="vrai")
    dam*=100;
  return dam;
}
