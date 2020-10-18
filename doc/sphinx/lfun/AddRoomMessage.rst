AddRoomMessage()
================

FUNKTION
--------

     void AddRoomMessage(string* msg, int time, mixed* func);

DEFINIERT IN
------------

     /std/room/description.c

ARGUMENTE
---------

     string* msg
          Array von Strings mit den Meldungen.
     int time
          Der Abstand zwischen zwei Meldungen in Sekunden.
     string|string* func (optional)
          String oder Array von Strings mit Funktionsnamen

BESCHREIBUNG
------------

     Mit dieser Funktion legt man fest, dass in bestimmten Zeitabstaenden
     Meldungen an den Raum geschickt werden sollen.

     Es wird alle 'time' Sekunden (beim ersten Betreten durch einen Spieler
     ein random() des Wertes) zufaellig eine der in msg angegebenen
     Meldungen ausgegeben.
     Hat man func angegeben, so wird diese Funktion (bei einem Array:
     eine zufaellig ausgesuchte Funktion) im Raum aufgerufen. Als
     Parameter bekommt die Funktion die Nummer der ggf zuvor ausgegebenen
     Meldung (Default also 0).

     Bevor man allerdings jeden Raum mit AddRoomMessage() pflastert, sollte
     man folgendes bedenken (Fazit: weniger ist mehr):

     * Viele Meldungen in vielen Raeumen tendieren dazu, den Spielern auf
       die Nerven zu gehen.
     * Die das Timing steuernden call_out() sind nicht umsonst.

BEMERKUNGEN
-----------

     * falls time < 15 Sekunden ist, wird auf 15 Sekunden aufgerundet
     * der Praefix Add... taeuscht hier. Ein Aufruf von AddRoomMessage()
       ueberschreibt alle vorherigen Werte
     * this_player() NICHT VERWENDEN!
     * Abschalten der Raumnachrichten:

       * passiert automatisch nur, wenn alle Spieler den Raum verlassen haben
       * ist manuell nur ueber Loeschen der Nachrichten umsetzbar, also mit
         ``AddRoomMessage(0, 0, 0);``

     In Funktionen, die durch AddRoomMessage() ausgeloest werden, darf
     this_player() nicht verwendet werden, da die call_out()-Kette den
     ersten ausloesenden Spieler speichert. Anwesende Spieler also bitte
     ggf mit
     ``filter(all_inventory(this_object()), #'interactive)``
     suchen.

BEISPIELE
---------

.. code-block:: pike

     // Beispiel 1:
     // Es soll alle halbe Minute eine Meldung ausgegeben werden. Falls es
     // unter den Fuessen knackt, soll man zudem mit 30%-iger
     // Wahrscheinlichkeit zusammenzucken:

     inherit "/std/room";

     void create() {
       ::create();
       AddRoomMessage(({"In der Ferne schreit ein Kaeuzchen.\n",
                         "Es raschelt im Gebuesch.\n",
                         "Etwas knackt unter Deinen Fuessen.\n"}),
                      30,
                      ({"sound", "sound_more_rnd"}) );
     }

     void sound(int msg) {
       if (msg == 2)         // Es hat geknackt...
         if (random(10) < 3) // Schreck lass nach! ;-)
           tell_room(this_object(), "Erschrocken faehrst Du zusammen!\n" );
     }

     // Extra-Beispiel: wir setzen die Wartedauer (Parameter tim) neu
     void sound_more_rnd(int msg) {
       sound(msg);
       SetProp(P_MSG_PROB, 25+random(20));  // neue Wartedauer
     }


.. code-block:: pike

     // Beispiel 2: Abschalten der Raumnachrichten
     public int action_laerm(string str);
     protected void reset_room_messages();

     void create() {
       ::create();
       reset_room_messages();

       AddCmd("mach|mache&laerm|krach",
              "action_laerm",
              "Was willst du machen?");
       AddCmd("schlag|schlage&laerm|krach",
              "action_laerm",
              "Was willst du schlagen?");
     }

     protected void reset_room_messages() {
       AddRoomMessage(({"Das Reh quakt leise.\n",
                        "Der Frosch grunzt.\n",
                        "Gelbe Schnorpfel pupsen im Takt.\n"}),
                      45);
     }

     public int action_laerm(string str) {
       AddRoomMessage(0, 0, 0);

       this_player()->ReceiveMsg(
         "Du schreist dir kurz die Seele aus dem Leib. Alle Tiere "
         "verstummen sofort.", MT_NOTIFICATION);
       send_room(this_object(),
         this_player()->Name()+" schreit laut. Alle Tiere verstummen "
         "sofort.", MT_LISTEN, 0, 0, ({this_player()}));
       return 1;
     }

     void reset() {
       :reset();
       if(!QueryProp(P_ROOM_MSG))
         reset_room_messages();
     }

SIEHE AUCH
----------

     Verwandt:
       tell_room(), :doc:`../sefun/send_room`, :doc:`../lfun/ReceiveMsg`
     Props:
       :doc:`../props/P_MSG_PROB`, :doc:`../props/P_FUNC_MSG`, :doc:`../props/P_ROOM_MSG`

28. Mar 2016 Gloinson

