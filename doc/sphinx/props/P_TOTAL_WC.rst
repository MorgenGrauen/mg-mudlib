P_TOTAL_WC
==========

NAME
----
::

	P_TOTAL_WC			"total_wc"

DEFINIERT IN
------------
::

	/sys/properties.h

BESCHREIBUNG
------------
::

	In dieser Property wird der numerische Wert der Angriffsstaerke
	eines Lebewesens registriert.
  Hierzu werden die P_WC von P_WEAPON bzw. P_HANDS sowie die Kraft des
  Lebewesens beruecksichtigt.
	Nicht eingerechnet in die Angriffsstaerke sind natuerlich Extraspells und
  -skills des Angreifers.
  Braucht man den Wert mehrfach kurz hintereinander, sollte man die Prop aber
  nur einmal abfragen und den Wert speichern (sofern sich in der Zwischenzeit
  nichts an der Waffe, den Hands oder den Attributen des Lebenwesens aendert).

BEMERKUNGEN
-----------
::

  Auf diese Property sollte nicht mittels Query() oder Set() zugegriffen 
  werden, das Setzen von Query- oder Setmethoden bitte auf jeden Fall 
  unterlassen.

SIEHE AUCH
----------
::

	P_HANDS, P_WC, P_XP


05.09.2008, Zesstra

