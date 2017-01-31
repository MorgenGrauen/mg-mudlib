P_REMOVE_FUNC
=============

NAME
----
::

     P_REMOVE_FUNC "remove_func"

DEFINIERT IN
------------
::

     <armour.h>

BESCHREIBUNG
------------
::

     Falls ein Objekt eine RemoveFunc() fuer die Ruestung oder Kleidung 
     definiert, so muss dieses Objekt in dieser Property eingetragen sein.

     Die Auswertung dieser Property erfolgt im Laufe des DoUnwear() in
     der nicht-oeffentlichen Funktionen _check_unwear_restrictions().

BEISPIELE
---------
::

     Siehe das Beispiel zu RemoveFunc()

SIEHE AUCH
----------
::

     /std/armour.c, /std/clothing.c, clothing, armours
     RemoveFunc()


Letzte Aenderung:
15.02.2009, Zesstra

