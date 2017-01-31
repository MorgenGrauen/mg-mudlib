P_LAST_DAMTIME
==============

NAME
----
::

	P_LAST_DAMTIME			"last_damtime"

DEFINIERT IN
------------
::

	/sys/living/combat.h

BESCHREIBUNG
------------
::

	In dieser Property wird genau die Zeit festgehalten, zu der ein
	Lebewesen zuletzt einen Schaden abbekommen hat. Die Property
	enthaelt diese Information hierbei immer in Form eines
	Integerwertes.
	Auch der Zeitpunkt der Einwirkung von Giftschaden durch die Wirkung
	einer Vergiftung wird hier festgehalten.
	Dieser Wert koennte z.B. wichtig sein, wenn man wissen moechte, wann
	ein Lebewesen zuletzt verletzt wurde.

SIEHE AUCH
----------
::

	P_LAST_COMBAT_TIME, P_LAST_DAMAGE, P_LAST_DAMTYPES, Defend(),
	/std/living/combat.c, /std/living/life.c


Last modified: Wed May 26 16:44:38 1999 by Patryn

