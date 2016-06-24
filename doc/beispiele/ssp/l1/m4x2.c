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
  "Du bist im Hoehlenlabyrinth der Schreckensspitze. Das hier scheint wohl\n"+
  "der Aufenthaltsraum von ein paar stinkenden Trollen zu sein. Eine Menge\n"+
  "alter Knochen liegen auf dem Boden rum und in der Ecke stinkt ein grosser\n"+
  "Haufen Trollkot vor sich hin. Alles in allem kein gemuetlicher Ort,\n"+
  "vielleicht solltest Du ihn nach Sueden verlassen.\n");
  AddDetail("knochen", "Die Knochen wurden von Trollen abgenagt.\n");
  AddDetail("boden", "Auf dem Boden liegen Knochen.\n");
  AddDetail("decke", "Die Decke hier ist nicht so grauenhaft interessant, ich wette es wird\ninteressanter, wenn Du ein paar Ebenen tiefer vordringst.\n");
  AddDetail(({"ebene", "ebenen"}), "Tja, wenn Du wuesstest wieviele Hohlraeume sich unter Dir befinden, dann\nwuerdest Du hier nicht so ruhig stehen.\n");
  AddDetail(({"hohlraum", "hohlraeume"}), "Erkunde sie! Sofort!\n");
  AddDetail(({"wand", "waende"}), "Sie sind verschmiert und ekelhaft.\n");
  AddDetail("troll", "Keiner da! Aetschbaetsch!\n");
  AddDetail("ecke", "Wuerg!\n");
  AddDetail(({"trollkot", "kot", "haufen"}), "Igitt! Das stinkt ja erbaermlich.\n");
  AddExit("sueden",L1("m4x3"));
  AddItem(MON("troll"), REFRESH_REMOVE);
  AddItem(MON("troll"), REFRESH_REMOVE);
  AddCmd("erkunde", "erkunden");
}

erkunden(str)
{ 
   notify_fail("Was moechtest Du erkunden?\n");
   if (!str || (str!="hohlraum" && str !="hohlraeume"))
      return 0;
   write("Also ein bisschen mehr musst Du da schon fuer tun.\n");
   return 1;
}
