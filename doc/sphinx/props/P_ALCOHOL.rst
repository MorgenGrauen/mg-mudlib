P_ALCOHOL
=========

NAME
----
::

     P_ALCOHOL			"alcohol"

DEFINIERT IN
------------
::

     /sys/living/life.h

BESCHREIBUNG
------------
::

     - Lebewesen
       Numerischer Wert fuer den Grad der Betrunkenheit eines Lebewesens.
       Der maximale Wert steht in P_MAX_ALCOHOL.

     - Speisen/Kneipen
       Numerischer Wert fuer den Grad, den eine Portion der Speise den
       Konsumenten betrunken macht

HINWEISE
--------
::

		 Die Property nicht direkt manipulieren, sondern drink_alcohol()
		 verwenden.

SIEHE AUCH
----------
::

   Properties:
      P_FOOD, P_MAX_FOOD, P_FOOD_DELAY
      P_DRINK, P_MAX_DRINK, P_DRINK_DELAY
      P_MAX_ALCOHOL, P_ALCOHOL_DELAY
   Methoden:
      consume(), eat_food(), drink_soft(), drink_alcohol(), defuel_food(),
      defuel_drink()
   Uebersicht Essensobjekte
      wiz/food

Last modified: Thu Oct 28 12:15:00 2010 by Caldra

