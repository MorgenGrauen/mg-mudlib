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
#include <thing/properties.h>

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

protected void create()
{
  if( clonep() )
    call_out(#'check_for_environment, 3, object_name(previous_object()));
}

// Kein Envcheck fuer Auto-Objekte, dafuer entfernen diese sich im naechsten
// Reset, sofern sie noch kein Environment haben.
// BTW: Abschalten des Resets sorgt fuer kein Aufraeumen.
public int SetAutoObject(int autostate)
{
  if (autostate)
  {
    Set("p_lib_autoobject", 1, F_VALUE);
    Set("p_lib_autoobject", PROTECTED|NOSETMETHOD, F_MODE_AS);
    remove_call_out(#'check_for_environment);
    if (autostate > 1)
      set_next_reset(autostate);
    // else: Standardresetzeit
  }
  else
  {
    // (Re-)Enable envchk
    Set("p_lib_autoobject", 0, F_VALUE);
    Set("p_lib_autoobject", PROTECTED|NOSETMETHOD, F_MODE_AD);
    if (clonep() && find_call_out(#'check_for_environment) == -1)
      call_out(#'check_for_environment, 3, object_name(previous_object()));
  }
  return autostate;
}

void reset()
{
  // wenn kein env und Autoobject: zerstoeren
  if (clonep() && !environment()
      && Query("p_lib_autoobject", F_VALUE))
  {
    remove(1);
    // hier wirklich erzwingen, um unter allen Umstaenden zu vermeiden, dass
    // sich dauerhaft Clones ansammeln, die niemand mehr referenziert.
    if (objectp(this_object()))
      destruct(this_object());
  }
}
