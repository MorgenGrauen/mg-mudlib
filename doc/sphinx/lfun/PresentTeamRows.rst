PresentTeamRows()
=================

FUNKTION
--------
::

	mixed *PresentTeamRows()

DEFINIERT IN
------------
::

	/std/living/team.c

ARGUMENTE
---------
::

	keine

BESCHREIBUNG
------------
::

	Ergibt die Reihen mit den anwesenden Teammitgliedern.

RUECKGABEWERT
-------------
::

	Ein Array bestehend aus MAX_TEAMROWS Arrays mit den Objekten
        der anwesenden Teammitglieder.

BEMERKUNGEN
-----------
::

	Wenn der Spieler in keinem Team ist, enthaelt das erste Array
	nur den Spieler und die anderen Arrays sind leer.

SIEHE AUCH
----------
::

	teams

