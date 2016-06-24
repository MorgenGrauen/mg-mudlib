// MorgenGrauen MUDlib
//
// living/helpers.c -- (Query)Methoden fuer Hilfsobjekte, z.B. zum Tauchen
//
// $Id: moneyhandler.h,v 3.1 1997/02/12 13:29:09 Wargon Exp %

#pragma strong_types
#pragma save_types
#pragma range_check
#pragma no_clone
#pragma pedantic

#include <living/helpers.h>
#define NEED_PROTOTYPES
#include <thing/properties.h>
#undef NEED_PROTOTYPES

public int RegisterHelperObject(object helper, int type, 
                                string|closure callback) 
{
  // cb: closure auf die Callback-Funktion in previous_object()
  closure cb;
  // helpers: Mapping aller eingetragenen Helfer-Objekte
  mapping helpers;

  // Kein positiver Integerwert als Helfertyp uebergeben?
  if ( !intp(type) || type < 1 )
     raise_error(sprintf( "Wrong argument 1 to RegisterHelperObject(). "
        "Expected positive <int>, got %O.\n", type));
  // Kein Objekt vorhanden, an dem die Callback-Funktion gerufen werden soll?
  if ( !objectp(helper) )
    return HELPER_NO_CALLBACK_OBJECT;

  // Funktionsname zum Zweck des Callbacks uebergeben?
  if ( stringp(callback) ) {
    // Dann Closure davon erstellen.
    cb = symbol_function(callback, helper);
    // Wenn das nicht klappt (zB weil die Funktion private ist), dann
    // Fehler werfen und abbrechen.
    if ( !closurep(cb) )
      raise_error(sprintf("Error in RegisterHelperObject(): Unable to call "
        "function %s in object %O.\n", callback, helper));
  }
  // Wenn schon eine Closure uebergeben wurde, dann diese direkt speichern.
  else if ( closurep(callback) ) {
    cb = callback;
  }
  // Weder Funktionsname, noch Closure, dann Fehler werfen und abbrechen.
  else
    raise_error(sprintf("Wrong argument 2 to RegisterHelperObject(). "
      "Expected <string/closure>, got %O.\n",callback));

  // Property auslesen und zwischenspeichern
  helpers = QueryProp(P_HELPER_OBJECTS);
  // Wenn die Prop leer ist, hier initialisieren
  if ( !helpers ) {
    helpers = ([type:({})]);
  }
  // Wenn der Typ noch nicht existiert, hier nachtragen.
  else if ( !pointerp(helpers[type]) ) {
    helpers[type] = ({});
  }

  // Closure eintragen, wenn noch nicht vorhanden
  if ( member(helpers[type], cb)==-1 ) {
    helpers[type] = helpers[type]+({cb});
    SetProp(P_HELPER_OBJECTS, helpers);
    return HELPER_SUCCESS;
  }
  else
    return HELPER_ALREADY_LISTED;
}

public int UnregisterHelperObject(object helper, int type) {
  if ( !intp(type) || type < 1 )
     raise_error(sprintf( "Wrong argument 2 to UnregisterHelperObject(). "
        "Expected positive <int>, got %O.\n", type));
  if ( !objectp(helper) )
    return HELPER_NO_CALLBACK_OBJECT;

  mapping helpers = Query(P_HELPER_OBJECTS, F_VALUE);

  if ( mappingp(helpers) ) {
    foreach(closure cl: helpers[type]) {
      if ( get_type_info(cl,2) == helper ) {
        helpers[type] = helpers[type]-({cl});
        return HELPER_SUCCESS;
      }
    }
  }
  return HELPER_NOTHING_TO_UNREGISTER;
}

// Querymethode fuer P_AQUATIC_HELPERS
public mapping _query_lib_p_aquatic_helpers() {
  mapping ret = ([]);
  // eingetragene Callback-Closures auslesen
  closure *helpers = 
    ( Query(P_HELPER_OBJECTS, F_VALUE) || ([]) )[HELPER_TYPE_AQUATIC];
  // Es sind gar keine Werte eingetragen? Dann gleich rausspringen.
  if ( !pointerp(helpers) )
    return ret;

  // Nullelement substrahieren
  helpers -= ({0});

  if ( sizeof(helpers) ) {
    // Mapping erstellen: Keys sind die Objekte, deren Closures eingetragen
    // sind. Values sind die Rueckgabewerte der Closures,
    // die dabei das Spielerobjekt und das abfragende Objekt uebergeben
    // bekommen.
    object *keys = map(helpers, #'get_type_info, 2);
    int *vals = map(helpers, #'funcall, this_object(), previous_object(1));
    ret = mkmapping(keys,vals);
  }
  return ret;
}

// Querymethode fuer P_AERIAL_HELPERS
public mapping _query_lib_p_aerial_helpers() {
  mapping ret = ([]);
  // eingetragene Callback-Closures auslesen
  closure *helpers = 
    ( Query(P_HELPER_OBJECTS, F_VALUE) || ([]) )[HELPER_TYPE_AERIAL];

  // Es sind gar keine Werte eingetragen? Dann gleich rausspringen.
  if ( !pointerp(helpers) )
    return ret;
  
  // Nullelement substrahieren
  helpers -= ({0});

  if ( sizeof(helpers) ) {
    // Mapping erstellen: Keys sind die Objekte, deren Closures eingetragen
    // sind. Values sind die Rueckgabewerte der Closures,
    // die dabei das Spielerobjekt und das abfragende Objekt uebergeben
    // bekommen.
    object *keys = map(helpers,#'get_type_info, 2);
    int *vals = map(helpers, #'funcall, this_object(), previous_object(1));
    ret = mkmapping(keys,vals);
  }
  return ret;
}

// Querymethode fuer P_HELPER_OBJECTS
public mapping _query_lib_p_helper_objects() {
  return deep_copy(Query(P_HELPER_OBJECTS,F_VALUE));
}

