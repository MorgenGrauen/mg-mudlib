AddExit()
=========

FUNKTION
--------
::

     void AddExit(string|string* cmd, closure|string dest);

DEFINIERT IN
------------
::

     /std/room/exits

ARGUMENTE
---------
::

     string/string* cmd
          die Richtung(en), in die der Ausgang fuehrt
     string/closure dest
          das Ziel des Ausgangs mit Text/Closure

BESCHREIBUNG
------------
::

     Es wird ein Ausgang in die Richtung(en) cmd eingefuegt. Die Art des
     Ausgangs haengt ab von dest:

     - ein String:
       - mit einem Dateinamen:
         Der Ausgang fuehrt in den Raum, den der Dateiname bezeichnet.
       - der Form "<msg>#dateiname"
         Der Ausgang fuehrt in den Raum, den der Dateiname bezeichnet,
         bei der Benutzung wird jedoch statt "<name> geht nach <richtung>"
         "<name> geht nach <msg>" ausgegeben.
     - eine Closure:
       Die Closure wird bei Nutzung des Ausgangs aufgerufen. Das entspricht
       eine SpecialExit - in der gerufenen Funktion muss man den Spieler
       selbst in den Zielraum bewegen.
       Gegebenenfalls kann das durch AddCmd() ersetzt werden.

BEMERKUNGEN
-----------
::

     Man kann fuer den Dateinamen des Zielraumes auch einen relativen Pfad
     angeben. Die Auswertung erfolgt nach folgendem Schema:
     - "./<dateiname>"
       Es wird ein Zielraum relativ zum gleichen Verzeichnis wie dieser
       Raum angesprochen.
     - "../<dateiname>"
       Es wird ein Zielraum relativ zur Verzeichnisebene ueber der
       dieses Raumes angesprochen (analog mit mehrerern "../..")

     Mittels P_HIDE_EXITS kann man Ausgaenge verstecken.

     Bei der Benutzung eines Ausgangs wird der Hook H_HOOK_EXIT_USE
     ausgeloest.

BEISPIELE
---------
::

     ### normale Ausgaenge ###
     // Beim Kommando "sueden" kommt: "<name> geht nach Sueden."
     AddExit("sueden", "/gilden/abenteurer");

     // Beim Kommando "sueden" kommt: "<name> geht in die Gilde."
     AddExit("sueden", "in die Gilde#/gilden/abenteurer");

     ### Ausgaenge mit relativen Pfaden ###
     // Der Name des Raumes sei "/d/inseln/wargon/hafen1"
     // Dieser Ausgang geht nach "/d/inseln/wargon/kneipe":
     AddExit("norden", "./kneipe" );

     // Und dieser nach "/d/inseln/anthea/anlege":
     AddExit("sueden", "../anthea/anlege" );

     ### dynamische Ausgaenge ###
     // ein Ausgang soll nur von Froeschen benutzbar sein:

     static int lochfkt(string dir);		// Prototyp
     ...
     AddExit("loch", #'lochfkt);
     // auch identisch zu:
     // AddSpecialExit("loch", #'lochfkt); [eine Closure] oder
     // AddSpecialExit("loch", "lochfkt"); [ein Funktionsname]

     static int lochfkt(string dir) {
       if (!(this_player()->QueryProp(P_FROG))) {
         // Kein Frosch => passt nicht!
         notify_fail("Du bist zu gross!\n");
         return 0;
       }
       // Meldungen werden im move() gleich mitgegeben
       return this_player()->move("/room/loch", M_GO, 0,
                    "huepft ins Loch", "huepft herein");
     }

SIEHE AUCH
----------
::

     AddSpecialExit(), GetExits(),
     RemoveExit(), RemoveSpecialExit(),
     GuardExit(),
     H_HOOK_EXIT_USE, P_EXITS, P_HIDE_EXITS, /std/room/exits.c
     ausgaenge

Letzte Aenderung: 22.12.2016, Bugfix

