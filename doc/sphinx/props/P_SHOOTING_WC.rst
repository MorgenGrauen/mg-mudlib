P_SHOOTING_WC
=============

NAME
----
::

    P_SHOOTING_WC     "shooting_wc"

DEFINIERT IN
------------
::

    <combat.h>

BESCHREIBUNG
------------
::

    Legt in einer Fernkampfwaffe UND ihrer Munition die Waffenstaerke fest.
    Bei einem Schuss wird die Summe kombiniert mit der Geschicklichkeit
    zur Berechnung der Angriffstaerke benutzt.

BEISPIELE
---------
::

    SetProp(P_SHOOTING_WC, 70);     // Langbogen
    SetProp(P_SHOOTING_WC, 50);     // Kurzbogen

    SetProp(P_SHOOTING_WC, 40);     // Bambuspfeil
    SetProp(P_SHOOTING_WC, 60);     // Aluminiumpfeil

    // So haetten Langbogen mit Aluminiumpfeil eine P_SHOOTING_WC von 70+60.

SIEHE AUCH
----------
::

    Generell:  P_AMMUNITION, P_STRETCH_TIME
    Methoden:  FindRangedTarget(L), shoot_dam(L), cmd_shoot(L)
    Gebiet:    P_RANGE, P_SHOOTING_AREA, P_TARGET_AREA
    Waffen:    P_WEAPON_TYPE, P_WC, P_EQUIP_TIME, P_NR_HANDS
    Kampf:     Attack(L), Defend(L), P_DISABLE_ATTACK, P_ATTACK_BUSY
    Sonstiges: fernwaffen

29.Jul 2014 Gloinson

