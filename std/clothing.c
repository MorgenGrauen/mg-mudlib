// MorgenGrauen MUDlib
//
// armour.c -- armour standard object
//
// $Id: armour.c,v 3.8 2003/08/25 09:36:04 Rikus Exp $

#pragma strict_types
#pragma save_types
#pragma no_clone
#pragma range_check
#pragma pedantic

inherit "/std/thing/properties";
inherit "/std/thing/commands";
inherit "/std/thing/restrictions";
inherit "/std/thing/light";
inherit "/std/clothing/description";
inherit "/std/clothing/moving";
inherit "/std/clothing/wear";
inherit "/std/thing/language";
inherit "/std/thing/envchk";

//#define NEED_PROTOTYPES

#include <config.h>
#include <properties.h>
#include <language.h>
#include <combat.h>
#include <wizlevels.h>
#include <defines.h>

protected void create()
{
  properties::create();
  commands::create();
  light::create();
  restrictions::create();
  description::create();
  wear::create();
  envchk::create();
  AddId("Ding");
}

protected void create_super() {
  set_next_reset(-1);
}

// zum Ueberschreiben, damit es nicht buggt, wenn Leute (wie gewuenscht) in 
// ihren Objekten ::reset() aufrufen.
void reset() {
}

int IsClothing() {return 1;}

