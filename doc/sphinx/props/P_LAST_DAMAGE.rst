P_LAST_DAMAGE
=============

NAME
----
::

	P_LAST_DAMAGE			"last_damage"

DEFINIERT IN
------------
::

	/sys/living/combat.h

BESCHREIBUNG
------------
::

	In dieser Property wird genau die Schadensstaerke festgehalten,
	welche ein Lebewesen zuletzt abbekommen hat. Die Property enthaelt
	diese Information hierbei immer in Form eines Integerwertes.
	Auch die Staerke des Giftschadens durch die Wirkung einer Vergiftung
	wird hier festgehalten.
	Dieser Wert koennte z.B. wichtig sein, wenn man wissen moechte,
	welche Schadensstaerke nach Einwirkung von Defendern, Ruestung,
	Hooks und Skills uebriggeblieben ist.

SIEHE AUCH
----------
::

	P_LAST_DAMTIME, P_LAST_DAMTYPES, Defend(),
	/std/living/combat.c, /std/living/life.c


Last modified: Tue Jan 26 12:34:29 1999 by Patryn

