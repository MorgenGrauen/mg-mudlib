FindRangedTarget()
==================

FUNKTION
--------
::

    static string FindRangedTarget(string str, mapping shoot)

DEFINIERT IN
------------
::

    /std/ranged_weapon.c

ARGUMENTE
---------
::

    string str    - Schusssyntax
    mapping shoot - Schussdaten

BESCHREIBUNG
------------
::

    Erhaelt von /std/ranged_weapon::cmd_shoot() die Schussdaten und eine
    eventuell bereits modifizierte Syntax und versucht einen passenden Gegner
    im Raum oder im Gebiet (P_SHOOTING_AREA) zu finden.
    Dieser wird in SI_ENEMY im Mapping 'shoot' eingetragen und ein Wert != 0
    zurueckgegeben.

RUECKGABEWERT
-------------
::

    0     bei Fehlschlag
    != 0  bei gueltigem SI_ENEMY in 'shoot'

BEMERKUNGEN
-----------
::

    'shoot' enthaelt normalerweise folgende Eintraege:
    * Key P_WEAPON:       die Schusswaffe
    * Key P_WEAPON_TYPE:  P_AMMUNITION, also die Munitions-ID
    * Key P_STRETCH_TIME: P_STRETCH_TIME der Waffe
    * Key P_WC:           P_SHOOTING_WC der Waffe

SIEHE AUCH
----------
::

    Generell:  P_AMMUNITION, P_SHOOTING_WC, P_STRETCH_TIME
    Methoden:  shoot_dam(L), cmd_shoot(L)
    Gebiet:    P_RANGE, P_SHOOTING_AREA, P_TARGET_AREA
    Team:      PresentPosition(L)
    Suche:     present, SelectFarEnemy(L)
    Syntax:    _unparsed_args(L)
    Sonstiges: fernwaffen

28.Jul 2014 Gloinson

