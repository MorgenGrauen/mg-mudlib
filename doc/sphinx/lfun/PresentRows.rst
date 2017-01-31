PresentRows()
=============

FUNKTION
--------
::

    mixed *PresentRows(object env);

DEFINIERT IN
------------
::

    TEAM_OBJECT (s. <living/team.h>)

ARGUMENTE
---------
::

    object env
        Das Environment des gewuenschten Objektes.

BESCHREIBUNG
------------
::

    Mit dieser Funktion bekommt man die aktuellen Teamreihen, die im Argument
    env anwesend sind, des Teams zurueckgegeben. Ist env kein Objekt, so
    wird environment(this_player()) als solches angenommen.

RUECKGABEWERT
-------------
::

    Es wird ein mixed-Array zurueckgegeben, dabei sind die einzelnen Reihen
    selbst wiederum Arrays mit den Spielerobjekten.

BEISPIEL
--------
::

    Ein NPC im Kampf laesst eine Feuerwalze auf ein Team los, welche aber nur
    Spieler in der ersten und zweiten Teamreihe Schaden zufuegen soll.

    void Attack( object enemy )
    {
     ...

     object team = enemy->QueryProp(P_TEAM);

     if ( objectp(team) )
      {
       mixed teamrows = team->PresentRows(enemy);

//  Inhalt von "teamrows" zu diesem Zeitpunkt:

//  ({ ({[/dwarf:hauweg]}),({}),({[/elf:spitzohr]}),({}),({}),({}) })

//  In der Umgebung von Zwerg Hauweg steht also noch Elf Spitzohr, und zwar
//  in der dritten Teamreihe (der hat Glueck gehabt).
//  Wenn dem Team noch mehr Spieler angehoeren, befinden sie sich gerade
//  nicht in der Umgebung (sprich im selben Raum) wie Hauweg.

       foreach ( i : 2 )
        {
         foreach ( object pl : teamrows[i] )
          {
           tell_object(pl,"Der Feuerteufel laesst eine Feuerwalze auf Dich "
               "und Dein Team los.\n");

           pl->Defend(200+random(200),({DT_FIRE}),([SP_SHOW_DAMAGE:1]),TO);
          }
        }
      }
     else
      {
       tell_object(enemy,"Der Feuerteufel laesst eine Feuerwalze auf Dich "
           "los.\n");

       enemy->Defend(200+random(200),({DT_FIRE}),([SP_SHOW_DAMAGE:1]),TO);
      }

     ...
    }

BEMERKUNG
---------
::

    Man beachte, dass das erste Argument (also Argument 0) die erste
    Teamreihe ist.

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
        Positionen: PresentPosition, PresentEnemyRows, PresentTeamPosition,
                    SwapRows
        Sonstiges:  TeamPrefix, teamkampf_intern


Last modified: 16-08-2010, Gabylon

