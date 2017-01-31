P_BAD_MSG
=========

NAME
----
::

     P_BAD_MSG                     "std_food_bad_msg"

DEFINIERT IN
------------
::

     <sys/food.h>

BESCHREIBUNG
------------
::

     Meldung, wenn eine Speise gerade verdirbt.
     Befindet sich die Speise in einem Spieler, geht die Meldung an genau
     diesen, liegt die Speise im Raum, geht die Meldung an alle anwesenden
     Spieler.

     

BEMERKUNGEN
-----------
::

     Diese Meldung wird von replace_personal mit den Argumenten:
     1. Speise
     2. Konsument
     verarbeitet, kann als entsprechende Platzhalter enthalten

     

DEFAULT
-------
::

     "@WER1 verdirbt."

SIEHE AUCH
----------
::

     P_LIFETIME, P_RESET_LIFETIME, P_NO_BAD,
     wiz/food, replace_personal


Last modified: Thu Oct 28 12:15:00 2010 by Caldra

