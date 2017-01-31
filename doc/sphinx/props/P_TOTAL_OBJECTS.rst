P_TOTAL_OBJECTS
===============

NAME
----
::

    P_TOTAL_OBJECTS                "total_objects"                

DEFINIERT IN
------------
::

    /sys/container.h

BESCHREIBUNG
------------
::

     Anzahl der Objekte im Container. Diese Property kann man nur abfragen!
     Es werden nur Objekte gezaehlt, deren Methode short() einen
     Wert != 0 zurueckgibt. Insofern koennen Spielern beliebig
     viele unsichtbare Objekte gegeben werden ohne sie zu behindern.

SIEHE AUCH
----------
::

     P_MAX_OBJECTS

26.Jan 2005 Gloinson

