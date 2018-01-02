Defend()
========

FUNKTION
--------

  public int Defend(int dam, string|string* dam_type, int|mapping spell, 
    object enemy)

DEFINIERT IN
------------

  /std/living/combat

ARGUMENTE
---------

  int dam
    Initiale Staerke des Angriffs (10 dam ~ 1 HP)
  string* dam_type
    Art des Schadens, der angerichtet werden soll
    Muss ein Array von Schadenstypen sein, alte Objekte uebergeben hier
    manchmal strings.
  mapping spell
    Mapping mit zusaetzlichen Informationen zum Angriff(Siehe unten)
    Alte Objekte uebergeben manchmal einen Integer (0 fuer Physikalischen
    Angriff, 1 fuer Zauber.
  object enemy
    Der Feind/Schadenverursacher

BESCHREIBUNG
------------

  1. Generell
    Wenn das Lebewesen angegriffen wird, wird geprueft, wie stark die
    Ruestungen und koerpereigenen Abwehrkraefte sind und die Staerke des
    Schadens dementsprechend vermindert.
    Ggf. wird der Schaden zugefuegt und der Feind in  die Liste der Feinde
    aufgenommen. Der Schaden betraegt:
    (dam-Summe(Ruestungsstaerken)-random(P_BODY+A_DEX))*CheckResistance/10
    aber nicht unter 0.

  2. Der Parameter 'spell'
    Ist 'spell' 0, dann gilt der Angriff als normale physische Attacke
    Uebergibt man als 'spell'-Parameter ein Mapping, so gibt es dafuer
    diverse Flags, die das Ergebnis manipulieren (in new_skills.h
    enthalten). Nichtangabe eines Flags gilt als 0.

    - SP_PHYSICAL_ATTACK ---------- 0/1
      1, wenn Ruestungen wirken sollen, 0 sonst
      -> entspricht !spell, wenn dieses Int ist
    - SP_NO_ENEMY ----------------- 0/1
      1, falls der Angriff nicht toedlich ist, es also keinen echten Gegner
      gibt
      -> es wird dann reduce_hit_points() gerufen statt do_damage()
    - SP_NO_ACTIVE_DEFENSE -------- 0/1
      1, falls aktive Abwehren (wie zurueckschlagende Amulette,
      Karateabwehren oder Ausweichmanoever) unterbleiben sollen
      -> zB bei Kratzen durch Dornen oder Fall aus grosser Hoehe
      ist aktive Abwehr oder Ausweichen unlogisch
    - SP_RECURSIVE ---------------- 0/1
      1, falls der Spell aus einem Defend gerufen wurde (oder einer DefendFunc)
      -> verhindert Rekursionsprobleme
    - SP_NAME --------------------- string
      Name des Spells
    - SP_GLOBAL_ATTACK ------------ 0/1
      1 bei Flaechenspells
    - SP_REDUCE_ARMOUR ------------ Mapping: keys AT_X/P_BODY, values int>=0
      Die Schutzwirkung durch P_AC/Magie einer Ruestung wird typabhaengig 
      reduziert. Aufbau eines Mappings im Beispiel:
        ([AT_BOOTS: 0,  // Stiefel schuetzen gar nicht
        P_BODY:  50,  // Koerper zu 50%
        AT_BELT: 600  // Guertel zu 600%
        ])
      -> alle 'fehlenden' Eintraege wirken normal
    - SP_SHOW_DAMAGE -------------- 0/1 oder Array von Arrays
      0, fuer keine Treffermeldung, 1 sonst
      In einem Array koennen Ersatz-Treffermeldungen definiert werden. Format ist:
                  ({
        ({ int lphit1, string mess_me, string mess_en, string mess_room }),
        ({ lphit2, mess_me, mess_en, mess_room }),
        ...
        ({ lphitn, mess_me, mess_en, mess_room }),
        })
        wobei lphit1<lphit2<...<lphitn sein muss, d.h. das Array-Array
        aufsteigend sortiert.

      Ist ein Treffer x LP hart, werden die Meldungen des lphit-Arrays
      ausgegeben, dessen Wert am naehesten unter dem Schaden liegt.

      In den Meldungen mess_me (an den Getroffenen), mess_en (an den Feind),
      mess_room (an die restlichen Umstehenden) koennen Ersatzstrings wie
      folgt verwendet werden:
        @WER1/@WESSEN1/@WEM1/@WEN1 - name(casus) des Getroffenen (TO)
        @WER2/@WESSEN2/@WEM2/@WEN2 - name(casus) des Feindes (enemy)
    - EINFO_DEFEND ------------ Mapping
      Dieses Mapping liefert erweiterte Informationen zu dem
      bisherigen Ablauf des aktiven Attacks.
      Die verfuegbaren Informationen sind in der Manpage zu
      DefendInfo festgehalten.

  3. Reihenfolgen in Defend
    - das Living wird angegriffen, wenn
      - P_NO_ATTACK != 0
      - 'enemy' existiert und kein netztoter Spieler ist
    - P_DEFENDERS werden durchgegangen (und eventuell benachrichtigt)
    - P_TMP_ATTACK_HOOK wird abgefragt
    - die Ruestungen werden vom Schaden gegebenenfalls abgezogen
    - magischer Ausweichskill beruecksichtigt
    - sensitive Objekte werden ggf. benachrichtigt
    - InternalModifyDefend wird gerufen
    - Koerperabwehr abgezogen
    - der Schaden an do_damage()/reduce_hit_points() uebergeben
    - Flucht ueberpruefen mit CheckWimpyAndFlee()

BEMERKUNGEN
-----------

  Ruestungen wirken konventionell nur, wenn mindestens ein Schadensanteil
  mechanisch ist und es kein Spell oder ein Spell mit SP_PHYSICAL_ATTACK
  auf 1 ist.

  Defend() beruecksichtigt magische Verteidigungen, die der Spieler bei
  sich hat, sollte also aus Fairness gegenueber den Objekten anderer
  Magier immer dem direkten reduce_hit_points() oder do_damage()
  vorgezogen werden. Mittels der Flags in 'spell' kann man sehr viel
  aendern.

RUECKGABEWERT
-------------

  Hoehe des tatsaechlichen Schadens. Dies kann mehr sein als die
  Lebenspunkte des Lebewesens.

BEISPIELE (SIEHE AUCH Defend_bsp):
  // ein simpler Angriff:
  enem->Defend(100, ({DT_BLUDGEON}), ([SP_PHYSICAL_ATTACK:1]), this_object());

  // ein magischer Angriff (ohne Treffermeldung):
  enem->Defend(100, ({DT_BLUDGEON, DT_FIRE}), ([SP_PHYSICAL_ATTACK:0]), this_object());

  // ein magischer Angriff mit Treffermeldung:
  enem->Defend(100, ({DT_BLUDGEON, DT_FIRE}), ([SP_SHOW_DAMAGE:1]),
    this_object());

SIEHE AUCH
----------

  Angriff: :doc:`Attack`, :doc:`../props/P_NO_ATTACK`, :doc:`InsertEnemy`
  Schaden:   :doc:`../props/P_ENABLE_IN_ATTACK_OUT`, 
    :doc:`../props/P_LAST_MOVE`, :doc:`do_damage`,
    :doc:`reduce_hit_points`
  Schutz:    :doc:`../props/P_DEFENDERS`, :doc:`InformDefend`,
    :doc:`DefendOther`, :doc:`../props/P_ARMOURS`,
    :doc:`../props/P_AC`, :doc:`../props/P_DEFEND_FUNC`,
    :doc:`QueryDefend`, :doc:`../props/P_BODY`
  Daten:     :doc:`../props/P_LAST_COMBAT_TIME`,
    :doc:`../props/P_LAST_DAMTYPES`,
    :doc:`../props/P_LAST_DAMTIME`, :doc:`../props/P_LAST_DAMAGE`,
    :doc:`../props/P_DAMAGE_MSG`
  Resistenz: :doc:`../props/P_RESISTANCE_STRENGTHS`,
    :doc:`CheckResistance`
  Sonstiges: :doc:`CheckSensitiveAttack`,
    :doc:`InternalModifyDefend`,
    :doc:`UseSkill`,
    :doc:`DefendInfo`

Letzte Aenderung: 29.12.2017, Bugfix
