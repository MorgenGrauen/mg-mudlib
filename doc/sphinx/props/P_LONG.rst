P_LONG
======

NAME
----
::

     P_LONG					"long"

DEFINIERT IN
------------
::

     <thing/description.h>

BESCHREIBUNG
------------
::

     Die Langbeschreibung des Objektes als String oder Closure (diese muss
     einen String zurueckgeben). Die Langbeschreibung wird beim Untersuchen
     des Objektes ausgegeben. Sie sollte umgebrochen sein.

     Diese Property bestimmt die Ansicht des Objektes von aussen. Fuer die
     Innen(lang)ansicht von Raeumen muss man P_INT_LONG benutzen.

BEMERKUNGEN
-----------
::

     - long() ("untersuche") filtert P_LONG durch process_string()
       -> daher sind Closures moeglich

BEISPIELE
---------
::

     SetProp(P_LONG, "Diese Axt ist eine furchtbare Waffe!\n");

SIEHE AUCH
----------
::

     Aehnliches:	P_SHORT, long()
     Sonstiges:		P_INT_LONG, process_string(), break_string()


Last modified: Sun May 19 20:10:18 1996 by Wargon

