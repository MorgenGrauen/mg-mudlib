InList()
========

FUNKTION
--------
::

     int InList(object room, int *potionlist, int *knownlist)

DEFINIERT IN
------------
::

     /secure/potionmaster.c

ARGUMENTE
---------
::

     Implizit: previous_object() - Spielerobjekt, das ZT bekommen soll
     object room                 - Objekt, welches ZT vergeben will
     int* potionlist             - Liste der ZTs des Spielers
     int* knownlist              - Liste der bekannten ZTs des Spielers

BESCHREIBUNG
------------
::

     Prueft, ob 'room' einen ZT vergeben darf und gibt zurueck, ob die
     Nummer dieses ZTs in der 'knownlist' enthalten ist.

RUeCKGABEWERT
-------------
::

     0  Kein aktiver ZT oder nicht in Liste enthalten.
     1  Aktiver ZT und in Liste.

SIEHE AUCH
----------
::

     Sonstiges: zaubertraenke, /room/orakel.c
     Verwandt:  FindPotion(), RemoveKnownPotion(), AddKnownPotion()
     Props:     P_POTIONROOMS, P_KNOWN_POTIONROOMS

6.Feb 2016 Gloinson

