P_NOSELL
========

NAME
----
::

    P_NOSELL                      "nosell"

DEFINIERT IN
------------
::

    /sys/properties.h

BESCHREIBUNG
------------
::

     Wenn diese Property gesetzt ist, kann das Objekt nicht in einem
     Laden verkauft werden.
     Gibt man in der Property einen String an, wird dieser ausgegeben,
     ansonsten erfolgt eine Meldung "Du kannst <NAME> nicht verkaufen!"

     Diese Meldung beinhaltet auch den Namen des in P_NAME einge-
     tragenen Besitzer des Ladens. Ist dies nicht gesetzt, wird per
     default 'Der Haendler' ausgegeben.

BEISPIEL
--------
::

     SetProp(P_NOSELL,"Den Schrott behaeltst Du lieber selber.");

     ==> Apu sagt: Den Schrott behaeltst Du lieber selber.
     ==> Der Haendler sagt: Den Schrott behaeltst Du lieber selber.

     SetProp(P_NOSELL,1);

     ==> Apu sagt: Du kannst <name> nicht verkaufen!
     ==> Der Haendler sagt: Du kannst <name> nicht verkaufen!

SIEHE AUCH
----------
::

     P_NOBUY, P_NODROP, P_KEEPER


03.09.2010, Zesstra

