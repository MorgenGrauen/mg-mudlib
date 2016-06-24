#pragma strong_types, save_types, rtt_checks
#pragma no_inherit, no_shadow

#include <language.h>
#include <properties.h>
#include <fishing.h>

inherit STD_KOEDER;

private int snailtype = random(8);

private < <string|string*>*>* desc = ({
//private mixed desc = ({ 
    ({
    "Eine purpurne Purpurschnecke.\n",
    "Eine rosa-gestreifte Kreiselschnecke.\n",
    "Eine suesse kleine Kaurischnecke.\n",
    "Eine kleine rote Buckelschnecke.\n",
    "Eine kleine gestrichelte Turbanschnecke.\n",
    "Eine kleine gestreifte Lochnapfschnecke.\n",
    "Ein gruenes Seeohr.\n",
    "Eine suesse kleine Seeschnecke.\n" }),
    ({
    "purpurschnecke",
    "kreiselschnecke",
    "kaurischnecke",
    "buckelschnecke",
    "turbanschnecke",
    "napfschnecke",
    ({"ohr","seeohr"}),
    "seeschnecke"})
  });

protected void create() {
  ::create();
  AddId(({WURM_ID, "schnecke", "\nfa_snails", "seeschnecke"}));
  AddId(desc[1][snailtype]);
  SetProp(P_NAME, "Schnecke");
  SetProp(P_GENDER, FEMALE);
  SetProp(P_ARTICLE, 1);
  SetProp(P_SHORT, "Ein Seeschnecke");
  SetProp(P_LONG, desc[0][snailtype]);
  SetProp(P_MATERIAL, MAT_MISC_LIVING);
  SetProp(P_FISH, snailtype*random(10));
  SetProp(P_WATER, W_SALT);
  SetProp(P_VALUE,1);
  SetProp(P_WEIGHT, 5);
}
