P_NODRINK_MSG
=============

NAME
----
::

     P_NODRINK_MSG                 "std_food_nodrink_msg"

DEFINIERT IN
------------
::

     <sys/food.h>

BESCHREIBUNG
------------
::

     Meldung an den Konsumenten, wenn versucht wird, ein Nicht-Getraenk
     zu trinken. Sobald eine Speise einen Wert in P_FOOD setzt, gilt es als
     Nicht-Getraenk.

     

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

     "@WEN1 kann man nicht trinken!"

SIEHE AUCH
----------
::

     P_FOOD, P_DRINK, wiz/food, replace_personal


Last modified: Thu Oct 28 12:15:00 2010 by Caldra

