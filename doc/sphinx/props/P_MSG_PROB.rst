P_MSG_PROB
==========

NAME
----
    P_MSG_PROB                    "msg_prob"                    

DEFINIERT IN
------------

    /sys/room/description.h

BESCHREIBUNG
------------
     Der Name ist irrefuehrend: es ist ein Parameter fuer die Wartezeit
     in Sekunden bis zur naechsten Ausgabe einer Raumnachricht.
     Nur direkt nach dem Betreten des Raums wird ein Zufallswert dieser
     Zahl erstellt.

     Wird in AddRoomMessage() explizit mitgesetzt und gilt fuer den Raum.

     Kann von einer Nachrichtenmethode geaendert werden, um mehr Zufall
     in die Intervalle zu bringen.

     Werte kleiner als 15 werden bei der Auswertung auf 15 gesetzt. Setzen
     auf 0 schaltet die Nachrichten nicht ab!

BEISPIELE
---------

.. code-block:: pike

     // Beispiel:
     // Je laenger Spieler im Raum sind, desto langsamer tropft das
     // Wasser. Wenn der letzte den Raum verlaesst, wird die Zeit
     // fuer den naechsten Spieler wieder auf den Initialwert gesetzt.

     inherit "/std/room";

     #define START_SPEED 30

     void create() {
       ::create();
       AddRoomMessage(({"Das Wasser tropft.\n",
                        "Ein Tropfen plitscht.\n",
                        "Tripf tropf macht das Wasser.\n"}),
                      START_SPEED,
                      ({"slowdown_speed"}));
     }

     void slowdown_speed(int msg) {
       SetProp(P_MSG_PROB, QueryProp(P_MSG_PROB)+10);
     }

     void exit() {
       ::exit();
       
       if(!sizeof(filter(all_inventory(this_object())-({this_player()}),
                         #'interactive)))
         SetProp(P_MSG_PROB, START_SPEED);
     }

SIEHE AUCH
----------

     LFuns:
       :doc:`../lfun/AddRoomMessage`
     Props:
       :doc:`P_ROOM_MSG`, :doc:`P_FUNC_MSG`
     Verwandt:
       call_out()

28. Mar 2017 Gloinson
