// MorgenGrauen MUDlib
//
// envchk.c -- verhindern, dass objekte ohne env herumfliegen
//
// $Id: thing.c 6283 2007-05-09 21:30:33Z Zesstra $                      

#pragma strict_types
#pragma save_types
#pragma range_check
#pragma no_clone
#pragma pedantic

#include <moving.h>
#define NEED_PROTOTYPES
#include <thing/moving.h>

protected void check_for_environment(string cloner)
{
  // Clones, die innerhalb von 10 Sekunden kein Environment haben,
  // sollen auf -debug scrollen.
  if ( clonep() && !environment() ) {
    // mal in den Muellraum bewegen, damit diese Objekte zwar nicht zerstoert
    // werden, aber zumindest hinterher noch einfach auffindbar sind. (Und
    // entweder per hand oder automatisch aufgeraeumt werden koennen.)
    move("/room/muellraum",M_NOCHECK|M_SILENT);
    if ( !stringp(cloner) || !sizeof(cloner) )
      cloner = "<Unbekannt>";
    raise_error("Objekt hat kein Environment. Cloner: ["+cloner+"] ");
  }
}

void create()
{
	if( clonep() ) 
    call_out(#'check_for_environment, 3, object_name(previous_object()));
}
