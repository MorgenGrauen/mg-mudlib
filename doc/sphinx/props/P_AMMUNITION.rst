P_AMMUNITION
============

NAME
----
::

    P_AMMUNITION     "munition"

DEFINIERT IN
------------
::

    <combat.h>

BESCHREIBUNG
------------
::

    Enthaelt die fuer eine Waffe gueltige Munition als String. Die
    Munition muss diesen String als ID besitzen.

    Fuer die Munitionsobjekte gilt:
    * es kann ein Skill am Spieler definiert werden, dieser wirkt dann
      zusaetzlich zum generellen SK_SHOOT-Skill.
    * sie koennen eine HitFunc besitzten, die beim Schuss abgefragt wird

BEMERKUNGEN
-----------
::

    Bitte das #define MUN_MISC(x) benutzen. Munition sollte auch immer
    in Deutsch und Plural angegeben werden, da P_AMMUNITION direkt
    fuer Ausgaben an den Spieler ausgewertet wird.

    Momentan sind vier Munitionsarten in der combat.h vordefiniert:
    MUN_ARROW, MUN_STONE, MUN_BOLT, MUN_MISC

BEISPIELE
---------
::

    // fuer ein kleines Blasrohr im Blasrohr
    SetProp(P_AMMUNITION, MUN_MISC("Erbsen"));

    // Entsprechend in der Munition:
    AddId(MUN_MISC("Erbsen"));

SIEHE AUCH
----------
::

    Generell:  P_SHOOTING_WC, P_STRETCH_TIME
    Methoden:  FindRangedTarget(L), shoot_dam(L), cmd_shoot(L)
    Gebiet:    P_RANGE, P_SHOOTING_AREA, P_TARGET_AREA
    Waffen:    P_WEAPON_TYPE, P_WC, P_EQUIP_TIME, P_NR_HANDS
    Kampf:     Attack(L), Defend(L), P_DISABLE_ATTACK, P_ATTACK_BUSY
    Team:      PresentPosition(L)
    Sonstiges: fernwaffen

29.Jul 2014 Gloinson

