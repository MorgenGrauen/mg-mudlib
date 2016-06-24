#pragma strict_types

#include <moving.h>
#include <properties.h>
#include <language.h>

inherit "std/thing";

void create()
{
  ::create();
  SetProp( P_NAME, "Stempel des Todes" );
  SetProp( P_GENDER, MALE );
  SetProp( P_SHORT, 0 );
  SetProp( P_INVIS, 1 );
  SetProp( P_NEVERDROP, 1 );
  SetProp( P_WEIGHT, 0);
  AddId(({"death_mark","\ndeath_mark"}));
}

void reset()
{
  ::reset();
  if (environment() && !query_once_interactive(environment()))
    remove();
}


/*
 * Function name: start_death
 * Description:   Start the death sequence.
 */
void start_death()
{
  if ( !environment() || !query_once_interactive(environment())
       || !environment()->QueryProp(P_GHOST) )
  {
      destruct(this_object());
      return;
  }

  say("Du siehst eine dunkle Gestalt, die etwas Dunst einsammelt ... oder vielleicht\n"+
      "bildest du Dir das auch nur ein ...\n");
 environment()->move("/room/death/virtual/death_room_"+getuid(environment()),
		     M_GO|M_SILENT|M_NO_SHOW|M_NOCHECK);
 // direkt in virtuellen Todesraum moven
}

protected int PreventMove(object dest, object oldenv, int method) {
  // wenn die marke schon ein Env hat oder der move nicht in einen Spieler
  // geht, gehts nur mit M_NOCHECK.
  if ( (environment(this_object()) || !query_once_interactive(dest)) 
      && !(method & M_NOCHECK)) {
    return ME_CANT_BE_DROPPED;
  }

  return ::PreventMove(dest, oldenv, method);
}

int _query_autoloadobj() { return 1; }

void init()
{
  ::init();
  if (this_player() == environment())
    start_death();
}
