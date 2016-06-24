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
  "Du bist im Hoehlenlabyrinth der Schreckensspitze. Ein schmaler Gang, der\n"+
  "von Norden nach Sueden fuehrt, ist hier durch den harten Fels gehauen.\n"+
  "Der Gang ist nur grob behauen, ein Zeichen dafuer, dass hier nicht Zwerge\n"+
  "am Werk waren, sondern boese Wesen, wahrscheinlich Orks.\n");
  AddDetail("fels", "Der Fels ist hart und nur grob behauen.\n");
  AddDetail(({"gang", "gaenge"}), "Der Gang fuehrt von Norden nach Sueden.\n");
  AddDetail("boden", "Auf dem Boden siehst Du zahlreiche Ueberreste von Abenteurern, die nicht\nvorsichtig genug waren.\n");
  AddDetail(({"rest", "ueberrest", "reste", "ueberreste", "abenteurer"}),
  "Uhoh ... blutige Sache ...\n");
  AddDetail("blut", "Weia ...\n");
  AddDetail(({"wand", "waende"}), "Die Waende sind aus Fels und nicht sonderlich interessant.\n");
  AddDetail("decke", "Wenn Du ueberlegst, was da alles ueber Deinem Kopf schwebt ... ohoh!\n");
  AddDetail(({"ork", "orks"}), "Davon hat es sicher einige in der Naehe.\n");
  AddDetail(({"zwerg", "zwerge"}), "Zwerge haben hiermit nichts zu tun!\n");
  AddExit("sueden",L1("m3x3"));
  AddExit("norden",L1("m3x1"));
}
