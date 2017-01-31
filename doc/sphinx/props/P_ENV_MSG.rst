P_ENV_MSG
=========

NAME
----
::

     P_ENV_MSG                     "std_food_env_msg"

DEFINIERT IN
------------
::

     <sys/food.h>

BESCHREIBUNG
------------
::

     Meldung an den Konsumenten, wenn eine Speise konsumiert werden soll,
     die nicht im eigenen Inventory liegt.
     Wenn diese Property leer ist, kann man die Speise dann sogar
     konsumieren!

     

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

     "Vielleicht solltest Du @WEN1 vorher nehmen."

SIEHE AUCH
----------
::

     wiz/food, replace_personal


Last modified: Thu Oct 28 12:15:00 2010 by Caldra

