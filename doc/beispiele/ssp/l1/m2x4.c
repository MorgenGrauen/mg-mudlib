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
  "Du bist im Hoehlenlabyrinth der Schreckensspitze. Der voellig mit Metall\n"+
  "ausgeschlagene Gang macht hier einen scharfen Knick von Norden nach Osten\n"+
  "und verschwindet dort in der Dunkelheit.\n");
  AddDetail(({"wand", "waende", "boden", "decke", "metall"}), "Alles hier ist aus Metall.\n");
  AddDetail("gang", "Der Gang fuehrt nach Norden und Osten.\n");
  AddDetail("dunkelheit", "Du kannst hingehen und schauen, ob sie dann noch da ist.\n");
  AddExit("norden",L1("m2x3"));
  AddExit("osten",L1("m3x4"));
}
