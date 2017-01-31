FindFarEnemyVictim()
====================

FUNKTION
--------
::

	object FindFarEnemyVictim(string wen, object pl, string msg,
	                          int min, int max)

DEFINIERT IN
------------
::

	/std/spellbook.c

ARGUMENTE
---------
::

	wen - id des gewuenschten Gegners, SelectFarEnemy falls n/a.
	pl  - Caster.
	msg - Nachricht falls Gegner nicht anwesend ist.
	min - minimale Kampfreihe
	max - maximale Kampfreihe

BESCHREIBUNG
------------
::

	Findet einen Gegner aus Kampfreihe <min> bis <max>
	z.B. fuer einen Angriffsspruch.

	

RUECKGABEWERT
-------------
::

	Der Auserwaehlte :-)

BEMERKUNGEN
-----------
::

	1. Der Gegner wird auf jeden Fall angegriffen.
	2. Die Reihenangaben werden NICHT mit Skillattributen modifiziert.
	3. Die Angabe der Reihe ist absolut.

SIEHE AUCH
----------
::

	teams, FindEnemyVictim, FindNearEnemyVictim, FindDistantEnemyVictim

