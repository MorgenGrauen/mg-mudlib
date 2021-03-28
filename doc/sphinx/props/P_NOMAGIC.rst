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

     Wirkt standardmaessig nur bei Gildenspells, die ueber das Spellbook
     gezaubert werden (Auswertung in TryAttackSpell() und UseSpell()),
     d.h. bei Objekten wirkt es nur, wenn diese es explizit beruecksichtigen;
     das gleiche gilt fuer NPC-Spells, die mit AddSpell() erstellt wurden.

BEISPIEL
--------
::

     // in einem Raum keine Spells zulassen
     SetProp(P_NOMAGIC, 100)

HINWEIS
-------
:: 

     Es ist nicht zulaessig, diese Property in irgendeiner Weise in Spielern
     auf Werte ungleich 0 zu setzen. Das gilt auch fuer Querymethoden, die
     eine entsprechende Wirkung haben.


SIEHE AUCH
----------
::

     Aehnlich:     P_MAGIC_RESISTANCE_OFFSET, SpellDefend

2021-03-23 Arathorn
