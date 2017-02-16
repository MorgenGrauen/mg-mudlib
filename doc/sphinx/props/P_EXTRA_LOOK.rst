P_EXTRA_LOOK
============

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
	direkt oder in Objekten gesetzt, die im Besitz von Lebewesen
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

  NUR dann benutzen, wenn ihr auch unabhaengig vom Extralook ein Objekt im
  Spieler benoetigt, ansonsten IMMER AddExtraLook() verwenden.

SIEHE AUCH
----------
::

	long(), /std/living/description.c, /std/player/base.c
  AddExtraLook(), RemoveExtraLook()


16.02.2017, Bugfix

