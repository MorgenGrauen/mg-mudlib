tell_room
=========

BEMERKUNGEN
-----------

  Wird in einem catch_msg() der Wert von <msg> veraendert, erhalten
  alle nachfolgenden Objekte das veraenderte <msg> (Referenz!)

BEISPIELE
---------

  .. code-block:: pike

    // Dies ist ein einfaches Beispiel fuer eine Meldung an alle An-
    // wesenden im Raum.

    tell_room(this_object(),"Ein leichter Wind kommt auf.\n");

    // Diese Meldung wird im Raum /d/ebene/ark/raum.c ausgegeben, dieser
    // Raum muss nicht derjenige sein, in dem das tell_room() ausgefuehrt
    // wird.

    tell_room("/d/ebene/ark/raum","Ein leichter Wind kommt auf.\n");


    // Diese Meldung wird an alle Anwesenden im Raum AUSSER this_player()
    // (der diese Meldung ausgeloest hat) ausgegeben. Der muss eine ge-
    // sonderte Meldung ueber sein Stolpern per write() oder
    // tell_object() bekommen.
    tell_room(this_object(),
              break_string(this_player()->Name()+" stolpert.", 78),
              ({ this_player() }));
    tell_object(this_player(), "Du stolperst.\n");

    // Ein Beispiel mit zwei Objekten, das zeigt, wie das Zusammenspiel
    // von catch_tell() und tell_room() ablaueft. Objekt1 ist ein
    // Lebewesen mit Namen "Dummymonster", Objekt2 verteilt die Meldung:

    Objekt1 (ein Lebewesen, steht im Env von this_player()):
        void catch_tell(string str) {
            write("Empfangen: "+str+"\n");
        }

    Objekt2:
        void fun() {
            tell_room(environment(this_player()), "Hallo Welt!\n");
        }

