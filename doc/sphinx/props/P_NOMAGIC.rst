P_NOMAGIC
=========

NAME
----
::

    P_NOMAGIC                     "nomagic"                     

DEFINIERT IN
------------
::

    /sys/properties.h

BESCHREIBUNG
------------
::

     Angabe in Prozent, mit welcher Wahrscheinlichkeit Magie fehlschlaegt.

     

     Fuer einen Raum ist es eine generelle Aussage, wie wahrscheinlich ein
     Spell in ihm fehlschlaegt. Bei NPC zeigt es an, wie wahrscheinlich
     ein auf ihn gesprochener Spell fehlschlaegt.

BEISPIEL
--------
::

     // in einem Raum keine Spells zulassen
     SetProp(P_NOMAGIC, 100)

SIEHE AUCH
----------
::

     Aehnlich:     P_MAGIC_RESISTANCE_OFFSET, SpellDefend

29.Dez 2007 Gloinson

