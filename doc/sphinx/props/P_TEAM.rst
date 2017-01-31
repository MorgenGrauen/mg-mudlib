P_TEAM
======

NAME
----
::

	P_TEAM                         "team"

DEFINIERT IN
------------
::

	/sys/living/team.h

BESCHREIBUNG
------------
::

	Liefert das Teamobjekt, falls Spieler in einem Team ist, sonst 0.

SIEHE AUCH
----------
::

        Uebersicht: teamkampf
        Properties: P_ASSOC_MEMBERS, P_TEAM_ATTACK_CMD, P_TEAM_AUTOFOLLOW,
                    P_TEAM_COLORS, P_TEAM_LEADER, P_TEAM_NEWMEMBER,
                    P_TEAM_WANTED_ROW, P_TEAM_WIMPY_ROW
        Bewegung:   IsTeamMove, TeamFlee
        Mitglieder: IsTeamLeader, TeamMembers
        Kampf:      DeAssocMember, InsertEnemyTeam, SelectNearEnemy,
                    SelectFarEnemy
        Positionen: PresentPosition, PresentRows, PresentEnemyRows,
                    PresentTeamPosition, SwapRows
        Sonstiges:  TeamPrefix, teamkampf_intern


Last modified: 16-08-2010, Gabylon

