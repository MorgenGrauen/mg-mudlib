/*
 * Beispieldatei fuer einen einfachen Master, der Spielerdaten auch
 * ueber reboots, resets und updates hinweg speichert, und gleichzeitg
 * dafuer sorgt, dass die Datenmengen nicht immer groesser werden.
 *
 * By: Rumata@MorgenGrauen 3/99
 *
 */

// Von diesem Objekt gibt es keine Clones, sondern nur die Blueprint.
// Das Konstrukt if( clonep(ME) ) destruct(this_object()); ist dadurch
// obsolet.
#pragma no_clone

#include <properties.h>
#include <defines.h>

// Ort, an dem die Daten gespeichert werden koennen. Die Endung .o
// wird vom System angehaengt.
#define SAVEFILE "/doc/beispiele/master/opferstocklog"

// Dieses ist der Klient, der diesen Master benutzt. Dieser Wert wird
// in diesem Programm zwar nicht benutzt, steht hier aber, damit man
// weiss, wofuer dieser Master gut ist.
#define CLIENT "/doc/beispiele/master/opferstock"

// Es braucht kein Objekt inheritet werden, da wir keinerlei Spiel-
// Funktionitaet brauchen. Der Master kann nicht genommen, bewegt oder
// sonstwie innherlab des Muds benutzt werden. Insbesondere sollen
// im savefile zum Master keine Properties oder so auftauchen.
// inhert "/std/thing";

// Um diese Daten geht es.
// Das Mapping speichert zu jedem Spieler, wann das letzte Mal durch einen
// der Klienten das Alignment geaendert wurde. Alte Daten werden bei
// Gelegenheit geloescht.
mapping data;

void purge();

void create() {

  // Damit Schreibzugriff auf Savefile moeglich.
  seteuid(getuid());

  if( restore_object( SAVEFILE ) ) {
    purge();
  } else {
    data = ([]);
    save_object( SAVEFILE ); // Damit Savefile und Daten immer synchron sind.
  }
}

// Diese Funktion testet einen einzelnen Eintrag, ob er veraltet ist.
// (ist nicht Jahr 2038-fest :-)
int notExpired( string name, int date ) {
  return time() - date < 86400;
}

// Das Mapping untersuchen, ob Eintraege vorhanden sind, die nicht
// mehr benoetigt werden.
// (In diesem Fall sind das Eintraege, die aelter als einen Tag sind.)
// Es reicht uns, diese Funktion einmal pro reboot auszufuehren. Bei
// anderen Anwendungen koennte das natuerlich haeufiger noetig sein.
void purge() {
  data = filter_indices( data, #'notExpired );
  save_object( SAVEFILE );
}

// Diese Funktion ist die eingetliche Funktion, die "gemastert" werden
// soll, also für mehrere Opferstoecke gemeinsam benutzt wird.
// Der Opferstock uebergibt das Spielerobjekt und die gewuenschte
// Alignmentaenderung, als Ergebnis wird 1 geliefert, wenn eine Aenderung
// vorgenommen wurde (0 sonst) und das Alignment des Spielers entsprechend
// gesetzt.
int addAlignment( object player, int align ) {
  int newAlign;
  string name;

  /*
  // Falls man verhindern will, dass nur der Klient auf die Daten zugreift,
  // kann man hier noch Abfragen einbauen, typischerweise sieht das dann so
  // aus:
  if( object_name(previous_object())[0..xxx] != CLIENT ) return -1;
  // oder
  if( geteuid(previous_object()) != geteuid() ) return -1;
  // etc. etc.
  */

  name = geteuid(player);

  // Nur eine Aenderung pro Tag.
  // Wir benutzen hier, dass data[name] == 0 ist, falls data den Namen nicht
  // enthaelt!
  if( notExpired( name, data[name] ) ) return 0;

  // Daten setzen und speichern.
  data[name] = time();
  save_object( SAVEFILE );

  // Maximale Aenderung: 200
  if( align < -200 ) align = -200;
  if( align > 200 ) align = 200;

  newAlign = player->QueryProp( P_ALIGN ) + align;

  // Kappung bei +-1000
  if( newAlign < -1000 ) newAlign = -1000;
  if( newAlign > 1000 ) newAlign = 1000;
 
  player->SetProp( P_ALIGN, newAlign );
  return 1;
}

// Schlussbemerkung:
//
// Gewitzte Programmierer koennten den Klient und den Master in einer
// Datei zusammen ablegen. Die Blueprint wird als Master, die Clones werden
// als Klienten benutzt. Ich habe das hier bewusst anders gemacht und empfehle
// das auch als Vorbild, weil so der Code wesentlich besser zu verstehen ist.
