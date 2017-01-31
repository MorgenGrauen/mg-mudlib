P_NO_PLAYERS
============

NAME
----
::

    P_NO_PLAYERS                     "no_players"                     

DEFINIERT IN
------------
::

    /sys/rooms.h

BESCHREIBUNG
------------
::

    Wenn in einem Raum die Property P_NO_PLAYERS auf einen Wert != 0 gesetzt
    ist, kann dieser von Spielern auf normalem Wege nicht mehr betreten werden.
    Magier und Testspieler(*) koennen den Raum betreten; Spieler muessen mit
    M_NOCHECK hineinbewegt werden.

    Auf diese Weise koennen Gebiete, die noch nicht offiziell angeschlossen
    sind, vor 'unbeabsichtigtem' Betreten durch Spieler geschuetzt werden.

    Moechte man zu einem schon angeschlossenen Gebiet nachtraeglich eine
    Parallelwelt einbauen, so sollte P_NO_PLAYERS *dringend* in den
    Parallelweltraeumen gesetzt werden, bis die Parallelwelt ausdruecklich
    fuer Spieler freigegeben wird. Andernfalls sind alle Parallelweltraeume,
    zu denen angeschlossene Normalweltraeume mit gleichem Filenamen existieren,
    automatisch durch Spieler erreichbar!

    (*) Ausschliesslich Testspieler, die auf den Namen eines existierenden
    Magiers markiert sind, koennen 'geschuetzte' Raeume betreten.
    Gildentesties werden wie Spieler behandelt.

ANMERKUNG
---------
::

    Im Gegensatz zu Bewegungen von Livings wird bei der Bewegung von Gegen-
    staenden P_NO_PLAYERS nur beim Wechsel der Welt ausgewertet, um z.B. zu
    verhindern, dass Bumerangs in noch nicht angeschlossene Gebiete fliegen.

    Moechte man in seinem eigenen Gebiet mit Bumerangs o.ae. testen, muss
    in diesen P_TESTPLAYER gesetzt sein. Das ist zwar eher ein Missbrauch
    der Property, aber ein Umkompieren vom Werfer war auf Dauer zu teuer. ;-)

SIEHE AUCH
----------
::

    P_PARA, move

