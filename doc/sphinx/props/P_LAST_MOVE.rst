P_LAST_MOVE
===========

NAME
----
::

	P_LAST_MOVE			"last_move"

DEFINIERT IN
------------
::

	/sys/thing/moving.h

BESCHREIBUNG
------------
::

	In dieser Property wird die Zeit der letzten Bewegung eines
	Lebewesens festgehalten. Selbsterklaerend ist auch der dazugehoerige
	Quellcode innerhalb move() in '/std/living/moving.c':
	  SetProp(P_LAST_MOVE,time());
	Wichtig ist diese Property insbesondere fuer die Unterbindung von
	Rein-Angriff-Raus-Taktiken. Dieser Modus ist standardmaessig in jedem
	NPC aktiviert und kann ueber die Property P_ENABLE_IN_ATTACK_OUT
	ausgeschalten werden.

SIEHE AUCH
----------
::

	move(), time(), P_ENABLE_IN_ATTACK_OUT, /std/living/moving.c


Last modified: Sat Jan 30 12:53:00 1999 by Patryn

