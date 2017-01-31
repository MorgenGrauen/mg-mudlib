skill_info_liste
================

SI_*
----
::

DEFINIERT IN
------------
::

    /sys/newskills.h

BESCHREIBUNG
------------
::

    Die folgende Liste der SI-Typen ist grob nach Gueltigkeit fuer Skills
    und Spells sortiert.

    

    Anwendungsorte der SI_-Werte sind:
    - /std/living/combat und von dort gerufene Skills (Kampf)
    - /std/gilden_ob und in Gilden lernbare Spells/Skills
    - /std/spellbook und von Spellbooks ausgefuehrte Spells
    - /std/living/life und von dort gerufene Skills (Alkohol)
    - /std/shells/* und entsprechende Rassenskills

    Im Skillsystem unter /std/living/skills wird vor auf Informationen
    aus dem Teil #1 Ruecksicht genommen. Einige dieser Werte
    werden auch permanent im Spieler gespeichert (wie zB die
    SI_SKILLABILITY).

AKTUELLE LISTE: (5. Oktober 2011)
 #1 Skills und Spells:
    SI_SKILLFUNC: string
     Beinhaltet den Namen der Methode, die die eigentliche Funktionalitaet
     des Skills/Spells implementiert.
     Falls nicht angegeben, wird bei Spells durch UseSpell() das Verb, das
     der Spieler eingegeben hat, als Spell-Methodenname angenommen.
     Im Gildenobjekt kann hier abweichend von Spell-Namen stehen, wie die
     Spellfunktion im Spellbook heisst.

    SI_CLOSURE: closure
     Optimiert den Zugriff fuer den internen Gebrauch, indem die gefundene
     Spell/Skillfunktion darin direkt gespeichert wird und so lange gueltig
     ist, bis das/die Objekt(e)/Closure(s) zerstoert sind.
     Kann theoretisch auch fuer externe Skills durch (Autoload-)Objekte
     benutzt werden.

    SI_SKILLABILITY: int
     Faehigkeit, den Spell/Skill zu benutzen. Normal ist von
     -MAX_ABILITY bis MAX_ABILITY.

    SI_SKILLDAMAGE_TYPE: string*
     Art des Schadens eines Angriffs: siehe Schadenstypen in /sys/combat.h

    SI_SKILLDAMAGE_MSG: string
     Meldung anstatt der Waffe oder Haende-Meldung.
    SI_SKILLDAMAGE_MSG2: string
     Meldung anstatt der Waffe oder Haende-Meldung fuer den Raum.

    SI_INHERIT: string
     Enthaelt den Namen des Skills/Spells, von dem geerbt wird. Das
     bedeutet einerseits, das LearnSkill() auch diesen uebergeordneten
     Skill beeinflusst und andererseits, dass bei Skills auch dieser
     uebergeordnete Skill im Rahmen einer Skill-Anwendung gerufen wird.

    SI_DIFFICULTY: int / [Spells: mixed]
     Schwierigkeit eines Skills/Spells. Beeinflusst die jeweils oberen
     Schranken fuer das Lernen eines Skills/Spells in Abhaengigkeit 
     von Spielerlevel.
     Wenn in einem Spell-Info-Mapping kein Wert steht wird bei Spells
     automatisch SI_SPELLCOST genommen, der Wert im Spell-Info-Mapping
     geht beim Lernen aber immer vor Parametern.
    FACTOR(SI_DIFFICULTY): int / mixed
    OFFSET(SI_DIFFICULTY): int / mixed
     Nur fuer Spellbooks/Spells. Der mixed-Parameter kann Funktionen
     enthalten. Siehe unten bei SI_SPELLCOST.

    SI_LASTLIGHT: int
     Zeitpunkt, bis wann der Standardskills SK_NIGHTVISION den Spieler
     sehen laesst.

    SI_TESTFLAG: int
     Wenn gesetzt, dann soll der Skill nicht genutzt, sondern nur getestet
     werden, ob er erfolgreich waere. Entspricht also in etwa dem Aufruf
     von SpellSuccess() in Spellbooks, wird aber bei UseSkill() als
     Skill-Parameter uebergeben.
     Der Skill muss entsprechend implementiert sein!

    SI_GUILD: string
     Angabe der Gilde, aus der der Skill stammt. Sinnvoll, wenn der Skill
     nicht aus der aktuellen P_GUILD stammt. Fuer generelle Skills/Spells
     zB waere das "ANY".
     Gilt nicht fuer Spells!

    SI_ENEMY: object
     Aktuelles Feindesobjekt, wird bei Skill-Anwendung aus dem Kampf heraus
     von std/living/combat.c an den Skill uebergeben.
     Beispiel: Standardwaffenskills, SK_MAGIC_DEFENSE, SK_MAGIC_ATTACK,
               SK_TWOHANDED, SK_SPELL_DEFEND, SK_INFORM_DEFEND und
               SK_DEFEND_OTHER.

    SI_FRIEND: object
     Zu verteidigendes Freundesobjekt, wird bei Skill-Anwendung aus dem
     Kampf heraus von std/living/combat.c an den Skill uebergeben.
     Beispiele: SK_INFORM_DEFEND und SK_DEFEND_OTHER.

    SI_MAGIC_TYPE: string*
     Enthaelt ein Array der Magietypen, die zum Einsatz kommen. Die Angabe
     geschieht zB im Spellbook und beeinflusst SpellDefend().

    SI_LAST_USE: int (eventuell obsolet)
     Letzte Nutzung des Skills.

    

    SI_LEARN_PROB: int (eventuell obsolet)
     Lernwahrscheinlichkeit.

    SI_SKILLDURATION: int
     Dauer des Skills/Spells. Momentan nicht allzu verbreitet in Nutzung.

 #2 nur fuer Spells:
    SI_SPELLBOOK: string
     Kann direkt das enthaltende Spellbook fuer einen Spell enthalten.

    SM_RACE: mapping
      Mapping, das als Key die Rasse und als Value ein Mapping X
      enthaelt. Dieses Mapping X wird direkt zu sinfo hinzugefuegt und
      ueberschreibt damit bei Bedarf Defaultwerte durch rassenspezifische
      Werte.

    SI_SPELLCOST: int / mixed
    FACTOR(SI_SPELLCOST): int / mixed
    OFFSET(SI_SPELLCOST): int / mixed
     Beinhaltet die Werte, die fuer die Berechnung der Spellkosten
     zustaendig sind.
     Dabei gilt: Realkosten = OFFSET(COST) + (COST * FACTOR(COST))/100
     Die einzelnen Eintraege koennen anstatt eines fixen Wertes auch den
     Verweis auch eine Funktion in Form von Closure/Methodenname/Array mit
     Objekt/Objektname und Methodenname enthalten. Siehe dazu auch
     execute_anything().

    SI_TIME_MSG: string
     Meldung, die dem Spieler mitteilt, dass er noch nicht wieder einen
     Spell casten kann. Falls dieser Eintrag nicht gesetzt ist, wird
     ein Standardtext ausgegeben.

    SI_SP_LOW_MSG: string
     Meldung, die dem Spieler mitteilt, dass er zu wenige
     Konzentrationspunkte besitzt, um den Spell zu casten. Falls dieser
     Eintrag nicht gesetzt ist, wird ein Standardtext ausgegeben.

    SI_PREPARE_MSG: string
     Meldung, die dem Spieler ausgegeben werden soll, wenn er einen Spell
     vorbereitet. Ansonsten wird ein Standardtext ausgegeben.

    SI_PREPARE_BUSY_MSG: string
     Meldung, die dem Spieler ausgegeben werden soll, wenn er schon diesen
     Spell vorbereitet. Ansonsten wird ein Standardtext ausgegeben.

    SI_PREPARE_ABORT_MSG: string
     Meldung, die dem Spieler ausgegeben werden soll, wenn er die
     Vorbereitung dieses Spells durch einen anderen Spell unterbricht.
     Ansonsten wird ein Standardtext ausgegeben.

    SI_NOMAGIC: int
     Wert zwischen 0 und 100 (oder mehr), der gegen die P_NOMAGIC-Wirkung
     eines Raumes wirkt.
     Je hoeher der Wert ist, desto unwahrscheinlicher ist es, dass ein
     Raum den Spell durch Antimagie stoert. Ein Raum sollte nur Werte
     zwischen 0 und 100 gesetzt haben. Ist der Wert des Raums groesser
     als der hier angegeben, dann blockiert er Magie mit einer gewissen
     eben seiner angegebenen Wahrscheinlichkeit.

    SI_NOMAGIC_MSG: string
     Meldung, die bei Fehlschlag durch P_NOMAGIC des Raumes ausgegeben
     wird. Ansonsten wird ein Standardtext ausgegeben.

      

    SI_SPELLFATIGUE: int / mixed
    FACTOR(SI_SPELLFATIGUE): int / mixed
    OFFSET(SI_SPELLFATIGUE): int / mixed
     Werte, die fuer die Berechnung der Wieder-Cast-Zeit benutzt werden.
     Die Berechnung und die moeglichen Angaben (Closure etc.) sind
     identisch zu SI_SPELLCOST.
     Das Spellbook gibt bei Nicht-Wieder-Bereitschaft SI_TIME_MSG aus.

    SI_X_SPELLFATIGUE: mapping mit ([string key: int val])
     Werte, die fuer die Berechnung der Wieder-Cast-Zeit benutzt werden.
     Spellbook-Casten: Mapping wird durch Aufruf von CheckSpellFatigue(<key>)
     am Caster gefiltert. Nur wenn das resultierende Mapping leer ist (kein
     <key> hat einen gueltigen Fatigue-Eintrag), ist Spell castbar, sonst
     Ausgabe von SI_TIME_MSG.
     Nach Spellbook-Casten: Setzen der Fatigue fuer alle <val> > 0 mit <key>.

    SI_SKILLLEARN: int / mixed
    FACTOR(SI_SKILLLEARN): int / mixed
    OFFSET(SI_SKILLLEARN): int / mixed
     Werte, die fuer die Berechnung der Lerngeschwindigkeit benutzt
     werden, normalerweise pro A_INT/2 je 0.01% (also 1 von MAX_ABILITY).
     Die Berechnung und die moeglichen Angaben (Closure etc.) sind
     identisch zu SI_SPELLCOST.

    SI_LEARN_ATTRIBUTE: mapping
     Mapping, welches die Attribute, nach denen gelernt werden soll
     als Keys enthaelt. Der Value legt die Gewichtung fest, denn bei
     mehreren Attributen wird ein Mittelwert gebildet.
     Der Normalfall entspraeche ([A_INT: 1]) fuer SI_LEARN_ATTRIBUTE.

    SI_NO_LEARN
     Wenn man (temporaer!) nicht will, dass dieser Skill gelernt wird.
     Muss von den Spellbooks beachtet werden.
     Sollte niemals im Spieler abgespeichert werden. Oder permanent in
     Gilde/Spellbook gesetzt sein. Sondern im Laufe einesr Nutzung in der
     jew. Kopie von sinfo gesetzt werden, die gerade genutzt wird.

    

     SI_SKILLARG: string
     Beinhaltet den Text, den der Spieler nach dem Spellkommando eingegeben
     hat. Z.B. stuende bei "krallenschlag ork 2" der Text "ork 2" im
     Parameter.

    SI_SKILLRESTR_USE: mixed
     Einschraenkungen fuer das Nutzen des Spells.
     Wird normalerweise direkt im Spellbook fuer den Spell eingetragen.
     Die einzelnen Moeglichkeiten werden in der manpage zu
     "check_restrictions" erlaeutert.

     

    SI_SKILLRESTR_LEARN: mixed
     Einschraenkungen fuer das Lernen des Spells.
     Wird normalerweise direkt im Gildenobjekt fuer den Spell
     eingetragen.
     Die einzelnen Moeglichkeiten werden in der manpage zu
     "check_restrictions" erlaeutert.

    SI_SKILLINFO: string
    SI_SKILLINFO_LONG: string
     Beschreibung des Spells. Wird im Gildenobjekt eingetragen und
     SI_SKILLINFO wird von SkillListe angezeigt.

    SI_SKILLDAMAGE: int / mixed
    FACTOR(SI_SKILLDAMAGE): int / mixed
    OFFSET(SI_SKILLDAMAGE): int / mixed
     Werte, die fuer die Schadenshoehenberechnung des Spells benutzt
     werden (random ueber den Gesamtwert normalerweise).
     Die Berechnung und die moeglichen Angaben (Closure etc.) sind
     identisch zu SI_SPELLCOST.

    SI_SKILLDAMAGE_BY_ROW
     Ein Mapping mit Boni fuer den Angriff aus bestimmten Kampfreihen.
     Funktioniert nur bei Verwendung von TryDefaultAttackSpell-Spells
     aus dem Spellbook.
     Der Key steht fuer eine bestimmte Reihe, sein Wert fuer den
     randomisierten Bonus fuer Lebewesen, die aus dieser Reihe casten.
    OFFSET(SI_SKILLDAMAGE_BY_ROW)
     Ein Mapping mit fixem Wert fuer Row-Boni im Kampf.

     Beispiel: AddSpell("feuerball", 20,
                        ([SI_SKILLDAMAGE:50,
                          OFFSET(SI_SKILLDAMAGE):100,
                          SI_SKILLDAMAGE_BY_ROW:([2:40,3:20}),
                          OFFSET(SI_SKILLDAMAGE_BY_ROW):([2:20]), ...
     gibt
      Reihe 1: random(50)+100;
      Reihe 2: random(50)+100+random(40)+20
      Reihe 3: random(50)+100+random(20)
     ACHTUNG: Hier gilt nicht die selbe Struktur wie bei SI_SPELLCOST!

    SI_SPELL: mapping
     Dieser Eintrag enthaelt verschiedene Unterwerte, die den Spell
     gerade fuer Angriffs-Spells genauer beschreiben. Siehe Defend()
     fuer eine Beschreibung der verschiedenen Eintraege (die so auch
     in das Spellbook uebernommen werden koennen).

    SI_COLLATERAL_DAMAGE: int
     Hiermit kann man einen Prozentwert von SI_SKILLDAMAGE (inklusive
     Offsets und Factor) fuer Kollateralschaden an Freunden im definierten
     Bereich eines Flaechenspells (TryGlobalAttackSpell()) angeben.
     10 bedeutet bei OFFSET(SI_SKILLDAMAGE)=100 zB 10% von 100 = 10 = 1 LP
     an mit reduce_hit_points() verursachtem Schaden.

    

    SI_NUMBER_ENEMIES, SI_NUMBER_FRIENDS: int
     Wird von TryGlobalAttackSpell() im Spell gesetzt und enthaelt die
     Anzahl der im angegebenen Bereich befindlichen Feinde und Freunde.
     Ist dementsprechend von informativem Nutzen fuer den Angegriffenen
     und das Spellbook NACH Aufruf von TryGlobalAttackSpell().

    

    SI_DISTANCE, SI_WIDTH, SI_DEPTH: int
     Limitiert die Entfernung, Tiefen und Breite der Wirkung eines
     TryGlobalAttackSpell()

    SI_SKILLHEAL: int
     Konvention fuer Spells im Spellbook, dort den Heilwert zu hinterlegen,
     hat keine Auswirkungen unter /std/.

    SI_USR: mixed
     Fuer selbst definierte Infos, spellbookabhaengig.

    SI_PREPARE_TIME: int
     Dauer der Zeit fuer zu praeparierende Spells.

    SI_ATTACK_BUSY_MSG: string
     Meldung, die dem Spieler mitteilt, dass er schon zu beschaeftigt
     mit einem Angriff ist, um einen Spell zu casten. Falls dieser
     Eintrag nicht gesetzt ist, wird  ein Standardtext ausgegeben.

    SI_NO_ATTACK_BUSY: int
     Enthaelt als Flag NO_ATTACK_BUSY_QUERY wenn der Spell NICHT wie
     eine Spezialwaffe die Aktionen einschraenkt. Siehe P_ATTACK_BUSY.

    SI_ATTACK_BUSY_AMOUNT: int
     Menge des P_ATTACK_BUSY fuer diesen Spell. Falls dieser Wert nicht
     gesetzt ist, aber auch SI_NO_ATTACK_BUSY nicht mit
     NO_ATTACK_BUSY_QUERY ausgezeichnet ist wird 1 angenommen.

SIEHE AUCH
----------
::

    Skills Lernen:  LearnSkill, ModifySkill, LimitAbility
    * Nutzung:      UseSpell, UseSkill
    * Abfragen:     QuerySkill, QuerySkillAbility
    * Modifikation: ModifySkillAttribute, QuerySkillAttribute,
                    QuerySkillAttributeModifier, RemoveSkillAttributeModifier
      * Properties: P_SKILL_ATTRIBUTES, P_SKILL_ATTRIBUTE_OFFSETS
    * sonstig:      spruchermuedung
    * Properties:   P_NEWSKILLS
    Spellbook:      UseSpell, Learn, SpellSuccess
    Gilden:         gilden-doku
    Kampf:          Defend

7. Nov 2012 Gloinson

