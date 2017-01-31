Gildenproperties()
==================

Name			Beschreibung

P_GUILD_SKILLS		Property fuer Skills in der Gilde
P_GUILD_RESTRICTIONS 	Restriktionen fuer den Eintritt in die Gilde
P_GUILD_DEFAULT_SPELLBOOK  
P_GUILD_MALE_TITLES 	Maennliche Gildentitel
P_GUILD_FEMALE_TITLES	Weibliche Gildentitel
P_GUILD_LEVELS		Gildenstufen
P_SB_SPELLS 		Property fuer Spells in Spellbook
P_GLOBAL_SKILLPROPS	Properties der Gilde UND des Spellbooks

Properties des Spielers
P_GUILD			String mit dem Namen der Gilde
P_GUILD_LEVEL		Gildenstufe
P_GUILD_TITLE		Gildentitel
P_GUILD_RATING		Rating in der Gilde
P_NEWSKILLS		mapping mit Skills und Spells
P_NEXT_SPELL_TIME	Zeit bis der naechste Spell gesprochen werden kann
P_TMP_ATTACK_HOOK	Angriffs-Ersatzfunktion
P_TMP_ATTACK_MOD	Angriffs-Modifizierung
P_TMP_DEFEND_HOOK	Abwehr-Ersatzfunktion
P_TMP_DIE_HOOK		"Die"-Ersatzfunktion
P_TMP_MOVE_HOOK		Bewegungs-Ersatzfunktion
P_DEFENDERS		Verteidiger
P_PREPARED_SPELL	Vorbereiteter Spell
P_DEFAULT_GUILD		Standardgilde
P_MAGIC_RESISTANCE_OFFSET Offset fuer Magie-Resistenzen

Standard-Skills
SK_FIGHT		Kampfskill(global)
SK_SWORDFIGHTING	Schwertkampf
SK_WEAPONLESS		Waffenloser Kampf
SK_TWOHANDED       	Zweihandkampf
SK_CASTING         	Zauber-Skill
SK_MAGIC_ATTACK    	Magischer Angriff
SK_MAGIC_DEFENSE   	Magische Abwehr
SK_NIGHTVISION     	Nachtsicht
SK_BOOZE           	Sauf-Skill
SK_INFORM_DEFEND   	
SK_DEFEND_OTHER		
SK_CARRY		Trage-Skill
SK_SPELL_DEFEND		Spruch-Abwehr

Skill-Attribute
P_SKILL_ATTRIBUTES	Property fuer Skill-Attribute
P_SKILL_ATTRIBUTE_OFFSETS   Property fuer Offsets der Skill-Attribute
SA_QUALITY		Allgemeine Qualitaet
SA_DAMAGE		Schaden
SA_SPEED		Geschwindigkeit
SA_DURATION		Dauer
SA_EXTENSION		Ausdehnung
SA_RANGE		Reichweit
SA_ENEMY_SAVE identisch zu SA_SPELL_PENETRATION (OBSOLET!)
SA_SPELL_PENETRATION Chance des _Casters_, einen Spell durch ein
                      P_NOMAGIC durchzukriegen.

Skill-Info
FACTOR(x)		Faktor
OFFSET(x)		Offset
SI_SKILLFUNC		Funktion, die aufgerufen wird
SI_CLOSURE		Intern (Geschwindigkeitsoptimierung)
SI_SPELLBOOK		Spellbook, in dem der Spell steht
SI_SPELLCOST		Kosten des Spells
SI_TIME_MSG		Die Meldung wird anstelle von "Du bist noch zu 
			erschoepft von Deinem letzten Spruch." ausgegeben.
SI_SP_LOW_MSG		Die Meldung wird anstelle von "Du hast zu wenig 
			Zauberpunkte fuer diesen Spruch." ausgegeben.
SI_NOMAGIC		Prozentwert, mit dem P_NOMAGIC mgangen werden kann
SI_SPELLFATIGUE		Erschoepfung - Zeit, in der keine weiteren Spells
			aufgerufen werden koennen
SI_SKILLLEARN		Lerngeschwindigkeit in 0.01% pro A_INT/2
SI_SKILLABILITY		Faehigkeit, diesen Spruch zu benutzen
SI_SKILLARG		Argumente, die uebergeben wurden
SI_SKILLRESTR_USE	Beschraenkungen beim Gebrauch
SI_SKILLRESTR_LEARN	Beschraenkungen beim Lernen
SI_SKILLINFO		Kurzer Informationstext
SI_SKILLINFO_LONG	Langer Informationstext
SI_SKILLDAMAGE		Schaden
SI_SKILLDAMAGE_TYPE	Art des Schadens
SI_SKILLDAMAGE_MSG	Meldung die anstelle einer Waffe kommen soll
SI_SKILLDAMAGE_MSG2	dito fuer den Text fuer andere im Raum
SI_SPELL		Spell, mit dem angegriffen wurde
SI_INHERIT		Skill, von dem etwas uebernommen werden soll
SI_DIFFICULTY		Wert, der die Obergrenze der Faehigkeit abgrenzt
SI_LASTLIGHT		Fuer Nachtsicht Wann hat der Spieler zum letzten 
			mal Licht gesehen.
SI_SKILLHEAL		Heilung
SI_USR			selbst definierte Infos
SI_GUILD		Gilde, falls Auswahl aus mehreren moeglich
SI_ENEMY		Feind bei Kampfspruechen
SI_FRIEND		Der zu verteidigende Freund bei DefendOther 
			und InformDefend
SI_MAGIC_TYPE		Von was fuer einer Art ist die Magie (s.u.)
SI_PREPARE_TIME		Zeit die zur Vorbereitung benoetigt wird.
SI_ATTACK_BUSY_MSG	Meldung, wenn der Spieler zu beschaeftigt ist
SI_NO_ATTACK_BUSY	Wenn der Spell nicht als Taetigkeit zaehlen/gezaehlt 
			werden soll, kann man hier 
			NO_ATTACK_BUSY[_SET|_QUERY] setzen
SP_NAME			Name des Spells, falls Mapping
SP_SHOW_DAMAGE		Treffermeldung soll gezeigt werden.
SP_REDUCE_ARMOUR	AC soll Typabhaengig reduziert werden.
SP_PHYSICAL_ATTACK	Koerperlicher Angriff
SP_RECURSIVE		Rekursionen

Skill-Restrictions
SR_FUN			Funktion, die weitere Einschraenkungen prueft
SR_EXCLUDE_RACE		Ausgeschlossene Rassen
SR_INCLUDE_RACE		Eingeschlossene Rassen
SR_GOOD			Align < 
SR_BAD			Align >
SR_FREE_HANDS		Benoetigte freie Haende
SR_SEER			Muss Seher sein
SR_MIN_SIZE		Mindestgroesse
SR_MAX_SIZE		Maximalgroesse
SM_RACE			Rassenspezifische Besonderheiten

Magie-Arten
MT_ANGRIFF      	Angriff
MT_SCHUTZ       	Schutz
MT_ILLUSION     	Illusion
MT_BEHERRSCHUNG 	Beherrschung
MT_HELLSICHT    	Hellsicht
MT_VERWANDLUNG  	Verwandlung
MT_BESCHWOERUNG 	Beschwoerung
MT_BEWEGUNG     	Bewegung
MT_SAKRAL       	'Goettliches'
MT_HEILUNG      	Heilung
MT_CREATION     	Erschaffung
MT_PSYCHO       	Psycho-Kram
MT_MISC         	Sonstiges
------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
::

