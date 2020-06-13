P_SHORT
=======

NAME
----

   P_SHORT
     "short"

DEFINIERT IN
------------

   /sys/thing/description.h

BESCHREIBUNG
------------

   Diese Property enthaelt die Kurzbeschreibung eines Objektes bei Ansicht
   *von aussen* als String. Fuer die Innen(kurz)ansicht von Raeumen muss man
   P_INT_SHORT benutzen.

   Die Kurzbeschreibung darf *nicht* mit einem "\n" abgeschlossen sein (dies
   wird von den zustaendigen Funktionen erledigt).
   Aus historischen Gruenden wird ein Punkt ergaenzt, wenn das letzte
   Zeichen kein Punkt, Ausrufezeichen oder Fragezeichen ist.

     Setzt man diese Property auf 0, so ist das Objekt unsichtbar, allerdings
     ansprechbar, wenn der Spieler eine ID des Objektes kennt. D.h. Objekte
     koennen mitgenommen, weggeworfen oder ggf. auch angegriffen werden. Will
     man dies nicht, sollte man das Objekt mit P_INVIS unsichtbar machen.

     Diese Property bestimmt die Ansicht des Objektes von aussen. Fuer die
     Innen(kurz)ansicht von Raeumen muss man P_INT_SHORT benutzen.

BEMERKUNGEN
-----------

   * In altem Code wird manchmal eine Closure als Wert (nicht als
     Querymethode) eingetragen, welche einen String zurueckgibt. Dies ist
     *deprecated* und sollte nicht mehr formuliert werden.

   * Die Funktion, die die Kurzbeschreibung ausgibt (short()), filtert P_SHORT
     durch process_string(). Dieses Feature ist *deprecated* und von der
     Nutzung wird in neuem Code abgeraten.

   * Soll eine dyn. Kurzbeschreibung geschaffen werden, bitte eine
     F_QUERY_METHOD einsetzen oder short() passend ueberschreiben.

BEISPIELE
---------

.. code-block:: pike

     // eine Axt sieht natuerlich so aus:
     SetProp(P_SHORT, "Eine Axt");

SIEHE AUCH
----------

     Aehnliches: :doc:`P_LONG`, :doc:`../lfun/short`, :doc:`P_INT_SHORT`


13.06.2020, Zesstra

