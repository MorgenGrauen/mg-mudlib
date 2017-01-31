TeamPrefix()
============

FUNKTION
--------
::

        string TeamPrefix()

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

        Ergibt Team-Prefix eines Spielers.

RUECKGABEWERT
-------------
::

        "[Team Teamname] ", falls der Spieler in einem Team ist,
        ""                  sonst.

BEMERKUNGEN
-----------
::

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
        Positionen: PresentPosition, PresentRows, PresentEnemyRows,
                    PresentTeamPosition, SwapRows
        Sonstiges:  teamkampf_intern


Last modified: 16-08-2010, Gabylon

