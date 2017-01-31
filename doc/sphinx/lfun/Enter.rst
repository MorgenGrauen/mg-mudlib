Enter()
=======

FUNKTION
--------
::

     int Enter();

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

     Wenn sich der Spieler noch nicht auf dem Transporter befindet, und der
     Transporter momentan an einer Haltestelle liegt, betritt der Spieler
     den Transporter.

RUeCKGABEWERT
-------------
::

     Null, wenn der Spieler den Transporter nicht betreten konnte, sonst
     ungleich Null.

BEMERKUNGEN
-----------
::

     Es werden keine Tests durchgefuehrt, ob der Transporter ueberhaupt
     angesprochen wurde! Das muss man selber machen.

BEISPIELE
---------
::

     int myEnter(string str)
     {
       if (str && id(str))
         return Enter();

       notify_fail("Was willst Du betreten?\n");
       return 0;
     }

SIEHE AUCH
----------
::

     Leave(), /std/transport.c


Last modified: Wed May 8 10:18:42 1996 by Wargon

