SelectEnemy()
=============

FUNKTION
--------
::

	varargs object SelectEnemy(object*here);

DEFINIERT IN
------------
::

	/std/living/combat.c

ARGUMENTE
---------
::

	here
	  Gegner, aus welchen ausgewaehlt werden soll (optional!)

RUeCKGABEWERT
-------------
::

	Ausgewaehlter Gegner, der angegriffen werden soll.

BESCHREIBUNG
------------
::

	Diese Funktion waehlt einen Gegner aus, der angegriffen werden soll.
	Werden keine Gegner im Parameter <here> angegeben, so wird der
	Rueckgabewert der Funktion PresentEnemies() verwandt, welche die
	aktuell anwesenden Gegner im Raum liefert.
	Sind keine Gegner anwesend, so wird 0 zurueckgeliefert.
	Danach wird geschaut, ob Gegner bevorzugt ausgewaehlt werden sollen.
	Dies geschieht mittels der Funktion QueryPreferedEnemy().
	Auch dieser bevorzugte Gegner muss im selben Raum sein! Wenn nicht,
	wird doch irgendein anderer Gegner ausgewaehlt und zurueckgegeben.

SIEHE AUCH
----------
::

	PresentEnemies(), QueryPreferedEnemy(), P_PREFERED_ENEMY,
	InsertEnemy(), StopHuntFor()


Last modified: Thu May 27 15:01:48 1999 by Patryn

