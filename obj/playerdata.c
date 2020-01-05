// Dieses Ding kann ein Spieler-Savefile laden, um dort Daten via
// Query/QueryProp abzufragen. Sollte das Spielerobjekt existieren, wird
// stattdessen dort gefragt.

// Das Objekt ist funktionsidentisch zum gleichnamigen Objekt unter /secure/,
// bekommt hier aufgrund des Speicherortes (/obj/) beim Clonen/Laden
// aber die UID vom Clonenden/Ladenden. (Ausser es ist ein ROOT-Objekt,
// dieses exportieren ihre UID nicht. Dann ist die UID von diesem Objekt die
// Backbone-ID STD).

// Spielerladen duerfen nur Objekte, welche die gleiche UID wie dieses Objekt
// (damit also der Erzeuger) haben.
// Daten abfragen darf nur das Objekt, welches den Spieler auch geladen hat.
// Damit ist sichergestellt, dass Zugriffsrechte beachtet werden.

#pragma no_inherit, no_shadow
#pragma strong_types,save_types,rtt_checks

inherit "/secure/playerdata";

protected void create()
{
  ::create();
  replace_program();
}

