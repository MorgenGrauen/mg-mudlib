P_TELNET_RTTIME
===============

NAME
----
::

    P_TELNET_RTTIME                                  "p_lib_telnet_rttime"

DEFINIERT IN
------------
::

    /secure/telnetneg.h

BESCHREIBUNG
------------
::

    In dieser Properties steht die letzte gemessene 'round-trip' Zeit
    (in Mikrosekunden) einer 'Telnet timing mark' vom MUD zum Client und
    zurueck.

    Voraussetzung hierfuer ist allerdings, dass das Telnet des Spielers
    Telnetnegotiations unterstuetzt und 'telnet keepalive' eingeschaltet
    ist, ansonsten bleibt diese Property 0.
    Die meisten Telnets/Clients antworten zumindest eine Ablehnung auf
    die 'timing marks', so dass trotzdem eine Zeit bestimmt werden kann.

    Die Prop kann nicht gesetzt werden bzw. es hat keinen Effekt.

SIEHE AUCH
----------
::

    P_TTY_COLS, P_TTY_ROWS, P_TTY_SHOW, P_TTY, P_TTY_TYPE

LETZTE AeNDERUNG
----------------
::

    03.02.2013, Zesstra

