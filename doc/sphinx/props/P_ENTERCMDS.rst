P_ENTERCMDS
===========

NAME
----
::

    P_ENTERCMDS                   "leavecmds"                   

DEFINIERT IN
------------
::

    /sys/transport.h

BESCHREIBUNG
------------
::

    Ein Array mit Befehlen, die zum Betreten des Transporters fuehren. 

BEISPIEL
--------
::

    SetProp(P_ENTERCMDS,({ "betrete","betritt" }) );

    Gibt der Spieler nun 'betrete xxx' ein, so sorgt /std/transport.c 
    dafuer, das der Spieler auch auf oder in den Transporter bewegt 
    wird. Vorausgesetzt natuerlich, er ist an einem Haltepunkt ange-
    langt und es ist genuegend Platz dort.

BEMERKUNGEN
-----------
::

    Um /std/transport.c nicht aufzublaehen, werden weitere Argumente wie
    etwa 'betrete das|die|den xxx' _nicht_ unterstuetzt!

    Hier muss der verantwortliche Magier schon eine eigene Loesung finden
    und in seinen Transporter schreiben.

    Sollen Kommandos zum Betreten UND Verlassen eines Transporters ver-
    wendet werden koennen, muessen diese in P_TRAVEL_CMDS gesetzt werden.

SIEHE AUCH
----------
::

    P_LEAVEFAIL, P_ENTERFAIL, P_LEAVECMDS, P_TRAVEL_CMDS, transporter,

LETZTE AENDERUNG
----------------
::

    Don, 24.01.2002, 10:15:07h von Tilly

    

