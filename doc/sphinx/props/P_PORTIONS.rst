P_PORTIONS
==========

NAME
----
::

     P_PORTIONS                    "std_food_portions"

DEFINIERT IN
------------
::

     <sys/food.h>

BESCHREIBUNG
------------
::

     In dieser Property steht die Anzahl der Portionen einer Speise.
     Es duerfen nur Werte > -1 gesetzt werden. Ist diese Property 0,
     wird die Speise als leer bzw. verbraucht angesehen und kann nicht
     konsumiert werden.

     

DEFAULT
-------
::

     Initial ist diese Property auf 1 gesetzt, die Speise ist also mit
     einem Bissen/Schluck verbraucht bzw. leer.

SIEHE AUCH
----------
::

     /std/food.c, wiz/food


Last modified: Thu Oct 28 12:15:00 2010 by Caldra

