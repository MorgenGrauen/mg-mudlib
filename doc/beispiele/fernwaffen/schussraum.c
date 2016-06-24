inherit "/std/room";

#include "./path.h"
#include <properties.h>

void create() {
  ::create();

  SetProp(P_LIGHT, 1);
  SetProp(P_INT_SHORT, "Auf einem Baum");
  SetProp(P_INT_LONG, break_string(
    "Du hockst auf einem Baum und kannst auf die Lichtung unter Dir sehen. "
    "'schau runter' hilft sicherlich, Ziele auszumachen."));

  AddDetail("runter", function string { 
                        return __PATH__(0)"zielraum"->int_long(this_player());
                      });

  AddItem(__PATH__(0)"langbogen", REFRESH_REMOVE);
  AddItem(__PATH__(0)"kurzbogen", REFRESH_REMOVE);
  AddItem(__PATH__(0)"pfeile", REFRESH_REMOVE, ([P_AMOUNT: 20]));

  AddExit("zielraum", __PATH__(0)"zielraum");
  load_object(__PATH__(0)"zielraum");

  SetProp(P_TARGET_AREA, __PATH__(0)"zielraum"); // anderer Raum beschiessbar
  SetProp(P_SHOOTING_AREA, 15);                  // 15 Entfernung
}

// nur wegen des P_NEVER_CLEAN im Zielraum und nur hier in doc relevant
public varargs int remove(int silent) {
  __PATH__(0)"zielraum"->remove();
  return ::remove();
}
