P_NEWBIE_GUIDE
==============

NAME
----

  P_NEWBIE_GUIDE
    "newbie_guide"

DEFINIERT IN
------------

  /sys/player.h

BESCHREIBUNG
------------

  Enthaelt einen Integer, der beschreibt, ob der Spieler sich als Cicerone
  gemeldet hat.

  - 0 Bedeutet kein Cicerone
  - 1 Bedeutet dauerhaft Cicerone
  - Ein Wert zwischen 60 und 86400 bedeutet, dass der Spieler als Cicerone
    zur Verfuegung stehen will, solange er nicht laenger als n Sekunden idle
    ist.

SIEHE AUCH
----------

  cicerone, cicerones

Letzte Aenderung: 01.10.2019, Bugfix
