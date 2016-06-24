// MorgenGrauen MUDlib
//
// living/helpers.h -- Konstanten und Methoden fuer Hilfsobjekte
//
// $Id: moneyhandler.h,v 3.1 1997/02/12 13:29:09 Wargon Exp %

#ifndef __LIVING_HELPERS_H__
#define __LIVING_HELPERS_H__

// Properties

#define P_HELPER_OBJECTS    "lib_p_helper_objects"

#define P_AQUATIC_HELPERS   "lib_p_aquatic_helpers"
#define P_AERIAL_HELPERS    "lib_p_aerial_helpers"

// Defines fuer Helferobjekt-Typen und Rueckgabewerte

#define HELPER_TYPE_AQUATIC 1
#define HELPER_TYPE_AERIAL  2

#define HELPER_NO_CALLBACK_OBJECT    -1
#define HELPER_ALREADY_LISTED        -2
#define HELPER_NOTHING_TO_UNREGISTER -3
#define HELPER_SUCCESS                1

#endif // __LIVING_HELPERS_H__

// Prototypes

#ifdef NEED_PROTOTYPES

#ifndef __LIVING_HELPERS_H_PROTO__
#define __LIVING_HELPERS_H_PROTO__

int RegisterHelperObject(object helper, int type, mixed callback);
int UnregisterHelperObject(object helper, int type);

#endif // __LIVING_HELPERS_H_PROTO__

#endif // NEED_PROTOYPES

