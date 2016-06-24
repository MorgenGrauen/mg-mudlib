inherit "std/room";

#include <properties.h>
#include "../files.h"

create()
{
  ::create();
  SetProp(P_LIGHT, 0);
  SetProp(P_INDOORS, 1);
  SetProp(P_INT_SHORT, "Steinebene");
  SetProp(P_INT_LONG,
  "Du bist im Hoehlenlabyrinth der Schreckensspitze. Ein mit roher Gewalt in\n"+
  "den Fels genagter Gang fuehrt von Sueden nach Norden. Du spuerst, dass\n"+
  "hinter jeder Ecke Orks oder anderes uebles Gezuecht lauern koennen.\n");
  AddDetail(({"gang", "gaenge"}), "Der Gang fuehrt von Sueden nach Norden.\n");
  AddDetail("fels", "Es ist ein harter Fels, der von boesen Wesen durchbohrt wurde.\n");
  AddDetail(({"ork", "orks", "gezuecht"}), "Momentan siehst Du keine Orks, aber das kann sich ja schnell aendern.\n");
  AddDetail("boden", "Blanker Fels.\n");
  AddDetail("fels", "Das wird allmaehlich langweilig.\n");
  AddDetail("decke", "An der Decke haengt eine bunte Girlande.\n");
  AddDetail("girlande", "Wer die wohl da aufgehaengt hat?\n");
  AddDetail(({"wand", "waende"}), "Da gibt es nichts interessantes zu sehen.\n");
  AddExit("norden",L1("m5x1"));
  AddExit("sueden",L1("m5x3"));
}
