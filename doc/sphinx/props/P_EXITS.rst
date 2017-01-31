P_EXITS
=======

NAME
----
::

    P_EXITS                       "exits"                       

DEFINIERT IN
------------
::

    /sys/properties.h

BESCHREIBUNG
------------
::

    Mapping (der Breite 2) aller unmittelbar sichtbaren Ausgaenge mit
    zugehoerigen Nachbarraeumen und der jeweiligen Bewegungsmeldung.

BEMERKUNG
---------
::

    Enthaelt auch SpecialExits, bei der Abfrage mit QueryProp() werden
    diese jedoch ausgefiltert.

    

    Zugriff nur mit den dafuer vorgesehenen Funktionen, bitte nicht aendern.

  

SIEHE AUCH
----------
::

     AddExit(), AddSpecialExit(), GetExits(),
     RemoveExit(), RemoveSpecialExit(),
     GuardExit(),
     H_HOOK_EXIT_USE, P_SPECIAL_EXITS, P_HIDE_EXITS, /std/room/exits.c
     ausgaenge

Letzte Aenderung: 22.12.2016, Bugfix

