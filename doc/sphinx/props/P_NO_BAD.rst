P_NO_BAD
========

NAME
----
::

     P_NO_BAD                      "std_food_no_bad"

DEFINIERT IN
------------
::

     <sys/food.h>

BESCHREIBUNG
------------
::

     Flag, ob die Speise ewig haltbar ist.
     0: Speise verdirbt nach der Anzahl Sekunden, die in P_LIFETIME
        angegeben ist bzw. nach einem Reset
     1: Speise verdirbt nicht

     

     ACHTUNG: Diese Property darf nur in Absprache mit der Balance
              geaendert werden.

     

DEFAULT
-------
::

     Initial ist diese Property auf 0 gesetzt.

SIEHE AUCH
----------
::

     /std/food.c, wiz/food


Last modified: Thu Oct 28 12:15:00 2010 by Caldra

