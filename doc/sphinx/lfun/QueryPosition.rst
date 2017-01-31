QueryPosition()
===============

FUNKTION
--------
::

     string *QueryPosition();

DEFINIERT IN
------------
::

     /std/transport.c

ARGUMENTE
---------
::

     keine

BESCHREIBUNG
------------
::

     Es wird ein Array von zwei Strings mit Kennzeichnern der letzten (oder
     aktuellen) und der naechsten Station im Fahrplan zurueckgegeben.

     Die Kennzeichner sind dabei die ersten Argumente der entsprechenden
     AddXXX()-Aufrufen, also der Name der Haltestelle bei AddRoute, der Text
     der Meldung bei AddMsg() und der Name der Funktion bei AddFun().

RUeCKGABEWERT
-------------
::

     Array mit zwei Strings. Der erste String gibt den Kennzeichner der
     momentanen Fahrplanstation an, der zweite String den Kennzeichner der
     naechsten Fahrplanstation.

SIEHE AUCH
----------
::

     AddRoute(), AddMsg(), AddFun(), /std/transport.c


Last modified: Wed May 8 10:22:52 1996 by Wargon

