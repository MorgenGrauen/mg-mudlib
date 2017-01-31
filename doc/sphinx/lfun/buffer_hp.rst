buffer_hp()
===========

FUNKTION
--------
::

    int buffer_hp( int val, int rate );

DEFINIERT IN
------------
::

    /std/living/life.c

ARGUMENTE
---------
::

    val:  Gesamte Heilung.
    rate: LP-Rate.

        

BESCHREIBUNG
------------
::

    Erhoeht die LP eines Spielers automatisch insgesamt um den Wert "val".
    Pro heart_beat() wird ihm dabei der Wert "rate" zugefuehrt.
    Sollen neben P_HP noch weitere Props manipuliert werden - bspw. zur
    P_FOOD - bietet sich die Funktion consume() an.

RUECKGABEWERTE
--------------
::

    Der getankte Wert pro heart_beat().

BEMERKUNG
---------
::

    Sollte von jeder tragbaren Heilung genutzt werden, welche den Spieler
    darauf schliessen lassen kann, auf natuerlichem und nichtmagischem Weg
    (Essen, Trinken) geheilt worden zu sein.

BEISPIEL
--------
::

    #define TP this_player()
    ...

    int heilung=1;
    ...

    create()
    {
     ::create();
     SetProp(P_NAME,"Heilpflanze");
     ...

     AddCmd("iss","eat");
    }

    int eat(string str)
    {
      notify_fail("WAS willst Du essen?\n");
      if ( !str || !id(str) )
       return 0;
      ...

      if ( !TP->eat_food(25) )
       return 1;

      TP->buffer_hp(20,5);
      TP->buffer_sp(80,10);
      heilung--;
      write(BS("Du fuehlst langsam, wie Deine Kraefte zurueckkehren."));

      return 1;
    }

    reset()
    {
      heilung=1;
      ::reset();
    }

    Es wird durch eat_food getestet, ob der Spieler noch genuegend essen kann.
    Wenn ja, kriegt unser Held die 25 automatisch oben drauf und ausserdem
    20 LP in 5-LP-Schritten und 80 KP in 10-LP-Schritten gutgeschrieben.

SIEHE AUCH
----------
::

     Aehnlich:  heal_self, restore_spell_points, restore_hit_points, 
                buffer_sp
     Tanken:    consume, drink_alcohol, drink_soft, eat_food
     Props:     P_SP, P_HP,
     Konzepte:  heilung

9. August 2015 Gloinson

