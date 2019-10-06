P_MAX_ALCOHOL
=============

NAME
----
::

	P_MAX_ALCOHOL			"max_alcohol"

DEFINIERT IN
------------
::

	/sys/living/life.h

BESCHREIBUNG
------------
::

	Numerischer Wert fuer den maximalen Grad der Betrunkenheit eines
	Lebewesens.

ANMERKUNGEN
-----------
::

	Der Wert von P_MAX_ALCOHOL ist bei den einzelnen Rassen verschieden,
	ebenso wie der fuer P_ALCOHOL_DELAY. Die genauen Werte stehen in den
	Rassen-Shells (/std/shells).

SIEHE AUCH
----------
::

   Properties:
      P_FOOD, P_MAX_FOOD, P_FOOD_DELAY
      P_DRINK, P_MAX_DRINK, P_DRINK_DELAY
      P_ALCOHOL, P_ALCOHOL_DELAY
   Methoden:
      consume(), eat_food(), drink_soft(), drink_alcohol(), defuel_food(),
      defuel_drink()
   Uebersicht Essensobjekte
      wiz/food

Last modified: Tue Dec 18 12:16:10 2001 by Patryn

