remove_multiple()
=================

FUNKTION
--------
::

     public varargs int remove_multiple(int limit, mixed fun);

DEFINIERT IN
------------
::

     /std/container/items.c

ARGUMENTE
---------
::

     limit: Anzahl gleicher Objekte, die verbleiben sollen.
     fun:   Funktionsname (string) oder Closure.

BESCHREIBUNG
------------
::

     Wird diese Funktion aufgerufen, werden alle gleichen Objekte
     (standardmaessig definiert als gleicher Ladename, gleiche
      Kurzbeschreibung und gleiche Langbeschreibung), deren Anzahl <limit>
     ueberschreitet, entfernt.

     

     Ausnahmen: Lebewesen und per AddItem() hinzugefuegte Objekte.

     Mit dem Argument <fun> lassen sich die Kriterien fuer die "Gleichheit"
     aendern.
     Ist <fun> ein Funktionsname, wird diese Funktion an allen in Frage
     kommenenden Objekten im Container gerufen.
     Ist <fun> eine Closure, werden sie fuer jedes in Frage kommende Objekt
     einmal gerufen und ihr das Objekt uebergeben.
     Als 'gleich' werden alle Objekte betrachtet, fuer die <fun> den gleichen
     Wert zurueckliefert.
     Objekte, fuer die <fun> 0 zurueckliefert, werden ignoriert.

BEMERKUNGEN
-----------
::

     1) Raeume rufen remove_multiple(3) standardmaessig im reset(), jedoch
        nur, wenn kein Spieler anwesend ist und mehr als 10 Objekte im Raum
        sind.
     2) remove_multipe(0) entfernt alle entfernbaren Objekte (s. Ausnahmen).

BEISPIELE
---------
::

     Ein Container enthaelt 5 Fackeln. Im reset() ruft man nun
     remove_multiple(3) auf. Anschliessend sind noch 3 Fackeln uebrig.

     

     Alle verbleibenden Objekte im Container sollen unterschiedliche Namen
     haben:
     remove_multiple(1, "name");

     Ein Container soll nur ein Objekt behalten, welches groesser als 70 cm
     ist:
     int groessen_filter(object ob) {
       if (ob->QueryProp(P_SIZE > 70)) return 1;
       return 0; // damit das Objekt ignoriert wird.
       // Alternativ koennte man statt 0 auch object_name(ob) zurueckliefern,
       // dann sind diese Objekte alle unterschiedlich.
     }
     ...
     remove_multiple(1, #'groessen_filter);

SIEHE AUCH
----------
::

     reset()

31.01.2009, Zesstra

