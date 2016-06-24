// Ein transparenter Container, in dem die Gegenstaende landen,
// die ein Spieler bei seinem Tod verliert.
// Sinn dieses Objektes ist es, das invenotry eines Raumes klein
// zu halten und die kosten im init() eines Spielers zu senken.

#pragma strict_types
#pragma save_types
#pragma pedantic
#pragma range_check

inherit "std/container";

// Spielerhaufen sollen sich etwas anders als NPC-haufen verhalten.
private nosave status spielerhaufen;

#include <properties.h>
#include <moving.h>

void create() {
	string von;
	if (object_name(this_object()) == __FILE__[0..<3]) {
	  set_next_reset(-1);
	  return;
	}   
	if( !clonep(this_object()) ) return;
	::create();
	von = (string)previous_object()->QueryProp(P_PILE_NAME);
	SetProp( P_PILE_NAME, "\nhaufen "+von );
	spielerhaufen = (int)previous_object()->IsPlayerCorpse();

	AddId( ({"haufen","krempel","kluengel","gegenstaende"}) );
	AddId( QueryProp(P_PILE_NAME) );
	SetProp( P_NAME, "Haufen Krempel" );
	SetProp( P_GENDER, MALE );
	SetProp( P_INFO, "Diese Gegenstaende gehoerten einmal " + von + ".\n" );
	SetProp( P_LONG, break_string("Du betrachtest einen Haufen, der aus einer von Leichenresten zusammengebackenen Menge diverser Gegenstaende besteht. Zum Glueck ist von der Leiche nicht mehr genug uebrig, um Ekelgefuehle zu wecken. Ganz im Gegenteil: Dir juckt es ein wenig in den Fingern, Dich an diesem Haufen zu bedienen. Aber man kann sich nie ganz sicher sein, ob nicht doch ein Fluch auf den Dingen lastet. Vielleicht hat der verschiedene Besitzer sie auch jemandem vererbt? Andererseits, wenn gerade niemand guckt ...\n" ) );
	AddDetail( ({"fetzen","leiche","leichenteile"}), "Mit blossen Augen kann man nicht mehr viel erkennen, aber einige Fetzen deuten darauf hin, dass die Sachen von "+von+" stammen.\n" );
	AddDetail( ({"ekel","ekelgefuehle"}), "Es ist nicht mehr genug da, um welche zu wecken.\n" );
	AddDetail( "menge", "Viele kleine Fetzen, die an den Gegenstaenden kleben.\n" );
	AddDetail( "fluch", "Dem Anschein nach lastet auf den Sachen kein Fluch, aber wer weiss ...?\n" );
	call_out( "merge_pile", 1 );
}

mixed _query_short() {
	int sz = sizeof(all_inventory());
	if( sz==0 ) {
		call_out("remove",0,1); // fallback, wenn inhalt auf unerwartete weise verschwindet
		return 0;
	} else if( sz<10 ) {
		return "Ein kleiner Haufen Krempel";
	} else if( sz<30 ) {
		return "Ein Haufen Krempel";
	} else {
		return "Ein grosser Haufen Krempel";
	}
  return 0;
}

// Prueft, ob wir in einen Spieler gemovt werden sollen.
// Liefert passendes Moveresult, ansonsten 0.
protected int PreventMove(object dest, object oldenv, int method) {
    if( !objectp(dest) ) return 0;
    if( method & M_NOCHECK ) return 0;
    if( query_once_interactive(dest) ) {
      return ME_NOT_ALLOWED;
    // default meldung in put_and_get:pick
    // IDEE: im spieler per hook eine andere meldung unterbringen
    }
    return ::PreventMove(dest, oldenv, method);
}

// Sorgt dafuer, dass im NPC sich der Haufen selbst zerstoert.
protected void NotifyMove(object dest, object oldenv, int method) {
    if( query_once_interactive(dest)) {
	log_file( "PILE.log", dtime(time()) + ": pile in " 
	    + dest->query_real_name() + " gelandet.\n" );
    }
    if( living(dest) ) {
	filter_objects( all_inventory(), "move", dest, M_SILENT | M_NOCHECK );
	// NPCs erwarten nicht, dass dinge im move verschwinden, deshalb
	// abwarten
	call_out( "remove", 0, 1 );
    }
    return ::NotifyMove(dest, oldenv, method);
}

// wenn nur noch ein Objekt im Haufen ist, soll dieser zerstoert werden.
public void NotifyLeave( object ob, object dest ) {
	if( sizeof(all_inventory())>1 ) return;
	call_out( "remove", 0, 1 ); // verzoegern, um prepare_destruct gnaedig zu stimmen
}

// dito fuers loeschen
public void NotifyRemove( object ob ) {
    NotifyLeave( ob, 0);
}

// Haufen, die von zweimal dem selben NPC kommen, werden
// zusammengelegt
void merge_pile() {
	object* other = filter_objects(
		all_inventory(environment()) - ({ this_object() }),
		"id",
		QueryProp( P_PILE_NAME )
	);
	if( !sizeof(other) ) return;
	filter_objects( all_inventory(), "move", other[0], M_SILENT | M_NOCHECK );
	remove();
}

void reset() {
  // wenn es ein NPC-Haufen ist, werden Gegenstaende, die oefter als 3mal
  // vorkommen, zerstoert. Schraenkt die Objektflut an Stellen ein, wo NPC
  // ihren Kram nicht mit AddItem clonen.
  if (!spielerhaufen)
    remove_multiple(3);
  ::reset();
  // wenn nur noch unsichtbare items im Haufen: aufloesen
  if (!sizeof(filter(all_inventory(), function int (object o)
          { return !o->short();} )))
    remove(1);

}

