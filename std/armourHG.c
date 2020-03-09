// MorgenGrauen MUDlib
//
// armourHG.c -- armour standard object for hats and glasses
//
// $Id: armourHG.c 7804 2011-07-10 20:37:52Z Zesstra $

#pragma strict_types
#pragma save_types
#pragma no_clone
#pragma range_check

inherit "/std/armour";

#define NEED_PROTOTYPES
#include <thing/commands.h>
#undef NEED_PROTOTYPES

#include <language.h>
#include <defines.h>

int setzen(string s);

void create() {
    ::create();
    AddCmd(({"setz","setze"}),"setzen");
}

varargs void doWearMessage( int all ) {
  if( query_once_interactive(PL) ) {
    write( "Du setzt " + name(WEN,1) + " auf.\n" );
  }
  if (objectp(environment()) && objectp(environment(environment())))
    tell_room(environment(environment()),
	capitalize((string)PL->name(WER)) + " setzt " + name(WEN,0) + 
	" auf.\n");
}

varargs void doUnwearMessage( object worn_by, int all ) {
  if( query_once_interactive(worn_by) ) {
    tell_object(worn_by,  "Du setzt " + name(WEN,1) + " ab.\n" );
  }
  tell_room(environment(worn_by), 
	    (capitalize((string)worn_by->name(WER))) + " setzt " +
	    name(WEN,0) + " ab.\n", ({worn_by}));
}

int setzen(string str) {
  string ob;

  if(!str)
    return 0;
  if(sscanf(str, "%s auf", ob)==1 )
    return _do_wear(ob, 0, 0);
  if(sscanf(str, "%s ab", ob)==1 )
    return _do_unwear(ob, 0, 0);
  return 0;
}

