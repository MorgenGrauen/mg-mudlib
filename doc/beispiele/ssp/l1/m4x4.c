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
  "Du bist im Hoehlenlabyrinth der Schreckensspitze. Dies ist zweifellos die\n"+
  "Heimat eines der beruechtigten Eisenfresser, die Waende sehen auch schon\n"+
  "ganz zernagt aus. Der einzige Ausgang fuehrt nach Sueden.\n");
  AddDetail(({"wand", "waende"}), "Die Waende sind aus reinem Eisen und wurden von einem Eisenfresser schon\nziemlich stark misshandelt.\n");
  AddDetail("boden", "Der Boden ist aus Metall und funkelt im Licht. Solltest Du natuerlich Nacht-\nsicht haben, dann funkelt es nur in Deiner Einbildung.\n");
  AddDetail("decke", "Die Decke hat eine gewaltige Last zu tragen.\n");
  AddDetail("last", "Frag mich nicht wieviele Tonnen Gestein sich ueber Dir befinden.\n");
  AddDetail("gestein", "Sowas gibt es im Gebirge, wenn es auch hier nicht so offensichtlich ist.\n");
  AddDetail("gebirge", "Ach hoer doch auf ...\n");
  AddDetail("eisenfresser", "Der hat sich wohl zu Tode gefressen.\n");
  AddDetail(({"metall", "eisen"}), "Es glaenzt matt.\n");
  AddDetail("ausgang", "Er liegt im Sueden.\n");
  AddItem(MON("eisenfresser"), REFRESH_REMOVE);
  AddExit("sueden",L1("m4x5"));
}
