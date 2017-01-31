set_object_next_reset()
=======================

FUNKTION
--------
::

	int set_object_next_reset(object ob, int zeit );

DEFINIERT IN: /secure/debug.c

ARGUMENTE
---------
::

	ob: Das Objekt, dess Reset gesetzt werden soll.
  zeit: Zeit bis zum naechsten Reset von ob.

FUNKTION
--------
::

  Die Funktion ruft letztendlich set_next_reset() in <ob> auf und setzt daher
  dessen Resetzeit auf einen neuen Wert. Dies ist zu Debugzwecken gedacht.

  Die Benutzung ist nur fuer EM+ moeglich.

RUECKGABEWERT
-------------
::

	Gibt den Rueckgabewert des set_next_reset()-Aufrufs in <ob> zurueck.

SIEHE AUCH
----------
::

	set_next_reset(E)

10.10.2007, Zesstra

