NewDoor()
=========

FUNKTION
--------
::

     varargs int NewDoor(string|string* cmds, string dest, string|string* ids,
                         mapping|<int|string|string*>* props);

DEFINIERT IN
------------
::

     /std/room/doors.c

ARGUMENTE
---------
::

     cmds (string|string*)
          String oder Array von Strings mit den Befehlen, mit denen man
          durch die Tuer geht (in der Regel Richtungen wie "norden").
     dest (string)
          Pfad des Zielraumes, OHNE ".c", sonst wird eine zweiseitige Tuer
          (wie sie ueblich ist) nicht als solche erkannt.
     ids (string|string*)
          String oder Array von Strings mit den Bezeichnern der Tuer. Kann
          auch 0 sein; in diesem Fall laesst sich die Tuer nur als "tuer"
          ansprechen.
     props (mapping|<int|string|string*>*)
          Die Eigenschaften der Tuer (s.u.).

BESCHREIBUNG
------------
::

     Es wird eine neue Tuer geschaffen. Die Tuer laesst sich, wenn sie
     geoeffnet ist, mit den in <cmds> angegebenen Befehlen durchschreiten.
     Man gelangt dann in den Raum <dest>.

     Die Kommandos werden bei geoeffneter Tuer in die Liste der sichtbaren
     Ausgaenge eingefuegt.

     In <props> lassen sich Aussehen und Eigenschaften der Tuer festlegen.
     Historisch ist es ein Array mit Paaren von Schluesseln und Werten, d.h.
     es kommt immer erst ein Element, welches die Eigenschaft bezeichnet und
     dann ein Element mit dem Wert dieser Eigenschaft.
     Moderner ist es, ein Mapping mit den entsprechenden Schluesseln und
     Werten zu uebergeben. Dies ist auch dringend empfohlen.

     In <doorroom.h> sind dazu folgende Eigenschaften definiert:
     D_FLAGS
          DOOR_OPEN
               Die Tuer ist beim Erzeugen geoeffnet.
          DOOR_CLOSED
               Die Tuer ist beim Erzeugen geschlossen.
          DOOR_NEEDKEY
               Man benoetigt einen Schluessel zum Oeffnen der Tuer.
          DOOR_CLOSEKEY
               Man benoetigt einen Schluessel zum Schliessen der Tuer.
          DOOR_RESET_CL
               Die Tuer schliesst sich beim Reset.
          DOOR_RESET_OP
               Die Tuer oeffnet sich beim Reset.

     D_LONG
          Die Langbeschreibung der Tuer. 
          Hier kann ein Mapping eingetragen werden, das als Keys den Tuer-
          Status hat und die zugehoerige Langbeschreibung dazu. Beispiel:
          ([ D_STATUS_CLOSED : "Die Tuer ist geschlossen.\n",
             D_STATUS_OPEN   : "Die Tuer ist offen.\n" ])

          Default: "Eine Tuer.\n"

     D_SHORT
          Die Kurzbeschreibung der Tuer. Ein "%s" wird durch "geoeffnet"
          bzw. "geschlossen" ersetzt.

          Es werden die Kurzbeschreibungen aller im Raum vorhandenen Tueren
          aneinandergehaengt (es wird jeweils ein Leerzeichen eingeschoben),
          das Ganze mit break_string() umgebrochen und an P_INT_LONG
          angehaengt.

          Default: "Eine %se Tuer."

     D_NAME
          Der Name der Tuer. Dieser Name wird beim Oeffnen und Schliessen
          der Tuer sowie bei Fehlermeldungen ausgegeben. Man kann wie bei
          P_NAME einen String oder ein Array von Strings angeben.

          Default: "Tuer"

     D_GENDER
          Das Geschlecht der Tuer.

          Default: FEMALE

     D_MSGS
          String oder Array von drei Strings. Diese Strings werden beim
          Durchschreiten der Tuer an move() als dir bzw. dir, textout und
          textin uebergeben.

     D_FUNC
          String mit dem Namen einer Funktion, die im Startraum vor dem
          Durchschreiten der Tuer aufgerufen werden soll. Diese Funktion
          kann das Durchschreiten jedoch nicht verhindern!

     D_FUNC2
          String mit dem Namen einer Funktion, die im Zielraum nach dem
          Durchschreiten der Tuer aufgerufen werden soll.

     D_TESTFUNC
          Falls auf den Namen einer Funktion gesetzt, wird diese Funktion
          vor dem Durchschreiten im Startraum aufgerufen. Wenn sie einen
          Wert != 0 zurueckliefert, wird die Tuer NICHT durchschritten. 

     D_RESET_MSG
          Meldung, die beim Reset der Tuer ausgegeben wird.

     D_OPEN_WITH_MOVE
          Tuer oeffnet automatisch bei Eingabe des Befehls zum 
          Hindurchgehen.

          

RUECKGABEWERT
-------------
::

     1, wenn die Tuer ordungsgemaess eingebaut werden konnte, sonst 0.

BEMERKUNGEN
-----------
::

     Zwei Tuerseiten gelten als verschiedene Seiten einer Tuer, wenn als
     Ziel in Raum A Raum B und in Raum B Raum A angegeben ist. Der Zustand
     wird abgefragt, wenn der Raum betreten wird (init), wenn die Tuer
     geoeffnet/geschlossen wird, P_INT_LONG oder P_EXITS abgefragt wird
     und beim Reset.

     Es sind auch Tueren moeglich, die nur auf einer Seite existieren, oder
     auch solche, die auf beiden Seiten verschieden aussehen oder gar auf
     einer Seite nur mit einem Schluessel zu oeffnen sind, auf der anderen
     jedoch kein Schluessel noetig ist.

     Wer aus irgendeinem Grund den Zustand einer Tuer selber abfragen oder
     veraendern will, kann dafuer in /obj/doormaster die Funktionen
     QueryDoorStatus(ziel) bzw. SetDoorStatus(ziel,status) aufrufen.

     *** ACHTUNG ***
     Es gibt eine Questbelohnung (Phiole aus der Sternenlicht-Quest von
     Miril), mit der man durch Tueren hindurchschauen kann. Derzeit ist das
     per default fuer alle Tueren erlaubt. Wenn man das nicht moechte,
     oder andere Texte ausgeben, als die Phiole normalerweise erzeugt,
     dann kann man dies durch Nutzung bestimmter Funktionen bzw. Flags
     erreichen. Zur Dokumentation siehe Manpage zu GetPhiolenInfos().

BEISPIELE
---------
::

  ** Dies ist eine gewoehnliche Tuer ohne Besonderheiten und ohne
     Extra-Beschreibung:

     NewDoor("sueden","/players/rochus/room/test1");

  ** Ein Portal:

     NewDoor("norden","/players/rochus/room/test2",
             "portal",
             ([ D_NAME:   "Portal",
                D_GENDER: NEUTER,
                D_SHORT:  "Im Norden siehst Du ein %ses Portal.",
                D_LONG:   "Das Portal ist einfach nur gigantisch.\n",
              ]) );

     Alternativ mit props in alter Arraynotation:
     NewDoor("norden","/players/rochus/room/test2",
             "portal",
             ({ D_NAME,   "Portal",
                D_GENDER, NEUTER,
                D_SHORT,  "Im Norden siehst Du ein %ses Portal.",
                D_LONG,   "Das Portal ist einfach nur gigantisch.\n"
              }) );

     

  ** Tueren mit Bewegungsmeldung:

     NewDoor("norden","/room/see2",
             ({"gitter","eisengitter"}),
             ({ D_MSGS,  ({"nach Norden","schwimmt",
                           "schwimmt von Sueden herein"}),
                D_GENDER, NEUTER}) );

  ** Eine Tuer mit Testfunktion:

     NewDoor("osten","/mein/zielraum",
             ({"tuer"}),
             ({ D_TESTFUNC, "blocker_da" }) );

     Die Funktion blocker_da:

     int blocker_da()      // KEINE protected-Funktion! Sie wird sonst NICHT
     {                     // aufgerufen und ausgewertet!
       if ( present("mein_fieses_mo\nster",this_object()) )
       {
         tell_object(this_player(),
             "Ein fieses Monster stellt sich Dir in den Weg.\n");
         return 1;
       }
       return 0;
     }

  ** Nun noch eine Tuer mit einigen Extras:

     NewDoor("nordwesten","/rooms/kammer",
             ({"tuer","holztuer"}),
             ({
               D_FLAGS, (DOOR_CLOSED|DOOR_RESET_CL),
               D_MSGS,  ({"nach Nordwesten","geht",
                        "kommt durch eine Tuer herein"}),
               D_SHORT, "Im Nordwesten siehst Du eine %se Holztuer.",
               D_LONG,  "Sie trennt den Laden vom dahinterliegenden Raum.\n",
               D_NAME,  "Holztuer",
               D_FUNC,  "view",
               D_FUNC2, "look_up"
             }) );

     Im Startraum:

     void view()
     {
       tell_object(this_player(), break_string("Der Angestellte wirft Dir "
         "einen missbilligenden Blick zu, laesst Dich aber passieren.",78));
     }

     Im Zielraum:

     void look_up()
     {
       tell_object(this_player(), break_string("Ein alter Mann schaut kurz "
         "zu Dir auf und vertieft sich dann wieder in seine Akten.",78));
     }

SIEHE AUCH
----------
::

    QueryDoorKey(), QueryDoorStatus(), SetDoorStatus(),
    /std/room/doors.c, /obj/doormaster.c, GetPhiolenInfos(), QueryAllDoors()


08.02.2015, Arathorn

