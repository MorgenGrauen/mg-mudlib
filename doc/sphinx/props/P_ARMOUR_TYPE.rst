P_ARMOUR_TYPE
=============

NAME
----
::

     P_ARMOUR_TYPE "armour_type"

DEFINIERT IN
------------
::

     <armour.h>

BESCHREIBUNG
------------
::

     In dieser Property wird der Typ einer Ruestung festgehalten. Man sollte
     hier nur die in <combat.h> definierten Konstanten verwenden:

        AT_AMULET     Amulett
        AT_ARMOUR     Ruestung
        AT_BELT       Guertel
        AT_BOOT       Schuhe
        AT_CLOAK      Umhang
        AT_GLOVE      Handschuhe
        AT_HELMET     Helm
        AT_QUIVER     Koecher
        AT_RING       Ring
        AT_SHIELD     Schild
        AT_TROUSERS   Hosen
        AT_MISC       Sonstiges

     Der Ruestungstyp AT_MISC ist schnoedem Tand und anderem nutzlosen
     Kram vorbehalten. Auf keinen Fall darf eine AT_MISC-Ruestung ueber
     eine AC > 0 verfuegen noch irgendwie kampfrelevante Bedeutung be-
     sitzen. Ruestungen des Typs AT_MISC, die KEINE DefendFunc benoetigen,
     muessen mittels /std/clothing als einfaches Kleidungsstueck realisiert
     werden.

BEMERKUNGEN
-----------
::

     Die P_AC wird bei AT_MISC-Ruestungen gar nicht erst ausgewertet.
     DefendFuncs werden zwar ausgewertet, aber bitte ueberlegt euch gut, ob
     ihr sie braucht (Rechenzeit im Kampf ist kritisch!) und ob sie seitens 
     der Balance in eurem Fall erlaubt sind.

SIEHE AUCH
----------
::

     Verwandt:          QueryArmourByType(L), P_WEAPON
     Ruestungen:        P_AC, P_NR_HANDS (Schilde)
     Sonstiges:         P_EQUIP_TIME, P_LAST_USE
     Code:              /std/armour.c, /std/clothing.c
     Gildenergaenzung:  P_GLOVE_FINGERLESS

27. Mai 2015, Arathorn

