IsTeamLeader()
==============

FUNKTION
--------
::

        object IsTeamLeader()

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

        Testet, ob der Spieler ein Teamleiter ist.

RUECKGABEWERT
-------------
::

        Falls der Spieler Teamleiter ist, wird das Teamobjekt zurueckgegeben,
        sonst 0.

BEISPIEL
--------
::

        string leader_test(object pl)
        {
         ...

         if ( objectp(pl->IsTeamLeader()) )
          return "Als Leiter eines Teams hast Du grosse Verantwortung zu "
            "tragen!";
         else
          return "Wenn Du mal Leiter eines Teams sein moechtes, solltest "
            "Du Dir vorher der Verantwortung bewusst werden!";
        }

SIEHE AUCH
----------
::

        Uebersicht: teamkampf
        Properties: P_TEAM, P_ASSOC_MEMBERS, P_TEAM_ATTACK_CMD,
                    P_TEAM_AUTOFOLLOW, P_TEAM_COLORS, P_TEAM_LEADER,
                    P_TEAM_NEWMEMBER, P_TEAM_WANTED_ROW, P_TEAM_WIMPY_ROW
        Bewegung:   IsTeamMove, TeamFlee
        Mitglieder: TeamMembers
        Kampf:      AssocMember, DeAssocMember, InsertEnemyTeam,
                    SelectNearEnemy, SelectFarEnemy
        Positionen: PresentPosition, PresentRows, PresentEnemyRows,
                    PresentTeamPosition, SwapRows
        Sonstiges:  TeamPrefix, teamkampf_intern


Last modified: 16-08-2010, Gabylon

