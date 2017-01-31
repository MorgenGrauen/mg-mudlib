UseHands()
==========

FUNKTION
--------
::

     public varargs int UseHands(object ob, int num)

DEFINIERT IN
------------
::

     /std/living/combat.c

ARGUMENTE
---------
::

     ob  - das Objekt, das die Haende belegen soll
     num - die Anzahl der zu belegenden Haende    

RUECKGABEWERT
-------------
::

     1, fuer Erfolg
     0, sonst     

BESCHREIBUNG
------------
::

     Belegt, wenn moeglich Haende eines Livings durch ein bestimmtes
     Objekt. Wenn die Anzahl der freien Haende (P_MAX_HANDS-P_USED_HANDS)
     kleiner ist als "num", dann schlaegt diese Belegung fehl.

BEISPIELE
---------
::

     > halte seil fest
     ...
     this_player()->UseHands(this_object(),2);
     ...

     > lasse seil los
     ...
     this_player()->FreeHands(this_object());
     ...

SIEHE AUCH
----------
::

     P_HANDS, P_HANDS_USED_BY
     P_MAX_HANDS, P_USED_HANDS, P_FREE_HANDS
     FreeHands

1.Feb.2004 Gloinson

