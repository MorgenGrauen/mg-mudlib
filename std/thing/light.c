// MorgenGrauen MUDlib
//
// thing/description.c -- Lichtsystemkomponenten fuer Standardobjekte
//
// $Id: description.c 7635 2010-08-19 21:21:30Z Zesstra $

#pragma strict_types
#pragma save_types,rtt_checks
#pragma range_check
#pragma no_clone
#pragma pedantic

#define NEED_PROTOTYPES
#include <thing/properties.h>
#include <thing/language.h>
#include <hook.h>
#undef NEED_PROTOTYPES

#include <thing/lighttypes.h>
#include <properties.h>

//                       #####################
//######################## System-Funktionen ############################
//                       #####################

// Objekt erzeugen
protected void create()
{
  Set( P_LIGHT, 0 );
  Set( P_LIGHT_TYPE, LT_MISC);
}

protected void create_super() {
  set_next_reset(-1);
}     

// Lichtsy ... sys ...
static int _query_total_light() { return QueryProp(P_LIGHT); }

static int _set_light( int light )
{
    object env = this_object();

    // TODO: Temporaer Lichtlevel in geeignete Wertefenster zwingen.
    if (light > 100)
      light = 100;
    else if (light < -100)
      light = -100;
   
    while ( objectp(env = environment(env)) )
        // Ja. Man ruft die _set_xxx()-Funktionen eigentlich nicht direkt auf.
        // Aber das Lichtsystem ist schon *so* rechenintensiv und gerade der
        // P_LAST_CONTENT_CHANGE-Cache wird *so* oft benoetigt, dass es mir
        // da um jedes bisschen Rechenzeit geht.
        // Der Zweck heiligt ja bekanntlich die Mittel. ;-)
        //
        // Tiamak
        env->_set_last_content_change();
    
    return Set( P_LIGHT, light, F_VALUE);
}

