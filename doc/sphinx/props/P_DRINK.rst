P_DRINK
=======

NAME
----
::

     P_DRINK                       "drink"

DEFINIERT IN
------------
::

     /sys/living/life.h

BESCHREIBUNG
------------
::

     - Lebewesen
       Numerischer Wert fuer den Saettigungsgrad eines Lebewesens mit
       Getraenken. Der maximale Wert steht in P_MAX_DRINK.

     - Speisen/Kneipen
       Numerischer Wert fuer den Saettigungsgrad einer Portion eines
       Getraenkes. Ist diese Property nicht gesetzt oder zusaetzlich
       P_FOOD gesetzt, kann man diese Speise nicht trinken. Eine
       funktionierende Speise _muss_ entweder P_FOOD oder P_DRINK
       groesser 0 gesetzt haben.

HINWEISE
--------
::
     Die Property nicht direkt manipulieren, sondern drink_soft() 
     verwenden

SIEHE AUCH
----------
::

  Properties:
     P_FOOD, P_MAX_FOOD, P_FOOD_DELAY
     P_MAX_DRINK, P_DRINK_DELAY
     P_ALCOHOL, P_MAX_ALCOHOL, P_ALCOHOL_DELAY
  Methoden:
     consume(), eat_food(), drink_soft(), drink_alcohol(), defuel_food()
  Uebersicht Essensobjekte
     wiz/food

Last modified: Thu Oct 28 12:15:00 2010 by Caldra

