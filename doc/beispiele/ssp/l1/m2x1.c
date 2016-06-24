inherit "std/room";

#include <properties.h>
#include "../files.h"

create()
{
  replace_program("std/room");
  ::create();
  SetProp(P_LIGHT, 0);
  SetProp(P_INDOORS, 1);
  SetProp(P_INT_SHORT, "Metallebene");
  SetProp(P_INT_LONG,
  "Du bist im Hoehlenlabyrinth der Schreckensspitze. Dieser Gang hier ist aus\n"+
  "reinstem Stahl gefertigt, der in Deinem Licht matt funkelt. Aus einer\n"+
  "Kammer im Westen dringen Rauchschwaden hervor und ein Schnauben ist zu\n"+
  "hoeren. Du kannst es allerdings auch vorziehen, nach Sueden dem Unheil\n"+
  "zu entgehen.\n");
  AddDetail(({"gang", "gaenge", "roehre", "roehren"}), "Der Gang macht eine Biegung von Sueden nach Westen.\n");
  AddDetail(({"boden", "decke", "wand", "waende", "metall", "stahl"}), "Alles hier ist aus hartem Stahl.\n");
  AddDetail("kammer", "Im Westen liegt eine Kammer.\n");
  AddDetail(({"rauchschwaden", "rauch", "schwaden"}), "Der Rauch zieht durch ein paar Ritzen in der Decke ab.\n");
  AddDetail(({"ritze", "ritzen"}), "Die Ritzen haben nichts Bemerkenswertes an sich.\n");
  AddDetail("schnauben", "Von wem wohl das Schnauben kommt?\n");
  AddExit("westen",L1("m1x1"));
  AddExit("sueden",L1("m2x2"));
}
