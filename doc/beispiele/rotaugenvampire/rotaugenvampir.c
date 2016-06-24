/*
   Ein Beispiel-NPC (von Zesstra, angelehnt an Boings bspmon1.c)
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

// Erbt den Standard-NPC / das Standard-Monster
inherit "/std/npc";

// Inkludiert Definitionen fuer Properties und einige Konstanten
#include <properties.h>
#include <language.h>
#include <class.h> // Konstanten fuer AddClass()

// create() wird beim Erzeugen des Objekt vom Driver gerufen und initialisiert
// diesen NPC. Es bekommt kein Argument, gibt keinen Wert zurueck (Typ: void)
// und kann nicht von ausserhalb des Objektes gerufen werden (protected).
protected void create()
{
  // Nicht vergessen, ohne das geht nichts. Fuehrt das create() des geerbten
  // /std/npc aus und initialisiert den Standard-NPC.
  ::create();

/* Die Kurzbeschreibung wird zum Beispiel angezeigt wenn man in einen Raum
   mit dem Monster reinlaeuft */
  SetProp(P_SHORT, "Rotaugenvampir");

/* Beim Anschauen des Monsters wird die long-description angezeigt,
 * Zeilenumbruch nach Bedarf nach 78 Zeichen. */
  SetProp(P_LONG, break_string(
    "Ein finster aussehender Rotaugenvampir.",78));

/* Ein Name muss sein, da sonst z.B. im Todesfall 'Leiche von 0' daliegt */
  SetProp(P_NAME, "Rotaugenvampir");
  SetProp(P_NAME_ADJ,"ausgemergelt");
  
/* Das Geschlecht des Monsters. Als Geschlechter sind */
/* die in <language.h> definierten Symbole NEUTER, MALE   */
/* und FEMALE zulaessig.                                  */
  SetProp(P_GENDER, MALE);

/* Ein Monster braucht mindestens eine ID, unter der man es ansprechen kann */
/* Es kann aber auch mehrere verschiedene ID's haben.                       */
  AddId( ({"vampir","rotaugenvampir"}) );

/* Zuerst sollte man dem Monster einen Grundlevel geben. */
/* Setzt Standardwerte fuer P_LEVEL, P_MAX_HP (Lebenspunkte), P_MAX_SP
 * (Magiepunkte), P_HANDS, P_BODY, P_XP und die Attribute A_STR, A_INT, A_DEX
 * und A_CON. Alle koennen spaeter noch geaendert werden.
 */
  create_default_npc( 25 );

/* Nun machen wir es etwas widerstandsfaehiger, indem wir P_BODY setzen.  */
/* Nie P_TOTAL_AC oder P_AC setzen, P_TOTAL_AC wird automatisch berechnet */
/* und P_AC ist nur fuer Ruestungen da.                                   */
  SetProp(P_BODY, 55);

/* Das Monster schlaegt mit blossen Haenden zu, also wird P_HANDS gesetzt.   */
/* Auch hier weder P_TOTAL_WC noch P_WC setzen.                              */
  SetProp(P_HANDS, ({" mit seinen Haenden", 135}));
/*                    ^ dieses Leerzeichen ist wichtig                       */
/* Beim Kampf erscheint nun: 'Das Monster greift Dich mit seinen Haenden an' */
/* 135 entspricht der Waffenklasse                                            */

/* Gesinnung des Monsters, 0 ist neutral, negativ boese und positiv gut */
  SetProp(P_ALIGN, -100);  /* etwas boese, aber nichts besonderes */

/* Die Rasse des Monsters */
  SetProp(P_RACE, "Vampir");

/* Erfahrungspunkte des Monsters, beim Tod erhaelt der 'Killer' ein   */
/* hundertstel dieses Wertes. Schon im Kampf erhaelt man bei jedem    */
/* Schlag weapon_class*schaden/10 punkte (weapon_class hier 55), also */
/* insgesamt weapon_class*hit_points/10.                              */
/* Ab 200000 gibt der NPC einen Stufenpunkt, ab 600000 2 Stufenpunkte */
  SetProp(P_XP, 130000);
  
/* Die Groesse des Monsters in cm. Dies wird bei einigen Kampfbefehlen */
/* ausgewertet, sowie bei einigen Identifikationsspruechen von Gilden.
 * Irgendwas zwischen 170 und 190 cm.  */
  SetProp(P_SIZE, 170 + random(21));

/* Ein Gewicht (in Gramm) sollte der NPC haben, irgendwas zwischen 40 und 48
 * kg.
 */
  SetProp(P_WEIGHT, 40000 + random(8000));

/* Weitere Werte: P_(MAX)_FOOD, P_(MAX)_DRINK, P_(MAX)_ALCOHOL,       */
/* P_MSGIN, P_MSGOUT, P_MMSGIN, P_MMSGOUT, P_MAX_HANDS                */

/* Spezielle Infos (fuer den Identifiziere-Spruch) */
  SetProp(P_INFO, break_string(
    "Der Rotaugenvampir laesst sich vielleicht mit Knoblauch vertreiben.",
    78));

/* NPC koennen einer Klasse angehoeren und Waffen koennten z.B. gegen
 * bestimmte Klassen/Rassen besonders (in)effektiv sein.
 */
  AddClass(({CL_VAMPIRE,CL_UNDEAD}));

/* Resistenzen (negativ) und Anfaelligkeiten (positiv) */
  SetProp(P_RESISTANCE_STRENGTHS, ([
        DT_BLUDGEON:0.2,DT_RIP:0.2,
        DT_MAGIC: -0.2, DT_COLD: -0.1, DT_TERROR: -0.8 ]) );
  
// Wer hat den Spieler gekillt und was sagt man dann?
  SetProp(P_KILL_NAME,"Ein Rotaugenvampir");
  SetProp(P_KILL_MSG,({"Grossartig %s! Ein weiteres Opfer fuer Zesstra!", 
                       MSG_SAY}));

  // beschwerde auf -moerder
  SetProp(P_MURDER_MSG,"Du steckst doch mit Jof im Bunde, %s!");
  SetProp(P_DIE_MSG," bricht stoehnend zusammen.\n");

  // Auch NPC sollten Details haben:
  AddDetail( ({"augen","augenlider","lider"}), break_string(
    "Der Rotaugenvampir schaut Dich mit weit aufgerissenen, rot "
    "schimmernden Augen wild an.",78));
  // ...

  // riecht der nach was? -> AddSmells()
  
  // Kann man den nach was fragen? -> AddInfo().

}

