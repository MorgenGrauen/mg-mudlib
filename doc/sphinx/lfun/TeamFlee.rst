TeamFlee()
==========

FUNKTION
--------
::

        int TeamFlee()

DEFINIERT IN
------------
::

        /std/living/team.c

ARGUMENTE
---------
::

        keine

BESCHREIBUNG
------------
::

        Spieler wird zur Flucht in hintere Reihe veranlasst, falls er dies
        eingestellt hat.

RUECKGABEWERT
-------------
::

        1, falls der Spieler in eine hintere Reihe fliehen wollte und nach
        dem Versuch nicht mehr in der ersten Reihe steht, sonst 0.

BEMERKUNGEN
-----------
::

        Beim Teamleiter fuehrt der Fluchtversuch dazu, dass sein Team nicht
        mehr folgt.

SIEHE AUCH
----------
::

        Uebersicht: teamkampf
        Properties: P_TEAM, P_ASSOC_MEMBERS, P_TEAM_ATTACK_CMD,
                    P_TEAM_AUTOFOLLOW, P_TEAM_COLORS, P_TEAM_LEADER,
                    P_TEAM_NEWMEMBER, P_TEAM_WANTED_ROW, P_TEAM_WIMPY_ROW
        Bewegung:   IsTeamMove
        Mitglieder: IsTeamLeader, TeamMembers
        Kampf:      AssocMember, DeAssocMember, InsertEnemyTeam,
                    SelectNearEnemy, SelectFarEnemy
        Positionen: PresentPosition, PresentRows, PresentEnemyRows,
                    PresentTeamPosition, SwapRows
        Sonstiges:  TeamPrefix, teamkampf_intern


Last modified: 16-08-2010, Gabylon

