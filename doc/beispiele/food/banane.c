/*
Beispiel fuer eine Banane, die in einem Haps weg ist,
aber eine Schale hat, die uebrig bleibt und genau wie die
komplette Banane vergammelt und vergeht.
*/

#include <food.h>

inherit "/std/food";

string _query_long() {
  string m = "Du haeltst ";
  if (is_not_empty()) {
    m += "eine Banane in der Hand.";
    if (!is_bad())
      m += " Sie ist schoen gelb und ohne braune Flecken.";
    else
      m += " Sie ist total braun und schon ganz matschig.";

  } else {
    m += "eine Bananenschale in der Hand.";
    if (is_bad())
      m += " Sie ist ganz verschimmelt.";
  }
  return break_string(m);
}

void create() {
  if (!clonep(this_object())) {
    set_next_reset(-1);
    return;
  }
  ::create();

  SetProp(P_SHORT,"Eine schoene reife Banane");
  SetProp(P_NAME, "Banane");
  SetProp(P_GENDER,FEMALE);
  SetProp(P_VALUE,20); // ohne Schale
  SetProp(P_WEIGHT,50); // ohne Schale
  // SetProp(P_POTION,1); // ist eh Standard
  SetProp(P_CONSUME_MSG,
    "@WER2 schaelt eine Banane und isst sie genuesslich.");
  SetProp(P_EATER_MSG,
    "Du schaelst eine Banane und isst sie genuesslich.");
  SetProp(P_BAD_MSG,
    "Die Banane wird ganz matschig und schlecht.");
  SetProp(P_REMOVE_MSG,
    "Die Banane ist so verschimmelt, dass Du sie entsorgst.");
  SetProp(P_EMPTY_MSG,
    "Aber das ist doch nur eine BananenSCHALE, die willst Du nicht essen!");

  SetProp(P_MATERIAL,([MAT_MISC_PLANT:100]));

  SetProp(P_EMPTY_PROPS, ([
    P_SHORT:"Eine Bananenschale",
    P_NAME:"Bananenschale",
    P_EMPTY_IDS:({"schale","bananenschale","banane"}),
    P_VALUE:5, // Wert der Bananenschale
    P_WEIGHT:5  // Gewicht der Bananenschale
  ]));

  //SetProp(P_RESET_LIFETIME,1); // ist eh Standard
  SetProp(P_DESTROY_BAD,300); // verdorbene Bananen verschwinden nach 5 Min

  SetProp(P_FOOD,50); // Fuellgrad der Banane
  SetProp(P_HP,5);
  SetProp(P_SP,5);

  AddId(({"banane"}));

}

// Der Behaelter vergammelt auch
public int make_destroy() {
  if (!::make_destroy()) {
    return remove(1);
  }
  return 1;
}

public int make_empty() {
  SetProp(P_BAD_MSG,
    "Die Bananenschale wird ganz schimmlig.");
  SetProp(P_REMOVE_MSG,
    "Die Bananenschale zersetzt sich vollstaendig.");
  return ::make_empty();
}

// parent methode immer aufrufen und pruefen!!
void make_bad() {
  if (!::make_bad()) {
    if (is_not_empty()) {
      SetProp(P_SHORT,"Eine matschige Banane");
      AddAdjective(({"matschige","matschigen"}));
      SetProp(P_EATER_MSG,"Du lutschst die matschige Banane aus ihrer"
        "Schale. Baeh, die schmeckt ueberhaupt nicht mehr.");
      SetProp(P_CONSUME_MSG,
        "@WER2 lutscht eine matschige Banane aus ihrer Schale.");
      // Die Schale ist dann natuerlich auch vergammelt
      SetProp(P_EMPTY_PROPS, ([
        P_SHORT:"Eine verschimmelte Bananenschale",
        P_NAME:"Bananenschale",
        P_EMPTY_IDS:({"schale","bananenschale","banane"}),
        P_EMPTY_ADJ:({"verschimmelte","verschimmelten"}),
        P_VALUE:5, // Wert der Bananenschale
        P_WEIGHT:5  // Gewicht der Bananenschale
      ]));
    } else {
      message(P_BAD_MSG);
      SetProp(P_SHORT,"Eine verschimmelte Bananenschale");
      AddAdjective(({"verschimmelte","verschimmelten"}));
    }
  }
}
