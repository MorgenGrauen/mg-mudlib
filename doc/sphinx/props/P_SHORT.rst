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

   Setzt man diese Property auf 0, so wird das Objekt von Spielern nicht mehr
   wahrgenommen (es wird in Inventaren nicht mehr angezeigt). Der Effekt ist
   der eines "PAL-Feldes" ("Problem anderer Leute"). Es bleibt allerdings
   ansprechbar, wenn Spieler eine ID des Objektes kennen. Diese koennen dann
   damit alles anstellen, was sie sonst auch koennten. D.h. Objekte koennen
   insb. mitgenommen, weggeworfen, untersucht oder ggf. auch angegriffen
   werden und es kann angreifen.
   Will man dies nicht, muss man das Objekt mit P_INVIS richtig unsichtbar
   machen.

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

     Aehnliches
       :doc:`P_LONG`, :doc:`P_INT_SHORT`, :doc:`P_INVIS`,
       :doc:`../lfun/short`


13.06.2020, Zesstra

