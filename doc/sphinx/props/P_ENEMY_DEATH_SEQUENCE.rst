P_ENEMY_DEATH_SEQUENCE
======================

NAME
----
::

     P_ENEMY_DEATH_SEQUENCE        "enemy_death_sequence"

DEFINIERT IN
------------
::

     /sys/living/combat.h

BESCHREIBUNG
------------
::

     Ueber diese Property kann Einfluss auf die Todessequenz eines getoeten
     Spielers genommen werden. Sie muss im toetenden Objekt, d.h. dem
     Objekt welches die()/do_damage()/Defend() im Spieler gerufen hat,
     gesetzt sein.

     Es gibt folgende gueltige Werte:
     - string: Pfad zu einer eigenen Todessequenz im gueltigen Format
     - mixed*  Eine Todessequenz im Format des Todesraumes:
               ({<int gesamtlaenge>,
                 ([<int index1>: <string umgebrochene Meldung1>,
                   <int index2>: <string umgebrochene Meldung2>,
                   ...])
               })
     - mapping In die Standard-Lars-Todessequenz einzufuegende Zeilen:
               ([<int zeitindex>: <string umgebrochener Text>])

BEISPIELE
---------
::

     // Pfad zu einer eigenen DSQ
     SetProp(P_ENEMY_DEATH_SEQUENCE,  ".../passende_dsq.txt");

     // eigene DSQ im Todesraumformat:
     SetProp(P_ENEMY_DEATH_SEQUENCE,
             ({ 2, ([1: "DU LERNST AUS DEINEM FEHLER.\n"])}));

     // Einfuegen einer Meldung (des NPCs) in die Standard-Todessequenz
     SetProp(P_ENEMY_DEATH_SEQUENCE,
             ([5: "Du hoerst "+name(WEN)+" hoehnisch kichern.\n"]));

SIEHE AUCH
----------
::

     Tod:            die(L)
     Todesmeldungen: P_KILL_NAME, P_KILL_MSG, P_DIE_MSG, P_MURDER_MSG
                     P_ZAP_MSG, P_NEXT_DEATH_SEQUENCE
     Sonstiges:      P_CORPSE, P_NOCORPSE, /room/death/death_room.c

10. Nov 2011 Gloinson

