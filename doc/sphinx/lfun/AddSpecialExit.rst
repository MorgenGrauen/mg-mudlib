AddSpecialExit()
================

FUNKTION
--------
::

     void AddSpecialExit(string|string* cmd, string|closure func);

DEFINIERT IN
------------
::

     /std/room/exits

ARGUMENTE
---------
::

     string/string* cmd
          die Richtung(en), in die der Ausgang fuehrt
     string/closure func
          der Name der aufzurufenden Funktion/Closure

BESCHREIBUNG
------------
::

     Es wird ein Ausgang in die Richtung(en) cmd eingefuegt. Wird der
     Ausgang benutzt, so wird die Closure bzw. Funktion func ausgefuehrt.

     AddSpecialExit(cmd, "func") entspricht:
     - AddExit(keys, #'func)

BEMERKUNGEN
-----------
::

     In func muss man den Spieler selbst in den Zielraum bewegen. Im
     Erfolgsfall sollte man einen Wert >0 zurueckgeben, im Fehlerfall einen
     Wert <=0.

     func bekommt als Parameter einen String mit der gewaehlten
     Bewegungsrichtung uebergeben.

BEISPIELE
---------
::

     // ein Ausgang soll nur von Froeschen benutzbar sein:

     AddSpecialExit("loch", "lochfkt");
     // der gleiche Aufruf, nur anders:
     // static int lochfkt(string dir);		// Prototyp
     // ...
     // AddSpecialExit("loch", #'lochfkt);
     // auch identisch zu:
     // AddExit("loch", #'lochfkt);

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

     AddExit(), GetExits(),
     RemoveExit(), RemoveSpecialExit(),
     GuardExit(),
     H_HOOK_EXIT_USE, P_EXITS, P_HIDE_EXITS, /std/room/exits.c
     ausgaenge

Letzte Aenderung: 22.12.2016, Bugfix

