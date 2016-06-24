inherit "std/room";

#include <properties.h>
#include <rooms.h>
#include "../files.h"

create()
{
  ::create();
  SetProp(P_LIGHT, 0);
  SetProp(P_INDOORS, 1);
  SetProp(P_INT_SHORT, "Steinebene");
  SetProp(P_INT_LONG,
  "Du bist im Hoehlenlabyrinth der Schreckensspitze. Der schmale Gang macht\n"+
  "hier eine Biegung von Norden nach Osten, wo er jeweils in die Dunkelheit\n"+
  "fuehrt.\n");
  AddDetail("gang", "Der Gang macht eine Biegung.\n");
  AddDetail("dunkelheit", "Schwarze Dunkelheit.\n");
  AddDetail("boden", "Auf dem Boden ist nichts Besonderes zu sehen.\n");
  AddDetail("decke", "Die Decke ist total langweilig.\n");
  AddDetail(({"wand", "waende"}), "An der Wand haengt eine Hand.\n");
  AddDetail("hand", "Ach nee, hat sich nur gereimt.\n");
  AddDetail("biegung", "Sie fuehrt nach Norden und Osten.\n");
  AddExit("norden",L1("m3x2"));
  AddExit("osten",L1("m4x3"));
  AddItem(MON("ork"), REFRESH_REMOVE);
}
