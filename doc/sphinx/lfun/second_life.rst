second_life()
=============

FUNKTION
--------

	varargs int second_life(object obj);

DEFINIERT IN
------------

	/std/player/life.c

ARGUMENTE
---------

	obj
	  Leiche des Lebewesens (sofern es eine hat)

BESCHREIBUNG
------------

  Diese Funktion wird im die() des Lebewesens aufgerufen, wenn sicher
  ist, dass es stirbt. Die Funktion bestimmt dabei, ob ein Lebewesen
  nach dem Tod zerstoert (NPC) oder nur zum Geist wird (Spieler).
  
  Ueblicherweise ist diese Funktion nur im Spieler definiert und regelt
  EP-Verlust und dergleichen. Sie wird aber auch in NPCs gerufen und man
  kann dort z.B. Items clonen oder entsorgen.

  NPC *muessen* 0 zurueckgeben, Spieler geben immer 1 zurueck.

RUeCKGABEWERT
-------------

  0
    wenn das Objekt nach dem Tod zerstoert wird (NPC)
  1
    wenn das Objekt im Tod nicht zerstoert wird (Spieler)


SIEHE AUCH
----------

  die()

Last modified: 17.03.2019, Zesstra

