IsTeamMove()
============

FUNKTION
--------
::

        object IsTeamMove()

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

        Testet, ob momentan die Angriffsbewegung ausgefuehrt wird.

RUECKGABEWERT
-------------
::

        Falls die Angriffsbewegung gerade ausgefuehrt wird, wird das
        Teamobjekt zurueckgegeben, sonst 0.

BEMERKUNGEN
-----------
::

        Wird intern benoetigt, damit der Begruessungsschlag verzoegert
        werden kann, bis alle Teammitglieder ihre Angriffsbewegung
        ausgefuehrt haben.

SIEHE AUCH
----------
::

        Uebersicht: teamkampf
        Properties: P_TEAM, P_ASSOC_MEMBERS, P_TEAM_ATTACK_CMD,
                    P_TEAM_AUTOFOLLOW, P_TEAM_COLORS, P_TEAM_LEADER,
                    P_TEAM_NEWMEMBER, P_TEAM_WANTED_ROW, P_TEAM_WIMPY_ROW
        Bewegung:   TeamFlee
        Mitglieder: IsTeamLeader, TeamMembers
        Kampf:      AssocMember, DeAssocMember, InsertEnemyTeam,
                    SelectNearEnemy, SelectFarEnemy
        Positionen: PresentPosition, PresentRows, PresentEnemyRows,
                    PresentTeamPosition, SwapRows
        Sonstiges:  TeamPrefix, teamkampf_intern


Last modified: 16-08-2010, Gabylon

