P_WEIGHT_PERCENT
================

NAME
----
::

    P_WEIGHT_PERCENT              "weight_percent"              

DEFINIERT IN
------------
::

    /sys/properties.h

BESCHREIBUNG
------------
::

     Diese Property gibt an, wieviel Prozent des Gewichts des Inhaltes
     "nach aussen" wiedergegeben werden.

BEMERKUNGEN
-----------
::

     Alle Werte die < 50% liegen sollten sehr gut begruendet und mit Vor-
     sicht verwendet werden. Hier koennten dann zum Beispiel P_MAX_OBJECTS
     auf einen kleinen Wert begrenzt werden.

     Container die hier einen Wert ueber dem des Postpakets haben, sollten
     auch schwer zu erreichen sein. Auf jeden Fall mit dem Regionsmagier
     besprechen!

BEISPIELE
---------
::

     Um sich zu orientieren kann das Postpaket von Loco als Beispiel hin-
     zugezogen werden (/p/service/loco/obj/parcel).

SIEHE AUCH
----------
::

     P_MAX_OBJECTS, P_MAX_WEIGHT, P_LIGHT_TRANSPARENCY, container

