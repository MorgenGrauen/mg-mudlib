P_NOCORPSE
==========

NAME
----
::

	P_NOCORPSE			"nocorpse"

DEFINIERT IN
------------
::

	/sys/properties.h

BESCHREIBUNG
------------
::

	Diese Property ist gesetzt, wenn im Todesfall kein Leichnam
	automatisch erzeugt werden soll.

BEMERKUNGEN
-----------
::

	In diesem Fall wird die Property P_CORPSE ignoriert, mit der man
	ein spezielles Leichenobjekt angeben kann, sofern nicht die
	Standardleiche "/std/corpse.c" verwendet werden soll.
	Da die Moerdermeldungen ueber ebendiese Objekt laufen, werden
	hierbei auch keine ausgegeben.

BEISPIELE
---------
::

	Das Lebewesen soll keine Leiche hinterlassen, weil es zu Staub
	zerfaellt:
	  SetProp(P_DIE_MSG," zerfaellt zu Staub!\n");
	  SetProp(P_NOCORPSE,1)
	Es wurde auch gleich die Sterbemeldung dementsprechend gesetzt.

SIEHE AUCH
----------
::

	P_CORPSE, P_ZAP_MSG, P_DIE_MSG, P_MURDER_MSG, P_KILL_MSG,
	P_NEVERDROP, /std/corpse.c


Last modified: Thu Jun 14 22:26:29 2001 by Patryn

