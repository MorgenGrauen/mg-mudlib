FindGroupN()
============

FUNKTION
--------
::

	object*FindGroupN(object pl,int who,int n);

DEFINIERT IN
------------
::

	/std/spellbook.c

ARGUMENTE
---------
::

	pl
	  Lebewesen, von welchem die Freunde oder Feinde in der Umgebung
	  gefunden werden sollen.
	who
	  Flag, welches anzeigt, ob Freunde oder Feinde gefunden werden
	  sollen (Konstanten definiert in '/sys/new_skills.h'):
	    FG_ENEMIES - (Wert -1) Feinde sollen gefunden werden
	    FG_FRIENDS - (Wert  1) Freunde sollen gefunden werden
	    FG_ALL     - (Wert  0) alle Lebewesen sollen gefunden werden
	n
	  Anzahl der Lebewesen, die zurueckgegeben werden sollen.
	  Hierbei geht vorher noch das Skillattribute SA_EXTENSION ein!
	  Es wird mindestens 1 Lebewesen zurueckgeliefert (sofern gefunden).

RUeCKGABEWERT
-------------
::

	Array mit gefundenen Lebewesen

BESCHREIBUNG
------------
::

	Ausgesucht werden die Lebewesen genauso wie bei FindGroup(), nur
	dass zum Schluss die Anzahl noch begrenzt wird.

BEISPIELE
---------
::

	Man moechte maximal 5 Feinde finden, die man gleichzeitig mit einem
	Spell belegen kann:
	  enemyList=FindGroupN(caster,FG_ENEMIES,5);
	Dies gilt jedoch nur bei SA_EXTENSION==100, sonst wird
	dementsprechend mehr oder weniger zurueckgegeben.
	 (also bei SA_EXTENSION==200 doppelt so viele -> 10 Lebewesen)
	Das Skillattribute SA_EXTENSION kann auch durch SA_QUALITY
	veraendert worden sein; das sollte beachtet werden.

SIEHE AUCH
----------
::

	FindGroup(), FindGroupP(), P_FRIEND, P_NO_GLOBAL_ATTACK


Last modified: Mon Jan 25 15:04:31 1999 by Patryn

