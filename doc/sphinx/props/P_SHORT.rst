P_SHORT
=======

NAME
----
::

     P_SHORT				"short"

DEFINIERT IN
------------
::

     /sys/thing/description.h

BESCHREIBUNG
------------
::

     Diese Property enthaelt die Kurzbeschreibung des Objektes als String 
     oder Closure (diese muss einen String zurueckgeben).

     ACHTUNG: Die Kurzbeschreibung sollte dabei nicht mit einem "\n" 
     abgeschlossen sein (dies wird von den zustaendigen Funktionen
     erledigt).
     Aus historischen Gruenden wird ein Punkt ergaenzt, wenn das letzte
     Zeichen kein Punkt, Ausrufezeichen oder Fragezeichen ist.

     Setzt man diese Property auf 0, so ist das Objekt unsichtbar, allerdings
     ansprechbar, wenn der Spieler eine ID des Objektes kennt. D.h. Objekte
     koennen mitgenommen, weggeworfen oder ggf. auch angegriffen werden. Will
     man dies nicht, sollte man das Objekt mit P_INVIS unsichtbar machen.

     Diese Property bestimmt die Ansicht des Objektes von aussen. Fuer die
     Innen(kurz)ansicht von Raeumen muss man P_INT_LONG benutzen.

BEMERKUNGEN
-----------
::

     Die Funktion, die die Kurzbeschreibung ausgibt (short()), filtert P_SHORT
     durch process_string(). Von der Nutzung dieses Features wird in neuem
     Code abgeraten.
     Soll eine dyn. Kurzbeschreibung geschaffen werden, bitte eine
     F_QUERY_METHOD einsetzen oder short() passend ueberschreiben.

BEISPIELE
---------
::

     // eine Axt sieht natuerlich so aus:
     SetProp(P_SHORT, "Eine Axt");

SIEHE AUCH
----------
::

     Aehnliches:	P_LONG, short()
     Sonstiges:		P_INT_SHORT, process_string()


27.05.2015, Zesstra

