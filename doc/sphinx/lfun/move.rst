move()
======

move()

FUNKTION: 
     Fuer unbelebte Gegenstaende (/std/thing):
       varargs int move(object|string dest, int method);

     Fuer Lebewesen (/std/living, /std/npc, usw.):
       varargs int move(object|string dest, int method, string dir, 
                        string textout, string textin);

DEFINIERT IN
------------
::

     /std/thing/moving.c
     /std/living/moving.c

ARGUMENTE
---------
::

     dest
          Das Zielobjekt (entweder der Dateiname oder das Objekt selbst).

     method
          Die Art der Bewegung (eine der unten aufgefuehrten Arten; es
          koennen auch mehrere zusammenge-ODER-t werden).

     dir
          Die Richtung, in die ein Lebewesen geht. Normalerweise ist das die
          eingeschlagene Laufrichtung (zB. "nach Norden").

     textout
          Verlaesst ein Lebewesen einen Raum, so wird diese Meldung in den
          Raum geschickt. Ist bei dir ein String angegeben, so wird dieser
          noch an textout angehaengt. Der Name des Lebewesens wird der
          Meldung in jedem Fall vorangestellt.

     textin
          Dieser Text wird im Zielraum ausgegeben, wenn ein Lebewesen ihn
          betritt. Bei normaler Fortbewegung ist das "kommt an". Dem Text
          wird noch der Name des Spielers vorangestellt.

BESCHREIBUNG
------------
::

     Es wird versucht, das Objekt in das Zielobjekt dest zu bewegen.
     Abhaengig vom momentanen Aufenthaltsort und dem Zielobjekt ist die
     Bewegungsmethode method zu waehlen.

     In <moving.h> sind folgende Konstanten fuer die Art der Bewegung
     definiert:
     M_NOCHECK
          Es werden keine Abfragen durchgefuehrt, ob das Objekt ueberhaupt
          in das Zielobjekt hineinpasst (was zB. aus Gewichtsgruenden der
          Fall sein koennte).

     M_GO
          Ein Lebewesen bewegt sich gehend von einem Raum in den naechsten.
          Bei normalem Gehen wird diese Methode benutzt; man sollte sie auch
          benutzen, wenn man Spieler ueber einen SpecialExit in einen
          benachbarten Raum bewegt.

     M_TPORT
          Ein Lebewesen wird von einem Raum in einen anderen teleportiert.
          Im Gegensatz zu M_GO kann ein Raum verhindern, dass man ihn
          mittels M_TPORT verlaesst oder betritt.

     M_NO_SHOW
          Beim Bewegen von Lebewesen bekommen diese die Beschreibung des
          Zielraumes nicht ausgegeben.

     M_NO_ATTACK
          Beim Bewegen von Lebewesen bekommen diese keinen
          Begruessungsschlag, wenn ein Feind im Zielraum steht.

     M_SILENT
          Es werden beim Bewegen keine Meldungen ausgegeben. Dieser
          Parameter wirkt sich nur auf das Bewegen von Lebenwesen aus.
          (Magier mit 'mschau an' sehen die Bewegungen dennoch, es wird
           eine Standardmeldung ausgegeben.)

     M_GET
          Das Objekt wird von einem unbelebten Objekt (zB. einem Raum, einer
          Leiche, einem Beutel) in ein lebendes Objekt (Spieler oder NPC)
          bewegt.

     M_PUT
          Das Objekt wird von einem lebenden Objekt in ein unbelebtes Objekt
          bewegt.

     M_GIVE
          Das Objekt wird von einem Lebewesen an ein anderes Lebewesen
          weitergereicht.

     M_MOVE_ALL (Nur fuer Objekte, die /std/unit.c erben)
          Es wird die gesamte Menge bewegt, nicht nur ein Teil.

     Soll das Objekt auf jeden Fall und ohne jede Abfrage bewegt werden, so
     reicht es, als method M_NOCHECK zu uebergeben.

     Waffen und Ruestungen werden, soweit sie gezueckt bzw. angezogen sind,
     beim Bewegen auf jeden Fall weggesteckt bzw. ausgezogen. Ist in method
     M_SILENT enthalten, so geschieht dies ohne Meldungen.

     Die erste Art des Funktionsaufrufs ist sowohl beim Bewegen von
     Lebewesen als auch von unbelebten Objekten moeglich. Die zweite Art
     laesst sich nur bei Lebewesen anwenden.

ANMERKUNG
---------
::

     Diese Funktion sollte nicht (mehr) ueberschrieben werden. Stattdessen
     greift bitte auf PreventMove() und NotifyMove() zurueck. RMs sind
     aufgerufen, Objekt mit ueberschriebenen move() nur noch dann
     anzuschliessen, wenn der Zweck sonst nicht erreicht werden kann. Solltet
     ihr move() ueberschreiben: Seid euch sehr genau im klaren, was move()
     genau macht. ;-)

     

     Wenn Livings bewegt werden, sorgt move() automatisch in Abhaengigkeit
     von P_PARA dafuer, dass das Lebewesen in der korrekten (Parallel-)Welt
     landet.

     Bei Gegenstaenden wird ebenfalls versucht, die richtige Zielwelt
     auszuwaehlen (damit z.B. in der Parallelwelt geworfene Bumerangs auch nur
     innerhalb der Parallelwelt fliegen). Um Rechenzeit zu sparen, wird das
     allerdings nur versucht, wenn 'dest' ein Filename ist und kein Objekt.

     Grund: bei Zielobjekten handelt es sich meist um Bewegungen in das Inv
     oder Env eines Spielers - und die sind uninteressant. Raumwechsel dagegen
     erfolgen fast immer unter Angabe eines Filenamens anstatt eines Objektes.

RUeCKGABEWERT
-------------
::

     Alle Rueckgabewerte sind als symbolische Konstanten in <moving.h>
     definiert. (MOVE_OK ist 1, alle anderen sind <0 und symbolisieren Fehler.
     Traditionell erfolgt die Pruefung auf erfolgreiches Move mit == 1, in
     Zukunft wird == MOVE_OK empfohlen.)

     

     MOVE_OK
          Die Bewegung wurde erfolgreich abgeschlossen.

     ME_PLAYER
          Lebewesen lassen sich nicht ohne weiteres bewegen. Es muss
          mindestens eine der Methoden M_NOCHECK, M_GO oder M_TPORT
          angegeben werden.

     ME_TOO_HEAVY
          Das Zielobjekt kann dieses Objekt aus Gewichtsgruenden nicht mehr
          aufnehmen.

     ME_CANT_TPORT_IN
          Das Zielobjekt verbietet das Teleportieren in sich hinein (nur bei
          M_TPORT ohne M_NOCHECK).

     ME_CANT_TPORT_OUT
          Der Raum, in dem sich das Lebewesen befindet, verbietet das
          Teleportieren aus sich hinaus (nur bei M_TPORT ohne M_NOCHECK).

     ME_CANT_BE_DROPPED
          Das Objekt kann nicht fallen gelassen werden (zB. weil P_NODROP
          gesetzt ist).

     ME_CANT_BE_TAKEN
          Das Objekt kann nicht genommen werden (zB. weil P_NOGET gesetzt
          ist).

     ME_CANT_BE_INSERTED
          Das Zielobjekt verhindert das Einfuegen aus bestimmten Gruenden.

     ME_CANT_LEAVE_ENV
          Der Container verhindert ein verlassen des Objektes

     ME_TOO_HEAVY_FOR_ENV
          Ein Objekt kann einen Behaelter nicht verlassen, da es dem 
          Lebewesen sonst zu schwer wuerde.

     TOO_MANY_OBJECTS
          Das Zielobjekt kann soviele Objekte nicht mehr aufnehmen.

     ME_NOT_ALLOWED
          Raeume mit gesetzter Property P_NO_PLAYERS koennen nur von
          Testspielern und Magiern betreten werden. Bei Spielern oder
          Gildentesties gibt es diese Fehlermeldung.
     ME_WAS_DESTRUCTED
          Das Objekt hat sich entweder im Verlaufe der Bewegung selbst
          zerstoert oder wurde zerstoert, sodass move() nicht erfolgreich
          beendet werden konnte. (Bsp: sensitive Objekte)

     ME_DONT_WANT_TO_BE_MOVED
          Das Objekt moechte nicht bewegt werden.

BEISPIELE
---------
::

        o Ein Objekt "gibt sich" dem Spieler:

          move(this_player(), M_GET);

        o Ein Lebewesen wird in die Abenteurergilde teleportiert:

          lv->move("/gilden/abenteurer", M_TPORT);

        o Ein Spieler "wird in die Gilde gegangen":

          this_player()->move("/gilden/abenteurer", M_GO, "in die Gilde");

          Spieler, die mit ihm im gleichen Raum stehen, sehen folgende
          Meldung:
          "<name> geht in die Gilde."

        o Ein Spieler schwimmt durchs Meer:

          this_player()->move("meer_xy", M_GO, "nach Norden", "schwimmt",
                              "schwimmt herein");

          Spieler in seinem Startraum sehen "<name> schwimmt nach Norden.",
          Spieler in seinem Zielraum sehen "<name> schwimmt herein."

SIEHE AUCH
----------
::

  Seherkommandos
      setmin, setmmin, setmout, setmmout, review
  Verwandt:
      move_object(), remove(), PreventInsert(), PreventLeave(), 
      PreventInsertLiving(), PreventLeaveLiving(), PreventMove(), 
      NotifyInsert(), NotifyLeave(), NotifyMove(), NotifyRemove(), 
      init(), exit()
  Properties:
      P_MSGIN, P_MSGOUT, P_MMSGIN, P_MMSGOUT, P_NO_PLAYERS, P_PARA,
      P_NO_TPORT
  Objekte:
      /std/thing/moving.c, /std/living/moving.c

2019-Aug-20, Arathorn

