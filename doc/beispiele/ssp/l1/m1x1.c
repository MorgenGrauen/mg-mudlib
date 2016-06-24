inherit "std/room";

#include <properties.h>
#include <rooms.h>
#include "../files.h"

create()
{
  replace_program("std/room");
  ::create();
  SetProp(P_LIGHT, 0);
  SetProp(P_INDOORS, 1);
  SetProp(P_INT_SHORT, "Metallebene");
  SetProp(P_INT_LONG,
  "Du bist im Hoehlenlabyrinth der Schreckensspitze. Der Raum ist gefuellt\n"+
  "mit grau-blauem Rauch, der von dem grossen Drachen stammt, der ihn be-\n"+
  "wohnt. Es ist sehr warm hier drin und an den Waenden und an der Decke\n"+
  "kondensiert der Dampf zu kleinen Tropfen. Der einzige Ausgang liegt im\n"+
  "Osten.\n");
  AddDetail("raum", "Ein Drache wohnt hier normalerweise.\n");
  AddDetail(({"drache", "drachen"}), "Er scheint gerade nicht da zu sein.\n");
  AddDetail("rauch", "Der Rauch zieht zum Ausgang im Osten hinaus.\n");
  AddDetail(({"wand", "waende", "boden", "decke", "metall", "stahl"}), "Alles hier ist aus reinem Stahl.\n");
  AddDetail("dampf", "Er kondensiert an den Waenden.\n");
  AddDetail(({"wasser", "kondenswasser"}), "Es tropft zu Boden.\n");
  AddDetail("tropfen", "Sie tropfen auf den Boden.\n");
  AddDetail("ausgang", "Er liegt im Osten.\n");
  AddItem(MON("stahldrache"), REFRESH_REMOVE);
  AddExit("osten",L1("m2x1"));
}
