make_invlist()
==============

FUNKTION
--------
::

     varargs mixed make_invlist(object viewer, mixed inv, int flags)

DEFINIERT IN
------------
::

     /std/container/description.c

ARGUMENTE
---------
::

     object viewer	- das Objekt, welches sich den Inhalt ansieht (in
			  der Regel this_player())
     mixed inv		- ein Array von Objekten, die in die Liste
			  aufgenommen werden sollen
     int flags		- Folgende Werte koennen verODERt werden:
			  1: das Inv nicht als String, sondern als ein
			     Array zurueckgeben
			  2: gleiche Objekte nicht zusammengefassen
			  4: unterdrueckt die Ausgabe des Dateinamens hinter
			     jedem trotz eingeschaltetem Magiermodus

BESCHREIBUNG
------------
::

     Die Kurzbeschreibungen der Objekte in inv werden zu einer Liste
     zusammengefasst (eine Zeile pro Objekt). Unsichtbare Objekte tauchen in
     dieser Liste nicht auf.

     Ist viewer ein Magier mit eingeschaltetem Magiermodus, so wird hinter
     die Kurzbeschreibungen noch der Dateiname des jeweiligen Objektes in
     eckigen Klammern angegeben. Unsichtbare Objekte erscheinen in diesem
     Fall als Filenamen.

RUeCKGABEWERT
-------------
::

     Ein String oder ein Array, die das inv beschreiben.

SIEHE AUCH
----------
::

     Kommandokette:	short()
     Properties:	P_SHORT, P_INVIS, P_WANTS_TO_LEARN
     Sonstiges:		P_REFERENCE_OBJECT

Last modified: Tue Oct 15 10:10:00 2002 by Rikus

