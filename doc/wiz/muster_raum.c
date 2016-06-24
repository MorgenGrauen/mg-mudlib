/*
 *----------------------------------------------------------------------
 * Ein kleiner Beispielraum.
 * Er laesst sich als /obj/doc/muster_raum ansprechen.
 *
 * Dieses File ist Teil der MorgenGrauen-Mudlib von Jof und Rumata
 * Letzte Aenderung: 17.08.92
 * #include eingefuegt: Rumata 10.11.98
 *----------------------------------------------------------------------
 */
#pragma strong_types,rtt_checks

inherit "/std/room";
	/* Diese Zeile deutet an, dass der Raum aus der Standard mudlib */
	/* abgeleitet wird. Diese Zeile sollte in JEDEM Raum vorkommen. */

#include <properties.h>
	/* Diese Zeile definiert die grossbuchstabigen Namen, die in den */
	/* SetProp-Befehlen benutzt werden. Es gibt noch weitere *.h     */
	/* Dateien, die andere nuetzliche Dinge definieren.              */

protected void create()
{
	::create();
		/* Diese Zeile initialisiert die Standard-attribute. */
		/* Sie darf nicht fehlen ! */

	SetProp(P_INT_SHORT, "Muster-raum" );
		/* Die Beschreibung des Raumes fuer Spieler, die den */
		/* kurz/brief-Modus eingestellt haben. */

	SetProp(P_INT_LONG, "Dieses ist ein Musterraum, der nur zur Erlaeuterung\n"
		+ "dient. Ein unangenehmer Raum voller Sterne und schraeger\n"
		+ "Striche. Im Sueden kannst Du in vertrautere Raeume fliehen.\n"
		+ "Im Norden ist etwas seltsames.\n" );
		/* Diese Beschreibung bekommen Spieler, die den kurz- */
		/* Modus nicht benutzen, oder wenn sie "schau" eingeben. */

	SetProp(P_LIGHT, 1 );
		/* Diese Zahl ist 1 fuer Raeume, in denen man ohne */
		/* Lichtquelle etwas sehen kann und 0 fuer Raeume, */
		/* in denen man eine Solche braucht. */

	AddExit( "sueden", "room/adv_guild" );
		/* Baue einen Ausgang nach Sueden, er wird mittels des */
		/* Kommandos "exits" angezeigt. */

	AddSpecialExit( "norden", "go_nord" );
		/* Wenn der Spieler versucht, diesen Ausgang zu */
		/* benutzen, so wird er nicht in einen anderen Raum */
		/* bewegt, sondern es wird im Raum die angegebene */
		/* Funktion aufgerufen. */

	AddCmd( "hilfe", "gib_hilfe" );
		/* Dieses Kommando ruft die Funktion "gib_hilfe" auf, */
		/* wenn der Spieler das Kommando "hilfe" eingibt. */
		/* Das Kommando taucht nicht in der "exits-liste" auf. */

}

/*
 *----------------------------------------------------------------------
 * So jetzt ist der Raum fetig.
 * Wirklich? Nein, fast.
 * Die oben angegebenen Funktion fehlen noch, aber das sind bereits
 * "Extras".
 *----------------------------------------------------------------------
 */

int go_nord()
{
	write( "Hmm das war wohl doch nur die Wand.\n" );
	return 1;
		/* Das Kommando wirde erfolgeich beendet. */
}

int gib_hilfe()
{
	write( "Gehe nach Sueden, und Du kommst in die Abenteurer-Gilde.\n" );
	return 1;
}

