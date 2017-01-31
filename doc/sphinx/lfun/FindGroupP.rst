FindGroupP()
============

FUNKTION
--------
::

	object*FindGroupP(object pl,int who,int pr);

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
	pr
	  Wahrscheinlichkeit, mit der ein Lebewesen ausgesucht werden soll.
	  Hierbei geht vorher noch das Skillattribute SA_EXTENSION ein!

RUeCKGABEWERT
-------------
::

	Array mit gefundenen Lebewesen

BESCHREIBUNG
------------
::

	Ausgesucht werden die Lebewesen genauso wie bei FindGroup(), nur
	dass zum Schluss die einzelnen Lebewesen per Zufall ausgewaehlt
	werden. Es ist also nicht gesichert, dass ueberhaupt ein Lebewesen
	zurueckgeliefert wird, trotzdem welche gefunden wurden.

BEISPIELE
---------
::

	Man moechte im Schnitt 50% der Feinde finden, die man gleichzeitig
	mit einem Spell belegt:
	  enemyList=FindGroupP(caster,FG_ENEMIES,50);
	Dies gilt jedoch nur bei SA_EXTENSION==100, sonst wird mit
	dementsprechend mehr oder weniger Wahrscheinlichkeit zurueckgegeben.
	 (also bei SA_EXTENSION==200 doppelt so viele -> 100%, also alle)
	Das Skillattribute SA_EXTENSION kann auch durch SA_QUALITY
	veraendert worden sein; das sollte beachtet werden.

SIEHE AUCH
----------
::

	FindGroup(), FindGroupP(), P_FRIEND, P_NO_GLOBAL_ATTACK


Last modified: Mon Jan 25 15:04:31 1999 by Patryn

