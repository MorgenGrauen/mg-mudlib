P_DEFEND_FUNC
=============

NAME
----
::

     P_DEFEND_FUNC "defend_func"

DEFINIERT IN
------------
::

     <armour.h>

BESCHREIBUNG
------------
::

     Falls ein Objekt eine DefendFunc() fuer die Ruestung definiert, so muss
     dieses Objekt in dieser Property eingetragen sein.

     Die Auswertung dieser Property erfolgt in QueryDefend().

BEMERKUNGEN
-----------
::

     1. Es funktioniert _nicht_ hier eine Closure reinzuschreiben.
     2. Diese Prop laesst sich _nicht_ sinnvoll mit Set() setzen, also z.B.
        keine Query-Methoden hier reinzuschreiben.
     3. Definieren von _set_defend_func() oder Set-Methoden via Set()
        funktioniert auch nicht, zumindest nicht mit dem gewuenschten
        Ergebnis. ;-)
     4. Bei Verwendung bitte Balance-Richtlinien beachten!

BEISPIELE
---------
::

     Siehe das Beispiel zu DefendFunc()

SIEHE AUCH
----------
::

     /std/armour.c, DefendFunc(), balance, grenzwerte


Last modified: Sat May 18 15:26:17 1996 by Wargon

