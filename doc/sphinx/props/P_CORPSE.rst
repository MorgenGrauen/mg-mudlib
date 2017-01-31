P_CORPSE
========

NAME
----
::

    P_CORPSE		"corpse"

DEFINIERT IN
------------
::

    /sys/properties.h

BESCHREIBUNG
------------
::

    Hier kann man angeben, welche Art von Leiche beim Tod des NPCs
    hinterlassen wird. Damit die Leiche auf dem Moerder-Kanal senden
    kann, muss das Leichen-Objekt /std/corpse sein oder erben.

BEISPIELE
---------
::

    Die uebliche Standardleiche befindet sich unter "/std/corpse.c",
    welches auch die Defaulteinstellung fuer diese Property darstellt:
      SetProp(P_CORPSE,"/std/corpse");
    Man koennte aber auch einen Zombie entstehen lassen:
      SetProp(P_CORPSE,PATH("zombie"));
    PATH kennzeichnet hierbei den Pfad, unter welchem "zombie.c" zu
    finden ist.

SIEHE AUCH
----------
::

    P_NOCORPSE, P_ZAP_MSG, P_DIE_MSG, P_MURDER_MSG, P_KILL_MSG


Last modified: Mon Apr 07 11:02:06 2003 by Mandragon

