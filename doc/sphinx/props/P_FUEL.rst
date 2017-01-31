P_FUEL
======

NAME
----
::

    P_FUEL                        "fuel"                        

DEFINIERT IN
------------
::

    /sys/properties.h

BESCHREIBUNG
------------
::

     Numerischer Wert fuer die Zeitdauer, die die Lichtquelle noch
     leuchten kann. Standardmaessig ist der Wert auf 20 gesetzt.

     Setzt man die Property auf einen Wert, der groesser ist als der vorige
     Maximalwert, wird dieser auf den neuen Wert erhoeht. Aendert man den
     Brennstoffvorrat der Lichtquelle hingegen mittels AddFuel(), so wird
     der Wert von P_FUEL ueber den Maximalwert hinaus erhoeht.

HINWEIS
-------
::

     Fuer Aenderungen an der Property kann auch die Funktion AddFuel()
     verwendet werden. 

SIEHE AUCH
----------
::

     Basisklassen: /std/lightsource.c
     Properties:   P_LIGHTDESC, P_DO_DESTRUCT, P_LIGHT
     Methoden:     AddFuel(L)

LETZTE AENDERUNG
----------------
::

    22. Dez. 2015, Arathorn

