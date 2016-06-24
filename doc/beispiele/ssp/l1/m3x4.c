inherit "std/room";

#include <properties.h>
#include <rooms.h>
#include <moving.h>
#include "../files.h"

create()
{
  ::create();
  SetProp(P_LIGHT, 0);
  SetProp(P_INDOORS, 1);
  SetProp(P_INT_SHORT, "Metallebene");
  SetProp(P_INT_LONG,
  "Du bist im Hoehlenlabyrinth der Schreckensspitze. Metallbeschlagene Waende\n"+
  "umringen Dich im Norden, Osten und Sueden waehrend ein roehrenartiger Gang\n"+
  "nach Westen fuehrt. Ausserdem befindet sich im Boden ein Loch, durch das\n"+
  "eine Leiter nach unten fuehrt.\n");
  AddDetail("loch", "Was einen da unten wohl erwarten mag?\n");
  AddDetail("leiter", "Die Leiter fuehrt nach unten.\n");
  AddDetail("boden", "Im Boden ist ein Loch.\n");
  AddDetail(({"wand", "waende", "decke", "metall"}), "Alles ist aus Metall.\n");
  AddDetail(({"roehre", "gang"}), "Der Gang fuehrt nach Westen.\n");
  AddItem(MON("titanwalze"), REFRESH_REMOVE);
  AddExit("westen",L1("m2x4"));
  AddSpecialExit("unten", "unten");
}

unten()
{
  if (present("titanwalze"))
  {
    write("Die Titanwalze laesst Dich nicht vorbei.\n");
    return 1;
  }
  this_player()->move(L2("m3x4"), M_GO, "nach unten");
  this_player()->SetProp("boing:drom_marker", 1);
  return 1;
}
