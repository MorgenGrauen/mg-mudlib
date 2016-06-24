inherit "std/room";

#include <properties.h>
#include <rooms.h>
#include "../files.h"

create()
{
  ::create();
  SetProp(P_LIGHT, 0);
  SetProp(P_INDOORS, 1);
  SetProp(P_INT_SHORT, "Metallebene");
  SetProp(P_INT_LONG,
  "Du bist im Hoehlenlabyrinth der Schreckensspitze. Eigentlich koennte man\n"+
  "es auch fast Roehrenlabyrinth nennen, denn Du befindest Dich in einer\n"+
  "solchen, die hier eine Biegung von Norden nach Westen macht. Die Waende\n"+
  "sind aus einem glaenzenden Metall gefertigt, was Dir ziemlich merkwuerdig\n"+
  "vorkommt.\n");
  AddDetail(({"wand", "waende", "roehre", "roehren", "gaenge", "gang"}), "Die Gaenge sind voellig mit Metall ausgekleidet.\n");
  AddDetail("metall", "Das Metall glaenzt silbern.\n");
  AddDetail("boden", "Der Boden ist aus Metall.\n");
  AddDetail("decke", "Selbst die Decke glaenzt metallisch, Du fragst Dich, wer das angelegt hat.\n");
  AddDetail("biegung", "Die Biegung fuehrt von Norden nach Westen.\n");
  AddItem(MON("kampfblech"), REFRESH_REMOVE);
  AddExit("norden",L1("m5x4"));
  AddExit("westen",L1("m4x5"));
}
