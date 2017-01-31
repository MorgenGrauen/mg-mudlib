InformRowChange()
=================

FUNKTION
--------
::

	varargs void InformRowChange(int from, int to, object caster);

DEFINIERT IN
------------
::

	/std/living/team.c

ARGUMENTE
---------
::

	from   - Reihe, in die der Spieler vorher war.
	to     - Reihe in der der Spieler jetzt ist.
	caster - Der Spieler, falls die Funktion in der Gilde aufgerufen wird.

        

BESCHREIBUNG
------------
::

	Diese Funktion wird im Spieler und in seiner Gilde aufgerufen,
	falls sich die aktuelle Kampfreihe in der Teamaufstellung
	aendert.

RUECKGABEWERT
-------------
::

	Keiner.

SIEHE AUCH
----------
::

	teams

