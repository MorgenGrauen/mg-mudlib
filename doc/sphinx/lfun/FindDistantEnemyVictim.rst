FindDistantEnemyVictim()
========================

FUNKTION
--------
::

	object FindDistantEnemyVictim(string wen, object pl, string msg,
	                              int dist, int dy)

DEFINIERT IN
------------
::

	/std/spellbook.c

ARGUMENTE
---------
::

	wen  - id des gewuenschten Gegners, falls nicht angegeben:
               SelectEnemy(FindDistantGroup(pl,-1,dist,dy,10000)
	pl   - Caster.
	msg  - Nachricht falls Gegner nicht anwesend ist.
	dist - Entfernung
	dy   - 2*erlaubte Abweichung von der Entfernung, default 100

BESCHREIBUNG
------------
::

	Findet einen Gegner in Entfernung dist-dy/2 bis dist+dy/2
	z.B. fuer einen Angriffsspruch.

	

RUECKGABEWERT
-------------
::

	Der Auserwaehlte :-)

BEMERKUNGEN
-----------
::

	1. Der Gegner wird auf jeden Fall angegriffen.
	2. dist wird mit SA_RANGE modifiziert,
	   dy wird mit SA_EXTENSION modifiziert.
	3. Die Entfernung ist relativ zum Spieler.

SIEHE AUCH
----------
::

	teams, FindEnemyVictim, FindNearEnemyVictim, FindFarEnemyVictim

