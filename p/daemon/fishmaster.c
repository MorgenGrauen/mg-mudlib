/*Fisch-Master, (c) Vanion@MG, 26.05.02

  Fische die im MG geangelt werden koennen, sollen nicht als endlose
  Geldquelle dienen. Daher habe ich einen Master gebaut, der sich merkt
  Welcher Spieler wieviel Fisch geangelt hat. Je mehr Fisch der Spieler
  faengt, desto billiger wird der Fisch.

  Schnittstelle:

  Der Master hat abgesehen von Standard-Funktionen nur eine Schnittstelle

  public int PriceOfFish(object player, object fish)

  Der erste Parameter ist der Spieler, der den Fisch gecloned hat, der
  zweite ist der Fisch, den der Spieler gefangen hat. Der Fisch muss
  hierbei von Fraggles Standardfisch erben.

  Der Preis des Fisches faellt linear je naeher der Spieler an die
  Maximal-Grenze fuer Fisch kommt (Diese ist durch die Konstante MAX_FISH
  festgelegt.) Der Master merkt sich ueber einen Zeitraum von FORGET_IT
  Sekunden, dass der Spieler diesen Fisch gefangen hat. Ist der Zeitraum
  vergangen, geht der Fisch nicht mehr in die Rechnung ein.
 */

#pragma strong_types, save_types, rtt_checks
#pragma no_clone, no_shadow, no_inherit

#define SAVE_FILE __DIR__+"save/fishes"

#define FORGET_IT 1728000 // Nach 20 Tagen
#define MAX_FISH  20000   // Wenn Spieler Max-Fish geholt hat, hat der
                          // Fisch keinen Wert mehr. (Einheit Gramm)

#include <properties.h>

// Hier wird gespeichert, welcher Spieler wieviel Fisch gefangen hat.
mapping fishes = m_allocate(0,1);
static int save_me_soon;

protected void create() {
  // Ohne das darf dieses Objekt hier nicht schreiben.
  seteuid(getuid());
  // Globale Variablen einlesen/initialisieren.
  restore_object(SAVE_FILE);
  set_next_reset(4*3600);
}

static void CleanupData() {
  int expire_date = time()-FORGET_IT;
  // Ueber alle Spielernamen laufen.
  foreach(string pl, mixed *fishlist: fishes) {
    // Ueber alle Fischdaten aller Spieler laufen.
    foreach(int *fish: fishlist) {
      if (fish[0]<expire_date) {
        fishlist-=({fish});
        save_me_soon=1;
      }
    }
    if ( !sizeof(fishlist) )
      fishes = m_delete(fishes, pl);
    else
      fishes += ([pl:fishlist]);
  }
}

//ZZ: man koennte hier ggf. das neue Gewicht bestimmen statt es bei jedem
//ZZ:Abruf zu tun.
// Fuegt einen neuen Fisch in die Liste mit Fischen ein
static int AddFish(string player_id, int weight) {
  // Fisch hinzufuegen
  if ( member(fishes, player_id) )
    fishes[player_id] +=  ({({ time(),weight})});
  else 
    fishes += ([ player_id: ({ ({time(),weight}) }) ]);
  return 1;
}

// Hier wird das Gewicht des bereits gefangenen Fisches umgewandelt
// in den Preis-Prozentsatz, den der Fisch noch kostet.
static int WeightToPrice(int weight) {
  if (weight >= MAX_FISH) return 0;
  if (weight <= 0       ) return 100;
  // Eine lineare Kurve. Je mehr Fisch geholt, desto billiger
  // wird der. // Arathorn: Spannend, so eine lineare Kurve. :-)
  return 100-(weight*100/MAX_FISH);
}

// Gibt das Gesamtgewicht des gefangenen Fisches zum Spieler zurueck
static int QueryWeight(string player_id) {
  int weight;
  mixed list = fishes[player_id];

  // Erstmal werden ungueltige Fische aus der Liste entfernt
  //ZZ: ich finde, das muss nicht unbedingt bei jeden Abruf getan werden
  //ZZ::und nicht fuer alle Spieler. reicht nicht einfach alle 4h? 
  CleanupData();

  // Zusammenzaehlen der Einzelgewichte der gueltigen Fische.
  foreach(int *fish : fishes[player_id]) {
    weight += fish[1];
  }
  return weight;
}

// Wenn ein Spieler einen neuen Fisch bekommt, wird der Preis fuer den
// Fisch bestimmt. Dieser resultiert aus der Menge (Gewicht) des Fisch,
// die der Spieler in letzter Zeit schon gefangen hat. Zurueckgegeben
// wird der Preis in Teilen von 100.
public int PriceOfFish(object player, object fish) {
  string player_id;
  int price_of_new_fish;

  // Sind Spieler und Fisch uebergeben?
  if ( !player || !fish ) return 0;

  // Spieler-ID finden.
  player_id = getuuid(player);

  // Ist player ein echter Spieler?
  // NPC clonen sich Fische, die 100% kosten.
  if (member(users(),player)==-1) return 100;

  // Is der Fisch ein Fisch von Fraggle?
  if (member(inherit_list(fish), __PATH__(1)+"/fish.c")==-1)
    return 0;

  // Feststellen wieviel Fisch ein Spieler gefangen hat, und
  // daraus den Preis berechnen.
  price_of_new_fish = WeightToPrice(QueryWeight(player_id));

  // Diesen Fisch in die Datenbank eintragen
  AddFish(player_id, fish->QueryProp(P_WEIGHT));

  // Ich bin neugierig.
  /*DEBUG(player->Name()+" hat einen Fisch mit "+fish->QueryProp(P_WEIGHT)+
        " g gefangen. Der Wert liegt bei "+price_of_new_fish+"%");*/

  // Die Aenderung soll persistent sein.
  save_me_soon=1;

  // Rueckgabe des Fischpreises.
  return price_of_new_fish;
}

// Speichern der globalen Variablen
static void save() {
  save_object(SAVE_FILE);
  return;
}

// Da nichts geerbt wird, muss das destruct() selbst gemacht werden
// Wir speichern auf jeden Fall.
varargs int remove(int silent) {
  save();
  destruct(this_object());
  return 1;
}

// Daten aufraeumen und ggf. wegspeichern
void reset() {
  CleanupData();
  if ( save_me_soon )
    save();
  set_next_reset(4*3600); // alle 4 Stunden reicht.
}
