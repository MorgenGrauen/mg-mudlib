FindBestArmours()
=================

FUNKTION
--------
::

    varargs object *FindBestArmours(mixed type, int maxmon, int maxw,
                                    mapping minac, mixed restr)

 

DEFINIERT IN
------------
::

    /std/room/shop.c

 

ARGUMENTE
---------
::

    type   - gewuenschter Ruestungstyp / Ruestungstypen
    maxmon - Geld das ausgegeben werden darf
    maxw   - Maximales Gewicht
    minac  - minimale gewuenschte Ruestungsklasse pro Typ
    restr  - zusaetzliches Argument fuer CheckFindRestrictions()

BESCHREIBUNG
------------
::

    Sucht die besten Ruestungen, die der Laden verkaufen kann.

 

RUECKGABEWERT
-------------
::

    Die besten Ruestungen

 

BEMERKUNG
---------
::

    Die Qualitaet der Ruestung wird mit EvalArmour() bestimmt.
    Haben zwei Ruestungen die gleiche Qualitaet,
    wird die preiswertere genommen.

 

BEISPIEL
--------
::

    FindBestArmours(AT_ARMOUR,5000)
    Bestes Ruestung unter 5000 Muenzen.

    FindBestArmours(({AT_ARMOUR,AT_CLOAK,AT_BOOT}),10000,([AT_ARMOUR:20]))
    Finded beste Ruestung, Umhang und Schuhe, die der Laden fuer
    insgesamt 10000 Muenzen verkaufen kann, wobei die Ruestung mindestens
    AC 20 haben muss.

SIEHE AUCH
----------
::

    FindBestWeapon(), CheckFindRestrictions(), EvalArmour()

