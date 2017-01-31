P_DEATH_SPONSORED_BY
====================

NAME
----
::

    P_DEATH_SPONSORED_BY          "responsible_wizard_for_death"

DEFINIERT IN
------------
::

    /sys/living/combat.h

BESCHREIBUNG
------------
::

    Diese Property hat fuer den Spielverlauf keinerlei Bedeutung.
    Doch gibt es Magier, die ihren Namen gern in /log/KILLS lesen.
    Wird ein Spieler durch einen Npc getoetet, ist normalerweise der
    Magier fuer den Tod verantwortlich, in dessen Verzeichnis sich 
    das Monster befindet. 
    Doch gibt es nun auch den Fall, dass sich das Monster in einem 
    Verzeichnis /alle/mon/ befindet, oder der Spieler durch eine
    Aktion im Raum oder an einem Objekt stirbt.

    Ist in einem solchen Monster, Raum oder Objekt diese Property
    gesetzt, wird der dort angebene String an das Log-File ueber-
    geben.

BEISPIEL
--------
::

    SetProp(P_DEATH_SPONSORED_BY,"tilly");

 

Last modified Don Feb 15 140100 2001 von Tilly
----------------------------------------------
::

