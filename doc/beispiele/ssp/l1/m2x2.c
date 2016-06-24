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
  "Du bist im Hoehlenlabyrinth der Schreckensspitze. Hier verzweigt sich der\n"+
  "Gang in verschiedene Metallroehren, die nach Norden, Sueden und Westen\n"+
  "fuehren. Ausserdem ist in der Decke ein kleines Loch, durch das eine\n"+
  "Leiter nach oben fuehrt.\n");
  AddDetail(({"gang", "gaenge", "metallroehre", "metallroehren", "roehre", "roehren"}), "Gaenge fuehren in die verschiedensten Richtungen.\n");
  AddDetail(({"wand", "waende", "boden", "metall"}), "Hier ist einfach alles aus Metall.\n");
  AddDetail("leiter", "Die Leiter fuehrt durch ein Loch in der Decke nach oben.\n");
  AddDetail(({"loch", "decke"}), "In der Decke ist ein Loch.\n");
  AddExit("sueden",L1("m2x3"));
  AddExit("norden",L1("m2x1"));
  AddExit("westen",L1("m1x2"));
  AddExit("oben", L1("oben"));
}
