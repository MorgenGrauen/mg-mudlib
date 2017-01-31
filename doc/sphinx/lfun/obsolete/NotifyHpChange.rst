NotifyHpChange()
================

********************* OBSOLETE LFUN ***********************************
* Diese Efun bitte nicht mehr benutzen, sondern stattdessen die       *
* Hooks (s. /doc/std/hooks).                                          *
***********************************************************************
NotifyHpChange()

FUNKTION
--------
::

	void NotifyHpChange();

DEFINIERT IN
------------
::

	/std/player/life.c

ARGUMENTE
---------
::

	keine

BESCHREIBUNG
------------
::

	Wenn sich die Lebenspunkte eines Spielers aendern, so werden davon
	auch andere Objekte unterrichtet, sofern diese mittels der Funktion
	AddHpHook() bei eben diesem Spieler angemeldet wurden.
	Fortan wird dann die Funktion NotifyHpChange() in diesen
	angemeldeten Objekten aufgerufen, wenn sich die Property P_HP des
	Spielers aendert. Es werden hierbei keine Argumente an
	NotifyHpChange() uebergeben, die aktuellen Lebenspunkte kann man ja
	auch ohne weiteres ueber die Property P_HP in Erfahrung bringen und
	aeltere Werte muss man sich gesondert merken. Zu beachten ist, dass
	die Property P_HP bei Aufruf der Funktion NotifyHpChange() bereits
	den neuen Wert enthaelt.
	Bei dem Spieler angemeldete Objekte, die von Lebenspunkteaenderungen
	informiert werden sollen, werden automatisch aus der Liste entfernt,
	wenn sie zerstoert wurden. Diese Liste ist in der Property
	P_HP_HOOKS zu finden. Per Hand kann man sie auch explizit mittels
	der Funktion RemoveHpHook() entfernen.
	Stirbt ein Spieler, so wird die Funktion NotifyPlayerDeath()
	aufgerufen und nicht NotifyHpChange()!

RUeCKGABEWERT
-------------
::

	keiner

BEISPIELE
---------
::

	ist in Arbeit

SIEHE AUCH
----------
::

	P_HP, P_HP_HOOKS, AddHpHook(), RemoveHpHook(),
	Defend(), do_damage(), NotifyPlayerDeath()


Last modified: Thu Nov 19 13:54:33 1998 by Patryn

