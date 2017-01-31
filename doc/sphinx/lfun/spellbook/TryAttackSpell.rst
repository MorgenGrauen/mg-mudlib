TryAttackSpell()
================

** gilden-doku
 o TryAttackSpell(opfer,schaden,typen,is_spell,caster,info)
   Versucht den Angriffs-Spruch auf den Gegner anzuwenden. Die
   mittleren 4 Werte sind die, die auch bei Defend uebergeben werden.
   Dabei wird die Abwehrfaehigkeit des Gegners gegen Magie und das
   Skill-Attribut SA_DAMAGE automatisch beruecksichtigt. 

FUNKTION
--------
::

int TryAttackSpell(object victim, int damage, mixed dtypes,
                   mixed is_spell, object caster, mapping sinfo)

ARGUMENTE
---------
::

        victim   : Das arme Opfer.
        damage   : Der Schaden.
        dtypes   : Die Schadensarten.
	      is_spell : Ist es ein Spell? Werden noch Spezielle Parameter 
	           uebergeben (als mapping) ?
        caster   : Derjenige, der den Spruch spricht.
        sinfo    : Mapping mit allen moeglichen Informationen zum Spell

BESCHREIBUNG
------------
::

	Diese Funktion wird vom Spellbook aufgerufen, wenn der Spieler
	einen Angriffsspell gemacht hat und damit Schaden anrichten will.

RUECKGABEWERT
-------------
::

	Der Wert, der vom Defend() des Gegners zurueckgeliefert wird.

BEMERKUNGEN
-----------
::

	Zu erst wird ueberprueft, ob das Ziel ueberhaupt angreifbar ist. Dies
	verhindert das ueben von Spells an unangreifbaren NPCs.
	Als naechstes wird die Faehigkeit, Spells abzuwehren ueberprueft.
	Falls beide Abfragen ok sind, wird Defend aufgerufen.


Siehe auch:

TryDefaultAttackSpell (to be written)

07.10.2007, Zesstra

