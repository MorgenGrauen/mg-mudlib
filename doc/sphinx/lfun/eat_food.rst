eat_food()
==========

FUNKTION
--------
::

    public varargs int eat_food(int food, int testonly, string mytext)

DEFINIERT IN
------------
::

    /std/living/life.c

ARGUMENTE
---------
::

    food: Wird zu dem augenblicklichen Saettigungsgrad (P_FOOD) addiert.
    testonly: Ist das Flag gesetzt, wird dem Spieler kein FOOD zugefuehrt.
              Darf nur zum Testen der Heilstelle verwendet werden und muss
              im normalen Betrieb auf '0' stehen!
    mytext: Wer selber einen Text bei Misserfolg ausgeben lassen moechte,
            darf sich hier was nettes ausdenken.
            Achtung: Das unterdrueckt nicht die "Hunger"-Meldung, die bei
            negativem food auftreten kann, wenn P_FOOD wieder 0 ist.

BESCHREIBUNG
------------
::

    Es wird geprueft, ob dem Spieler der angebene Wert "strength" auf seine
    aktuelle P_FOOD addiert werden kann oder nicht. Ist dies moeglich, wird
    wird es gemacht, es sei denn das testonly != 0.

    Sollen neben P_FOOD noch weitere Props manipuliert werden - bspw. zur
    Heilung eines Lebewesens - bietet sich die Funktion consume() an.

RUECKGABEWERT
-------------
::

     0, wenn strength + P_FOOD > P_MAX_FOOD.
    >0, wenn Erfolg.

BEMERKUNG
---------
::

    eat_food() bitte anstatt eigener Manipulationen von P_FOOD und
    P_MAX_FOOD verwenden.

    Achtung: Immer erst VOR einer Heilung ausfuehren und bei Erfolg heilen.

    Bei Heilstellen sollte eine evtl. Heilung des Spielers mit der eigens
    dafuer eingerichteten Funktion check_and_update_timed_key realisiert
    werden.

BEISPIEL
--------
::

    int heilstelle() {
      // Wenn auf das P_FOOD des Spielers die angegebenen 10 nicht mehr addiert
      // addiert werden koennen (weil sonst P_MAX_FOOD ueberschritten wird),
      // wird die Fehlermeldung ausgegeben, dass der Spieler nichts mehr
      // essen/trinken kann.
      // Bei gesetztem 'mytext' wird 'mytext' an den Spieler ausgegeben.
      // Ansonsten wird die Standardmeldung ausgegeben.
      if (!this_player()->eat_food(10, 0, "Der Keks ist einfach "
                                          "zuviel fuer Dich.\n") )
        return 1;

      // Spieler hatte noch ausreichend Spielraum bei P_FOOD. Die 10 sind 
      // schon addiert worden. Jetzt Nachricht ausgeben:
      tell_object(this_player(), break_string("Du knabberst ein bisschen an "
                  "dem Keks herum und fuehlst Dich gleich viel besser.", 78));

      // alle Lebewesen im Raum bekommen das auch mit
      tell_room(environment(this_player()), 
          this_player()->Name()+" knuspert einen Keks weg.\n", 
          ({this_player()}));

      // Rassenabhaengige Heilung: Sofort oder in Schritten
      // Tragbare Heilungen sollten auch eher buffer_hp/_sp benutzen.
      if(this_player()->QueryProp(P_REAL_RACE)=="Kruemelmonster")
        this_player()->heal_self(30);
      else {
        this_player()->buffer_hp(30,5);
        this_player()->buffer_sp(30,5);
      }

      return 1;
    }

SIEHE AUCH
----------
::

     Aehnlich:  consume, drink_alcohol, drink_soft
     Heilung:   heal_self, restore_spell_points, restore_hit_points, 
                buffer_hp, buffer_sp
     Timing:    check_and_update_timed_key
     Enttanken: defuel_drink, defuel_food
     Props:     P_DRINK, P_FOOD, P_ALCOHOL, P_SP, P_HP,
                P_DEFUEL_TIME_DRINK
     Konzepte:  heilung, enttanken, food

9. August 2015 Gloinson

