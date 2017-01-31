AddResistanceModifier()
=======================

FUNKTION
--------
::

     varargs int AddResistanceModifier(mapping mod, string add)

DEFINIERT IN
------------
::

     /std/living/combat.c

ARGUMENTE
---------
::

     mapping mod:
      Mapping mit Schadensarten und ihrem Resistenzmodifikator (der im Bereich
      von -1.0 bis +x liegen kann), z.B. ([DT_FIRE:-1.0]) (Totalresistenz).
     string add:
      Ein Identifikator fuer _diesen_ Eintrag des setzenden Objektes.

BESCHREIBUNG
------------
::

     Es werden Resistenzen in dem Objekt gesetzt, die solange bestehen, wie
     das setzende Objekt existiert, oder nicht RemoveResistanceModifier
     (mit eventuellem Schluessel add) aufgerufen wird. Zusaetzliche Resistenzen
     werden eingerechnet.

BEMERKUNGEN
-----------
::

     Fuer Ruestungen kann und sollte man P_RESISTANCE_STRENGTHS verwenden.

BEISPIELE
---------
::

     // Oel mit vervierfachtem Feuerschaden
     int add_action() {
      ...
      write(break_string("Du schuettest das Oel ueber "+
			 npc->name(WEN)+".",78));
      ...
      npc->AddResistanceModifier(([DT_FIRE:3.0]), "oel");
      SetProp(P_INVIS,1);
      SetProp(P_EXTRA_LOOK, "Ueberall tropft Oel herunter.\n");
      move(npc,M_NOCHECK);
      ...
     }

RUeCKGABEWERT
-------------
::

     1 fuer Erfolg

SIEHE AUCH
----------
::

     Modifikatoren:	RemoveResistanceModifier(), P_RESISTANCE_MODIFIER
     simple Resistenz:	P_RESISTANCE, P_VULNERABILITY
     Hauptmapping:	P_RESISTANCE_STRENGTHS
     Berechnung:	CheckResistance(), UpdateResistanceStrengths()
     anderes:		balance, /std/armour/combat.c, /std/living/combat.c

29.Apr 2002, Gloinson@MG

