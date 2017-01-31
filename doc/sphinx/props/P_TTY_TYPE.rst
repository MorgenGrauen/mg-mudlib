P_TTY_TYPE
==========

NAME
----
::

    P_TTY_TYPE                                  "tty_type"

DEFINIERT IN
------------
::

    /secure/telnetneg.h

BESCHREIBUNG
------------
::

    In dieser Properties steht der Terminaltyp, den ein Spieler lokal auf
    seinem Rechner verwendet.

    Voraussetzung hierfuer ist allerdings, dass das Telnet des Spielers
    Telnetnegotiations unterstuetzt, ansonsten bleibt diese Property
    leer. Die meisten Telnets/Clients geben ihren Terminaltyp allerdings
    nicht preis.
    Das Setzen der Property aendert den Terminaltyp des Spielers
    natuerlich nicht.

SIEHE AUCH
----------
::

    P_TTY_COLS, P_TTY_ROWS, P_TTY_SHOW

LETZTE AeNDERUNG
----------------
::

    Sat, 06.02.1999, 14:00:00 von Paracelsus

