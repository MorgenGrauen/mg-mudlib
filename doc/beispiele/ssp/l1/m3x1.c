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
  "Du bist am Eingang zum Hoehlenlabyrinth der Schreckensspitze. Etwas Licht\n"+
  "dringt vom Ausgang im Norden herein, doch im Sueden lauert die pechschwarze\n"+
  "Dunkelheit.\n");
  AddDetail("eingang", "Es ist der Eingang zu mehr, als Du denkst.\n");
  AddDetail("boden", "Der Boden besteht aus grauem Fels.\n");
  AddDetail("fels", "Der Fels ist nicht besonders interessant, nur grau.\n");
  AddDetail(({"wand", "waende"}), "Die Waende sind von Orkblut beschmutzt.\n");
  AddDetail(({"orkblut", "blut"}), "Orks werden hier wohl zum Spass gemetzelt.\n");
  AddDetail("decke", "Ueber Deinem Kopf, das wird Dir hier noch oefters passieren.\n");
  AddDetail(({"hoehlenlabyrinth", "labyrinth", "hoehlen"}), "Diese Labyrinth erstreckt sich ueber etliche Ebenen, viel Spass beim\nErkunden.\n");
  AddDetail("licht", "Das letzte Tageslicht.\n");
  AddDetail("ausgang", "Der Ausgang aus der Dunkelheit.\n");
  AddDetail("dunkelheit", "Das wird Dir dort unten noch oefters begegnen.\n");
  AddItem(MON("ork"), REFRESH_REMOVE);
  AddItem(MON("ork"), REFRESH_REMOVE);
  AddExit("norden", "/d/gebirge/room/spitze4");
  AddExit("sueden",L1("m3x2"));
}
