InFight()
=========

FUNKTION
--------
::

        mixed InFight()

ARGUMENTE
---------
::

        keine

BESCHREIBUNG
------------
::

        Diese Funktion wurde dazu geschrieben, um herauszufinden,
        ob sich ein PC/NPC direkt im Kampf befindet. Dazu wird
        das Array mit den Namen der Feinden des PC/NPCs durch die
        Funktion environment gefiltert und so festgestellt, ob
        die Umgebung der beiden Feinde gleich ist.

RUECKGABEWERT: 
        Als Rueckgabewert enthaelt man entweder 0, wenn das Objekt
        im Moment keine Feinde hat bzw. die nicht im selben Raum
        sind, oder aber das Feindobjekt, das als erstes im Array
        steht und anwesend ist.

BEISPIEL
--------
::

        Selbsterklaerend ;)

BEMERKUNG
---------
::

        InFight() gibt lediglich das Ergebnis von EnemyPresent() zurueck.

SIEHE AUCH
----------
::

        EnemyPresent(), PresentEnemies()
        /std/living/combat.c

22.03.2009, Zesstra

