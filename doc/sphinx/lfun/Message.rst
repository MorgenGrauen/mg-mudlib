Message()
=========

FUNKTION
--------
::

        varargs int Message(string str, int flag)

ARGUMENTE
---------
::

        str: Den zu uebermittelnden Text.
	flag: Beschreibt die Art der Meldung naeher, siehe
	      /sys/player/comm.h

BESCHREIBUNG
------------
::

        Einem Spieler wird der Text str uebermittelt.

RUECKGABEWERT
-------------
::

        1 bei erfolgreicher Uebermittlung
        0 wenn der Kobold aktiv war
       <0 wenn Spieler oder verwendetes Verb ignoriert
          werden

SIEHE AUCH
----------
::

     P_IGNORE, TestIgnore()

12. Mai 2004 Gloinson

