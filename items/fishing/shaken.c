#pragma strong_types, save_types, rtt_checks
#pragma no_shadow, no_inherit

#include <language.h>
#include <properties.h>
#include <fishing.h>
#include <items/flasche.h>

inherit STD_HAKEN;

protected void create() {
  ::create();
  AddId(({"spezialhaken"}));
  SetProp(P_NAME, "Haken");
  SetProp(P_ARTICLE, 1);
  SetProp(P_WATER, W_UNIVERSAL);
  SetProp(P_FISH, -20+random(60)); // -20 ... +39
  SetProp(P_SHORT, "Ein Spezialhaken");
  SetProp(P_LONG, break_string(
    "Ein Angelhaken aus Metall. Es ist eine Spezialanfertigung aus Sam "
    "Harkwinds Anglerladen, die einen kuenstlichen Wurm enthaelt.\n",78));
  SetProp(P_INFO, break_string(
    "Der kuenstliche Koeder soll in jedem Gewaesser recht gut "
    "funktionieren, hat man Dir im Laden versprochen.",78));
  
  AddDetail(({"koeder","wurm"}), function string (string key) {
    string desc = "Der Koeder ist handwerklich %s gelungen. Du glaubst, "
      "dass er auf Fische %s wirken duerfte.";
    string craft, effect;
    switch(QueryProp(P_FISH)) {
      case -20..-6:
        craft = "ziemlich mies";
        effect = "eher abstossend"; 
        break;
      case -5..9:
        craft = "bestenfalls durchschnittlich";
        effect = "genauso durchschnittlich";
        break;
      case 10..24:
        craft = "wirklich solide";
        effect = "recht appetitlich";
        break;
      case 25..39:
        craft = "ueberragend";
        effect = "unwiderstehlich";
        break;
      default: 
        craft = "so la-la";
        effect = "komisch";
        break;
    }
    return break_string(sprintf(desc,craft,effect),78);
  });

  SetProp(P_LONG_EMPTY, "");
  SetProp(P_VALUE, 20);
  SetProp(P_WEIGHT, 10);
}

int QueryKoeder() { 
  return 1; 
}
