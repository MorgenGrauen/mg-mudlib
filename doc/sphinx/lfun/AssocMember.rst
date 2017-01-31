AssocMember()
=============

FUNKTION
--------
::

        int AssocMember(object npc)

DEFINIERT IN
------------
::

        /std/living/team.c

ARGUMENTE
---------
::

        Der zuzuordnende NPC.

BESCHREIBUNG
------------
::

        Ordnet einen NPC einem Spieler zu. Dieser ist dann
        immer im Team des Spielers, moeglichst in der gleichen Reihe.

RUECKGABEWERT
-------------
::

        1, falls Zuordnung erfolgreich, sonst 0.

BEISPIEL
--------
::

        void fun(object pl)
        {
          if ( pl && pl->AssocMember(this_object()) )
            tell_object(pl,break_string(
              "Ich kaempfe nun auf Deiner Seite!\n",78,"Ein Feuerteufel "
              "teilt Dir mit:");
         ...
        }

BEMERKUNGEN
-----------
::

        1. Kann nur von Gilden, Spellbooks, vom Objekt selber und vom
           zuzuordnenden NPC aufgerufen werden.
        2. Einem NPC, der selber einem Spieler zugeordnet ist, kann kein
           NPC zugeordnet werden.

SIEHE AUCH
----------
::

        Uebersicht: teamkampf
        Properties: P_TEAM, P_TEAM_ASSOC_MEMBERS, P_TEAM_ATTACK_CMD,
                    P_TEAM_AUTOFOLLOW, P_TEAM_COLORS, P_TEAM_LEADER,
                    P_TEAM_NEWMEMBER, P_TEAM_WANTED_ROW, P_TEAM_WIMPY_ROW
        Bewegung:   IsTeamMove, TeamFlee
        Mitglieder: IsTeamLeader, TeamMembers
        Kampf:      DeAssocMember, InsertEnemyTeam, SelectNearEnemy,
                    SelectFarEnemy
        Positionen: PresentPosition, PresentRows, PresentEnemyRows,
                    PresentTeamPosition, SwapRows
        Sonstiges:  TeamPrefix, teamkampf_intern


Last modified: 04.10.2011, Zesstra

