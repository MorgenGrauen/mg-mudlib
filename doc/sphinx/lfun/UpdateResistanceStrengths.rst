UpdateResistanceStrengths()
===========================

FUNKTION
--------
::

     public void UpdateResistanceStrengths()

DEFINIERT IN
------------
::

     /std/living/combat.c

BESCHREIBUNG
------------
::

     Die Funktion wird intern mehrmals (durch Defend, AddResistanceModifier
     und RemoveResistanceModifier) aufgerufen. In ihr wird das Resistenz-
     mapping zusammengerechnet und Eintraege geloescht, deren Eintraege
     invalid sind oder deren setzende Objekte geloescht wurden.

RUeCKGABEWERT
-------------
::

     keiner

SIEHE AUCH
----------
::

     Berechnung:	CheckResistance(), Defend()
     Modifikatoren:	AddResistanceModifier, RemoveResistanceModifier(),
			P_RESISTANCE_MODIFIER
     simple Resistenz:	P_RESISTANCE, P_VULNERABILITY
     Hauptmapping:	P_RESISTANCE_STRENGTHS
     anderes:		balance, /std/armour/combat.c, /std/living/combat.c

29.Apr 2002, Gloinson@MG

