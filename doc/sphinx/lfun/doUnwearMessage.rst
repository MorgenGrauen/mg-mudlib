doUnwearMessage()
=================

FUNKTION
--------
::

	void doUnwearMessage(object worn_by, int all);

DEFINIERT IN
------------
::

	/std/armour/combat.c

ARGUMENTE
---------
::

	worn_by
          Das Lebewesen, dass die Ruestung bisher getragen hat
        all
          Wurde "ziehe alles aus"/"ziehe alle ruestungen aus" eingegeben,
          so ist all gesetzt, ansonsten nicht.

BESCHREIBUNG
------------
::

        Diese Funktion gibt beim Ausziehen einer Ruestung die entspr.
        Meldung an den Traeger und die Umgebung aus.

RUeCKGABEWERT
-------------
::

	keiner

SIEHE AUCH
----------
::

        doWearMessage(), doWieldMessage(), doUnwieldMessage()


Last modified: Sat Jun 26 15:41:00 1999 by Paracelsus

