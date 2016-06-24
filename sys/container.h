// MorgenGrauen MUDlib
//
// container.h -- Properties und Definitionen fuer Behaelter
//
// $Id: container.h 9020 2015-01-10 21:49:41Z Zesstra $

#ifndef __CONTAINER_H__
#define __CONTAINER_H__

#include <thing/restrictions.h>

// properties
#define P_CONTENTS            "contents"            // Obsolet!!!

#define P_CONTAINER           "container"          // Objekt ist ein Container
#define P_CNT_STATUS          "cnt_status"         // Status des Containers
#define P_TRANSPARENT         "transparent"        // kann man hineinschaun?
#define P_LIGHT_TRANSPARENCY  "light_transparency" // Lichtdurchlaessigkeit

#define P_PREPOSITION         "preposition"        // Sachen IN dem Container
#define P_SOURCE_PREPOSITION  "source_preposition" // kann man AUS ihm nehmen
#define P_DEST_PREPOSITION    "dest_preposition"   // ...wieder IN ihn stecken

#define P_MAX_WEIGHT          "max_weight"         // max. Gewicht
#define P_WEIGHT_PERCENT      "weight_percent"     // Gewichtsreduktion in %

#define P_TOTAL_OBJECTS       "total_objects"      // # der non-invis Objekte
#define P_MAX_OBJECTS         "max_objects"        // max. Objektanzahl

#define P_INT_LIGHT           "int_light"

#define P_ITEMS               "items"              // siehe AddItem()

#define P_PREVENT_PILE        "prevent_pile"       // siehe /std/corpse
#define P_PILE_NAME           "pile_name"          // Name des Spielers im Dativ

// special defines
#define CNT_STATUS_OPEN   0
#define CNT_STATUS_CLOSED 1
#define CNT_STATUS_LOCKED 2

#define R_INSERT "insert"

#define CNT_ARRAY       1
#define CNT_COLLECT     2

#endif // __CONTAINER_H__

#ifdef NEED_PROTOTYPES

#ifndef __CONTAINER_H_PROTO__
#define __CONTAINER_H_PROTO__

// prototypes
// from: container/description.c
varargs mixed make_invlist(object viewer, mixed inv, int array);

// from: container/restrictions.c
int query_weight_contents();
static int _query_last_content_change();

int MayAddWeight(int w);
int MayAddObject(object ob);
public int PreventInsert(object ob);
public int PreventLeave(object ob, mixed dest);
object *present_objects(string complex_desc);
object *locate_objects(string complex_desc,int info);

// container/items.c
public varargs object     AddItem( mixed filename, int refresh, mixed props);
#endif // __CONTAINER_H_PROTO__

#endif // NEED_PROTOTYPES
