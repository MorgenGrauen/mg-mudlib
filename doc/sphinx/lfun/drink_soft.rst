drink_soft()
============

FUNKTION
--------
::

    public varargs int drink_soft(int strength, int testonly, string mytext)

DEFINIERT IN
------------
::

    /std/living/life.c

ARGUMENTE
---------
::

    strength: Wird zu dem augenblicklichen Saettigungsgrad (P_DRINK) addiert.
    testonly: Ist das Flag gesetzt, wird dem Spieler kein DRINK zugefuehrt.
              Darf nur zum Testen der Heilstelle verwendet werden und muss
              im normalen Betrieb auf '0' stehen!
    mytext: Wer selber einen Text bei Misserfolg ausgeben lassen moechte,
            darf sich hier was nettes ausdenken.
            Achtung: Das unterdrueckt nicht die "Durst"-Meldung, die bei
            negativem strength auftritt, wenn P_DRINK wieder 0 ist.

BESCHREIBUNG
------------
::

    Es wird geprueft, ob dem Spieler der angebene Wert "strength" auf
    aktuelle P_DRINK addiert werden kann oder nicht. Ist dies moeglich,
    wird es gemacht, es sei denn das testonly != 0.

    Sollen neben P_DRINK noch weitere Props manipuliert werden - bspw. zur
    Heilung eines Lebewesens - bietet sich die Funktion consume() an.

RUECKGABEWERT
-------------
::

     0, wenn strength + P_DRINK > P_MAX_DRINK
    >0, wenn Erfolg

BEMERKUNG
---------
::

    drink_soft() bitte anstatt eigener Manipulationen von P_DRINK und
    P_MAX_DRINK verwenden.

    Achtung: Immer erst VOR einer Heilung ausfuehren und bei Erfolg heilen.

    Bei Heilstellen sollte eine evtl. Heilung des Spielers mit der eigens
    dafuer eingerichteten Funktion check_and_update_timed_key realisiert
    werden.

BEISPIEL
--------
::

    int heilstelle() {
      if(this_player()->drink_soft(10, 0, "Du blubberst nicht ueberzeugt "
                                   "in der Limonade. Das ist zu viel.\n")) {
        // Platz fuer 10 "Trinken" war noch, diese sind jetzt bereits addiert
        // Nachricht an den Spieler:
        tell_object(this_player(), break_string("Du nimmst einen grossen "
                    "Schluck zuckersuesse Limonade.", 78));

        // alle anderen im Raum bekommen es ggf auch mit:
        // 1) filter(..., #'living) ergibt die Lebewesen im Raum
        // 2) filter_objects(..., "ReceiveMsg") ruft ReceiveMsg an jedem
        // 3) ReceiveMsg(...) bekommt die Folgeparameter
        filter_objects(filter(all_inventory(environment(this_player())),
                              #'living) - ({this_player()}),
                       "ReceiveMsg",
                       this_player()->Name()+
                         " trinkt einen Schluck Limonade.",
                       MT_LOOK|MT_LISTEN,
                       MA_DRINK);

        // Rassenabhaengige Heilung: Sofort oder in Schritten
        // Tragbare Heilungen sollten auch eher buffer_hp/_sp benutzen.
        if(this_player()->QueryProp(P_REAL_RACE)=="Saufziege")
          this_player()->heal_self(30);
        else {
          this_player()->buffer_hp(30,5);
          this_player()->buffer_sp(30,5);
        }
      }

      return 1;
     }

SIEHE AUCH
----------
::

     Aehnlich:  consume, drink_alcohol, eat_food
     Heilung:   heal_self, restore_spell_points, restore_hit_points, 
                buffer_hp, buffer_sp
     Timing:    check_and_update_timed_key
     Enttanken: defuel_drink, defuel_food
     Props:     P_DRINK, P_FOOD, P_ALCOHOL, P_SP, P_HP,
                P_DEFUEL_TIME_DRINK
     Konzepte:  heilung, enttanken, food

9. August 2015 Gloinson

