AddDrink()
==========

OBSOLETE LFUN
-------------

    Diese Efun existiert nicht mehr. Bitte stattdessen den Hook
    H_HOOK_INSERT benutzen. (s. /doc/std/hooks)

FUNKTION
--------

::

    void AddDrink(string nameOfDrink, mixed ids, int price, int heal,
                  int strength, int soak, mixed myFunction);


BEMERKUNGEN
-----------
::

    Die Funktion AddDrink() sollte NICHT MEHR BENUTZT werden.
    Bitte AddToMenu() verwenden.

    Bestehende Aufrufe koennen wie folgt ersetzt werden:

    AddToMenu(nameOfDrink, ids,
      ([ P_VALUE : price, P_DRINK : soak, P_ALCOHOL : strength,
         P_HP : heal/2, P_SP : heal/2 ]),
         min(heal/2, 5), myFunction, 0, 0, 0);


SIEHE AUCH
----------
::

    AddToMenu(), RemoveFromMenu(), AddFood()


Last modified: Fri Mar 03 13:23:00 2000 by Paracelsus

