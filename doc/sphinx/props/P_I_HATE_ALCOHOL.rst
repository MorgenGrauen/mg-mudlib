P_I_HATE_ALCOHOL
================

NAME
----
::

    P_I_HATE_ALCOHOL                        "i_hate_alcohol"

DEFINIERT IN
------------
::

    /sys/inpc/boozing.h

BESCHREIBUNG
------------
::

    Numerischer Wert, der die Obergrenze an P_ALCOHOL in einem Monster
    definiert, welcher beim eigenstaendigen Tanken beruecksichtigt wird.

BEMERKUNG
---------
::

    Geht der Npc (und nur fuer solche ist diese Prop gedacht) eigen-
    staendig tanken, werden vor dem Bestellen die Getraenke und Speisen
    auf ihren Alkoholgehalt geprueft und mit dem aktuellen Wert von
    P_ALCOHOL im Verhaeltnis zu P_I_HATE_ALCOHOL verglichen. Laege der
    Wert fuer P_ALCOHOL dann ueber dem von P_I_HATE_ALCOHOL, wird dieses
    Getraenk (diese Speise) nicht bestellt.

SIEHE AUCH
----------
::

     P_ALCOHOL, P_MAX_ALCOHOL


Last modified: Sam Apr 14 12:35:00 2001 by Tilly

