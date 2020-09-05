GiveMiniQuest()
===============

FUNKTION
--------
::

        int GiveMiniQuest(object winner)

DEFINIERT IN
------------
::

        /secure/questmaster

ARGUMENTE
---------
::

	winner
	  Spielerobjekt, das die Quest bestanden hat.

RUeCKGABEWERT
-------------
::

        (Die Defines fuer den Rueckgabewert finden sich in 
         /secure/questmaster.h)
         1 : Hat geklappt                           (OK)
        -1 : Spieler hat die Quest bereits geloest  (MQ_ALREADY_SET)
        -2 : Ungueltiger Questname                  (MQ_KEY_INVALID)
        -3 : Unbefugter Zugriff                     (MQ_ILLEGAL_OBJ)
        -4 : Quest zur Zeit inaktiv                 (MQ_IS_INACTIVE)
        -5 : Gaeste loesen keine Quests             (MQ_GUEST)

BESCHREIBUNG
------------
::

    Mit dieser Funktion wird nach dem erfolgreichen Loesen einer 
    MiniQuest die Quest im Spieler eingetragen. Dabei muss der
    Aufruf in dem Objekt erfolgen, welches im Questmaster 
    eingetragen ist. Als Argument muss das Spielerobjekt 
    angegeben werden, welches die Quest bestanden hat.

BEISPIEL
--------
::

	QM->GiveMiniQuest(this_player());

SIEHE AUCH
----------
::

        HasMiniQuest(), AddMiniQuest()
        /secure/questmaster.h, /secure/questmaster.c

HINWEIS
-------
::

        Die Informationen fuer den EM fuer Quests sollten diesem
        beim Eintragen der Miniquest in den Questmaster per Mail
        zugeschickt werden. Siehe dazu die Hilfe zu AddMiniQuest.


Zuletzt geaendert: Don, 30. Jan 2014, 22:14:12 von Ark.

