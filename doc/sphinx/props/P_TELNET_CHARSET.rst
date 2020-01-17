P_TELNET_CHARSET
================

NAME
----

    P_TELNET_CHARSET                                 "p_lib_telnet_charset"

DEFINIERT IN
------------

    /secure/telnetneg.h

BESCHREIBUNG
------------

    In dieser Properties steht der Zeichensatz, den der Spieler manuell
    konfiguriert hat. Das Spielerobjekt wird so konfiguriert, dass der Driver
    - falls notwendig - alle Eingaben des Spielers von diesem Zeichensatz nach
    UTF-8 und alle Ausgaben an den Spieler von UTF-8 in diesen Zeichensatz
    konvertiert.

    Hierbei wird der manuell konfigurierte Zeichensatz allerdings nur dann
    benutzt, wenn der Client des Spielers nicht mittels der Telnet-Option
    CHARSET etwas anderes aushandelt!

    Wird in dieser Property keine Einstellung vorgenommen und fuehrt der
    Client keine Aushandlung durch, ist der Default im Morgengrauen
    "ASCII//TRANSLIT".

SIEHE AUCH
----------

    :doc:`P_TTY_COLS`, :doc:`P_TTY_ROWS`, :doc:`P_TTY_SHOW`, :doc:`P_TTY`,
    :doc:`P_TTY_TYPE`, :doc:`P_TELNET_RTTIME`

LETZTE AeNDERUNG
----------------

    16.01.2020, Zesstra

