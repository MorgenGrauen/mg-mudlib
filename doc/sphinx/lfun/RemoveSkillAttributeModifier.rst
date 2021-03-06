RemoveSkillAttributeModifier()
==============================

FUNKTION
--------
::

    public int RemoveSkillAttributeModifier(object caster, string atrname)

DEFINIERT IN
------------
::

    /std/living/skill_attributes.c

ARGUMENTE
---------
::

    <atrname>   string
                Name des Skill-Attributes, von dem der Modifikator geloescht
                werden soll.
                (Definiert in /sys/living/skill_attributes.h)

    <caster>    object
                Objekt, dessen Modifikator wieder entfernt werden soll.

BESCHREIBUNG
------------
::

    Entfernt den Modifikator, den Object <caster> gesetzt hat, wieder. Dies
    ist nur notwendig, wenn der Effekt vor Ablauf der Gueltigkeit des
    Modifikators aufgehoben werden soll.

RUECKGABEWERT
-------------
::

    SA_MOD_REMOVED         wenn der Modifikator geloescht wurde
    SA_MOD_NOT_FOUND       wenn der Modifikator nicht gefunden wurde
    Wenn man nur wissen will, ob die Operation erfolgreich war, empfiehlt es
    sich, auf == SA_MOD_REMOVED zu pruefen.

BEISPIELE
---------
::

    // eine Waffe setzt im InformWield() einen Bonus auf SA_DAMAGE fuer 10min
    protected void InformWield(object pl, int silent) {
      if (objectp(pl)) {
        if (pl->ModifySkillAttribute(SA_DAMAGE, 20, 600) == SA_MOD_OK)
          // Erfolgsmeldung an Spieler
        else
          // Misserfolgsmeldung an Spieler.
      }
    }

    // wenn der Spieler die Waffe vor Ablauf der 600s wegstecken will, muss
    // der Bonus natuerlich auch wieder raus
    protected void InformUnwield(object pl, int silent) {
      if (objectp(pl))
        pl->RemoveSkillAttributeModifier(this_object(), SA_DAMAGE);
        // falls kein solcher Mod mehr gesetzt war, liefert RSAM()
        // SA_MOD_NOT_FOUND zurueck. Auswertung des Rueckgabewertes ist
        // vernachlaessigt.
    }

    

SIEHE AUCH
----------
::

    Skills Lernen:  LearnSkill, ModifySkill, LimitAbility
    * Nutzung:      UseSpell, UseSkill
    * Abfragen:     QuerySkill, QuerySkillAbility
    * Modifikation: ModifySkillAttribute, QuerySkillAttribute,
                    QuerySkillAttributeModifier
      * Properties: P_SKILL_ATTRIBUTES, P_SKILL_ATTRIBUTE_OFFSETS
    * sonstig:      spruchermuedung, skill_info_liste
    * Properties:   P_NEWSKILLS

13.08.2008, Zesstra

