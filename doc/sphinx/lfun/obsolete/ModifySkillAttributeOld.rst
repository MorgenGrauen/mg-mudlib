ModifySkillAttributeOld()
=========================

FUNKTION
--------
::

    varargs int ModifySkillAttributeOld(object caster, string atrname, 
                                        int value, int duration, mixed fun)

DEFINIERT IN
------------
::

    /std/living/skill_attributes.c

ARGUMENTE
---------
::

    <caster>    IGNORIERT
                frueher Objekt, das die Modifikation vornimmt, heute ist
                dieses Argument ohne Bedeutung und wird ignoriert.

    <atrname>   STRING
                Name des zu veraendernden Attributes
                (Definiert in /sys/living/skill_attributes.h)

    <value>     INT
                Neuer Wert des Attributs (Standard: 100)

    <duration>  INT
                Dauer in Sekunden

    <fun>       NICHT MEHR UNTERSTUETZT - ModifySkillAttribute() nutzen!!

BESCHREIBUNG
------------
::

    Diese Funktion existiert nur aus Kompatibilitaetsgruenden. Bitte in neuen
    Objekten NICHT mehr verwenden und in alten nach Moeglichkeit ausbauen!

    Aendert ein Skill-Attribut eines Living.

    

    Fuer <value> ist folgendes zu beachten:
    Frueher handelte es sich um einen multiplikativen Faktor. 100 hatte die
    Bedeutung von 1 und veraenderte nichts. Heute sind die Modifikatoren
    additiv. Diese Funktion macht eine einfache Umrechnung, indem sie vom hier
    uebergeben Wert 100 abzieht. (In der Annahme, dass frueher meist eh nur 
    ein Modifikator zur gleichen Zeit aktiv war.)
    Gibt man hier also hier eine 100 an, wird ein Modifikator von 0 einge-
    tragen, der nichts aendert, bei 200 wird ein Modifikator von 100 einge-
    tragen, bei 50 einer von -50, welcher das Skillattribute folglich 
    reduziert.

    Es sind momentan max. 5 gleichzeitige Skillattribute-Modifikatoren pro SA
    zulaessig.

RUECKGABEWERT
-------------
::

     0  wenn der Wert ungueltig ist oder aus sonstigem Grunde nicht gesetzt
        werden konnte (fuer bessere Diagnostik -> ModifySkillAttribute()).
    >0  wenn alles okay war

BEMERKUNGEN
-----------
::

    Frueher musste ein setzendes Objekt ein groesseres P_LEVEL haben als das
    Objekt, welches einen vorherigen Modifikator gesetzt hat, um diesen zu
    ueberschreiben. Dies ist inzwischen ohne Bedeutung.

BEISPIELE
---------
::

    Ein NPC:

    void
    create() {
    .
    .
    .
    AddSpell(1, 1,
      "Der fuerchterliche NPC haut Dir auf den Kopf.\n",
      "Der fuerchterliche NPC haut @WEN auf den Kopf.\n",
      DT_MAGIC, "schwaechen");
    .
    .
    }

    schwaechen(object enemy, int damage, mixed *dam_type) {
      int ergebnis;
      ergebnis = enemy->ModifySkillAttributeOld(this_object(), SA_QUALITY, 50, 5);
      if (ergebnis > 0)
          tell_object(enenmy, "Du fuehlst Dich ganz schwach.\n");
    }

    

    Der NPC schwaecht seinen Gegner erheblich! Alles wird fuer 5 Sekunden um
    50, d.h. 0.5 Skillattribute reduziert (50 - 100 => -50 als Modifikator).

SIEHE AUCH
----------
::

    P_SKILL_ATTRIBUTES, P_SKILL_ATTRIBUTE_OFFSETS,
    ModifySkillAttribute, QuerySkillAttribute(),
    RemoveSkillAttributeModifer(), QuerySkillAttributeModifier()


07.08.2008 Zesstra

