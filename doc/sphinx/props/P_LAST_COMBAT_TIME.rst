P_LAST_COMBAT_TIME
==================

NAME
----
::

	P_LAST_COMBAT_TIME		"last_combat_time"

DEFINIERT IN
------------
::

	/sys/combat.h

BESCHREIBUNG
------------
::

	In dieser Property wird genau die Zeit festgehalten, zu der ein
	Lebewesen zuletzt einen Angriff abgewehrt oder einen Angriff
	durchgefuehrt hat. Die Property enthaelt diese Information hierbei
	immer in Form eines Integerwertes.
	Dieser Wert koennte z.B. wichtig sein, wenn man wissen moechte, wann
	ein Lebewesen zuletzt gekaempft hat. Es ist beispielsweise nicht
	moeglich, waehrend oder auch unmittelbar nach einem Kampf den Befehl
	'Ende' zu nutzen, da dies zur Flucht missbraucht werden kann. Dafuer
	wird der Wert der Property zuvor ausgewertet.

SIEHE AUCH
----------
::

	P_LAST_DAMTIME, P_LAST_DAMAGE, P_LAST_DAMTYPES, Attack(), Defend(),
	/std/living/combat.c, /std/living/life.c


Last modified: Wed May 26 16:43:00 1999 by Patryn

