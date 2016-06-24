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
  "Du bist im Hoehlenlabyrinth der Schreckensspitze. Eine lange Metallroehre\n"+
  "fuehrt weiter von Sueden nach Norden und scheint kein Ende zu nehmen. Deine\n"+
  "Schritte hallen blechern durch die Gaenge und informieren deren Bewohner\n"+
  "ueber Deine Anwesenheit.\n");
  AddDetail(({"metallroehre", "roehre", "gang", "gaenge"}), "Du befindest Dich in einer langen Metallroehre.\n");
  AddDetail("metall", "Das Metall sieht sehr stabil aus.\n");
  AddDetail(({"boden", "decke", "wand", "waende"}), "Alles ist aus Metall.\n");
  AddDetail("bewohner", "Hier scheinen keine zu sein.\n");
  AddExit("norden",L1("m1x2"));
  AddExit("sueden",L1("m1x4"));
}
