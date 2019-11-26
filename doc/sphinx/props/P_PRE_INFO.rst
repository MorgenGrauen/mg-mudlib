P_PRE_INFO
==========

NAME
----
::

        P_PRE_INFO                        "npc_pre_info"

DEFINIERT IN
------------
::

        /sys/npc.h

BESCHREIBUNG
------------
::

        Ist die Property in einem NPC definiert, so wird ihr Ergebnis
        ausgewertet, bevor eine Frage an das Infosystem uebergeben wird.

        Moegliche Werte:
        - numerischer Wert > 0
          => der NPC gibt keinerlei Antwort, die Frage fuehrt sozusagen
             ins Leere

        - Stringwert
          => wird als Rueckgabe an den Fragenden ausgegeben, umstehende 
             Personen bekommen den Text:
            "XY ist nicht gewillt, Spieler YZ zu antworten".
            Der Fragende selbst bekommt bei angegebenem Stringwert:
            "XY " + Stringwert.

        - Wenn eine als Querymethode gesetzte Closure einen String oder
          einen Integer > 0 zurueckgibt, sind die  Auswirkungen identisch
          zu den beiden oben beschriebenen Faellen. Siehe auch Beispiel
          unten.


BEISPIEL
--------
::

        Ein NPC der manchmal herumrennt, um z.B. eine Aufgabe zu verrichten,
        koennte so lange Chats abschalten, z.B.

          SetProp(P_CHAT_CHANCE,0); // NPC latscht los

        Und eine Weile spaeter:

          SetProp(P_CHAT_CHANCE,5); // NPC ruht wieder, quasselt rum

        Waehrend des Herumlaufens, also wenn er nicht automatisch schwatzt,
        soll er auch keinerlei Fragen beantworten:

          Set(P_PRE_INFO, function string () {
            return (QueryProp(P_CHAT_CHANCE) ? 0 : 
              "hat gerade keine Zeit fuer Dich."); 
            }, F_QUERY_METHOD);

HINWEISE
--------
::

        Bitte beachten, dass der interne Name der Property "npc_pre_info" 
        ist und somit nur das Ueberschreiben von _query_npc_pre_info() 
        funktioniert. 

SIEHE AUCH
----------
::

        AddInfo(), /std/npc/info.c


Last modified: 2019-Okt-16 by Arathorn

