SetCoinsPerUnits()
==================

FUNKTION
--------
::

     void SetCoinsPerUnits(int coins, int units);

DEFINIERT IN
------------
::

     /std/unit.c

ARGUMENTE
---------
::

     coins
          Zahl der Muenzen
     units
          Zahl der Einheiten

BESCHREIBUNG
------------
::

     Es wird festgelegt, wieviel eine bestimmte Menge der Einheiten kostet.
     Eine einzelne Einheit kostet coins/units Muenzen.

RUeCKGABEWERT
-------------
::

     keiner

BEMERKUNGEN
-----------
::

     Bei der Preisberechnung wird abgerundet. Hat man also ein Verhaeltnis
     von einer Muenze pro zwei Einheiten, so ist der Preis einer einzelnen
     Einheit 0!

BEISPIELE
---------
::

     Eine Einheit soll 3.5 Muenzen wert sein:

     /* 7 Muenzen / 2 Einheiten = 3.5 Muenzen / Einheit */
     SetCoinsPerUnits(7,2);

SIEHE AUCH
----------
::

     QueryCoinsPerUnits(), SetGramsPerUnits(), QueryGramsPerUnits(),
     /std/unit.c


Last modified: Wed May 8 10:24:57 1996 by Wargon

