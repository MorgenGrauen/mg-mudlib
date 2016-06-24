inherit "std/room";

#include <properties.h>
#include <rooms.h>
#include "../files.h"

create()
{
  replace_program("std/room");
  ::create();
  SetProp(P_LIGHT, 0);
  SetProp(P_INDOORS, 1);
  SetProp(P_INT_SHORT, "Metallebene");
  SetProp(P_INT_LONG,
  "Du bist im Hoehlenlabyrinth der Schreckensspitze. Der Gang, in dem Du\n"+
  "Dich befindest fuehrt schnurgerade von Norden nach Sueden. Die Waende\n"+
  "reflektieren das Licht und Deine Schritte erzeugen einen metallischen\n"+
  "Klang auf dem Boden.\n"); 
  AddDetail(({"boden", "wand", "waende", "decke", "metall"}), "Alles ist aus Metall.\n");
  AddDetail("gang", "Er fuehrt nach Norden und Sueden.\n");
  AddDetail("licht", "Es kommt irgendwoher.\n");
  AddDetail("schritte", "Wenn Du laeufst, machst Du Schritte, das wusstest Du aber, oder?\n");
  AddDetail("klang", "Der Klang ist nicht sonderlich interessant, aber zweifellos vorhanden.\n");
  AddExit("norden",L1("m2x2"));
  AddExit("sueden",L1("m2x4"));
  AddItem(MON("dosenoeffner"), REFRESH_REMOVE);
}
