// MorgenGrauen MUDlib
//
// gilden_room.c -- Gildengebaeude
//
// $Id: gilden_room.c 6380 2007-07-20 22:32:38Z Zesstra $
#pragma strong_types
#pragma save_types
#pragma range_check
#pragma no_clone
#pragma pedantic

// #define NEED_PROTOTYPES
inherit "/std/gilde";
inherit "/std/gilden_ob";

#include <properties.h>
#include <attributes.h>
#include <new_skills.h>

void create() {
  gilde::create();
  gilden_ob::create();
  AddCmd(({"tritt","trete"}),"bei_oder_aus_treten");
  AddCmd(({"lern","lerne"}),"LearnSpell");
}

int advance(string arg) {
  int r1,r2;

  r1=gilde::advance(arg);
  r2=gilden_ob::try_advance();
  return (r1||r2);
}
