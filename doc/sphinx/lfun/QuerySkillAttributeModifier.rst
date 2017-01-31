QuerySkillAttributeModifier()
=============================

FUNKTION
--------
::

    public varargs mapping QuerySkillAttributeModifier(object caster, 
                                                    string *atrnames)

DEFINIERT IN
------------
::

    /std/living/skill_attributes.c

ARGUMENTE
---------
::

    <caster>    object
                Objekt, welches die gesuchten Mods gesetzt hat

    <atrnames>  string*
                Array von Skill-Attributen, welche durchsucht werden sollen.

BESCHREIBUNG
------------
::

    Diese Funktion liefert alle Mods von <caster> auf den Skill-
    Attributen <atrnames> in einem Mapping zurueck.
    Wird <atrnames> nicht angegeben oder ist ein leeres Array, werden alle
    Skill-Attribute nach Mods abgesucht.
    Wird <caster> nicht angeben oder ist 0, so werden alle Mods der 
    gewuenschten Skill-Attribute geliefert.
    Dementsprechend bekommt man alle Mods aller Skill-Attribute, wenn keins
    von beidem angegeben wird.

RUECKGABEWERT
-------------
::

    ([]), falls keine Modifikatoren gefunden wurden.
    In anderen Faellen ist die Datenstruktur des Mappings wie folgt:
    ([ atrname1: ([ <caster>: <value>; <duration> ]),
       atrname2: ([ <caster>: <value>; <duration> ]) ])
    Die Schluessel im Mapping sind die jeweiligen Skill-Attribute, die Werte
    des Mappings sind erneut Mappings, welche als Schluessel die Objekte
    haben, welche die Mods gesetzt haben. In diesem Mapping gehoeren zu jedem
    Schluessel zwei Werte, den Wert des Modifikators und die Ablaufzeit.
    <value> kann hierbei ein int oder eine closure sein, <duration> ist ein
    int, <caster> ist ein Objekt.
    Ggf. kann das innere Mapping natuerlich auch mehrere Modifikatoren
    enthalten (also caster1, caster2, usw.).

BEMERKUNGEN
-----------
::

BEISPIELE
---------
::

     Ein Objekt moechte seinen bestehenden Modifikator um 20 und die
     Gueltigkeit um 13 erhoehen.
     mapping res = ob->QuerySkillAttributeModifier(this_object(),
                                                  ({SA_DAMAGE}) );
     if (member(res, SA_DAMAGE) && member(res[SA_DAMAGE], this_object())) {
          // alten Mod ueberschreiben und Werte dabei anheben.
          ob->ModifySkillAttributeModifier(SA_DAMAGE,
              res[SA_DAMAGE][this_object(),0] + 20,
              res[SA_DAMAGE][this_object(),1] + 13 );
     }
     else
          // neuen Mod setzen.
          ob->ModifySkilAttributeModifier(SA_DAMAGE, 20, 13);

      

     Ein Objekt hat den Fluch der unpraezisen Schnelligkeit, welcher SA_DAMAGE
     reduziert, sofern das Lebewesen einen positiven Modifikator auf SA_SPEED
     hat:
     mapping res = ob->QuerySkillAttributeModifier(0, ({SA_SPEED}) );
     if (member(res, SA_SPEED) {
         int val, int dur;
         foreach(object caster, int v, int d: res[SA_SPEED]) {
             // groessten Mod rausfinden, dabei keine closures
             // beruecksichtigen.
             if (intp(v) && v>val) {
                 val=v;
                 dur=d;
             }
         }
         if (val > 0) {
             // pos. Mod auf SA_SPEED gefunden, entsprechenden neg. Mod auf
             // SA_DAMAGE setzen, der zum gleichen Zeitpunkt ungueltig wird
             // wie der Mod auf SA_SPEED.
             if (ob->ModifySkillAttribute(SA_DAMAGE, -val, dur) == SA_MOD_OK)
                tell_object(ob, "tolle Fluchmeldung.");
         }
     }

SIEHE AUCH
----------
::

    Skills Lernen:  LearnSkill, ModifySkill, LimitAbility
    * Nutzung:      UseSpell, UseSkill
    * Abfragen:     QuerySkill, QuerySkillAbility
    * Modifikation: ModifySkillAttribute, QuerySkillAttribute,
                    RemoveSkillAttributeModifier
      * Properties: P_SKILL_ATTRIBUTES, P_SKILL_ATTRIBUTE_OFFSETS
    * sonstig:      spruchermuedung, skill_info_liste
    * Properties:   P_NEWSKILLS

14.08.2008, Zesstra

