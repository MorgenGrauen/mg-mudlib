QueryQuest()
============

FUNKTION
--------
::

        int QueryQuest(string questname)

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

        (Die Defines fuer den Rueckgabewert finden sich in 
         /secure/questmaster.h)
         1 : Spieler hat die Quest bereits geloest  (OK)
         0 : Ungueltiger Questname oder der Spieler
             hat die Quest noch nicht.              (QQ_KEY_INVALID)
         2 : Gaeste koennen keine Quest loesen      (QQ_GUEST)

BESCHREIBUNG
------------
::

	Mit dieser Funktion kann getestet werden, ob ein Spieler eine 
        bestimmte Quest bereits geloest hat. Als Questname wird dazu
        der Name angegeben, der im Questmaster eingetragen wurde.

        Wer sich da nicht sicher ist, kann mit dem Questtool 
        (/obj/tools/questtool) nachsehen. 

SIEHE AUCH
----------
::

        /secure/questmaster.h, /obj/tools/questtool
        GiveQuest


Zuletzt geaendert: Mon, 17. Jul 2000, 12:16:41 von Zook.

