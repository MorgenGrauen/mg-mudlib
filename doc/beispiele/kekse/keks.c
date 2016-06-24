/* Einfaches Beispiel zur Demonstration von Vererbung.
   ACHTUNG: Dieses Beispiel stellt _kein_ sinnvolles Lebensmittelobjekt dar.
            Hierfuer sollte /std/food verwendet werden, siehe auch
            /doc/beispiele/food/
*/

#include <properties.h>
#include <moving.h>
inherit "/std/thing";

protected void create() {
  ::create();
  SetProp(P_NAME, "Keks");
  SetProp(P_GENDER, MALE);
  AddId(({"keks"}));
  AddCmd("iss&@ID", "action_essen", "Was willst du essen?");
}

public int action_essen(string cmd) {
  if(this_player()->eat_food(1, 0, 
                            "Du bekommst "+QueryPronoun(WEN)+
                            " nicht mehr hineingestopft.\n")>0) {
    write("Du isst "+name(WEN,1)+".\n");
    say(this_player()->Name(WER)+" isst "+name(WEN)+".\n");
    remove(1);
  }
  return 1;
}
