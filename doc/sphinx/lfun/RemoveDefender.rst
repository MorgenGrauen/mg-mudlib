RemoveDefender()
================

FUNKTION
--------
::

	void RemoveDefender(object friend);

DEFINIERT IN
------------
::

	/std/living/combat.c

ARGUMENTE
---------
::

	friend
	  Objekt (normal Lebewesen), welches zukuenftig nicht mehr ueber
	  Angriffe informiert werden soll und diese auch nicht mehr abwehrt.

BESCHREIBUNG
------------
::

	Ein Lebewesen, welches angegriffen wird, kann andere Objekte ueber
	einen solchen Angriff per InformDefend() informieren oder ihnen
	sogar die Moeglichkeit geben, per DefendOther() direkt in den
	laufenden Angriff einzugreifen (Schaeden abwehren oder umwandeln).
	Im Normalfall handelt es sich hierbei um andere Lebewesen, welche
	als Verteidiger des angegriffenen Lebewesens auftreten: Daher der
	Name der Funktion. Ausserdem besteht die Einschraenkung, dass diese
	Objekte in der gleichen Umgebung sein muessen, wie das zu
	verteidigende Lebewesen.
	Die Objekte sind in Form eines Arrays in der Property P_DEFENDERS
	abgespeichert und koennen dort abgerufen werden. Natuerlich kann
	man alte Objekte direkt dort loeschen, jedoch sollte man die
	hierfuer bereitgestellte Funktionen RemoveDefender() verwenden.
	Zum Hinzufuegen von Eintraegen im Array steht ebenfalls eine
	Funktion bereit: AddDefender().

SIEHE AUCH
----------
::

	AddDefender(), InformDefend(), DefendOther(),
	P_DEFENDERS, /std/living/combat.c


Last modified: Thu Jul 29 18:48:45 1999 by Patryn

