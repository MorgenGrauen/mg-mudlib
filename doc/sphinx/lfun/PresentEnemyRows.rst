PresentEnemyRows()
==================

FUNKTION
--------
::

        varargs mixed *PresentEnemyRows(object *here)

DEFINIERT IN
------------
::

        /std/living/team.c

ARGUMENTE
---------
::

        Rueckgabewert von PresentEnemies kann uebergeben werden.

BESCHREIBUNG
------------
::

        Ergibt die feindlichen Reihen.

RUECKGABEWERT
-------------
::

        Ein Array, bestehend aus MAX_TEAMROWS Arrays mit den Objekten
        der anwesenden Feinde.

BEMERKUNGEN
-----------
::

        Jede Reihe ist Summe der entsprechenden Reihen der
        anwesenden Teams.
        Feinde, die in keinem Team sind, stehen im ersten Array.

SIEHE AUCH
----------
::

        Uebersicht: teamkampf
        Properties: P_TEAM, P_ASSOC_MEMBERS, P_TEAM_ATTACK_CMD,
                    P_TEAM_AUTOFOLLOW, P_TEAM_COLORS, P_TEAM_LEADER,
                    P_TEAM_NEWMEMBER, P_TEAM_WANTED_ROW, P_TEAM_WIMPY_ROW
        Bewegung:   IsTeamMove, TeamFlee
        Mitglieder: IsTeamLeader, TeamMembers
        Kampf:      AssocMember, DeAssocMember, InsertEnemyTeam,
                    SelectNearEnemy, SelectFarEnemy
        Positionen: PresentPosition, PresentRows, PresentTeamPosition,
                    SwapRows
        Sonstiges:  TeamPrefix, teamkampf_intern


Last modified: 16-08-2010, Gabylon

