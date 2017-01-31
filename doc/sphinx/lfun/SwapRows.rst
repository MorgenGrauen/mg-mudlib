SwapRows()
==========

FUNKTION
--------
::

    int SwapRows( object ob1, object ob2 )

DEFINIERT IN
------------
::

    /std/living/team.c

ARGUMENTE
---------
::

    ob1, ob2 - Spieler, die die Reihen tauschen sollen.

BESCHREIBUNG
------------
::

    Die angegebenen Spieler tauschen die Reihen.

RUECKGABEWERT
-------------
::

    1 bei erfolgreichem Tausch, 0 sonst.

BEMERKUNG
---------
::

    Der Tausch wird nur durchgefuehrt, wenn die angegebenen Spieler auch
    tatsaechlich im Team sind und damit kein NPC vor einen Spieler gestellt
    wuerde.
    Moechte man wissen, ob ein Spieler eine Reihe gewechselt hat, muss man
    sich der Hilfe eines Hooks bedienen: H_HOOK_TEAMROWCHANGE

SIEHE AUCH
----------
::

        Uebersicht: teamkampf
        Properties: P_TEAM, P_TEAM_LEADER, P_TEAM_ASSOC_MEMBERS,
                    P_TEAM_NEWMEMBER
        Bewegung:   IsTeamMove, TeamFlee
        Mitglieder: IsTeamLeader, TeamMembers
        Kampf:      AssocMember, DeAssocMember, InsertEnemyTeam
                    SelectNearEnemy, SelectFarEnemy
        Positionen: PresentPosition, PresentRows, PresentEnemyRows,
                    PresentTeamPosition
        Sonstiges:  TeamPrefix, teamkampf_intern


Last modified: 16-08-2010, Gabylon

