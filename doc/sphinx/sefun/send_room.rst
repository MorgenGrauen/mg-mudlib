send_room()
===========

FUNKTION
--------
::

varargs void send_room(object|string room, string msg, int msg_type,
                       string msg_action, string msg_prefix,
                       object *exclude, object origin)

BESCHREIBUNG
------------
::

        Sendet msg an alle Objekte in room durch Aufruf von ReceiveMsg() mit
        den uebergebenen Argumenten.
        Zur Bedeutung der Argumente siehe Manpage von ReceiveMsg().

        Wenn das Raumobjekt mit seinem Namen angegeben ist, wird das Objekt
        unter diesem Namen gesucht und und geladen, falls notwendig.

        Mit dem Array <*exclude> kann man verhindern, dass die Nachricht an
        die darin enthaltenen Objekte gesendet wird.
        Das ist sinnvoll, wenn zB ein Spieler Ausloeser einer Meldung ist
        und diese selbst nicht erhalten soll.

        origin gibt an, welches Objekt die Meldung ausloest (muss nicht das
        sendende Objekt selber) und wird vor allem fuer die Ignorierepruefung
        verwendet. Default ist das sendende Objekt.

        Letztendlich ist die sefun vergleichbar zu tell_room().

SIEHE AUCH
----------
::

        ReceiveMsg(L)
        tell_object(E), tell_room(E), say(E), write(E)

13.03.2016, Zesstra

