P_FOOD
======

NAME
----
::

     P_FOOD                        "food"

DEFINIERT IN
------------
::

     /sys/living/life.h

BESCHREIBUNG
------------
::

     - Lebewesen
       Numerischer Wert fuer den Saettigungsgrad eines Lebewesens.
       Der maximale Wert steht in P_MAX_FOOD.

     - Speisen/Kneipen
       Numerischer Wert fuer den Saettigungsgrad einer Portion einer
       Speise. Ist diese Property nicht gesetzt, kann man diese
       Speise nicht essen. Eine funktionierende Speise _muss_ entweder
       P_FOOD oder P_DRINK groesser 0 gesetzt haben.

SIEHE AUCH
----------
::

     P_MAX_FOOD, P_FOOD_DELAY, consume,
     P_DRINK, P_ALCOHOL, wiz/food


Last modified: Thu Oct 28 12:15:00 2010 by Caldra

