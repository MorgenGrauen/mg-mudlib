PresentPosition()
=================

FUNKTION
--------
::

        varargs int PresentPosition(mixed pmap)

DEFINIERT IN
------------
::

        /std/living/team.c

ARGUMENTE
---------
::

        Rueckgabewert von PresentTeamRows() oder PresentTeamPositions()
        kann uebergeben werden.

BESCHREIBUNG
------------
::

        Ergibt die Nummer der Reihe, in der der Spieler gerade steht.

RUECKGABEWERT
-------------
::

        Reihennummer im Bereich von 1 bis TEAM_MAXROWS.

BEMERKUNGEN
-----------
::

        Ist ein Spieler in keinem Team, so steht er automatisch in Reihe 1.

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
        Positionen: PresentRows, PresentEnemyRows, PresentTeamPosition,
                    SwapRows
        Sonstiges:  TeamPrefix, teamkampf_intern


Last modified: 16-08-2010, Gabylon

