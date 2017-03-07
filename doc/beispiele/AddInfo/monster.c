#pragma strong_types, save_types, rtt_checks, pedantic
inherit "std/npc";

#include <properties.h>

protected string query_kekse();
protected string question_gold();

protected void create() {
  if (!clonep(this_object())) return;
  ::create();

  SetProp(P_SHORT, "Ein Monster");
  SetProp(P_LONG, break_string(
    "Das Monster sieht dich hungrig an. "
    "Wahrscheinlich will es Geld oder Kekse.", 78));
  SetProp(P_NAME, "Monster");
  SetProp(P_GENDER, NEUTER);
  AddId("monster");

  SetProp(P_RACE, "Monster");
  SetProp(P_XP, 0);
  SetProp(P_BODY, 50);
  SetProp(P_SIZE, random(20)+110);

  // Beispiel 1
  AddInfo(DEFAULT_INFO, "starrt Dir boese in die Augen.\n");

  // Beispiel 2
  AddInfo(({"knete","kohle"}),
    "sagt: ich habe so etwas nicht.");
  AddInfo("geld",
    "Ich habe zwar kein Geld, aber [...] blablabla [...] blub muh quak "
    "lorem blupsum Umbruch now!",
    "sagt: ");
  AddInfo("muenzen",
    "fluestert: Du willst Geld?",
    0,
    "fluestert @WEM etwas zu.");

  // Beispiel 3
  AddInfo(({"keks","kekse"}),
    #'query_kekse,        // ein Verweis auf die Funktion
    "sagt: ");

  // Beispiel 5
  AddInfo("gold", #'question_gold, "murmelt: ", 1);
}

protected string query_kekse() {
  if(present("keks", this_object()))
    return("Ich hab noch welche. Aetsch!");
  else if(present("keks", environment()))
    return("Da liegt einer!");
  return("Menno. Keine mehr da!");
}

protected string question_gold() {
  // wieviel Kohle hat der Spieler
  int money = this_player()->QueryMoney();
  string *y = map(deep_inventory(this_player()),
                function string(object o) {
                  if(o->QueryMaterial(MAT_GOLD)>0 &&
                     strstr(object_name(o),"/items/money")<0 &&
                     o->QueryProp(P_NAME))
                    return o->name(WER,1);
                })-({0});

  // das geht an alle anderen im Raum, silent bietet sich hier
  // nicht an, weil es mehrere Moeglichkeiten gibt
  send_room(environment(),
    (Name(WER,1)+" murmelt "+
     this_player()->name(WEM,1)+
     " etwas zu"+
     ((money || sizeof(y))?
       " und glotzt "+this_player()->QueryPronoun(WEN)+" gierig an.":
       ".")),
    MT_LOOK|MT_LISTEN,
    MA_EMOTE, 0, ({this_player()}));

  // und hier die Antwort an den Spieler selbst, mit vielen
  // Verzweigungen fuer dessen Besitztum
  return("Ich hab kein Gold bei mir."+
    ((money || sizeof(y))?
      " Aber du "+
      (money?
        "hast ja jede Menge Kohle bei dir, so etwa "+money+
        " Muenzen."+
        (sizeof(y)?
          " Ausserdem "+
          ((sizeof(y)==1)?"ist":"sind")+
          " auch noch "+CountUp(y)+" aus Gold.":
          ""):
        (sizeof(y)?"hast ja ein paar Wertsachen dabei: "+
        CountUp(y)+
        (sizeof(y)==1?" ist":" sind")+
        " aus Gold.":"")):
      ""));
}