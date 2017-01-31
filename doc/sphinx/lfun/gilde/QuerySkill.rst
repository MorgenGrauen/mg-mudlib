QuerySkill()
============

FUNKTION
--------
::

    mapping QuerySkill(string skill)

DEFINIERT IN
------------
::

    /std/gilden_ob.c

ARGUMENTE
---------
::

    string skill       Name des Skills

BESCHREIBUNG
------------
::

    Liefert fuer diesen Skill die Gildeninformationen in einem
    Mapping zurueck.

BEISPIEL
--------
::

    // /gilden/klerus->QuerySkill(FIGHT(WT_CLUB)) gibt zB zurueck:
    ([SI_FUN:                "Fight_club",
      OFFSET(SI_SKILLLEARN): 1
      SI_SKILLRESTR_USE:     ([SR_FUN:#'gilden/klerus->spellTest()]),
      OFFSET(SI_SKILLLEARN): #'gilden/klerus->learnOffset,
      OFFSET(SI_SPELLCOST):  #'gilden/klerus->costOffset,
      FACTOR(SI_SPELLCOST):  #'gilden/klerus->costFactor])

SIEHE AUCH
----------
::

    GObj Lernen:    LearnSkill, LearnSpell, InitialSkillAbility
    * Anzeigen:     SkillListe
    * Verwalten:    AddSkill, AddSpell, QuerySpell
    * Nutzen:       UseSpell (gilde)
    * Properties:   P_GUILD_SKILLS, P_GLOBAL_SKILLPROPS
    Gildenfkt.:     GuildRating
    * Ein/Austritt: beitreten, bei_oder_aus_treten, austreten

3. Okt 2011 Gloinson

