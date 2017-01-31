DeAssocMember()
===============

FUNKTION
--------
::

        int DeAssocMember(object npc)

DEFINIERT IN
------------
::

        /std/living/team.c

ARGUMENTE
---------
::

        Der NPC, der nicht mehr zugeordnet sein soll.

BESCHREIBUNG
------------
::

        Hebt die Zuordnung eines NPCs zu einem Spieler auf.

RUECKGABEWERT
-------------
::

        1, falls Aufhebung erfolgreich, sonst 0.

BEISPIEL
--------
::

        void fun(object pl)
        {
         if ( pl && pl->DeAssocMember(this_object()) )
          tell_object(pl,break_string(
              "Ich kaempfe nun nicht mehr auf Deiner Seite!\n",78,
              "Ein Feuerteufel teilt Dir mit:");
         ...
        }

BEMERKUNGEN
-----------
::

        Kann nur von Gilden, Spellbooks, vom Objekt selber und vom
        zugeordneten NPC aufgerufen werden.

SIEHE AUCH
----------
::

        Uebersicht: teamkampf
        Properties: P_TEAM, P_ASSOC_MEMBERS, P_TEAM_ATTACK_CMD,
                    P_TEAM_AUTOFOLLOW, P_TEAM_COLORS, P_TEAM_LEADER,
                    P_TEAM_NEWMEMBER, P_TEAM_WANTED_ROW, P_TEAM_WIMPY_ROW
        Bewegung:   IsTeamMove, TeamFlee
        Mitglieder: IsTeamLeader, TeamMembers
        Kampf:      AssocMember, InsertEnemyTeam, SelectNearEnemy,
                    SelectFarEnemy
        Positionen: PresentPosition, PresentRows, PresentEnemyRows,
                    PresentTeamPosition, SwapRows
        Sonstiges:  TeamPrefix, teamkampf_intern


Last modified: 16-08-2010, Gabylon

