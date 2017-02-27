P_EXTRA_LOOK
============

NAME
----
::

        P_EXTRA_LOOK                        "extralook"

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

        Zu beachten: der Extralook taucht an einem Lebewesen nur auf, wenn
        dieser schon existiert, wenn das Objekt in das Lebewesen bewegt wird
        (oder im Fall von F_QUERY_METHOD-Methoden: diese bereits gesetzt
         ist.)

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

        NUR dann benutzen, wenn ihr auch unabhaengig vom Extralook ein
        Objekt im Spieler benoetigt, ansonsten IMMER
        :doc:`../lfun/AddExtraLook` verwenden.

SIEHE AUCH
----------
::

        :doc:`../lfun/long`, :doc:`AddExtraLook`, :doc:`RemoveExtraLook`
        /std/living/description.c, /std/player/base.c

