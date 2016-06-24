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
  "Du bist im Hoehlenlabyrinth der Schreckensspitze. Der durch den harten\n"+
  "Fels gehauene Gang verzweigt sich hier nach Norden, Osten und Westen.\n"+
  "Du stellst fest, dass vom Norden her ein ziemlich unangenehmer Geruch\n"+
  "kommt.\n");
  AddDetail("fels", "Der Fels wurde von Orks oder Trollen bearbeitet, und das ziemlich schlecht.\n");
  AddDetail(({"gang", "gaenge"}), "Der Gang verzweigt sich hier.\n");
  AddDetail("geruch", "Man koennte es auch Gestank nennen.\n");
  AddDetail("gestank", "Wenn der Himmel hier sichtbar waere, wuerde es zu ihm stinken.\n");
  AddDetail("boden", "Auf dem Boden findest Du nichts von Bedeutung.\n");
  AddDetail("decke", "Grauer Fels.\n");
  AddDetail(({"wand", "waende"}), "An der Wand siehst Du eine Inschrift.\n");
  AddDetail("inschrift", "Du kannst sie lesen.\n");
  AddReadDetail("inschrift", "Du liest: Nicht alles, was man nicht sieht, ist auch wichtig.\n");
  AddDetail(({"orks", "trolle"}), "Die laufen hier irgendwo rum ... oder liegen tot in der Ecke.\n");
  AddDetail("ecke", "In dieser Ecke sind jedenfalls keine.\n");
  AddExit("norden",L1("m4x2"));
  AddExit("osten",L1("m5x3"));
  AddExit("westen",L1("m3x3"));
}
