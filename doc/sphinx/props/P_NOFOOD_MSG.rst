P_NOFOOD_MSG
============

NAME
----
::

     P_NOFOOD_MSG                  "std_food_nofood_msg"

DEFINIERT IN
------------
::

     <sys/food.h>

BESCHREIBUNG
------------
::

     Meldung an den Konsumenten, wenn versucht wird, ein Getraenk zu essen.
     Sobald eine Speise keinen Wert in P_FOOD und einen Wert in P_DRINK
     setzt, gilt es als Getraenk.

     

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

     "@WEN1 kann man nicht essen!"

SIEHE AUCH
----------
::

     P_FOOD, P_DRINK, wiz/food, replace_personal


Last modified: Thu Oct 28 12:15:00 2010 by Caldra

