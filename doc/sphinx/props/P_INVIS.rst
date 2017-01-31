P_INVIS
=======

NAME
----
::

     P_INVIS                       "invis"                       

DEFINIERT IN
------------
::

     /sys/player/base.h

BESCHREIBUNG
------------
::

     Die Property P_INVIS dient dazu, Objekte als unsichtbar zu kennzeichnen.
     Hierbei versucht P_INVIS die moeglichen Interaktionen mit Spielern zu
     minimieren (im Gegensatz zu einer fehlenden P_SHORT, welche das Objekt
     nur einfach nicht-sichtbar macht).

     

     Man sollte drei Arten von unsichtbaren Objekten unterscheiden:

     - Gegenstaende
       Setzt man P_INVIS auf eine Zahl ungleich 0, wird der Gegenstand
       unsichtbar und der Name zu "etwas". Zusaetzlich koennen Spieler ihn
       nicht mehr ansprechen, d.h. nehmen, wegwerfen, weitergeben etc.
       (Bei magier-eigenen Kommandos ist dies evtl. nicht umgesetzt...)
       Setzt man P_SHORT auf 0, wird der Gegenstand nur nicht mehr in
       der Inventarliste von Behaeltern/Raeumen angezeigt, er behaelt aber
       seinen Namen und ist durch Spieler ansprechbar, wenn sie eine ID
       kennen.

     - NPCs
       Bei gesetztem P_INVIS wird der NPC unsichtbar und sein Name wird zu
       "Jemand". Zusaetzlich koennen Spieler ihn nicht mehr ansprechen, z.B.
       toeten oder knuddeln.
       (Bei magier-eigenen Kommandos ist dies evtl. nicht umgesetzt...)
       Setzt man P_SHORT auf 0, wird der NPC nur nicht mehr in der
       Inventarliste von Behaeltern/Raeumen angezeigt, er behaelt aber seinen
       Namen und ist durch Spieler ansprechbar, wenn sie eine ID kennen. Auch
       angreifen und kaempfen ist moeglich.

     

     - Magier
       Magier macht man unsichtbar, indem man ihnen die Property P_INVIS auf
       einen Wert <> 0 setzt.
       *  Spieler duerfen nicht unsichtbar gemacht werden!               *
       *  Wird ein Magier unsichtbar gemacht, muss man ihm die Property	 *
       *  P_INVIS auf den Wert setzen, den die Property P_AGE zu diesem	 *
       *  Zeitpunkt hat (keine F_QUERY_METHOD !).				                 *
       Setzt man die Property auf den Wert 1, so erhaelt ein Spieler,
       wenn er den entsp. Magier fingert, die Ausgabe: Alter: 00:00:02,
       was genauso verraeterisch ist, wie ein Alter, dass bei einem
       scheinbar nicht eingeloggten Magier immer weiter hochgezaehlt
       wird.


27.05.2015, Zesstra

