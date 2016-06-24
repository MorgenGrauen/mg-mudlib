inherit "/std/npc";

#include "./path.h"
#include <properties.h>

void create() {
  ::create();
  AddId(({"npc", "gegner", "ziel"}));
  SetProp(P_NAME, "Ziel");
  SetProp(P_GENDER, NEUTER);
  SetProp(P_SHORT, "Ein Ziel");

  SetProp(P_HP, 1000);
  SetProp(P_BODY, 100);
}

varargs int Defend(int dam, string* dam_type, mixed spell, object enemy) {
  int predam = dam;
  int result = ::Defend(dam, &dam_type, &spell, enemy);
  
  string str = break_string(sprintf(
    "Die Zielscheibe sagt: %d Schaden rein und fuer %d getroffen.",
    predam, result), 78);

  tell_room(load_object(__PATH__(0)"zielraum"), str);
  tell_room(load_object(__PATH__(0)"schussraum"), str);

  return result;
}