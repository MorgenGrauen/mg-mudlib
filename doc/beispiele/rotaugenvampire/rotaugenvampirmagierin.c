/*
   Ein magisch begabter Beispiel-NPC (von Zesstra, angelehnt an Boings
   bspmon1.c)
   In diesem Fall soll eine Magierin sein, die aber die gleichen Stats wie der
   maennliche Kollege hat.
*/

// Diese Pragmas sorgen dafuer, dass der Driver darauf achtet, dass bei
// Funktionsargumenten, -Rueckgabewerten und teilweise bei Zuweisung von
// Werten an Variablen der richtige Datentyp verwendet wird (z.b. kein string
// anstelle eines int verwendet wird). Sollte in keinem Objekt fehlen.
#pragma strong_types, save_types, rtt_checks
// Einige Warnungen werden als Fehler betrachtet (empfohlen)
#pragma pedantic, range_check
// Schaltet bestimmte Warnungen ein (empfohlen)
#pragma warn_deprecated, warn_empty_casts, warn_missing_return
#pragma warn_function_inconsistent

// Dieser NPC soll eine Rotaugenvampirmagierin sein. Und nen
// Rotaugenvampirmagier gibt es schon. Also einfach den normalen
// Rotaugenvampirmagier aus diesem Verzeichnis erben.
inherit __DIR__+"rotaugenvampirmagier";

// Inkludiert Definitionen fuer Properties
#include <properties.h>
#include <language.h> // fuer FEMALE-Define

protected void create()
{
  // Nicht vergessen, ohne das geht nichts. Fuehrt das create() des geerbten
  // Rotaugenvampirmagiers aus und konfiguriert ihn.
  ::create();

  // Geschlecht aendern.
  SetProp(P_GENDER, FEMALE);

  // Kurz- und Langbeschreibung aendern.
  SetProp(P_SHORT, "Rotaugenvampirmagierin");
  SetProp(P_LONG, break_string(
    "Eine finster aussehende Rotaugenvampirmagierin in einer Kutte.",78));

  // Name anpassen
  SetProp(P_NAME, "Rotaugenvampirmagierin");

  // Meldung anpassen.
  SetProp(P_HANDS, ({" mit ihren Haenden", 85}));
}

