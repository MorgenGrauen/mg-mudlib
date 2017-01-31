P_RESISTANCE_MODIFIER
=====================

NAME
----
::

     P_RESISTANCE_MODIFIER             "rstr:mod"

DEFINIERT IN
------------
::

     /sys/living/combat.h

BESCHREIBUNG
------------
::

     Hier werden die Resistenzmodifikatoren in einem Mapping abgespeichert.

     Format:

     (["me":<Aufaddition aller Resistenz/Empfindlichkeitsmodifikationen>;0,
       "<Objektname>#<Schluessel>":<Resistenzmapping>;<Objekreferenz>,
       ...])

BEMERKUNGEN
-----------
::

     Nur ueber AddResistanceModifier(), RemoveResistanceModifier() aendern!

SIEHE AUCH
----------
::

     Modifikatoren:	AddResistanceModifier, RemoveResistanceModifier()
     simple Resistenz:	P_RESISTANCE, P_VULNERABILITY
     Hauptmapping:	P_RESISTANCE_STRENGTHS
     Berechnung:	CheckResistance(), UpdateResistanceStrengths()
     anderes:		balance, /std/armour/combat.c, /std/living/combat.c

29.Apr 2002, Gloinson@MG

