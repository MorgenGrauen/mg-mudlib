GetEnemies()
============

FUNKTION
--------
::

	mapping GetEnemies();

DEFINIERT IN
------------
::

	/std/living/combat.c

ARGUMENTE
---------
::

	keine

RUeCKGABEWERT
-------------
::

	Mapping mit bekannten Gegnern als Schluessel und Zeiten als
	Eintraegen.

BESCHREIBUNG
------------
::

	Diese Funktion liefert das interne Mapping zurueck, in dem alle
	Gegner abgespeichert sind. Diese Gegner stellen die Schluessel in
	dem Mapping dar. Als Eintraege sind die Zeiten vermerkt, nach
	welcher Zeit ein Gegner automatisch wieder vergessen werden soll.
	Mehr Informationen dazu sind in der Dokumentation zur aehnlich
	gearteten Funktion QueryEnemies() zu finden, die jedoch zwei Arrays
	getrennte zurueckliefert.
	Achtung: Es wird keine Kopie des Mappings erstellt. Saemtliche
	Veraenderungen in dem Mapping wirken sich unmittelbar auf die
	interne Gegnerliste aus. Die Funktion sollte deshalb nicht genutzt
	werden. Gegner ein- und austragen sollte man nur mittels
	InsertEnemy() und StopHuntFor().

SIEHE AUCH
----------
::

	QueryEnemies(), SetEnemies(), InsertEnemy(), StopHuntFor()


Last modified: Wed May 26 16:47:51 1999 by Patryn

