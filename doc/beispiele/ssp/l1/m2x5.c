inherit "std/room";

#include <rooms.h>
#include <properties.h>
#include "../files.h"

create()
{
  ::create();
  SetProp(P_LIGHT, 0);
  SetProp(P_INDOORS, 1);
  SetProp(P_INT_SHORT, "Metallebene");
  SetProp(P_INT_LONG,
  "Du bist im Hoehlenlabyrinth der Schreckensspitze. Die Waende des Ganges,\n"+
  "der von Osten nach Westen (oder andersrum) verlaeuft, sind mit glaenzendem\n"+
  "Metall ausgekleidet. Irgendwie kommt Dir diese Umgebung ziemlich seltsam\n"+
  "vor, aber keine Sorge, es kommt noch viel schlimmer!\n");
  AddDetail(({"gang", "roehre"}), "Der Gang verlaeuft von Osten nach Westen.\n");
  AddDetail(({"wand", "waende", "decke", "boden"}), "Alles ist aus glaenzendem Metall.\n");
  AddDetail("metall", "Das Metall ist an einer Stelle leicht eingedellt.\n");
  AddDetail(({"delle", "stelle", "beule"}), "Da wollte wohl jemand mit dem Kopf durch die Wand.\n");
  AddExit("osten",L1("m3x5"));
  AddExit("westen",L1("m1x5"));
  AddItem(MON("dosenoeffner"), REFRESH_REMOVE);
  AddItem(MON("dosenoeffner"), REFRESH_REMOVE);
}
