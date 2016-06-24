inherit "std/room";

#include <properties.h>
#include <rooms.h>
#include "../files.h"

create()
{
  ::create();
  SetProp(P_LIGHT, 0);
  SetProp(P_INT_SHORT, "Steinebene");
  SetProp(P_INT_LONG,
  "Du bist im Hoehlenlabyrinth der Schreckensspitze. Du hast die Hoehle eines\n"+
  "schrecklichen Untiers erreicht. Der Gestank ist wirklich grauenerregend\n"+
  "und der Anblick rumliegender Knochen und halb verwester Koerperteile von\n"+
  "armen Opfern tut sein Uebriges, um Dir den Appetit zu verderben. Der\n"+
  "einzige Ausgang aus diesem Loch befindet sich im Osten.\n");
  AddDetail(({"hoehle", "loch"}), "Ein uebles Loch, in das Du hier geraten bist.\n");
  AddDetail("gestank", "Dir wird beinahe schlecht.\n");
  AddDetail(({"teile", "koerperteile"}), "Buah, das ist ja widerlich.\n");
  AddDetail("ausgang", "Der Ausgang ist im Osten.\n");
  AddSpecialDetail("knochen", "knochen");
  AddDetail(({"wand", "waende"}), "Die Waende sind langweilig und grau.\n");
  AddDetail("decke", "Die Decke ist uninteressant.\n");
  AddDetail("boden", "Die verwesenden Koerperteile auf dem Boden sind kein schoener Anblick.\n");
  AddExit("osten",L1("m5x1"));
  AddItem(MON("trollmops"), REFRESH_REMOVE);
}

knochen()
{
   if (present("trollmops"))
      return "Der Trollmops laesst nicht zu, dass Du sein Abendessen durchwuehlst.\n";

   if (this_player()->FindPotion("Du wuehlst in den Knochen herum und findest einen Zaubertrank.\n"))
      return "";
   return "Du wuehlst in den Knochen herum, findest aber nichts.\n";
}
