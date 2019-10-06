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

HINWEISE
--------
::
     
     Die Property nicht direkt manipulieren, sondern eat_food() verwenden.

SIEHE AUCH
----------
::

  Properties:
     P_MAX_FOOD, P_FOOD_DELAY
     P_DRINK, P_MAX_DRINK, P_DRINK_DELAY
     P_ALCOHOL, P_MAX_ALCOHOL, P_ALCOHOL_DELAY
  Methoden:
     consume(), eat_food(), drink_soft(), drink_alcohol(), defuel_food(),
     defuel_drink()
  Uebersicht Essensobjekte
     wiz/food

Last modified: Thu Oct 28 12:15:00 2010 by Caldra

