P_TRAVEL_INFO
=============

NAME
----
::

    P_TRAVEL_INFO                 "travel_info"

DEFINIERT IN
------------
::

    /sys/transport.h

BESCHREIBUNG
------------
::

    Array mit Informationen zur vom Spieler gewuenschten Reiseroute.

    [0]        Der Raum (object), in dem die Reiseroute momentan 
               'aktiv' ist. Nur hier wird sie beruecksichtigt.

    [1]        Das gewuenschte Transportmittel (object) falls 
               gewaehlt. Ansonsten 0.

    [2]        Der gewuenschte Zielort (string) oder 0 (ziellos).

    [3]        Der gewuenschte Zielort als Richtung (string), falls
               gewaehlt (z.B. 'zur Feuerinsel'). Sonst 0. Wird aus
               P_HARBOUR des Zielraumes ausgelesen.

BEMERKUNGEN
-----------
::

    Diese Property wird von /std/transport.c sowie std/player/travel.c
    verwendet, und sollte NICHT von anderen Objekten oder per Hand 
    veraendert werden!

SIEHE AUCH
----------
::

    /std/transport.c, /std/player/travel.c, reise

LETZTER AENDERUNG
-----------------
::

    Don, 24.01.2002, 10:15:07h von Tilly

