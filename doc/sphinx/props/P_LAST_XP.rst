P_LAST_XP
=========

NAME
----
::

    P_LAST_XP                        "last_xp"

DEFINIERT IN
------------
::

    /sys/living/life.h

BESCHREIBUNG
------------
::

    Ein Array vom Typ ({ pfad, xp }).

    Der Eintrag "pfad" gibt das Gebiet an, in dem ein Spieler zuletzt XP
    bekommen hat. Dabei wird aus "/d/ebene/magier/room/raum1.c" dann
    "/d/ebene/magier/room".

    Der Wert "xp" zeigt an, wieviele XP der Spieler _in diesem Gebiet_
    gesammelt hat. Sobald er auch nur einen XP in einem anderen Gebiet
    bekommt, zeigt P_LAST_XP nur noch diesen neu erhaltenen XP an.

    Der Anwendungszweck waere z.B. eine Heilstelle, die nur dann Wirkung
    zeigt, wenn der Spieler wirklich in dem betreffenden Gebiet gemetzelt hat
    und nicht nur zum Tanken hergerannt ist oder eine Unterscheidung, ob
    jemand metzelt oder nur uebt (ueber die XP).

SIEHE AUCH
----------
::

     Funktionen:  AddExp()
     Verwandt:    P_NO_XP, P_XP

14.Feb 2007 Gloinson

