#pragma strong_types, save_types, rtt_checks
#pragma no_shadow, no_clone

inherit "/std/thing";

#include <language.h>
#include <properties.h>
#include <items/fishing/fishing.h>
#include <items/flasche.h>
#include <unit.h>

#define TP this_player()
#define ME this_object()

int QueryKoeder();

protected void create() {
  ::create();

  AddId(({HAKEN_ID,"haken","angelhaken"}));
  SetProp(P_NAME, "Haken");
  SetProp(P_GENDER, MALE);
  SetProp(P_ARTICLE, 1);
  SetProp(P_WEIGHT, 5);
  SetProp(P_SHORT, "Ein Angelhaken");
  SetProp(P_LONG, "Ein Angelhaken aus Metall.\n");
  SetProp(P_LONG_EMPTY,"Vielleicht kannst Du etwas damit aufspiessen?\n");

  // Lang- und Kurzbeschreibungen reagieren auf einen evtl. vorhandenen 
  // Koeder, allerdings nur auf ein tatsaechlich existierendes Koeder-Objekt,
  // nicht auf einen per ueberschriebenem QueryKoeder() vorgetaeuschten
  // Koeder, wie es der Spezialhaken tut.
  Set(P_SHORT, function string () {
    object koeder = present(WURM_ID, ME);
    return Query(P_SHORT,F_VALUE)+
      (objectp(koeder)?" mit "+koeder->name(WEM):"");
  }, F_QUERY_METHOD);
  Set(P_LONG, function string () { 
    object koeder = present(WURM_ID, ME);
    return Query(P_LONG,F_VALUE) + (objectp(koeder)?
      koeder->Name(WER)+" haengt daran.\n":QueryProp(P_LONG_EMPTY));
  }, F_QUERY_METHOD);

  // P_FISH und P_WATER liefern die Daten eines evtl. vorhandenen Koeders
  // Wenn kein Koeder dranhaengt, werden P_WATER und P_FISH des Hakens
  // zurueckgegeben, falls es sich um einen Fake-Koeder handelt wie beim
  // Spezialhaken; in diesem Fall hat der Haken selbst die Properties 
  // gesetzt.
  Set(P_FISH, function int () {
    object koeder = present(WURM_ID, ME);
    return (objectp(koeder)?koeder->QueryProp(P_FISH):Query(P_FISH));
  }, F_QUERY_METHOD);
  Set(P_WATER, function int () {
    object koeder = present(WURM_ID, ME);
    return (objectp(koeder)?koeder->QueryProp(P_WATER):Query(P_WATER));
  }, F_QUERY_METHOD);

  SetProp(P_MATERIAL,([MAT_STEEL:100]));

  AddCmd(({"spiess","spiesse"}),"spiessauf");
}

protected void create_super() {
  set_next_reset(-1);
}

static int spiessauf(string str) {
  string haken,wurmname,*substr;
  object wurm;
  int amount;

  notify_fail("Was willst Du denn aufspiessen?\n");
  if (!stringp(str) || !sizeof(str)) 
    return 0;
  
  if( sscanf(str, "%s auf%s", wurmname, haken) != 2 )
    return 0;

  haken = trim(haken);

  notify_fail("So etwas hast Du nicht bei Dir.\n");
  if ( !objectp(wurm=present(wurmname,TP)) )
    return 0;

  notify_fail("Das kannst Du nicht aufspiessen.\n");
  if( !wurm->id(WURM_ID) )
    return 0;

  notify_fail("Worauf willst Du "+wurm->name(WEN,1)+" denn spiessen?\n");
  if ( haken!="" && !id(haken) )
    return 0;

  notify_fail(break_string("Dazu solltest Du "+name(WEN,1)+" erst einmal "
    "bei Dir haben.",78));
  if ( environment(ME) != TP )
    return 0;

  notify_fail("An dem Haken haengt schon ein Koeder.\n");
  if ( QueryKoeder() )
    return 0;

  // Haken und Koeder sind im Inventar und fuehlen sich von der Syntax
  // angesprochen.
  if( wurm->IsUnit() ) {
    // Das QueryProp() ist nicht unnoetig. Bei der Abfrage von U_REQ wird
    // U_REQ genullt, wenn das aktuelle query_verb() != dem letzten ist.
    // Bei der ersten Abfrage wuerde also das hier gesetzte U_REQ wieder
    // geloescht. Daher muss das jetzt hier als erstes einmal abgefragt
    // werden...
    wurm->QueryProp(U_REQ);
    wurm->SetProp(U_REQ,1);
  }
  tell_object(TP, break_string(
    "Du spiesst "+wurm->name(WEN,1)+" auf "+name(WEN,1)+".",78));
  tell_room(environment(TP), break_string(
    TP->Name(WER)+" spiesst "+wurm->name(WEN)+" auf "+name(WEN)+".",78),
    ({TP}));
  // M_GET, damit auch P_NODROP-Koeder aufgespiesst werden koennen.
  if ( wurm->move(ME,M_GET) != MOVE_OK) {
    tell_object(TP, break_string(
      "Verdammt! Jetzt hast Du Dir fast in den Finger gestochen! "+
      wurm->Name(WEN,1)+" hast Du dabei natuerlich verfehlt.", 78));
  }
  return 1;
}

int QueryKoeder(){ 
  return objectp(present(WURM_ID, ME));
}

object QueryKoederObject() {
  return present(WURM_ID,ME);
}

int MayAddObject(object ob) {
  return (objectp(ob) && ob->id(WURM_ID) && sizeof(all_inventory(ME))<=1); 
}

int _query_total_weight() {
  int tw;
  foreach(int w: all_inventory(ME)+({ME}))
    tw += w->QueryProp(P_WEIGHT);
  return tw;
}

varargs int remove(int sil) {
  all_inventory(ME)->remove(); // funktioniert auch mit leeren Arrays
  return ::remove(sil);
}

int KoederGefressen() {
  // Nicht QueryKoeder() pruefen, da das bei Spezialhaken immer 1 ist.
  object koeder = present(WURM_ID, ME);
  if ( objectp(koeder) )
    return koeder->remove();
  return -1;
}
