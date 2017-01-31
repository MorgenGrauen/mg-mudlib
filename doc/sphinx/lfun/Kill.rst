Kill()
======

FUNKTION
--------
::

	int Kill(object enemy)

ARGUMENTE
---------
::

	enemy: Der boese, boese Feind.

BESCHREIBUNG
------------
::

	Nach Aufruf der Funktion wird der Feind in jedem
	heart_beat() attackiert.

RUECKGABEWERT
-------------
::

	0,  Falls der Feind nicht existiert
  -1, falls der Feind ein Geist ist
  -2, falls der Feind P_NO_ATTACK gesetzt hat
  -3, falls der Angreifer selber P_NO_ATTACK gesetzt hat
  -4, falls der Feind bereits angegriffen wurde
	1   falls der Feind erfolgreich als Find registriert wurde.

BEMERKUNGEN
-----------
::

SIEHE AUCH
----------
::

	InsertEnemy

