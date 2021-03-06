P_NEXT_DEATH_SEQUENCE
=====================

NAME
----
::

     P_NEXT_DEATH_SEQUENCE         "p_lib_next_death_sequence"

DEFINIERT IN
------------
::

     /sys/living/combat.h

BESCHREIBUNG
------------
::

     Im Spieler kann damit dessen eigene Todessequenz fuer den naechsten
     Tod festgelegt werden. Nach einem Tod (egal welche Todessequenz
     gewaehlt wurde) wird die Property geloescht und muesste neu gesetzt
     werden.

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

BEMERKUNGEN
-----------
::

     Eine Todessequenz eines Gegners, festgelegt ueber
     P_ENEMY_DEATH_SEQUENCE hat Vorrang vor dieser Property.

BEISPIELE
---------
::

     // Pfad zu einer eigenen DSQ
     SetProp(P_NEXT_DEATH_SEQUENCE,  ".../passende_dsq.txt");

     // eigene DSQ im Todesraumformat:
     SetProp(P_NEXT_DEATH_SEQUENCE,
             ({ 2, ([1: "Der Tod entlaesst dich eilig.\n"])}));

     // Einfuegen einer Meldung in die Standard-Todessequenz
     SetProp(P_NEXT_DEATH_SEQUENCE,
             ([5: "Du fuehlst dich etwas daemlich.\n"]));

SIEHE AUCH
----------
::

     Tod:            die(L)
     Todesmeldungen: P_KILL_NAME, P_KILL_MSG, P_DIE_MSG, P_MURDER_MSG
                     P_ZAP_MSG, P_ENEMY_DEATH_SEQUENCE
     Sonstiges:      P_CORPSE, P_NOCORPSE, /room/death/death_room.c

10. Nov 2011 Gloinson

