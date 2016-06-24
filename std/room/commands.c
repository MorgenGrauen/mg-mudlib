// MorgenGrauen MUDlib
//
// room/commands.c -- room commands handling
//
// $Id: commands.c 8201 2012-11-07 17:55:12Z Zesstra $

#pragma strong_types
#pragma save_types
#pragma pedantic
#pragma range_check
#pragma no_clone

inherit "/std/thing/commands";

//#define NEED_PROTOTYPES

#include <thing/properties.h>
#include <config.h>
#include <properties.h>
#include <language.h>
#include <defines.h>

void init() 
{
  ::init();

  add_action("imposs", "such");
  add_action("imposs", "suche");
}

/* Fuer etwas bessere Fehlermeldungen als 'Wie bitte?' bei einigen */
/* allgemeinen Kommandos.					   */
int imposs()
{
  _notify_fail("Du suchst, findest aber nichts.\n");
  return 0;
}
