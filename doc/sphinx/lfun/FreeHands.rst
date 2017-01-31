FreeHands()
===========

FUNKTION
--------
::

     public varargs int FreeHands(object ob)

DEFINIERT IN
------------
::

     /std/living/combat.c

ARGUMENTE
---------
::

     ob - das Objekt, dessen Handnutzung vorbei ist
        - kann 0 sein, dann wird PO benutzt

RUECKGABEWERT
-------------
::

     0, wenn kein Objekt uebergeben wurde oder kein PO existiert
     1, sonst

BESCHREIBUNG
------------
::

     Befreit die Haende eines Livings von ein bestimmten Objekt.
     Es werden _alle_ Haende wieder freigegeben.

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
     UseHands

1.Feb.2004 Gloinson

