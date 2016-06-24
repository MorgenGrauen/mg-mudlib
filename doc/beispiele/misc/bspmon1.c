/*
** Ein ganz normales Standard-monster ohne Besonderheiten
** (von Boing)
*/

// Diese Pragmas sorgen dafuer, dass der Driver darauf achtet, dass bei
// Funktionsargumenten, -Rueckgabewerten und teilweise bei Zuweisung von
// Werten an Variablen der richtige Datentyp verwendet wird (z.b. kein string
// anstelle eines int verwendet wird). Sollte in keinem Objekt fehlen.
#pragma strong_types, save_types, rtt_checks

// den Standard-NPC der Mudlib erben.
inherit "/std/npc";

#include <properties.h>
#include <language.h>

protected void create()
{
  ::create();    /* Nicht vergessen, ohne das geht nichts */

/* Die Kurzbeschreibung wird zum Beispiel angezeigt wenn man in einen Raum
   mit dem Monster reinlaeuft */
  SetProp(P_SHORT, "Beispielmonster");

/* Beim Anschauen des Monsters wird die long-description angezeigt */
  SetProp(P_LONG, "Ein Beispielmonster ohne Besonderheiten.\n");

/* Ein Name muss sein, da sonst z.B. im Todesfall 'Leiche von 0' daliegt */
  SetProp(P_NAME, "Monster");

/* Das Geschlecht des Monsters. Als Geschlechter sind */
/* die in <language.h> definierten Symbole NEUTER, MALE   */
/* und FEMALE zulaessig.                                  */
  SetProp(P_GENDER, NEUTER);

/* Ein Monster braucht mindestens eine ID, unter der man es ansprechen kann */
/* Es kann aber auch mehrere verschiedene ID's haben.                       */
  AddId("monster");
  AddId("beispielmonster");

/* Zuerst sollte man dem Monster einen Grundlevel geben. */
/* Die Spellpunkte und die Lebenpunkte sowie die Attribute werden dann */
/* automatisch angepasst. */
  create_default_npc( 10 );

/* Nun machen wir es etwas widerstandsfaehiger, indem wir P_BODY setzen.  */
/* Nie P_TOTAL_AC oder P_AC setzen, P_TOTAL_AC wird automatisch berechnet */
/* und P_AC ist nur fuer Ruestungen da.                                   */
  SetProp(P_BODY, 55);

/* Das Monster schlaegt mit blossen Haenden zu, also wird P_HANDS gesetzt.   */
/* Auch hier weder P_TOTAL_WC noch P_WC setzen.                              */
  SetProp(P_HANDS, ({" mit seinen Haenden", 55}));
/*                  ^ dieses Leerzeichen ist wichtig                         */
/* Beim Kampf erscheint nun: 'Das Monster greift Dich mit seinen Haenden an' */
/* 55 entspricht der Waffenklasse                                            */

/* Gesinnung des Monsters, 0 ist neutral, negativ boese und positiv gut */
  SetProp(P_ALIGN, 100);  /* Einigermassen nett, aber nichts besonderes */

/* Die Rasse des Monsters */
  SetProp(P_RACE, "Irgendnerasse");

/* Erfahrungspunkte des Monsters, beim Tod erhaelt der 'Killer' ein   */
/* hundertstel dieses Wertes. Schon im Kampf erhaelt man bei jedem    */
/* Schlag weapon_class*schaden/10 punkte (weapon_class hier 55), also */
/* insgesamt weapon_class*hit_points/10.                              */
  SetProp(P_XP, 10000);

/* Die Groesse des Monsters in cm. Dies wird bei einigen Kampfbefehlen */
/* ausgewertet, sowie bei einigen Identifikationsspruechen von Gilden  */
  SetProp(P_SIZE,180);

/* Weitere Werte: P_(MAX)_FOOD, P_(MAX)_DRINK, P_(MAX)_ALCOHOL,       */
/* P_MSGIN, P_MSGOUT, P_MMSGIN, P_MMSGOUT, P_MAX_HANDS, P_USED_HANDS  */

}
