P_EXTRA_LOOK
============

********************* VERALTETE PROPERTY ******************************
* Diese Property ist veraltet. Bitte benutzt sie NICHT mehr, sondern  *
* stattdessden AddExtraLook().                                        *
***********************************************************************

NAME
----
::

	P_EXTRA_LOOK			"extralook"

DEFINIERT IN
------------
::

	/sys/living/description.h

BESCHREIBUNG
------------
::

	Diese Property enthaelt einen String. Sie wird entweder in Lebewesen
	direkt oder in Objekten gesetzt wird, die im Besitz von Lebewesen
	sein koennen.
	Diese Strings erscheinen dann zusaetzlich in der Langbeschreibung
	des Lebewesens bzw. des Besitzers (wenn das Objekt sich direkt im
	 Lebewesen befindet, jedoch nicht in einem Behaelter im Lebewesen).
	Fuer den Zeilenumbruch muss man selbst sorgen.

BEISPIEL
--------
::

	Ein Spieler hat eine Pfeife im Mund. In dieser Pfeife setzt man z.B.
	in der Funktion zum Pfeife Rauchen folgendes:
	  SetProp(P_EXTRA_LOOK,break_string(
	 this_player()->Name(WER)+" ist ganz umnebelt.",78);

BEMERKUNG
---------
::

  BITTE NICHT MEHR BENUTZEN!

SIEHE AUCH
----------
::

	long(), /std/living/description.c, /std/player/base.c
  AddExtraLook(), RemoveExtraLook()


13.05.2007, Zesstra

