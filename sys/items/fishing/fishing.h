// MorgenGrauen MUDlib
//
// fishing.h -- Definitionen fuer alles, was sich ums Angeln dreht
//              Fragen, Vorschlaege etc. an Arathorn
//
// $Id: fishing.h,v 3.2 2002/12/17 10:01:43 Rikus Exp $

#ifndef __FISHING_H__
#define __FISHING_H__

// Standardobjekte/-pfade
#define ANGELBASE(x) ("/std/items/fishing/"+x)
#define STD_ANGEL    ANGELBASE("angel")
#define STD_FISCH    ANGELBASE("fish")
#define STD_HAKEN    ANGELBASE("haken")
#define STD_KOEDER   ANGELBASE("koeder")

#define ANGELOBJ(x)  ("/items/fishing/"+x)

// ******************
// * Gewaessertypen *
// ******************
#define P_WATER  "water"

// Gewaessertypen
// Salzwasser
#define W_BEACH     1    // Strand
#define W_HARBOR    2    // Hafen
#define W_OCEAN     4    // Ozean
// Suesswasser
#define W_RIVER     8    // Fluss
#define W_POOL     16    // Teich
#define W_LAKE     32    // See
#define W_ROCK     64    // Bergbach
#define W_STREAM   256   // Bach
// Sonstige
#define W_USER     128   // hier kann man ein eigenes Aquarium anhaengen!
#define W_DEAD     512   // Lebloses Wasser/enthaelt keine Fische/man kann
                         // aber die Stdflasche fuellen.
#define W_OTHER   1024   // Flasche enthaelt Fluessigkeit!=Wasser

// vordefinierte Kombinationen fuer Angeln:
#define W_SHORT (W_HARBOR|W_RIVER|W_POOL|W_LAKE|W_ROCK|W_USER|W_OCEAN|W_STREAM)
#define W_LONG  (W_BEACH|W_USER)
#define W_SALT  (W_HARBOR|W_OCEAN|W_BEACH)               // Salzwasser
#define W_SWEET (W_RIVER|W_POOL|W_LAKE|W_ROCK|W_STREAM)  // Suesswasser
#define W_UNIVERSAL (W_SHORT|W_LONG)

// **********
// * Fische *
// **********
#define P_FISH     "fish"
#define P_FISH_AGE "fish_age"

// Flags fuer Fische
#define F_NOROTTEN  1   // Fisch fault nicht.
#define F_NOTHUNGRY 2   // isst Koeder nicht auf, falls zu schwer fuer Angel
#define F_REPLACE   4   // Soll sich beim Entfernen von Angel verwandeln.
#define F_NOHEAL    8   // Fisch heilt nicht bei Verzehr

// Pfad zum "Aquarium", dort finden sich die Standardfische. Zuordnung
// zu den Gewaessertypen siehe /sys/items/fishing/aquarium.h
#define FISH(x) ANGELOBJ("aquarium/"+x)

// IDs fuer Angel,Haken,Wurm,Fisch
#define ANGEL_ID "\nf_angel"
#define HAKEN_ID "\nf_koeder"
#define WURM_ID  "\nf_worm"
#define FISCH_ID "\nf_fish"

#endif // __FISHING_H__

