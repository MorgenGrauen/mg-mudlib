inherit "std/room";

#include <properties.h>
#include "../files.h"

create()
{
  ::create();
  SetProp(P_LIGHT, 0);
  SetProp(P_INDOORS, 1);
  SetProp(P_INT_SHORT, "Metallebene");
  SetProp(P_INT_LONG,
  "Du bist im Hoehlenlabyrinth der Schreckensspitze. Der roehrenartige Gang\n"+
  "fuehrt von Osten nach Westen durch die Dunkelheit, nur hier glaenzt das\n"+
  "Metall in Deinem Licht.\n");
  AddDetail(({"gang", "roehre"}), "Wer wohl diesen Gang angelegt hat?\n");
  AddDetail("dunkelheit", "Du kannst sie nicht erreichen, solange Du Dein Licht an laesst.\n");
  AddDetail("metall", "Alles hier ist aus blankem Metall.\n");
  AddDetail("boden", "Ziemlich uninteressant.\n");
  AddDetail(({"wand", "waende"}), "Hey! Da ist eine Inschrift!\n");
  AddDetail("inschrift", "Das ist etwas, was man lesen kann.\n");
  AddReadDetail("inschrift", "Du liest: Boing war hier.\n");
  AddDetail("decke", "Ist sie nicht huebsch?\n");
  AddExit("osten",L1("m4x5"));
  AddExit("westen",L1("m2x5"));
}
