QueryQuestTime()
================

FUNKTION
--------
::

        int QueryQuestTime(string questname)

DEFINIERT IN
------------
::

        /std/player/quests.c

ARGUMENTE
---------
::

        questname
          Questname, wie er im Questmaster eingetragen wurde.

RUeCKGABEWERT
-------------
::

        Questzeitpunkt in Sekunden seit Epoch als positiver Integer-Wert
         0 : Tagebuchmaster hat keine Daten in den Logfiles gefunden und
             macht das auf diese Weise kenntlich
        -1 : Questzeitpunkt unbekannt

BESCHREIBUNG
------------
::

        Mit dieser Funktion kann der Zeitpunkt abgefragt werden, zu
        dem ein Spieler eine bestimmte Quest geloest hat. 
        Als Questname wird dazu der Name angegeben, der im Questmaster 
        eingetragen ist.

SIEHE AUCH
----------
::

        GiveQuest(L), QueryQuest(L), ModifyQuestTime(L)


Zuletzt geaendert: 19. Dez. 2015, Arathorn

