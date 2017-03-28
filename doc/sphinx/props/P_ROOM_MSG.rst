P_ROOM_MSG
==========

NAME
----

    P_ROOM_MSG                    "room_msg"                    

DEFINIERT IN
------------

    /sys/room/description.h

BESCHREIBUNG
------------

     Liste mit Meldungen, die zufaellig im Raum ausgegeben werden.

     Hier sind nur die Textmeldungen als String-Array gespeichert.

ANMERKUNGEN
-----------

     Bitte AddRoomMessage() zum Hinzufuegen/Ueberschreiben benutzen!

SIEHE AUCH
----------

     LFuns:
       :doc:`../lfun/AddRoomMessage`
     Verwandt:
       tell_room(), :doc:`../sefun/send_room`, :doc:`../lfun/ReceiveMsg`
     Props:
       :doc:`P_MSG_PROB`, :doc:`P_FUNC_MSG`

2.Feb 2016 Gloinson