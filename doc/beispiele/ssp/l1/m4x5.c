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
  "Du bist im Hoehlenlabyrinth der Schreckensspitze. Die metallischen Roehren\n"+
  "verzweigen sich hier nach Norden, Osten und Westen. Im Norden scheint etwas\n"+
  "Unangenehmes zu lauern, merkwuerdige Geraeusche dringen von dort hierher.\n");
  AddDetail(({"wand", "waende", "roehre", "roehren", "gaenge", "gang"}), "Die Gaenge sind voellig mit Metall ausgekleidet.\n");
  AddDetail("metall", "Das Metall glaenzt silbern.\n");
  AddDetail("unangenehmes", "Wer weiss, was sich dort verbirgt ...\n");
  AddDetail("boden", "Du bist froh, dass es hier einen Boden gibt, sonst wuerdest Du tief fallen.\n");
  AddDetail("decke", "Die Decke ist metallisch und glaenzt.\n");
  AddDetail(({"geraeusch", "geraeusche"}), "Es quietscht.\n");
  AddExit("norden",L1("m4x4"));
  AddExit("osten",L1("m5x5"));
  AddExit("westen",L1("m3x5"));
}
