InsertEnemyTeam()
=================

FUNKTION
--------
::

        varargs void InsertEnemyTeam(mixed ens, int rek)

DEFINIERT IN
------------
::

        /std/living/team.c

ARGUMENTE
---------
::

        ens     object Feind oder object *Feinde
        rek     Wurde von InsertEnemyTeam aufgerufen

BESCHREIBUNG
------------
::

        Alle Teammitglieder des Feindes bzw. die angegebenen Feinde (aber
        nicht deren Teammitglieder) werden zu Feinden
         a) des Spielers und aller Teammitglieder, falls rek==0
         b) des Spielers, falls rek!=0

RUECKGABEWERT
-------------
::

        keiner

BEMERKUNGEN
-----------
::

        1. Nur wenn Gegner und Teammitglied im gleichen Raum stehen (aber
           nicht notwendigerweise im gleichen Raum wie der Spieler) werden
           sie zu Feinden.
        2. Falls Teammitglied und Gegner beides Spieler sind, werden sie
           nicht zu Feinden gemacht.
        3. Ist der Gegner im eigenen Team, so wird nichts gemacht.

SIEHE AUCH
----------
::

        Uebersicht: teamkampf
        Properties: P_TEAM, P_ASSOC_MEMBERS, P_TEAM_ATTACK_CMD,
                    P_TEAM_AUTOFOLLOW, P_TEAM_COLORS, P_TEAM_LEADER,
                    P_TEAM_NEWMEMBER, P_TEAM_WANTED_ROW, P_TEAM_WIMPY_ROW
        Bewegung:   IsTeamMove, TeamFlee
        Mitglieder: IsTeamLeader, TeamMembers
        Kampf:      AssocMember, DeAssocMember, SelectNearEnemy,
                    SelectFarEnemy
        Positionen: PresentPosition, PresentRows, PresentEnemyRows,
                    PresentTeamPosition, SwapRows
        Sonstiges:  TeamPrefix, teamkampf_intern


Last modified: 16-08-2010, Gabylon

