second_life()
=============

FUNKTION
--------
::

	varargs int second_life(object obj);

DEFINIERT IN
------------
::

	/std/player/life.c

ARGUMENTE
---------
::

	obj
	  Leiche des Lebewesens.

BESCHREIBUNG
------------
::

        Diese Funktion wird im die() des Lebewesens aufgerufen, wenn sicher
        ist, dass es stirbt. Ueblicherweise ist diese Funktion nur im Spieler
        definiert und regelt EP-Verlust und dergleichen. Ein Shadow wuerde
        diese Funktion ueberlagern, um zu verhindern, dass ein Spieler stirbt.

RUeCKGABEWERT
-------------
::

        1, wenn das Lebewesen nicht stirbt, sonst 0

BEMERKUNG
---------
::

        Bei NPCs sollte man direkt die() ueberschreiben, wenn man nicht
        moechte, dass sie sterben.

SIEHE AUCH
----------
::

        die()


Last modified: 2015-Jan-19, Arathorn 

