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
  SetProp(P_INT_SHORT, "Beim Knochenschaeler");
  SetProp(P_INT_LONG,
  "Du bist im Hoehlenlabyrinth der Schreckensspitze. Dieser kleine Raum ist\n"+
  "die Heimat eines fuerchterlichen Ungeheuers, die Heimat des schrecklichen\n"+
  "Knochenschaelers. Im Boden des Raums ist ein kleines Loch, durch das eine\n"+
  "Leiter nach unten fuehrt.\n");
  AddDetail("raum", "Der Raum ist ziemlich klein.\n");
  AddDetail(({"wand", "decke", "waende", "metall"}), "Alles ist aus Metall.\n");
  AddDetail(({"boden", "loch"}), "Im Boden ist ein Loch, durch das eine Leiter nach unten fuehrt.\n");
  AddDetail("leiter", "Die Leiter fuehrt nach unten.\n");
  AddDetail("ungeheuer", "Der Knochenschaeler ist einfach fuerchterlich.\n");
  AddDetail("knochenschaeler", "Zum Glueck ist er gerade nicht daheim.\n");
  AddItem(MON("knochenschaeler"), REFRESH_REMOVE);
  AddExit("unten", L1("m2x2"));
}
