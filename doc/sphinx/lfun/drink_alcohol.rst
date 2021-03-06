drink_alcohol()
===============

FUNKTION
--------
::

    public varargs int drink_alcohol(int strength, int testonly, string mytext)

DEFINIERT IN
------------
::

    /std/living/life.c

ARGUMENTE
---------
::

    strength: wird zur aktuellen Saettigung P_ALCOHOL dazu addiert
    testonly: Ist das Flag gesetzt, wird dem Spieler kein ALCOHOL zugefuehrt.
              Darf nur zum Testen der Heilstelle verwendet werden und muss
              im normalen Betrieb auf '0' stehen!
    mytext: Wer selber einen Text bei Misserfolg ausgeben lassen moechte,
            darf sich hier was nettes ausdenken.
            Achtung: Das unterdrueckt nicht die "Nuechtern"-Meldung, die bei
            negativem strength auftreten kann, wenn P_ALCOHOL wieder 0 ist.

BESCHREIBUNG
------------
::

    Es wird geprueft, ob dem Spieler der angegebene Wert fuer 'strength'
    auf seine aktuelle P_ALCOHOL addiert werden kann oder nicht. Falls
    das moeglich ist und testonly = 0, wird P_ALCOHOL entsprechend
    aktualisiert.

    Sollen neben P_ALCOHOL noch weitere Props manipuliert werden - bspw. zur
    Heilung eines Lebewesens - bietet sich die Funktion consume() an.

RUECKGABEWERT
-------------
::

    0 bei [potentiellem] Misserfolg (strength + P_ALCOHOL > P_MAX_ALCOHOL)
    1 bei [potentiellem] Erfolg
    * potentiell bezieht sich hier auf Nutzung mit 'testonly' != 0

BEMERKUNG
---------
::

    drink_alocohol() bitte anstatt eigener Manipulationen von P_ALCOHOL und
    P_MAX_ALCOHOL verwenden.

    Achtung: Immer erst VOR einer Heilung ausfuehren und bei Erfolg heilen.

    Bei Heilstellen sollte eine evtl. Heilung des Spielers mit der eigens
    dafuer eingerichteten Funktion check_and_update_timed_key realisiert
    werden.

BEISPIEL
--------
::

    int heilstelle() {
      if(this_player()->drink_alcohol(10, 0,
                                      "Du prustest in den Schnaps. "
                                      "Der passt nicht mehr rein.\n")) {
        // Platz fuer 10 "Alkohol" war noch, diese sind jetzt bereits addiert
        // Nachricht an den Spieler:
        tell_object(this_player(),
                    break_string("Du trinkst den Schnaps aus.", 78));

        // Nachricht an andere Livings im Raum
        object ob = first_inventory(environment(this_player()));
        do {
          if(living(ob) && ob!=this_player())
            ob->ReceiveMsg(this_player()->Name()+" trinkt einen Schnaps aus.",
                           MT_LOOK|MT_LISTEN,
                           MA_DRINK);
          ob = next_inventory(ob);
        } while(ob);

        // Rassenabhaengige Heilung: Sofort oder in Schritten
        // Tragbare Heilungen sollten auch eher buffer_hp/_sp benutzen.
        if(this_player()->QueryProp(P_REAL_RACE)=="Schnapsdrossel")
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

     Aehnlich:  consume, eat_food, drink_soft
     Heilung:   heal_self, restore_spell_points, restore_hit_points, 
                buffer_hp, buffer_sp
     Timing:    check_and_update_timed_key
     Enttanken: defuel_drink, defuel_food
     Props:     P_DRINK, P_FOOD, P_ALCOHOL, P_SP, P_HP,
                P_DEFUEL_TIME_DRINK
     Konzepte:  heilung, enttanken, food

9. August 2015 Gloinson

