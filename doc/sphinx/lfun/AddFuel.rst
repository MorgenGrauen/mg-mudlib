AddFuel()
=========

FUNKTION
--------
::

     void AddFuel(int fuel);

DEFINIERT IN
------------
::

     /std/lightsource.c

ARGUMENTE
---------
::

     fuel
          Die zusaetzliche Brenndauer in Sekunden.

BESCHREIBUNG
------------
::

     Die Brenndauer der Lichtquelle wird um fuel Sekunden verlaengert.

RUeCKGABEWERT
-------------
::

     keiner

BEMERKUNGEN
-----------
::

     Es werden keine Checks durchgefuehrt! Wenn man seine Lichtquelle
     nachfuellbar gestalten will, sollte die Nachfuellfunktion vor dem
     AddFuel()-Aufruf nachsehen, wie voll die Lichtquelle noch ist, und fuel
     entsprechend begrenzen.

SIEHE AUCH
----------
::

     /std/lightsource.c


Last modified: Wed May 8 10:16:39 1996 by Wargon

