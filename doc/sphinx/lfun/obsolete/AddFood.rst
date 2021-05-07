AddFood()
=========

VERALTETE LFUN
--------------
::

    Diese Lfun ist als veraltet gekennzeichnet (deprecated); ihre Nutzung
    loest daher eine Warnung aus. Bitte stattdessen AddToMenu() verwenden.


FUNKTION
--------
::

      void AddFood(string nameOfFood, mixed ids, int price, int heal,
                   mixed myFunction)


BEMERKUNGEN
-----------
::

      Die Funktion AddFood() sollte NICHT MEHR BENUTZT werden.
      Bitte AddToMenu() verwenden.

      Bestehende Aufrufe koennen wie folgt ersetzt werden:

      AddToMenu( nameOfFood,ids,
          ([ P_VALUE : price, P_FOOD : heal, P_HP : heal, P_SP : heal ]),
          min(heal, 5), myFunction, 0,0,0);


SIEHE AUCH
----------
::

        AddToMenu(), RemoveFromMenu(), AddDrink()

Last modified: 2021-04-05, Arathorn
