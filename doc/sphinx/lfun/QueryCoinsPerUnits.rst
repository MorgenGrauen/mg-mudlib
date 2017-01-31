QueryCoinsPerUnits()
====================

QueryCoinsPerUnit()
-------------------
::

FUNKTION
--------
::

     int *QueryCoinsPerUnits();

DEFINIERT IN
------------
::

     /std/unit.c

ARGUMENTE
---------
::

     keine

BESCHREIBUNG
------------
::

     Liefert das Wertverhaeltnis fuer die Einheiten zurueck.

RUeCKGABEWERT
-------------
::

     Ein Array von zwei Zahlen. Die erste Zahl ist der Wert der in der
     zweiten Zahl angegebenen Einheiten.

BEISPIELE
---------
::

     Steht im Unit-Objekt folgende Wertzuweisung:

       SetCoinsPerUnits(7,2);

     so liefert QueryCoinsPerUnits() als Ergebnis ({7, 2}).

SIEHE AUCH
----------
::

     SetCoinsPerUnits(), SetGramsPerUnits(), QueryGramsPerUnit(),
     /std/unit.c


Last modified: Wed May 8 10:22:02 1996 by Wargon

