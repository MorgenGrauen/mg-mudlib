P_MAX_DRINK
===========

NAME
----
::

	P_MAX_DRINK			"max_drink"

DEFINIERT IN
------------
::

	/sys/living/life.h

BESCHREIBUNG
------------
::

	Numerischer Wert fuer die maximale Saettigung eines Lebewesens mit
	Getraenken.

ANMERKUNGEN
-----------
::

	Der Wert von P_MAX_DRINK ist bei den einzelnen Rassen verschieden,
	ebenso wie der fuer P_DRINK_DELAY. Die genauen Werte stehen in den
	Rassen-Shells (/std/shells).

SIEHE AUCH
----------
::

  Properties:
     P_FOOD, P_MAX_FOOD, P_FOOD_DELAY
     P_DRINK, P_DRINK_DELAY
     P_ALCOHOL, P_MAX_ALCOHOL, P_ALCOHOL_DELAY
  Methoden:
     consume(), eat_food(), drink_soft(), drink_alcohol(), defuel_food(),
     defuel_drink()
  Uebersicht Essensobjekte
     wiz/food

Last modified: Tue Dec 18 12:16:10 2001 by Patryn

