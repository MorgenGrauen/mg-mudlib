ExtraAttack()
=============

FUNKTION
--------
::

	varargs public void ExtraAttack(object enemy, int ignore_previous);

ARGUMENTE
---------
::

	enemy: Der Feind.
	ignore_previous: Ein Flag

BESCHREIBUNG
------------
::

	Der Feind wird der Staerke der Waffe (bzw. der Haende) entsprechend
	stark angegriffen. Hierbei wird Attack() aufgerufen.
	Ist ignore_previous ungleich 0, dann wird die Erstschlagsperre von
	Attack ignoriert. Dieser Angriff ist also auch dann moeglich, wenn
	das Lebewesen eigentlich keinen Schlag mehr in dieser Runde ausfuehren
	duerfte.

RUECKGABEWERT
-------------
::

	Keiner.

BEMERKUNG
---------
::

	Der Einsatz dieser Funktion ist genehmigungspflichtig.
	Weitere Hinweise siehe "man Attack".

SIEHE AUCH
----------
::

	"Attack"
	/std/living/combat.c


Last modified: Sun Nov 21 12:32:20 2004 by Bambi

