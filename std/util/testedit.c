// MorgenGrauen MUDlib
//
// util/testedit.c -- Tragbarer Editor
//
// $Id: testedit.c 6371 2007-07-17 22:46:50Z Zesstra $
#pragma strict_types
#pragma save_types
#pragma pedantic
#pragma range_check
#pragma no_clone

inherit "/std/thing";
inherit "/std/util/ex";

#include <properties.h>

void create()
{
  ::create();
  AddId("ex");
  SetProp(P_NAME, "Editor");
  SetProp(P_SHORT, "Ein Testeditor");
  SetProp(P_LONG, "Testeditor: Kommando: ex\n");
  AddCmd("ex", "ex");
}
