P_INT_SHORT
===========

NAME
----
::

     P_INT_SHORT			"int_short"

DEFINIERT IN
------------
::

     /sys/thing/description.h

BESCHREIBUNG
------------
::

     Diese Property enthaelt die Innen-Kurzbeschreibung des Containers
     als String oder Closure (mit String-Rueckgabewert).

     Container sind hierbei z.B. Raeume.
     ACHTUNG: Die Kurzbeschreibung sollte dabei nicht  mit einem
	      "\n" abgeschlossen sein
	      (dies wird von den zustaendigen Funktionen erledigt).
     
     Aus historischen Gruenden wird ein Punkt ergaenzt, wenn das letzte
     Zeichen kein Punkt, Ausrufezeichen oder Fragezeichen ist.

     Man sollte die Property nicht auf 0 setzen.

     Diese Property bestimmt die Ansicht des Containers von innen.
     Fuer die Aussen(kurz)ansicht muss P_SHORT benutzt werden.

BEMERKUNGEN
-----------
::

     - int_short() (bei Bewegung) filtert P_INT_SHORT durch process_string()
       -> daher sind Closures moeglich

BEISPIELE
---------
::

     // ein Gang sieht natuerlich so aus
     SetProp(P_INT_SHORT, "Ein Gang");

SIEHE AUCH
----------
::

     Aehnliches:	P_INT_LONG
     Sonstiges:		int_short(), P_SHORT
			process_string(), break_string()


Last modified: Thu May 31 15:34:05 2001 by Patryn

