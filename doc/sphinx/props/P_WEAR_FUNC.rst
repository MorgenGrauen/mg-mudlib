P_WEAR_FUNC
===========

NAME
----
::

     P_WEAR_FUNC "wear_func"

DEFINIERT IN
------------
::

     <armour.h>

BESCHREIBUNG
------------
::

     Falls ein Objekt eine WearFunc() fuer die Ruestung / Kleidung definiert, 
     so muss dieses Objekt in dieser Property eingetragen sein.

     Die Auswertung dieser Property erfolgt in Laufe eines DoWear() in der
     nicht-oeffentlichen Funktion _check_wear_restrictions()..

BEISPIELE
---------
::

     Siehe das Beispiel zu WearFunc()

SIEHE AUCH
----------
::

     armours, clothing, /std/clothing/wear.c, /std/armour/wear.c
     WearFunc(), InformWear()

LETZTE AeNDERUNG
----------------
::

15.02.2009, Zesstra

