P_INDOORS
=========

NAME
----
::

     P_INDOORS                     "indoors"

DEFINIERT IN
------------
::

     /sys/room/description.h

BESCHREIBUNG
------------
::

     Gesetzt, wenn von dem Raum aus der Himmel nicht sichtbar ist.

     Objekte oder Gilden werten diese Property teilweise aus, fuer
     Innenraeume sollte sie also sinnvollerweise gesetzt werden.

     Licht wird _nicht_ durch P_INDOORS beeinflusst, muss also
     selbst angepasst werden.

BEISPIEL
--------
::

     // Ein dunkler Innenraum:
     SetProp(P_INDOORS, 1);            // ein Innenraum liegt innen :)
     SetProp(P_LIGHT, 0);              // Licht auf 0

     // Ein richtig heller Aussenraum:
     SetProp(P_INDOORS, 0);
     SetProp(P_LIGHT, 2);

     // Ein heller Aussenraum mit Mondlicht (gut fuer Delfen)
     SetProp(P_INDOORS, 0);
     SetProp(P_LIGHT, 1);
     SetProp(P_LIGHT_TYPE, LT_STARS|LT_MOON);

SIEHE AUCH
----------
::

     P_LIGHT, P_LIGHT_ABSORPTION, P_LIGHT_TYPE

25.Aug 2008 Gloinson

