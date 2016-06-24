/*
** Ein Beispielmonster mit div Extras (von Boing, aktualisiert von Wim+Zesstra)
*/

// Diese Pragmas sorgen dafuer, dass der Driver darauf achtet, dass bei
// Funktionsargumenten, -Rueckgabewerten und teilweise bei Zuweisung von
// Werten an Variablen der richtige Datentyp verwendet wird (z.b. kein string
// anstelle eines int verwendet wird). Sollte in keinem Objekt fehlen.
#pragma strong_types, save_types, rtt_checks

inherit "/std/npc";

#include <properties.h>
#include <language.h>
#include <combat.h>    // fuer die damage types
#include <moving.h>    // fuer REJECT_KEEP   
#include <class.h>     // fuer AddClass 
#include <new_skills.h> // fuer SP_* bei AddSpell

protected void create()
{
  ::create();       /* WICHTIG!!! */

/* Standard-Knofiguration (Erlaeuterungen siehe bspmon1.c): */
  SetProp(P_SHORT, "Ein Zauberer");
  SetProp(P_LONG, "Dieser Zauberer zaubert wie wild und schwingt dabei "
    "seinen langen Bart.\n");
  SetProp(P_NAME, "Zauberer");
  SetProp(P_GENDER, MALE);
  AddId("zauberer");
  create_default_npc(18);
  SetProp(P_ALIGN, -700);   /* Brrr, ist der boese .... */
  SetProp(P_BODY, 125);
  SetProp(P_HANDS, ({" mit seinem langen Bart", 185}) );
  SetProp(P_SIZE,180);
  SetProp(P_MAX_HANDS, 2);  /* Anzahl der Haende (default ist 2) */
  // set_living_name() setzt einen Namen, mit der der Zauberer z.B. mit einem
  // 'finde' gefunden werden kann. Fuer die meisten NPC ist dies nicht noetig.
  // Speziell sollte man keine generischen Bezeichnungen hier anmelden, wenn
  // schon, dann individuelle Namen.
  /* set_living_name("zauberer"); */

/* Mit AddClass() und P_RACE wird festgelegt, in welche Gruppe von Lebe-  */
/* wesen der NPC gehoert, welche mit is_class_member(mixed str) abgefragt */
/* werden kann. Im Minimalfall ist der NPC von der Klasse, die bei P_RACE */
/* eingetragen ist. Mit AddClass() koennen aber noch weitere Eigen-       */
/* schaften hinzugefuegt werden.                                          */

  SetProp(P_RACE,"Superduperzauberer");
  AddClass( ({ CL_HUMAN, CL_MAMMAL }) );

/* Mit P_RESISTANCE und P_VULNERABILITY werden fixe Werte (50%) fuer      */
/* Resistenzen respektive Anfaelligkeiten gesetzt                         */
/* Die Liste der moeglichen Schadensarten ist in /sys/combat.h definiert. */  
/* z.B.  SetProp(P_RESISTANCE, ({ DT_MAGIC }));                           */
/*       SetProp(P_VULNERABILITY, ({ DT_COLD }))                          */
/*                                                                        */
/* Mit P_RESISTANCE_STRENGTHS koennen Resistenzen und Anfaelligkeit       */
/* konfiguriert werden. Diese Property enthaelt ein Mapping von           */
/* von Schadensarten enthaelt. Negative Faktoren bis maximal -1.0         */ 
/* (=Immunitaet) geben Resistenzen, positive Werte Anfaelligkeiten an.    */

  SetProp(P_RESISTANCE_STRENGTHS, ([ DT_MAGIC: -0.5, DT_COLD: 2.0]) );					      

/* Durch diesen Befehl wird eine Waffe geclont, sofern sie nicht im Raum  */
/* herumliegt (in dem Fall wird sie vom NPC aufgehoben), und gezueckt     */ 
/* (definiert in /std/npc/items.c resp. /sys/npc.h)                       */

  AddItem("/doc/beispiele/misc/bspwaffe1", CLONE_WIELD);
/* Und noch eine Ruestung clonen und anziehen.                            */  
  AddItem("/doc/beispiele/misc/bspruest1", CLONE_WEAR);

/* Jetzt wird gezaubert ....     */

/* SetProp(P_SPELLRATE,) legt die Wahrscheinlichkeit fest, mit der ein   */
/* Zauberspruch durchgefuehrt wird, hier 40%                            */
  SetProp(P_SPELLRATE,40);

/* Mit AddSpell() wird ein Zauberspruch hinzugefuegt. Das erste Argument  */
/* ist so etwas wie die relative Wahrscheinlichkeit des Spruchs im        */
/* Vergleich zu den anderen, hier beim ersten spruch 1 beim zweiten 2,    */
/* das heisst in 20% der Faelle wird der erste Zauberspruch genommen und  */
/* in 40% der zweite und in 60% der dritte.                               */
/* Das zweite Argument ist der maximale Schaden, entsprechend der WC      */
/* einer Waffe oder P_HANDS beim waffenlosen Kampf.                       */
/* Die zwei weiteren Argumente sind die Meldungen die der angegriffene    */
/* Spieler selbst und eventuelle Zuschauer erhalten. Hierbei stehen @WER, */
/* @WEN, @WEM und @WESSEN fuer den Namen des Opfers im entsprechenden     */
/* Fall.                                                                  */
/* Das 5. Argument ist optional und gibt den Schadenstyp an, fehlt er     */
/* wird DT_MAGIC angenommen.                                              */  
/* Im 6. Arugument kann eine Funktion stehen, die bei dem Spruch ausge-   */
/* fuehrt werden soll,                                                    */
/* Das siebte uebergibt die Spell Parameter, hier ist wichtig, dass bei   */
/* physikalischen Schaeden auch SP_PHYSICAL_ATTACK gesetzt wird, da sonst */
/* einige Gilden den "Zauberspruch" abwehren koennen (siehe SpellDefend)  */

  AddSpell(20,60, 
	   "  Der Zauberer wirft einen Feuerball nach Dir.\n",
	   "  Der Zauberer wirft einen Feuerball nach @WEM.\n",
	   ({ DT_FIRE }) );
  AddSpell(40,95,
	   "  Der Zauberer beschwoert einen Sturm ueber deinem Kopf hervor.\n",
	   "  Der Zauberer beschwoert einen Sturm ueber @WESSEN Kopf hervor.\n",
	   ({ DT_AIR, DT_LIGHTNING }) );  
  AddSpell(60,50,
           "  Der Zauberer tritt Dich hinterlistig.\n",
	   "  Der Zauberer tritt nach @WEM.\n",
	   ({ DT_BLUDGEON }), 0,
	   ([SP_SHOW_DAMAGE:1, SP_PHYSICAL_ATTACK:1]) );  	   


/* Reden kann der Zauberer auch von alleine, hier kommen die chats:     */

/* Das erste Argument ist die Wahrscheinlichkeit, mit der das Monster
 * einen Spruch bringt (weniger ist mehr!). Das zweite Argument ist die Liste
 * der Sprueche (oder Aktionen).
 */

  SetChats(2, ({
  "Der Zauberer laeuft im Kreis herum.\n",
  "Der Zauberer stolpert ueber seinen Bart.\n",
  "Der Zauberer sagt: Heh Du! Was machst Du da?\n",
  "Der Zauberer murmelt ein paar seltsame Worte in seinen Bart.\n",
  "Der Zauberer bricht in unkontrolliertes Gelaechter aus.\n",
  "Der Zauberer sagt: Hast mir mal ne Mark, ey?\n",
  "Der Zauberer sagt: Wenn ich diesen Olpp erwische ...\n",
  "Der Zauberer zaubert ein Kaninchen aus seinem Hut.\n"
  }) );

/* Das selbe fuer Sprueche die waehrend eines Kampfes kommen sollen    */

  SetAttackChats(20, ({
  "Der Zauberer macht: Buh!\n",
  "Der Zauberer wirft mit weissen Maeusen nach Dir.\n",
  "Der Zauberer sagt: Das war ein grosser Fehler!\n",
  "Der Zauberer beisst Dich in den Arm.\n",
  "Der Zauberer zaubert gruene Punkte auf deine Wange.\n",
  "Der Zauberer verwandelt sich in eine Kroete.\n",
  "Der Zauberer verschwindet und taucht hinter Dir wieder auf.\n"
  }) );

/* Wenn er stirbt soll eine besondere Meldung erscheinen. */
  SetProp( P_DIE_MSG, " loest sich in Luft auf.\n" );
/* Dann soll natuerlich auch kein Leichnam rumliegen */
  SetProp( P_NOCORPSE, 1 );
 
/* Nun wollen wir den Zauberer noch auf ein paar Fragen antworten lassen    */
/* AddInfo(DEFAULT_INFO,) setzt die Antwort auf alle Fragen, wo der         */
/* Schluessel nicht bekannt ist. Der Name des Zauberers wird immer vor den  */
/* String gesetzt. (SetProp(P_DEFAULT_INFO, ist obsolet und sollte nicht    */
/* mehr angewendet werden.)                                                 */ 

  AddInfo(DEFAULT_INFO,"sagt: Keine Ahnung, von was Du redest.\n",0,
                       "sagt zu @WEM: Keine Ahnung, von was Du redest.\n");

/* Die bekannten Schluessel werden mit AddInfo dazugefuegt, das erste       */
/* Argument ist das Wort (oder die Liste von Woertern), zu der der Zauberer */
/* befragt werden kann, das zweite Argument ist die entsprechende Antwort.  */
/* ist das dritte Argument gesetzt, so wird die Antwort automatisch umge-   */
/* brochen und bekommt den Text des Arguments vor jede Zeile gestellt.      */
/* Das vierte, ebenfalls optionale, Argument wird sofern gesetzt an die     */
/* umstehenden Personen ausgegeben, dadurch laesst sich ein Fluestern oder  */
/* eine spielerabhaengige Antwort realisieren.                              */

  AddInfo("kaninchen",
  "sagt: Die hol ich immer aus meinem Hut.\n");
  AddInfo("zauberei",
  "sagt: Ich bin ein Meister der Zauberei.\n");
  AddInfo(({"maus", "maeuse"}),
  "sagt: Maeuse sind meine Lieblingstiere.\n"); 
  
  AddInfo( ({"tier","tiere"}), 
          "Oh, hauptsaechlich beschaeftige ich mich mit Kaninchen und Maeusen.",
          "fluestert Dir zu: ",
          "fluestert mit @WEM.\n");

/* Normalerweile nehmen Monster alles was man ihnen gibt, ohne einen Ton */
/* zu sagen. Will man dort ein anderes Verhalten, so tut man das so:     */
/* Moeglich sind auch REJECT_GIVE (= zurueckgeben) REJECT_DROP (=fallen  */
/* lassen. */
  SetProp( P_REJECT, ({ REJECT_KEEP, "Der Zauberer sagt: Dankeschoen.\n" }) );

/* Der Zauberer kann auch zusaetzliche Verben haben, die nur er selber
 * benutzen kann, kein anderes Living. Diese koennen mit add_action() im
 * create eines NPC angemeldet werden (und idR NUR in dieser Funktion und NUR
 * in Lebewesen, nicht in sonstigen Objekten! */
  add_action("nasebohren", "bohreinnase");

/* Verben, die Spieler in Gegenwart des Zauberers koennen sollen, werden     */
/* mit AddCmd angemeldet. */
}

int nasebohren( string str ) // str ist das, was hinter dem verb eingegeben wurde.
{
   say( "Der Zauberer bohrt mit dem Stab in seiner Nase.\n" );
   return 1; // Verb war erfolgreich.
}

