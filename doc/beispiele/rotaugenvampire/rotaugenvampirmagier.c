/*
   Ein magisch begabter Beispiel-NPC (von Zesstra, angelehnt an Boings
   bspmon1.c)
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

// Dieser NPC soll ein Rotaugenvampir sein, der ein paar Spells kann. Und nen
// Rotaugenvampir gibt es schon. Also einfach den normalen Rotaugenvampir
// aus diesem Verzeichnis erben.
inherit __DIR__+"rotaugenvampir";

// Inkludiert Definitionen fuer Properties
#include <properties.h>

protected void create()
{
  // Nicht vergessen, ohne das geht nichts. Fuehrt das create() des geerbten
  // Rotaugenvampir aus und konfiguriert ihn.
  ::create();

/* Die Kurzbeschreibung wird zum Beispiel angezeigt wenn man in einen Raum
   mit dem Monster reinlaeuft */
  SetProp(P_SHORT, "Rotaugenvampirmagier");

/* Beim Anschauen des Monsters wird die long-description angezeigt,
 * Zeilenumbruch nach Bedarf nach 78 Zeichen. */
  SetProp(P_LONG, break_string(
    "Ein finster aussehender Rotaugenvampir in einer Kutte.",78));

/* Ein Name muss sein, da sonst z.B. im Todesfall 'Leiche von 0' daliegt */
  SetProp(P_NAME, "Rotaugenvampirmagier");
  SetProp(P_NAME_ADJ,"listenreich");

  // fast alle Eigenschaften uebernehmen wir einfach unveraendert unveraendert
  // vom normalen Rotaugenvampir. Die Hands nicht, der Magier haut nicht so
  // gut zu...
  SetProp(P_HANDS, ({" mit seinen Haenden", 85}));

  // ok, spielt wenig Rolle in diesem Fall, aber damits stilecht ist, kriegt
  // der hier jetzt etwas mehr an Magiepunkten (spellpoints)
  SetProp(P_MAX_SP, 350);

  // und noch ein paar Spells
  // Anmerkung: diese Spells haben nichts mit irgendwelchen Gildenspells zu
  // tun und - ja, tatsaechlich - verbrauchen keine Magiepunkte...

  // wahrscheinlichkeit in % pro runde, einen Spell zu casten:
  SetProp(P_SPELLRATE,30);
  // Ein Spell, der 100 Schadenspunkte macht und in 60% der Faelle ausgewaehlt
  // wird. Erste Meldung geht an den Spieler, die zweite an den Raum.
  // Schadenstyp ist Schlagschaden.
  AddSpell(60,100,
    break_string(Name(WER) + " macht eine Handbewegung vor Deinem Gesicht. "
      "So abgelenkt stolperst Du in faellst hart zu Boden.",78),
    break_string(Name(WER) + " macht eine Handbewegung vor @WESSEN Gesicht. "
      "@WER stolpert und faellt hart auf den Boden.",78),
    ({DT_BLUDGEON}) );
  // Ein Spell, der 800 Schadenspunkte macht und in 40% der Faelle ausgewaehlt
  // wird. Schadenstyp: Feuer.
  AddSpell(40,800,
    break_string(Name(WER) + " schaut Dich mit brennenden Augen "
      "durchdringend an. Du glaubst, innerlich zu verbrennen.",78),
    break_string(Name(WER) + " schaut @WEN mit gluehenden Augen an. "
      "@WER kruemmt sich vor Schmerzen.",78),
    ({DT_FIRE}) );
}

