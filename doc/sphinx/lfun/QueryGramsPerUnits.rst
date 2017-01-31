QueryGramsPerUnits()
====================

FUNKTION
--------
::

     int *QueryGramsPerUnits();

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

     Liefert das Gewichtsverhaeltnis fuer die Einheiten zurueck.

RUeCKGABEWERT
-------------
::

     Ein Array von zwei Zahlen. Die erste Zahl ist das Gewicht der in der
     zweiten Zahl angegebenen Einheiten.

BEISPIELE
---------
::

     Steht im Unit-Objekt folgende Gewichtszuweisung:

       SetGramsPerUnits(4,1);

     so liefert QueryGramsPerUnits() als Ergebnis ({4, 1}).

SIEHE AUCH
----------
::

     SetCoinsPerUnits(), QueryCoinsPerUnits() SetGramsPerUnits(),
     /std/unit.c


Last modified: Wed May 8 10:22:39 1996 by Wargon

