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
  "Du bist im Hoehlenlabyrinth der Schreckensspitze. Der Gang, der hier von\n"+
  "Norden nach Sueden fuehrt, aehnelt mehr einer Roehre, denn alles ist voll-\n"+
  "staendig aus einem glaenzenden Metall. Du fragst Dich, wer wohl diese\n"+
  "Gaenge angelegt hat.\n");
  AddDetail(({"gang", "gaenge", "roehre", "rohr"}), "Die Gaenge sind voellig mit Metall ausgekleidet.\n");
  AddDetail("metall", "Das Metall glaenzt silbern.\n");
  AddDetail(({"wand", "waende"}), "Die Waende glaenzen silbern.\n");
  AddDetail("boden", "Sei froh, dass er da ist.\n");
  AddDetail("decke", "Die Decke ist wirklich ziemlich langweilig.\n");
  AddExit("norden",L1("m5x3"));
  AddExit("sueden",L1("m5x5"));
}
