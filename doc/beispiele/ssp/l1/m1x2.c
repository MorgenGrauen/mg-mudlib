inherit "std/room";

#include <properties.h>
#include <rooms.h>
#include "../files.h"

create()
{
  replace_program("std/room");
  ::create();
  SetProp(P_LIGHT, 0);
  SetProp(P_INT_SHORT, "Metallebene");
  SetProp(P_INT_LONG,
  "Du bist im Hoehlenlabyrinth der Schreckensspitze. In dieser gemuetlichen\n"+
  "Ecke (natuerlich nur fuer Metall liebende Lebewesen) hat sich wohl eine\n"+
  "der merkwuerdigen Titanwalzen eingenistet, auf jeden Fall ist der Boden\n"+
  "dermassen plattgewalzt, wie es nur diese Art schaffen kann. Der Gang\n"+
  "selbst fuehrt hier nach Sueden und Osten.\n");
  AddDetail("ecke", "Richtig nett hier ...\n");
  AddDetail(({"wand", "waende", "decke", "metall"}), "Alles hier ist aus Metall.\n");
  AddDetail(({"walze", "titanwalze"}), "Die Titanwalze scheint gerade nicht daheim zu sein.\n");
  AddDetail("boden", "Der Boden ist absolut platt.\n");
  AddDetail("gang", "Ein aus Metall gearbeiteter Gang.\n");
  AddExit("sueden",L1("m1x3"));
  AddExit("osten",L1("m2x2"));
  AddItem(MON("titanwalze"), REFRESH_REMOVE);
}
