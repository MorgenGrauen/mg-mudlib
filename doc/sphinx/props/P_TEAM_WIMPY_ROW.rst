P_TEAM_WIMPY_ROW
================

NAME
----
::

	P_TEAM_WIMPY_ROW               "team_wimpy_row"

DEFINIERT IN
------------
::

	/sys/living/team.h

BESCHREIBUNG
------------
::

	Fluchtreihe des Spielers, von 1 bis MAX_TEAMROWS.

BEMERKUNG
---------
::

	Wenn die Fluchtreihe <=1 ist, ist die Flucht in eine hintere Reihe
	deaktiviert.

SIEHE AUCH
----------
::

        Uebersicht: teamkampf
        Properties: P_ASSOC_MEMBERS, P_TEAM_ATTACK_CMD, P_TEAM_AUTOFOLLOW,
                    P_TEAM_COLORS, P_TEAM_LEADER, P_TEAM_NEWMEMBER,
                    P_TEAM_WANTED_ROW
        Bewegung:   IsTeamMove, TeamFlee
        Mitglieder: IsTeamLeader, TeamMembers
        Kampf:      DeAssocMember, InsertEnemyTeam, SelectNearEnemy,
                    SelectFarEnemy
        Positionen: PresentPosition, PresentRows, PresentEnemyRows,
                    PresentTeamPosition, SwapRows
        Sonstiges:  TeamPrefix, teamkampf_intern


Last modified: 16-08-2010, Gabylon

