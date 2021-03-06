ModifyQuestTime()
=================

FUNKTION
--------
::

        int ModifyQuestTime(string questname, int zeit)

DEFINIERT IN
------------
::

        /std/player/quests.c

ARGUMENTE
---------
::

        questname
          Questname, wie er im Questmaster eingetragen wurde.
        zeit
          Zeitpunkt, zu dem die Quest geloest wurde

RUeCKGABEWERT
-------------
::

         1 : Questzeitpunkt erfolgreich nachgetragen
        -1 : keine Zugriffsrechte (nur EM+ und der Tagebuchmaster erlaubt)
        -2 : Questliste im Spielerobjekt ist unerwartet kein Mapping
        -3 : Spieler hat diese Quest noch nicht bestanden
        -4 : kein gueltiger Zeitpunkt uebergeben (kein Integer, Wert < -1
             oder Wert > time()).

BESCHREIBUNG
------------
::

        Mit dieser Funktion kann der Zeitpunkt nachgetragen werden, zu
        dem ein Spieler eine bestimmte Quest bereits geloest hat. 
        Als Questname wird dazu der Name angegeben, der im Questmaster 
        eingetragen ist. Der Zeitpunkt ist als Integer-Wert im ueblichen
        time()-Format anzugeben. Uebergibt man -1 als <zeit>, dann wird
        der Tagebuchmaster erneut versuchen, das Logfile einzulesen, 
        wenn der Spieler das naechste mal sein Abenteuertagebuch liest.

HINWEIS
-------
::

        Die Funktion mktime() ist nuetzlich, wenn der Zeitpunkt des 
        Bestehens einer Quest manuell rekonstruiert werden muss, der
        Zeitpunkt aber nur als Datums-/Zeitangabe in Textform vorliegt 
        (etwa aus einem Logfile oder aus der Quest-Feedbackmail).

SIEHE AUCH
----------
::

        /secure/questmaster.h, /obj/tools/questtool
        GiveQuest(L)
        mktime(E), dtime(SE)


Zuletzt geaendert: Mon, 27. Jan. 2015, Arathorn

