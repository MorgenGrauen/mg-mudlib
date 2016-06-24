// MorgenGrauen MUDlib
//
// /sys/food.sys  - Defines und Prototypen fuer Lebensmittel
//
// $Id: food.h 7702 2010-10-26 21:22:37Z Zesstra $

#ifndef _FOOD_H_
#define _FOOD_H_

#include <properties.h>

// Fuellgrad pro Portion Essen (defined in life.h)
// #define   P_FOOD

// Fuellgrad pro Schluck des Getraenkes (defined in life.h)
// #define   P_DRINK

// Menge an Alkohol, die die Speise pro Portion hat (defined in life.h)
// #define   P_ALCOHOL

// Heilwirkung der Speise (defined in life.h)
// #define   P_SP
// #define   P_HP

// Gewicht pro Portion (defined in restrictions.h)
// in P_EMPTY_PROPS ist es das Gewicht des Behaelters
// mit beiden Angaben wird das Gewicht immer korrekt berechnet
// #define   P_WEIGHT

// Wert pro Portion (defined in properties.h)
// in P_EMPTY_PROPS ist es der Wert des Behaelters
// mit beiden Angaben wird der Wert immer korrekt berechnet
// #define   P_VALUE

// Die Meldung, die die Umstehenden erhalten, wenn gegessen/getrunken wird.
#define   P_CONSUME_MSG        "std_food_consume_msg"

// Die Meldung, die derjenige kriegt, der das Ding isst/trinkt.
#define   P_EATER_MSG          "std_food_eater_msg"

// Message, wenn man die Speise beim Konsumieren gar nicht in der Hand hat
// Wenn man die Prop loescht, kann die Speise immer vertilgt werden
#define   P_ENV_MSG            "std_food_env_msg"

// Die Meldung, wenn die Speise leer ist
#define   P_EMPTY_MSG          "std_food_empty_msg"

// Die Meldung, wenn Getraenke gegessen werden
#define   P_NOFOOD_MSG         "std_food_nofood_msg"

// Die Meldung, wenn Speisen getrunken werden
#define   P_NODRINK_MSG        "std_food_nodrink_msg"

// Die Meldung, wenn die Speise gerade verdirbt
#define   P_BAD_MSG            "std_food_bad_msg"

// Die Meldung, wenn nichts mehr gegessen werden kann
#define   P_FOOD_FULL_MSG      "std_food_full_msg"
// Die Meldung, wenn nichts mehr getrunken werden kann
#define   P_DRINK_FULL_MSG     "std_food_drink_full_msg"
// Die Meldung, wenn nichts mehr alkoholisches geht
#define   P_ALC_FULL_MSG       "std_food_alc_full_msg"
// Die Meldung, wenn eine verdorbene Speise zerstoert wird
// wird nur verwendet, wenn P_DESTROY_BAD > 0 ist
#define   P_REMOVE_MSG         "std_food_remove_message"

// Mapping mit Properties fuer den leeren Behaelter.
// Alle enthaltenen Properties werden gesetzt, wenn
// keine Portionen mehr vorhanden sind, bereits in diesen Properties
// eingetragene Werte werden ueberschrieben!
// Wenn diese Property nicht gesetzt ist, wird die Speise zerstoert,
// wenn alle Portionen aufgebraucht ist.
// Achtung: es werden keine closures unterstuetzt!
#define   P_EMPTY_PROPS        "std_food_empty_props"

// Wieviele Portionen hat die Speise
#define   P_PORTIONS           "std_food_portions"

// Verteilung der Heilungsrate pro Portion (siehe H_DISTRIBUTION)
#define   P_DISTRIBUTION       "std_food_distribution"

// Anzahl der Sekunden, bis die Speise verdorben ist.
// Es muss ein Wert groesser 0 eingetragen werden.
// Wenn kein Wert gesetzt wird, verdirbt die Speise beim naechsten Reset
// Der Timer startet immer erst, wenn ein Spieler die Speise das erste Mal
// in die Hand bekommt
#define   P_LIFETIME           "std_food_lifetime"
// Anzahl der Resets, bis die Speise verdorben ist.
// P_RESET_LIFETIME und P_LIFETIME sind komplementaer, das heisst, dass
// beim Setzen der einen die andere automatisch berechnet wird. Man braucht
// daher nicht beide zu setzen, bzw. sollte dies gar nicht erst tun.
#define   P_RESET_LIFETIME     "std_food_lifetime_reset"

// Flag, ob die Speise verdirbt
// Standard: 0 -> Darf nur mit Balancegenehmigung geaendert werden!
#define   P_NO_BAD             "std_food_no_bad"

// Zeit in Sekunden, wann die Speise nach dem Schlechtwerden zerstoert wird.
// wenn P_EMPTY_PROPS gesetzt ist, wird der Behaelter geleert.
// Standard: -1 = sofort beim Schlechtwerden
//            0 = wird nicht zerstoert
//           >0 = Anzahl der Sekunden
#define   P_DESTROY_BAD        "std_food_destroy_bad"
#define   DESTROY_NEVER         0
#define   DESTROY_BAD          -1

//============================================================================
// Kompatibilitaetscode
#define   F_IS_FOOD            "old_is_food"
#define   F_IS_DRINK           "old_is_drink"
#define   F_IS_FULL            "old_is_full"
#define   F_MESSAGE            "old_mess"
#define   F_EATER              "old_eater"
#define   F_EMPTY_CONTAINER    "old_empty_con"
#define   F_EMPTY_GENDER       "old_empty_gender"
#define   F_EMPTY_ID           "old_empty_id"
#define   F_EMPTY_LONG         "old_empty_long"
#define   F_ALC                "old_alc"
#define   F_LIQUID             "old_water"
#define   F_FOOD_SIZE          "old_food_size"
#define   F_POTION             "old_potion"
#define   F_HEAL_SP            "old_heal_sp"
#define   F_HEAL_HP            "old_heal_hp"
#define   F_NO_CONTAINER       "old_no_con"
#define   F_BAD                "old_bad"
#define   F_POISON             "old_poison"

#endif // _FOOD_H_

#ifdef NEED_PROTOTYPES
#ifndef _FOOD_H_PROTOTYPES_
#define _FOOD_H_PROTOTYPES_

public int cmd_eat(string str);
public int cmd_drink(string str);
public int is_eatable();
public int is_drinkable();
public int is_not_empty();
public int is_bad();

// Anzahl der Sekunden, die das Lebensmittel bereits hinter sich hat,
// seit es mit einem Spieler in Beruehrung kam
// wenn 0 zurueckgegeben wird, ist der Timer zum Schlechtwerden noch nicht
// gestartet worden
public int get_current_lifetime();

// Startet den Timer zum Verderben der Speise, falls keine Gruende dagegen
// sprechen, wird von init() und NotifyMove aufgerufen, kann in speziellen
// Situationen aber auch manuell aufgerufen werden (siehe man food)
public void start_lifetime();

// wird aufgerufen, wenn die Speise verdorben ist und die Zeit aus
// P_DESTROY_BAD abgelaufen ist
// return 1, falls Objekt zerstoert wird, sonst 0
public int make_destroy();

// wird aufgerufen, wenn die Speise gerade verdirbt
// return 1, falls Objekt dabei zerstoert wird, sonst 0
public int make_bad();

// wird aufgerufen, sobald die letzte Portion gegessen/getrunken wurde
// Falls P_EMPTY_PROPS 0 ist, wird das Objekt zerstoert
// return 1, falls Objekt dabei zerstoert wird, sonst 0
public int make_empty();

// wird jedesmal aufgerufen, wenn eine verdorbene Speise konsumiert wird.
// Als Argument wird das Mapping uebergeben, das consume() im Lebewesen
// uebergeben werden soll. Dieses Mapping kann veraendert werden.
// Standard: Die Heilwirkung wird entfernt und stattdessen wird das Lebewesen
//           vergiftet.
protected void consume_bad(mapping entry_info);

// wird aus cmd_eat und cmd_drink aufgerufen und fuehrt das Konsumieren durch
protected int consume();
// wird aus consume() aufgerufen und fuehrt das Konsumieren durch
// ist ausgelagert, damit man das testflag bei Bedarf nutzen kann
protected varargs int try_consume(int testonly);

// Futtern hat geklappt
protected void success_consume();
// Futtern hat nicht geklappt (leerer Behaelter ist hier kein Grund!)
// Wird aus try_consume() gerufen.
// reason entspricht dem Rueckgabewert von consume() im Lebewesen.
protected void failed_consume(int reason);

// Methode, die Meldungen zu Statusaenderungen verarbeitet und korrekt ausgibt.
// Es wird der Name der auszugebenen Property uebergeben.
// Es werden alle notwendigen Ersetzungen per replace_personal gemacht und
// geprueft, ob dem Besitzer oder dem Raum die Meldung ausgegeben werden
// muss.
// Hierueber sollten nur Meldungen ausgegeben werden, die durch Aenderungen
// an der Speise im Reset ausgeloest werden, also im reset selbst und in den
// make_*-Methoden.
protected void message(string prop);

#endif // _FOOD_H_PROTOTYPES_
#endif // NEED_PROTOTYPES

