// MorgenGrauen MUDlib
//
// lighttypes.h -- Welche Typen von Licht gibt es im MorgenGrauen? 
//
// $Id: lighttypes.h,v 1.2 2004/06/12 14:00:48 Vanion Exp $
 
#ifndef __THING_LIGHTTYPES_H__
#define __THING_LIGHTTYPES_H__

#define P_LIGHT_TYPE   "light_type"  // Welches Licht herrscht hier vor?

// Mode-Parameter fuer CheckLightType
#define LT_CHECK_ANY     0 
#define LT_CHECK_ALL     1
#define LT_CHECK_MATCH   2
#define LT_CHECK_NONE    3

// Basislichttypen
#define LT_MISC          0  // Unbestimmt

// Natuerliche Lichtquellen (Tag)
#define LT_SUN           1  // Sonnenschein
#define LT_DIFFUSE       2  // Indirektes Tageslicht

// Natuerliche Lichtquellen (Nacht)
#define LT_MOON          4  // Mondlicht
#define LT_STARS         8  // Sternenlicht

// Drei besonders oft vorkommende Typen offenen Lichts
#define LT_CANDLE       16  // Kerzenlicht
#define LT_TORCH        32  // Fackelschein
#define LT_OPEN_FIRE    64  // Lagerfeuer etc.

// Magische erzeugtes Leuchten, wird nicht naeher definiert.
#define LT_MAGIC       128  // Magische Lichtquelle

// Alles was so gluehen kann: Von Gluehbirne bis zu Phosphor
#define LT_GLOWING     256  // Selbstleuchtende Lichtquellen

// Ein Raum kann auch explizit auf Dunkel gesetzt werden.
// Eine Mischung mit anderen Typen scheint aber wenig sinnvoll.
#define LT_DARKNESS    512  // Dunkelheit

// Gemischte Typen, sie dienen dazu, komfortabel Abfragen auf Lichttypen 
// zu implementieren. Beim setzen sollte immer besser auf die genaueren 
// Lichttypen zurueckgegriffen werden.

// Tageslicht im allgemeinen
#define LT_DAYLIGHT    (LT_SUN|LT_DIFFUSE)

// Da es verschiedene Typen offenen Feuers gibt, wird hier
// noch ein Abfragetyp definiert.
#define LT_FIRE        (LT_CANDLE|LT_TORCH|LT_OPEN_FIRE)

// Natuerliches Licht
#define LT_NATURAL     (LT_DAYLIGHT|LT_STARS|LT_MOON)

// Kuenstliches Licht
#define LT_ARTIFICIAL   (LT_MAGIC|LT_FIRE|LT_GLOWING)

#endif // _THING_LIGHTTYPES_H__

#ifdef NEED_PROTOTYPES

#ifndef __THING_LIGHTTYPES_H_PROTO__
#define __THING_LIGHTTYPES_H_PROTO__

// Diese Doku wird noch entfernt, wenn die Manpage erstellt worden ist.
// 
// Eine Funktion, mit der das Abfagen eines oder mehrerer Lichttypen 
// moeglich ist, ist im std/thing/description definiert. Beispiel: 
// 
// thing->CheckLightType(LT_DAYLIGHT) 
//       gibt true wenn ein Tageslichttyp gesetzt ist.
//
// thing->CheckLightType(LT_SUN|LT_MOON) 
//       gibt true, wenn einer der beiden Himmelskoerper leuchtet.

varargs int CheckLightType(int lighttype, int mode);

#endif // __THING_LIGHTTYPES_H_PROTO__

#endif // NEED_PROTOTYPES
