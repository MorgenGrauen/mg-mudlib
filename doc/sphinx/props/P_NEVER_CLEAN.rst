P_NEVER_CLEAN
=============

NAME
----
::

	P_NEVER_CLEAN			" never clean "                   

DEFINIERT IN
------------
::

	/sys/rooms.h

BESCHREIBUNG
------------
::

	Normalerweise wird ein Raum nach 2 Resets zerstoert, wenn er waerend
	dieser Zeit von keinem Lebewesen betreten wurde und wenn
	keine REFRESH_NONE- oder REFRESH_DESTRUCT-Objekte existieren, die
	nicht mehr im Raum vorhanden sind.
	Mit dieser Property kann man den sogenannten Clean-Up unterbinden.

BEISPIEL
--------
::

	Der folgende Raum wird nicht mehr zerstoert, wenn er einmal geladen
	wurde:
	  #include <properties.h>
	  // #include <rooms.h> ... wird schon von properties.h included!
	  inherit "std/room";
	  void create()
	  { ::create();
	    SetProp(P_SHORT,"Ein toller Raum");
	    ...
	    SetProp(P_NEVER_CLEAN,1);
	    ...
	  }
	Man sollte die Anwendung nicht uebertreiben! Wichtig ist diese
	Funktion zum Beispiel fuer Raeume, die gleichzeitig Masterobjekte
	darstellen.

SIEHE AUCH
----------
::

	/std/room.c


Last modified: Wed Feb  3 00:54:32 1999 by Patryn

