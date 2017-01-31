QueryHealInfo()
===============

FUNKTION
--------
::

     string QueryHealInfo();

DEFINIERT IN
------------
::

     /std/corpse.c,

ARGUMENTE
---------
::

     keine

BESCHREIBUNG
------------
::

     QueryHealInfo() liefert einen Standardtext zurueck, der grob Auskunft 
     darueber gibt, welche Auswirkungen das Essen einer Leiche haette. 
     Diese Info kann z.B. von Gilden verwendet werden, die P_HEAL (s.dort)
     einer Leiche nicht selbst auswerten wollen.

RUeCKGABEWERT
-------------
::

     Ein nicht umbrochener String. Fuer Zeilenumbrueche kann derjenige,
     der den Text ausliest, selbst sorgen.

SIEHE AUCH
----------
::

     /std/corpse.c, P_HEAL


Last modified: 31.03.2008, Arathorn

