inherit "std/room";

#include <properties.h>
#include "../files.h"

create()
{
  replace_program("std/room");
  ::create();
  SetProp(P_LIGHT, 0);
  SetProp(P_INDOORS, 1);
  SetProp(P_INT_SHORT, "Metallebene");
  SetProp(P_INT_LONG,
  "Du bist im Hoehlenlabyrinth der Schreckensspitze. Die Metallroehre, in der\n"+
  "Du Dich befindest fuehrt von Sueden nach Norden. Auch hier klingt der Boden\n"+
  "unter Deinen Fuessen recht hohl, es ist wohl wirklich etwas Besonderes\n"+
  "darunter. Allerdings sind keinerlei Anzeichen fuer einen Zugang zu sehen.\n");
  AddDetail(({"gang", "roehre", "metallroehre"}), "Der Gang fuehrt nach Norden und Sueden.\n");
  AddDetail("metall", "Hartes, glaenzendes Metall.\n");
  AddDetail(({"wand", "waende", "boden", "decke"}), "Alles ist aus Metall.\n");
  AddDetail("zugang", "Es ist keiner zu sehen.\n");
  AddExit("norden",L1("m1x3"));
  AddExit("sueden",L1("m1x5"));
}
