P_LAST_DAMTYPES
===============

NAME
----
::

	P_LAST_DAMTYPES			"last_damtypes"

DEFINIERT IN
------------
::

	/sys/living/combat.h

BESCHREIBUNG
------------
::

	In dieser Property werden genau die Schadensarten festgehalten,
	welche ein Lebewesen zuletzt abbekommen hat. Die Property enthaelt
	diese Information hierbei immer in Form eines Arrays.
	Auch der Giftschaden durch die Wirkung einer Vergiftung wird hier
	festgehalten.
	Dieser Wert koennte z.B. wichtig sein, wenn man nach dem Tod eines
	Lebewesens feststellen moechte, durch was es umgekommen ist.

SIEHE AUCH
----------
::

	P_LAST_DAMAGE, P_LAST_DAMTIME, Defend(),
	/std/living/combat.c, /std/living/life.c


Last modified: Tue Jan 26 12:34:29 1999 by Patryn

