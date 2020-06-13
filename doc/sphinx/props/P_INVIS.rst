P_INVIS
=======

NAME
----

     P_INVIS
       "invis"

DEFINIERT IN
------------

     /sys/player/base.h

BESCHREIBUNG
------------

     Die Property P_INVIS dient dazu, Objekte als nicht wahrnehmbar
     ('unsichtbar') zu kennzeichnen. Diese Unsichtbarkeit erstreckt sich auch
     auf andere Sinne als nur 'Sehen'.
     Hierbei versucht P_INVIS auch die moeglichen Interaktionen mit Spielern
     zu minimieren (im Gegensatz zu einer fehlenden P_SHORT, welche das
     Objekt nur 'verschleiert' bzw. nicht mehr in Inventaren anzeigt).

     Man sollte drei Arten von unsichtbaren Objekten unterscheiden:

     Gegenstaende
       Setzt man P_INVIS auf 1, wird der Gegenstand unsichtbar und der Name zu
       "etwas". Zusaetzlich koennen Spieler ihn nicht mehr ansprechen, d.h.
       nehmen, wegwerfen, weitergeben etc.
       (Bei magier-eigenen Kommandos ist dies evtl. nicht umgesetzt...)

     NPCs
       Bei gesetztem P_INVIS wird der NPC unsichtbar und sein Name wird zu
       "Jemand". Zusaetzlich koennen Spieler ihn nicht mehr ansprechen, z.B.
       toeten oder knuddeln.
       (Bei magier-eigenen Kommandos ist dies evtl. nicht umgesetzt...)

     Magier
       Magier macht man unsichtbar, indem man ihnen die Property P_INVIS auf
       einen Wert <> 0 setzt. Dieser muss dem Wert von P_AGE zu diesem
       Zeitpunkt entsprechen (keine F_QUERY_METHOD!).
       Spieler duerfen **nicht** unsichtbar gemacht werden!
       Setzt man die Property auf den Wert 1, so erhaelt ein Spieler,
       wenn er den entsp. Magier fingert, die Ausgabe: Alter: 00:00:02,
       was genauso verraeterisch ist, wie ein Alter, dass bei einem
       scheinbar nicht eingeloggten Magier immer weiter hochgezaehlt
       wird.


:Letzte Aenderung: 27.05.2015, Zesstra

