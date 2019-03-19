// MorgenGrauen MUDlib
//
// Ein simpler Raum fuer Test- und Demozwecke von vItems.

#pragma strong_types,save_types,rtt_checks
#pragma pedantic, range_check

inherit "/std/room";

#include <properties.h>
#include <defines.h>
#include <container/vitems.h>
#include <materials.h>

#define BS(x) break_string(x, 78)

protected void create()
{
  ::create();
  SetProp( P_LIGHT, 1 );
  SetProp( P_INT_SHORT, "Ein Testraum fuer vItems." );
  SetProp( P_INT_LONG, BS(
      "Dies ist Testraum fuer vItems. An der Wand haengt ein Seil und eine "
      "Fackel. Auf dem Boden liegen eine Eisstange und ein Ue-Ei."
         ));
  SetProp(P_INDOORS,1);
  AddDetail( ({ "boden", "fussboden" }), BS(
     "Auf dem Fussboden liegen eine Flasche, eine Schaufel und eine "
     "Eisstange."));
  AddDetail( ({ "wand", "waende" }),
     "An den Waenden haengt ein Seil, eine Fackel und ein Schild.\n" );

  AddVItem("fackel", VI_REFRESH_REMOVE,
           0,
           "/items/fackel",
           ([P_SHORT: "Eine pechschwarze Fackel.",
             P_LONG:BS("Eine pechschwarze Fackel. Man kann sie anzuenden "
                       "und wieder loeschen."),
             P_ADJECTIVES: ({"schwarze","pechschwarze"}),
             P_MATERIAL: ([MAT_MISC_WOOD: 50, MAT_TAR: 40,
                           MAT_MISC_MAGIC: 10]),
             P_DETAILS: ([
               "detail": "Das Detail gibts doch gar nicht!\n",
              ]),
            ])
          );
  AddVItem("seil", VI_REFRESH_REMOVE,
           ([
             P_DETAILS: ([
              "detail": "Das Detail gibts doch gar nicht!\n",
             ]),
            ]),
           "/items/seil"
           );
  AddVItem("schild", VI_REFRESH_NONE,
           ([P_SHORT:"Ein Schild.",
             P_LONG: "Ein einfaches Holzschild. Du kannst es lesen.\n",
             P_READ_DETAILS: ([SENSE_DEFAULT:
               "Dies ist ein vItem-Test.\n" ]),
             P_SMELLS: ([SENSE_DEFAULT:
               "Es riecht gar nicht. Wirklich nicht.\n"]),
             P_TOUCH_DETAILS: ([SENSE_DEFAULT:
               "Das Schild is grob bearbeitet und rau.\n",
               "holz": "Es ist ganz zersplittert.\n"]),
             P_DETAILS: (["holz": "Irgendein braunes Holz.\n"]),
            ])
          );
  AddVItem("eisstange", VI_REFRESH_REMOVE,
           0,
           "/items/eisenstange",
           ([P_LONG:BS("Diese Eisstange eignet sich hervorragend dazu, "
                       "mit ihr auf jemanden einzuschlagen.\n"),
             P_NAME: "Eisstange",
             VI_PROP_RESET(P_IDS): ({"stange", "eisstange"}),
             VI_PROP_RESET(P_MATERIAL): ([MAT_ICE: 100]),
             VI_PROP_RESET(P_DAM_TYPE): ({DT_BLUDGEON, DT_COLD}),
             P_WEIGHT: 5000,
             P_VALUE: 500,
             VI_PROP_RESET(P_SMELLS): ([SENSE_DEFAULT:
               "Das Eis riecht nach Vanille!\n"]),
            ])
          );
  AddVItem("ue-ei", VI_REFRESH_REMOVE,
           ([P_SHORT:"Ein Ue-Ei.",
             P_LONG: BS("Da liegt tatsaechlich ein Ue-Ei auf dem Boden. "
               "Wenn man es nimmt, verwandelt es sich in etwas anderes."),
             VI_PROP_RESET(P_IDS): ({"ue-ei","ei"}),
             ]),
           "/items/schaufel"
          );
}

