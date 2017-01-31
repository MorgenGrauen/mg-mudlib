P_NEVERDROP
===========

NAME
----
::

	P_NEVERDROP			"neverdrop"

DEFINIERT IN
------------
::

	/sys/thing/moving.h

BESCHREIBUNG
------------
::

	Objekte, welche diese Property gesetzt haben, werden beim Tod eines
	Lebewesens nicht automatisch in die Leiche oder in den umgebenden
	Raum (z.B. bei bei gesetztem P_NOCORPSE) transportiert.

BEMERKUNGEN
-----------
::

	Soll das Objekt vom Lebewesen nicht weggelegt werden koennen, so ist
	die Property P_NODROP zu verwenden.
	Beide Properties zusammen stellen sicher, dass ein Objekt nicht
	weitergegeben werden kann.

BEISPIELE
---------
::

	Eine dauerhafte Belohnung, die auch beim Tod des Spielers bei ihm
	verbleiben soll, setzt das so:
	  SetProp(P_NEVERDROP,1);
	Sollen auch Reboots ueberstanden werden, ist zusaetzlich
	P_AUTOLOADOBJ zu setzen.

SIEHE AUCH
----------
::

	P_NODROP, P_NOGET, P_NOCORPSE, P_AUTOLOADOBJ, /std/living/life.c


Last modified: Thu Jun 14 22:26:29 2001 by Patryn

