QueryPreferedEnemy()
====================

FUNKTION
--------
::

	object QueryPreferedEnemy();

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

	bevorzugter Gegner

BESCHREIBUNG
------------
::

	Diese Funktion liefert unter folgenden Bedingungen zufaellig eines
	der Lebewesen als bevorzugten Gegner zurueck, welche in der
	Property P_PREFERED_ENEMY in einem Array eingetragen sind:
	  (1) Der erste Eintrag des erwaehnten Propertyarrays enthaelt
	      einen Wert zwischen 0 und 100, der die Wahrscheinlichkeit
	      dafuer angibt, dass ein Lebewesen als Gegner bevorzugt werden
	      soll. Es wird also nicht immer bevorzugt, wenn dort ein Wert
	      kleiner 100 steht! In diesem Fall wird eine 0 zurueckgegeben.
	  (2) Das per Zufall aus den Arrayelementen ab Element 2 gewaehlte
	      Lebewesen muss auch wirklich existieren. Ist dies nicht der
	      Fall, wird das nunmehr leere Element aus dem Array entfernt
	      und eine 0 zurueckgeliefert.
	  (3) Das Lebewesen muss derzeit auch wirklich Feind sein! Ist dies
	      nicht der Fall, wird eine 0 zurueckgegeben.
	Will man eine andere Bevorzugung von Gegnern erreichen,
	ueberschreibt man am besten diese Funktion.

SIEHE AUCH
----------
::

	SelectEnemy(), IsEnemy(), P_PREFERED_ENEMY


Last modified: Wed May 26 16:47:51 1999 by Patryn

