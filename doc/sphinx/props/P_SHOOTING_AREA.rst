P_SHOOTING_AREA
===============

NAME
----
::

    P_SHOOTING_AREA     "shooting_area"

DEFINIERT IN
------------
::

    <combat.h>

BESCHREIBUNG
------------
::

    Legt die 'Groesse' eines Raumes fest. Ein Schuetze kann mit seiner
    Fernkampfwaffe nur dann aus diesem Raum in einen anderen Raum schiessen,
    wenn die P_RANGE seiner Waffe mindestens gleich ist.

    Erreichbare Raeume sind entweder das environment() oder der in
    P_SHOOTING_AREA festgelegte Raum.

SIEHE AUCH
----------
::

    Generell:  P_AMMUNITION, P_SHOOTING_WC, P_STRETCH_TIME
    Methoden:  FindRangedTarget(L), shoot_dam(L), cmd_shoot(L)
    Gebiet:    P_RANGE, P_TARGET_AREA
    Raeume:    P_NEVER_CLEAN
    Sonstiges: fernwaffen

29.Jul 2014 Gloinson

