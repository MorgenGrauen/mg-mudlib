// MorgenGrauen MUDlib
//
// /std/food.c  - Standardobjekt fuer Lebensmittel
//
// $Id: food.c 8248 2012-11-23 21:28:04Z Zesstra $
#pragma strict_types,save_types,rtt_checks
#pragma no_clone
#pragma pedantic,range_check

inherit "/std/thing";

#include <food.h>
#include <living/life.h>
#include <break_string.h>

// Standard-IDs und -Adjektive muessen gesichert werden
#define  ID_BACKUP       "std_food_id_backup"
#define  ADJ_BACKUP      "std_food_adj_backup"

// Zeitpunkt, an dem die Speise verdorben ist
private nosave int time_gone_bad;

// Zeitpunkt, an dem die Speise verderben darf
// wird immer erst gesetzt, wenn ein Spieler hiermit in Beruehrung kommt
private nosave int time_togo_bad;

// Zeitpunkt, an dem die Speise zerstoert werden darf
// wird immer erst gesetzt, wenn ein Spieler hiermit in Beruehrung kommt
private nosave int time_to_remove;

// Zeitpunkt, ab dem die Speise mit einem Spieler in Beruehrung kam
// bei ewig haltbaren Speisen ist sie immer 0;
private nosave int start_life = 0;

// Dauer des ersten Resets
private nosave int first_reset_length = 0;

// Wertet Properties aus (Ersetzungen werden durchgefuehrt)
protected mixed eval_property(string val,object consumer) {
  mixed what = QueryProp(val);
  if (stringp(what)) {
    what=capitalize(replace_personal(what,({this_object(),consumer}),1));
  }
  return break_string(what,78,"",BS_LEAVE_MY_LFS);
}

// wird nur verwendet, wenn der gesetzte Reset ein Timerende darstellt,
// um zu garantieren, dass der nicht abgestellt wird.
// Zugegebenenermassen keine schoener Hack :-/
private void my_set_next_reset(int zeit) {
  set_next_reset(zeit);
  // Damit der Reset auch wirklich ausgefuehrt wird ...
  call_out(#'id,1);
}

// Ausgabe von Meldungen an Besitzer oder Raum
protected void message(string prop) {
  object env = environment(this_object());
  if (objectp(env)) {
    if (interactive(env)) {
      tell_object(env,
        break_string(eval_property(prop,env),78,"",BS_LEAVE_MY_LFS));
    }
    else if(!living(env)) {
      tell_room(env,
        break_string(eval_property(prop,0),78,"",BS_LEAVE_MY_LFS));
    }
  }
}

// Flag, ob die Speise verdorben ist
public int is_bad() {
  return time_gone_bad > 0;
}

// Anzahl der Sekunden, die das Lebensmittel bereits hinter sich hat,
// seit es mit einem Spieler in Beruehrung kam
public int get_current_lifetime() {
  if (!start_life) return 0;
  return time() - start_life;
}

// Berechnet den Wert der Speise korrekt
int _query_value() {
  int result = 0;
  if (!is_bad()) {
    result += to_int(QueryProp(P_PORTIONS)*Query(P_VALUE,F_VALUE));
  }
  if (mappingp(QueryProp(P_EMPTY_PROPS))) {
    result += QueryProp(P_EMPTY_PROPS)[P_VALUE];
  }
  return result;
}

// Berechnet den Wert der Speise korrekt
int _query_weight() {
  int result = to_int(QueryProp(P_PORTIONS)*Query(P_WEIGHT,F_VALUE));
  if (mappingp(QueryProp(P_EMPTY_PROPS))) {
    result += QueryProp(P_EMPTY_PROPS)[P_WEIGHT];
  }
  return result;
}

// Dauer des ersten Resets wird wieder hinzugerechnet
// damit wird bei Nichtsetzen der Prop der Standard geliefert
int _query_std_food_lifetime() {
  return Query(P_LIFETIME,F_VALUE)+(first_reset_length);
}

// Setter fuer die Lifetime, damit die Resettime immer mitkorrigiert wird
// und Dauer des ersten Resets abgezogen werden, da die als Standard immer
// dazukommen
protected void set_std_food_lifetime(int val) {
  if (val > 0) {
    if (!time_togo_bad && !is_bad()) {
      Set(P_LIFETIME,val-(first_reset_length),F_VALUE);
      // Anzahl der Resets richtet sich nach dem Mittelwert
      Set(P_RESET_LIFETIME,to_int((val-(first_reset_length))
                           /(__RESET_TIME__*0.75))+1,F_VALUE);
      set_next_reset(val);
    }
  }
  else {
    write("Da ist was schief gelaufen! Sprich mal mit einem Magier.\n");
    raise_error("setting P_LIFETIME to invalid int value: "+
      to_string(val)+"\n");
  }
}

// Anzahl der Resets als Lebenszeit
void _set_std_food_lifetime_reset(int value) {
  if (value < 1) value = 1;

  Set(P_RESET_LIFETIME,value,F_VALUE);
  // per Zufall die Laenge der einzelnen Resets berechnen
  int length = first_reset_length;
  foreach(int i: value) {
    length += to_int((__RESET_TIME__ + random(__RESET_TIME__)) / 2);
  }
  set_std_food_lifetime(length);
}

// Flag, ob Speise "essbar" ist
public int is_eatable() {
  return QueryProp(P_FOOD) > 0;
}

// Flag, ob Speise "trinkbar" ist
public int is_drinkable() {
  return QueryProp(P_FOOD) == 0 && QueryProp(P_DRINK) > 0;
}

// Flag, ob noch Portionen der Speise vorhanden sind
public int is_not_empty() {
  return QueryProp(P_PORTIONS) > 0;
}

// Macht eine Speise leer bzw. zerstoert sie.
// Muss beim Ueberschreiben immer als letztes aufgerufen werden.
// Diese Methode wird aufgerufen, wenn ein Behaelter leergefuttert wird
// oder der Inhalt zerstoert werden soll
// Rueckgabewert: Ergebnis von remove() bzw. 0, wenn Objekt nicht
//                zerstoert werden muss
// Beim Aufruf bitte IMMER das Ergebnis pruefen, damit keine Aufrufe an
// geloeschten Objekten passieren! if (make_empty()) return ...;
public int make_empty() {
  mixed props = QueryProp(P_EMPTY_PROPS);
  if (mappingp(props)) {
    foreach(string key:m_indices(props)) {
      if (key == P_IDS) {
        RemoveId(QueryProp(P_IDS));
        AddId(QueryProp(ID_BACKUP));
        AddId(props[key]);
      }
      else if (key == P_ADJECTIVES) {
        RemoveId(QueryProp(P_ADJECTIVES));
        AddAdjective(QueryProp(ADJ_BACKUP));
        AddAdjective(props[key]);
      }
      else {
        SetProp(key,props[key]);
      }
    }

    // Sicherheitshalber :-)
    SetProp(P_PORTIONS,0);
    return 0;
  }
  else {
    return remove(1);
  }
}

// wird aufgerufen, wenn die Speise schlecht war und der Removetimer
// abgelaufen ist
// Zerstoert die Speise oder den Inhalt eines Behaelters
// Rueckgabewert: Ergebnis von remove() bzw. 0, wenn Objekt nicht
//                zerstoert werden muss
public int make_destroy() {
  message(P_REMOVE_MSG);
  set_next_reset(-1);
  return make_empty(); // Leermachen oder zerstoeren
}

// Laesst die Speise schlecht werden (mit Meldungen)
// muss beim Ueberschreiben immer mit aufgerufen werden
// Rueckgabewert: Ergebnis von remove() bzw. 0, wenn Objekt nicht
//                zerstoert werden muss
// Beim Aufruf bitte IMMER das Ergebnis pruefen, damit keine Aufrufe an
// geloeschten Objekten passieren! if (make_bad()) return ...;
public int make_bad() {
  time_gone_bad = time();
  if (is_not_empty()) {
    message(P_BAD_MSG);
  }
  if (QueryProp(P_DESTROY_BAD) == DESTROY_BAD && !QueryProp(P_EMPTY_PROPS)) {
    return remove(1);
  }
  return 0;
}

// Startet den Timer zum Schlechtwerden der Speise
public void start_lifetime() {
  // Test, ob Lebensmittel schlecht werden darf
  if (!QueryProp(P_NO_BAD) && !time_togo_bad) {
    start_life = time();
    int zeit = QueryProp(P_LIFETIME);
     // Zeitpunkt des Schlechtwerdens
    time_togo_bad = start_life + zeit;
    // Zeitpunkt des Zerstoerens
    time_to_remove = time_togo_bad + QueryProp(P_DESTROY_BAD);
    my_set_next_reset(zeit);
  }
}

// Reset wird in 2 Situationen aufgerufen:
// 1. Timer zum schlecht werden ist abgelaufen, das Futter muss also
//    schlecht werden und evtl. der Timer zum Zerstoeren gestartet werden
// 2. Timer zum Zerstoeren ist abgelaufen, also wird Futter zerstoert.
// Wird der Reset aus irgendeinem Grund zu falschen Zeiten aufgerufen,
// wird der naechste Reset korrekt initialisiert.
void reset() {
  // wenn kein env, Abbruch und reset abschalten.
  if (!environment()) {
      set_next_reset(-1);
      return;
  }

  // Test, ob Lebensmittel schlecht werden darf
  if (!QueryProp(P_NO_BAD)) {
    if (!is_bad()) {
      // Test, ob Timer laeuft
      if (time_togo_bad) {
        // Pruefen, ob der Reset so in etwa zur richtigen Zeit aufgerufen wurde
        // 10% Abweichung sind erlaubt
        int diff = time() - time_togo_bad;
        int std = to_int(QueryProp(P_LIFETIME)/10);
        if (abs(diff) < std || diff > 0) {
          if (make_bad()) return;
          // Reset zum Zerstoeren vorbereiten
          if (QueryProp(P_DESTROY_BAD) > 0) {
            my_set_next_reset(time_to_remove - time());
          }
          else {
            set_next_reset(-1);
          }
        }
        else {
          // Reset nochmal zum Ablauf der Frist
          my_set_next_reset(abs(diff));
        }
      }
    }
    else if (time_to_remove && QueryProp(P_DESTROY_BAD) > 0) {
      // wir sind im Reset nach dem Schlechtwerden
      int diff = time() - time_to_remove;
      int std = to_int((QueryProp(P_LIFETIME)+QueryProp(P_DESTROY_BAD))/10);
      // Pruefen, ob der Reset so in etwa zur richtigen Zeit aufgerufen wurde
      // 10% Abweichung sind erlaubt
      if (abs(diff) < std || diff > 0) {
        if (make_destroy()) return; // Leermachen oder zerstoeren
      }
      else {
        // Reset nochmal zum Ablauf der Frist
        my_set_next_reset(abs(diff));
      }
    }
  } // if (P_NO_BAD)

  ::reset();
}

private int safety_check() {
  // Damit ein Spieler auch gaaaanz sicher nicht Speisen ausserhalb der 
  // P_LIFETIME nutzen kann ...
  if (time_togo_bad && !is_bad() && time() > time_togo_bad) {
    if (make_bad()) return 0;
    // Reset zum Zerstoeren vorbereiten
    if (QueryProp(P_DESTROY_BAD) > 0) {
      my_set_next_reset(time_to_remove - time());
    }
    else {
      set_next_reset(-1);
    }
    return 0;
  }
  else if (time_to_remove && time() > time_to_remove
      && QueryProp(P_DESTROY_BAD) > 0) {
    make_destroy(); // Leermachen oder zerstoeren
    return 0;
  }
  return 1;
}

// Pruefung zum Starten ausgelagert, da das im init und in
// NotifyMove gebraucht wird
private void check_start_timer(object oldenv) {
  // Test, ob Lebensmittel schlecht werden kann
  if (!QueryProp(P_NO_BAD) && !time_togo_bad) {
    // Wenn wir irgendwie in einem Spieler stecken oder ein Spieler in der
    // Umgebung ist oder das Futter mehrfach bewegt wurde
    if (sizeof(filter(all_environment(),#'query_once_interactive))
        || sizeof(filter(all_inventory(environment()),#'query_once_interactive))
        || objectp(oldenv)) {
      start_lifetime();
    }
  }
  else {
    // Damit ein Spieler auch gaaaanz sicher nicht Speisen ausserhalb
    // der P_LIFETIME nutzen kann ...
    safety_check();
  }
}

// hier wird geprueft, ob das Futter dem Spieler zugaenglich ist.
void init() {
  ::init();
  check_start_timer(environment());
}

// hier wird geprueft, ob das Futter dem Spieler zugaenglich ist.
// das ist sicherer als nur im init()
protected void NotifyMove(object dest, object oldenv, int method) {
  ::NotifyMove(dest, oldenv, method);
  check_start_timer(oldenv);
}

void create() {
  // Sicherheitshalber,
  // falls jemand die Blueprint mit Props initialisieren will
  ::create();
  Set(P_LIFETIME, #'set_std_food_lifetime, F_SET_METHOD);
  Set(P_LIFETIME, PROTECTED, F_MODE_AS);

  time_togo_bad = 0;
  time_to_remove = 0;
  time_gone_bad = 0;

  // Standardlaufzeit 1 Reset
  first_reset_length = to_int((__RESET_TIME__ + random(__RESET_TIME__)) / 2);
  // Reset wird erstmal abgeschaltet, das wird im NotifyMove geprueft
  set_next_reset(-1);

  SetProp(P_SHORT,"Ein Lebensmittel");
  SetProp(P_LONG,"Das ist ein Lebensmittel.\n");
  SetProp(P_NAME,"Lebensmittel");
  SetProp(P_MATERIAL, MAT_MISC_FOOD);
  SetProp(P_GENDER,NEUTER);
  SetProp(P_WEIGHT,50);
  SetProp(P_VALUE,10);
  SetProp(P_PORTIONS,1); // einmal Abbeissen
  SetProp(P_DESTROY_BAD,DESTROY_BAD); // Zerstoeren beim Schlechtwerden
  SetProp(P_DISTRIBUTION,HD_STANDARD); // Heilung per Standardverteilung (5/hb)

  SetProp(P_CONSUME_MSG,"@WER2 konsumiert @WEN1.");
  SetProp(P_EATER_MSG,"Du konsumierst @WEN1.");
  SetProp(P_EMPTY_MSG,"@WER1 ist bereits leer.");
  SetProp(P_NOFOOD_MSG,"@WEN1 kann man nicht essen!");
  SetProp(P_NODRINK_MSG,"@WEN1 kann man nicht trinken!");
  SetProp(P_BAD_MSG,"@WER1 verdirbt.");
  SetProp(P_FOOD_FULL_MSG,"Du bist zu satt, das schaffst Du nicht mehr.");
  SetProp(P_DRINK_FULL_MSG,"So viel kannst Du im Moment nicht trinken.");
  SetProp(P_ALC_FULL_MSG,"Soviel Alkohol vertraegst Du nicht mehr.");
  SetProp(P_ENV_MSG,"Vielleicht solltest Du @WEN1 vorher nehmen.");
  SetProp(P_REMOVE_MSG,"@WER1 zerfaellt zu Staub.");

  // Sichern der Standards
  SetProp(ID_BACKUP,QueryProp(P_IDS));
  SetProp(ADJ_BACKUP,QueryProp(P_ADJECTIVES));

  AddId(({"lebensmittel","nahrung","\nfood"}));

  AddCmd(({"iss","esse"}),"cmd_eat");
  AddCmd(({"trink","trinke"}),"cmd_drink");
}

// Aendert Werte der Speise, wenn sie vergammelt ist und konsumiert wird
protected void consume_bad(mapping entry_info) {
  entry_info[P_HP] = 0;
  entry_info[P_SP] = 0;
  entry_info[H_EFFECTS] = ([P_POISON:1]);
}

// Stellt Mapping zum Konsumieren zusammen und konsumiert.
// kann mit testonly=1 aufgerufen werden, um zu pruefen, ob konsumieren klappt
// Gibt das Ergebnis von living->consume() zurueck:
// >0 -> erfolgreich konsumiert
// <0 -> nicht konsumiert (siehe Hilfe zu consume)
protected varargs int try_consume(object consumer, int testonly) {
  mapping entry_info = ([P_FOOD:QueryProp(P_FOOD),
                        P_DRINK:QueryProp(P_DRINK),
                           P_HP:QueryProp(P_HP),
                           P_SP:QueryProp(P_SP),
                       P_POISON:QueryProp(P_POISON),
                      P_ALCOHOL:QueryProp(P_ALCOHOL),
                 H_DISTRIBUTION:QueryProp(P_DISTRIBUTION)]);

  if (is_bad()) consume_bad(entry_info);

  int result = (int)consumer->consume(entry_info, testonly);
  if (!result) {
    tell_object(consumer,
      "Da ist was schief gelaufen! Sprich mal mit einem Magier.\n");
    raise_error("living->consume() mit falschen Parametern aufgerufen:\n"+
      sprintf("%O\n",entry_info));
  }

  return result;
}

// Konsumieren war erfolgreich
// Hier kann man beim Ueberschreiben noch zusaetzliche Sachen machen
// P_PORTIONS ist bereits runtergezaehlt, Speise ist aber noch nicht leer!
// make_empty() wird also eventuell noch danach aufgerufen
// entspricht alter Func BeimEssen()
protected void success_consume(object consumer) {
  tell_room(environment(consumer),eval_property(P_CONSUME_MSG,consumer),({consumer}));
  tell_object(consumer,eval_property(P_EATER_MSG,consumer));
}

// Konsumieren war nicht erfolgreich
// Hier kann man beim Ueberschreiben noch zusaetzliche Sachen machen
// in reason wird der Grund des Fehlschlagens uebergeben (siehe 'man consume')
protected void failed_consume(object consumer, int reason) {
  if (reason & HC_MAX_FOOD_REACHED)
    tell_object(consumer,eval_property(P_FOOD_FULL_MSG,consumer));
  else if (reason & HC_MAX_DRINK_REACHED)
    tell_object(consumer,eval_property(P_DRINK_FULL_MSG,consumer));
  else if (reason & HC_MAX_ALCOHOL_REACHED)
    tell_object(consumer,eval_property(P_ALC_FULL_MSG,consumer));
}

// Konsumiert die Speise
// Gibt das Ergebnis von living->consume() zurueck:
// >0 -> erfolgreich konsumiert
// <0 -> nicht konsumiert (siehe Hilfe zu consume)
protected int consume(object consumer) {
  if (!objectp(consumer) || !living(consumer)) {
    raise_error("argument pl not a living:\n"+
      sprintf("%O\n",consumer));
  }
  // damit abgelaufene Speisen auch wirklich abgelaufen sind
  if (!safety_check()) return 0;

  if (!is_not_empty()) {
    notify_fail(eval_property(P_EMPTY_MSG,consumer));
    return 0;
  }
  int result = try_consume(consumer);
  if (result > 0) {
    SetProp(P_PORTIONS,QueryProp(P_PORTIONS)-1);
    success_consume(consumer);
    if (!is_not_empty()) make_empty();
  }
  else if (result < 0) {
    failed_consume(consumer,result);
  }
  return result;
}

// Futtern
int cmd_eat(string str) {
  notify_fail("WAS moechtest Du essen?\n");
  if (!str || !id(str)) return 0;
  object env = environment(this_object());
  if (env != this_player() && QueryProp(P_ENV_MSG)) {
    notify_fail(eval_property(P_ENV_MSG,env));
    return 0;
  }

  if (!is_eatable() && !is_drinkable()) {
    write("Mit diesem Futter stimmt was nicht!"
      " Sprich mal mit einem Magier.\n");
    raise_error("Food ohne Werte.\n");
    return 0;
  }

  if (!is_eatable()) {
    notify_fail(eval_property(P_NOFOOD_MSG,env));
    return 0;
  }

  return consume(this_player());
}

// Trinken
int cmd_drink(string str) {
  notify_fail("WAS moechtest Du trinken?\n");
  if (!str || !id(str)) return 0;
  object env = environment(this_object());
  if (env != this_player() && QueryProp(P_ENV_MSG)) {
    notify_fail(eval_property(P_ENV_MSG,env));
    return 0;
  }

  if (!is_eatable() && !is_drinkable()) {
    write("Mit diesem Futter stimmt was nicht!"
      " Sprich mal mit einem Magier.\n");
    raise_error("Food ohne Werte.\n");
    return 0;
  }

  if (!is_drinkable()) {
    notify_fail(eval_property(P_NODRINK_MSG,env));
    return 0;
  }

  return consume(this_player());
}

//============================================================================
// Kompatibilitaetscode
// um eventuell vergessenes Food oder Zugriff von aussen auf altes Food
// mitzubekommen

nosave deprecated int alc, alc_pp, food, food_pp, heal, p, soft, soft_pp, c;

deprecated int eat_this( string str ) {
  return cmd_eat(str);
}

deprecated int drink_this(string str) {
  return cmd_drink(str);
}

deprecated void MakeDrink() {
  SetProp(P_DRINK,1);
  SetProp(P_FOOD,0);
}

deprecated mixed _query_old_is_food() {
  return is_eatable();
}

deprecated void _set_old_is_food(mixed value) {
  SetProp(P_DRINK,0);
  SetProp(P_FOOD,value);
}

deprecated mixed _query_old_is_drink() {
  return is_drinkable();
}

deprecated void _set_old_is_drink(mixed value) {
  SetProp(P_DRINK,value);
  SetProp(P_FOOD,0);
}

deprecated mixed _query_old_is_full() {
  return is_not_empty();
}

deprecated void _set_old_is_full(mixed value) {
  SetProp(P_PORTIONS,value);
}

deprecated mixed _query_old_alc() {
  return QueryProp(P_ALCOHOL);
}

deprecated void _set_old_alc(mixed value) {
  SetProp(P_ALCOHOL,value);
}

deprecated mixed _query_old_water() {
  return QueryProp(P_DRINK);
}

deprecated void _set_old_water(mixed value) {
  SetProp(P_DRINK,value);
}

deprecated mixed _query_old_food_size() {
  return QueryProp(P_FOOD);
}

deprecated void _set_old_food_size(mixed value) {
  SetProp(P_FOOD,value);
}

deprecated mixed _query_old_potion() {
  return QueryProp(P_PORTIONS);
}

deprecated void _set_old_potion(mixed value) {
  SetProp(P_PORTIONS,value);
}

deprecated mixed _query_old_heal_hp() {
  return QueryProp(P_HP);
}

deprecated void _set_old_heal_hp(mixed value) {
  SetProp(P_HP,value);
}

deprecated mixed _query_old_heal_sp() {
  return QueryProp(P_SP);
}

deprecated void _set_old_heal_sp(mixed value) {
  SetProp(P_SP,value);
}

deprecated mixed _query_old_bad() {
  return is_bad();
}

deprecated void _set_old_bad(mixed value) {
  return;
}

deprecated mixed _query_old_mess() {
  return QueryProp(P_CONSUME_MSG);
}

deprecated void _set_old_mess(mixed value) {
  SetProp(P_CONSUME_MSG,value);
}

deprecated mixed _query_old_eater() {
  return QueryProp(P_EATER_MSG);
}

deprecated void _set_old_eater(mixed value) {
  SetProp(P_EATER_MSG,value);
}

deprecated mixed _query_old_poison() {
  return QueryProp(P_POISON);
}

deprecated void _set_old_poison(mixed value) {
  SetProp(P_POISON,value);
}

// interne Methode
private mixed get_old_empty(string prop) {
  mixed props = QueryProp(P_EMPTY_PROPS);
  if (mappingp(props)) return props[prop];
  return 0;
}
// interne Methode
private void set_old_empty(string prop, mixed value) {
  mixed props = QueryProp(P_EMPTY_PROPS);
  if (!mappingp(props)) props = ([prop:value]);
  else props[prop] = value;
  SetProp(P_EMPTY_PROPS,props);
}

deprecated mixed _query_old_empty_con() {
  return get_old_empty(P_SHORT);
}

deprecated void _set_old_empty_con(mixed value) {
  set_old_empty(P_SHORT,value);
}

deprecated mixed _query_old_empty_gender() {
  return get_old_empty(P_GENDER);
}

deprecated void _set_old_empty_gender(mixed value) {
  set_old_empty(P_GENDER,value);
}

deprecated mixed _query_old_empty_id() {
  return get_old_empty(P_IDS);
}

deprecated void _set_old_empty_id(mixed value) {
  set_old_empty(P_IDS,value);
}

deprecated mixed _query_old_empty_long() {
  return get_old_empty(P_LONG);
}

deprecated void _set_old_empty_long(mixed value) {
  set_old_empty(P_LONG,value);
}

deprecated mixed _query_old_no_con() {
  return mappingp(QueryProp(P_EMPTY_PROPS));
}

deprecated void _set_old_no_con(mixed value) {
  if (value) SetProp(P_EMPTY_PROPS,([]));
  else SetProp(P_EMPTY_PROPS,0);
}

// Mupfel
deprecated int Essbar() {
  return 1;
}

deprecated mixed _query_food_info() {
  return ([]);
}

deprecated void _set_food_info(mixed value) {
  return;
}

// Troy
deprecated void food_decay() {
  return;
}

deprecated int eat_me(string str) {
  return 0;
}
