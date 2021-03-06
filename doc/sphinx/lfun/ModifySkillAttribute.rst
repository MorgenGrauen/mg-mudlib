ModifySkillAttribute()
======================

FUNKTION
--------
::

    public int ModifySkillAttribute(string atrname, mixed value, 
                                    int duration)

DEFINIERT IN
------------
::

    /std/living/skill_attributes.c

ARGUMENTE
---------
::

    <atrname>   string
                Name des zu veraendernden Attributes
                (Definiert in /sys/living/skill_attributes.h)

    <value>     int oder closure
                Wert des Modifikators
                oder
                eine Closure, welche bei Abfrage des betreffenden SAs
                abgefragt um den Modifikator zu bestimmen.

    <duration>  int
                Dauer in Sekunden

BESCHREIBUNG
------------
::

    Aendert temporaer, d.h. fuer eine bestimmte Zeit, ein Skill-Attribut
    eines Lebewesen, indem ein Modifikator hinzugefuegt wird.

    

    Der Standardwert eines SA wird von P_SKILL_ATTRIBUTE_OFFSETS festgelegt
    oder ist 100, wenn besagte Property leer ist.
    Alle Modifikatoren (negativ wie positiv) werden addiert und bilden
    zusammen mit dem Standardwert eine Gesamtsumme.
    Bei allen SAs ausser SA_QUALITY wird diese Gesamtsumme noch mit
    SA_QUALITY (welches sich damit auf alle anderen Skill-Attribute
    auswirkt) multipliziert und das Ergebnis stellt den Endwert des SA dar.
    (Beispiel s.u.)

    Der Wert eines Modifikators muss zwischen -1000 und 1000 liegen. Der
    Gesamtwert eines SA kann 10 nicht unter- und 1000 nicht ueberschreiten.

    Falle <value> eine Closure ist, wird diese Closure jedesmal
    ausgefuehrt, wenn das entsprechende SA abgefragt wird. Der
    Rueckgabewert dieser Closure stellt dann den Wert des Modifikators
    dar. Auch dieser muss zwischen -1000 und 1000 liegen. Gibt die
    Closure keinen int zurueck, wird der Modifikator geloescht.

    Gueltige Skill-Attribute sind momentan:
    * SA_QUALITY:    Allgemeine Qualitaet: wirkt sich auf alle anderen
                     Attribute auch aus (multplikativ auf Basis 100)
    * SA_DAMAGE:     Schaden, den das Lebewesen macht
    * SA_SPEED:      Geschwindigkeit des Lebewesens (zB Angriffe/Runde)
    * SA_DURATION:   Spell-/Skilldauer
    * SA_ENEMY_SAVE: identisch zu SA_SPELL_PENETRATION (OBSOLET!)
    * SA_SPELL_PENETRATION: Chance des _Casters_, einen Spell durch ein
                            P_NOMAGIC durchzukriegen.
    * SA_RANGE:      Reichweite des Lebewesens (eher unbenutzt)
    * SA_EXTENSION:  "Ausdehnung" bei Gruppen-/Flaechenspells: FindGroupN/P

RUECKGABEWERT
-------------
::

    SA_MOD_OK              wenn der Modifikator gesetzt wurde
    SA_TOO_MANY_MODS       wenn die max. Anzahl an Mods schon erreicht ist.
    SA_MOD_TOO_SMALL       wenn der Modifikator zu klein ist 
    SA_MOD_TOO_BIG         wenn der Modifikator zu gross ist
    SA_MOD_INVALID_ATTR    wenn das gewuenschte SA gar nicht existiert
    SA_MOD_INVALID_OBJECT  wenn das setzende Objekt ungueltig ist
    SA_MOD_INVALID_VALUE   wenn der Modifikator ungueltig ist
    Wenn man nur wissen will, ob die Operation erfolgreich war, empfiehlt
    es sich, auf == SA_MOD_OK zu pruefen.

BEMERKUNGEN
-----------
::

    Nachdem ein Objekt, welches Modifikatoren setzte, zerstoert wurde,
    werden die Modifikatoren spaetestens ungueltig, sobald in dem
    manipulierten Lebewesen erneut ModifySkillAttribute() gerufen wird!
    Bei Closures ist der Mod sofort weg.

BEISPIELE
---------
::

    // sei PL ein Spieler, den mein NPC schwaechen will:
    PL->ModifySkillAttribute(SA_QUALITY, -75, 13);
    // Fuer 13s wird SA_QUALITY um 75 reduziert. Dies wirkt sich auf alle
    // anderen SAs aus! (s. drittes Beispiel)

    // sei PL ein Lebewesen, welchem ich fuer 11s 2 Schlaege pro Kampfrunde
    // zusaetzlich geben moechte:
    PL->ModifySkillAttribute(SA_SPEED, 200, 11);
    // wenn keine weiteres Modifikatoren wirken, hat PL jetzt 3 Schlaege
    // pro Kampfrunde (Basiswert 100 + 200 == 300 => 3).

    Angenommen, ein Lebewesen hat einen Basiswert von 130 auf SA_SPEED und
    100 auf SA_QUALITY (P_SKILL_ATTRIBUTE_OFFSETS) und nun 3 Modifikatoren
    gesetzt: SA_SPEED +100, SA_SPEED -30 und SA_QUALITY von -10:
    Zunaechst wird SA_QUALITY bestimmt: 100 - 10 = 90  => 0.9
    Anschliessend wird SA_SPEED bestimmt: 130 + 100 - 30 = 200 => 2
    Nun wird SA_SPEED noch mit SA_QUALITY multipliziert: 2 * 0.9 = 1.8
    Das Lebewesen hat nun also im Endeffekt 1.8 Schlaege pro Kampfrunde.

    

SIEHE AUCH
----------
::

    Skills Lernen:  LearnSkill, ModifySkill, LimitAbility
    * Nutzung:      UseSpell, UseSkill
    * Abfragen:     QuerySkill
    * Modifikation: QuerySkillAttribute,
                    QuerySkillAttributeModifier, RemoveSkillAttributeModifier
      * Properties: P_SKILL_ATTRIBUTES, P_SKILL_ATTRIBUTE_OFFSETS
    * sonstig:      spruchermuedung, skill_info_liste
    * Properties:   P_NEWSKILLS

5. Okt 2011 Gloinson

