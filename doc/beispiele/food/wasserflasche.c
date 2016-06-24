/*
Beispiel fuer ein tragbares Getraenk in einer Flasche.
*/

#include <food.h>

inherit "/std/food";

string _query_long() {
  string m = "Du siehst eine Glasflasche,";
  if (is_not_empty()) {
    m += " in der sich Wasser befindet.";
    if (!is_bad())
      m += " Es ist glasklar und sieht irgendwie verlockend aus.";
    else
      m += " Es ist truebe und sieht nicht gerade verlockend aus.";

    m += " Die Flasche ist ";
    switch (to_int((QueryProp(P_PORTIONS)-1)/2.5)) {
      case 0:
        m += "fast leer.";
        break;
      case 1:
        m += "halb voll.";
        break;
      case 2:
        m += "fast voll.";
        break;
      case 3:
        m += "voll.";
        break;
      default:
        m += "nicht leer.";
    }
  } else {
    m += " in der frueher mal Wasser war.";
  }
  return break_string(m);
}

void create() {
  if (!clonep(this_object())) {
    set_next_reset(-1);
    return;
  }
  ::create();

  SetProp(P_SHORT,"Eine Flasche mit Wasser");
  SetProp(P_NAME, "Wasserflasche");
  SetProp(P_GENDER,FEMALE);
  SetProp(P_VALUE,5); // pro Portion (ohne Flasche)
  SetProp(P_WEIGHT,50); // pro Portion (ohne Flasche)
  SetProp(P_PORTIONS,10);
  SetProp(P_DESTROY_BAD,0); // verdorbenes Wasser wird nicht zerstoert
  SetProp(P_CONSUME_MSG,
    "@WER2 trinkt aus einer Flasche. Wasser laeuft ueber @WENQPPNS2 Kinn.");
  SetProp(P_EATER_MSG,
    "Du trinkst Wasser aus der Flasche. Etwas laeuft Dir ueber das Kinn.");
  SetProp(P_BAD_MSG,
    "Das Wasser in der Flasche wird truebe.");

  SetProp(P_MATERIAL,([MAT_GLASS:15, MAT_WATER:85]));

  SetProp(P_EMPTY_PROPS, ([
    P_SHORT:"Eine leere Flasche",
    P_NAME:"Flasche",
    P_EMPTY_IDS:({"flasche"}),
    P_EMPTY_ADJ:({"leere","leeren"}),
    P_MATERIAL:([MAT_GLASS:100]),
    P_VALUE:5, // Wert der Flasche ohne Wasser
    P_WEIGHT:20  // Gewicht der Flasche ohne Wasser
  ]));

  SetProp(P_RESET_LIFETIME,3); // 3 Resets, ehe das Wasser truebe wird
  SetProp(P_DRINK,50); // Fuellgrad pro Schluck
  SetProp(P_DISTRIBUTION,10); // Punkte Heilung pro Heartbeat
  SetProp(P_HP,30); // pro Schluck
  SetProp(P_SP,30); // pro Schluck

  AddId(({"flasche","wasserflasche","wasser"}));
  AddAdjective(({"volle","vollen"}));

}

// parent methode immer aufrufen und pruefen!!
void make_bad() {
  if (!::make_bad() && is_not_empty()) {
    // die Trinkmeldung aendern wir mal
    SetProp(P_EATER_MSG,"Du trinkst truebes Wasser aus der Flasche. "
      "Das war wohl nicht so gut. Dir wird ganz uebel!");
  }
}