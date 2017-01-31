SelectFarEnemy()
================

FUNKTION
--------
::

        varargs object SelectFarEnemy(object *here, int min, int max,
                                      int forcefrom)

DEFINIERT IN
------------
::

        /std/living/team.c

ARGUMENTE
---------
::

        here       - Rueckgabewert von PresentEnemies()
        min        - minimale Kampfreihe
        max        - maximale Kampfreihe
        forcefrom  - Gegner MUSS aus uebergebenem Array ausgewaehlt werden

BESCHREIBUNG
------------
::

        Waehlt einen Feind aus Reihe <min> bis <max> aus.

RUECKGABEWERT
-------------
::

        Der Auserwaehlte :-)

BEMERKUNGEN
-----------
::

        1. Wenn in den angegeben Reihen kein Feind ist oder <max> kleiner
           als <min> ist, wird auch keiner zurueckgegeben.
        2. Aus Effizienzgruenden sollte forcefrom nur benutzt werden, wenn
           es wirklich noetig ist (s. hierzu auch SelectNearEnemy()).
        3. 0 <= <min> <= <max> < MAX_TEAMROWS

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
                    SelectNearEnemy
        Positionen: PresentPosition, PresentRows, PresentEnemyRows,
                    PresentTeamPosition, SwapRows
        Sonstiges:  TeamPrefix, teamkampf_intern


Last modified: 16-08-2010, Gabylon

