AddExp()
========

FUNKTION
--------
::

     int AddExp(int e)

DEFINIERT IN
------------
::

     /std/living/life.c

ARGUMENTE
---------
::

     int e - Anzahl der hinzuzufuegenden (abzuziehenden) XP

BESCHREIBUNG
------------
::

     Dem Living werden e XP auf seine bisherigen P_XP addiert.

     Falls es sich um einen Spieler mit P_KILLS>0 handelt und
     e positiv ist, bekommt der Spieler keine XP gutgeschrieben.

     P_LAST_XP wird aktualisiert.

BEMERKUNG
---------
::

     - positive und negative Werte sind moeglich
     - P_XP wird nicht <0 gesetzt.

RUECKGABEWERT
-------------
::

     int - neuer XP-Wert

SIEHE AUCH
----------
::

     Funktionen:  do_damage(), DistributeExp()
     Properties:  P_XP, P_LAST_XP
     Sonstiges:   P_NO_XP, P_NO_SCORE
                  create_default_npc()

14.Feb 2007 Gloinson

