update_actions()
================

FUNKTION
--------
::

        void update_actions()

ARGUMENTE
---------
::

        keine

BESCHREIBUNG
------------
::

        Falls eine Aktion ein add_action() ausgeloest hat, werden mit dieser
        Funktion die neuen Befehle bei allen Lebewesen im aufrufenden Objekt
        bzw. in der Umgebung des aufrufenden Objektes aktiv.

RUECKGABEWERT
-------------
::

        keiner

BEMERKUNGEN
-----------
::

        Diese Funktion wird eigentlich nur benoetigt, wenn man mit add_action()
        anstelle von AddCmd() arbeitet (zB. bei Objekten, die nicht
        /std/thing/commands inheriten).

BEISPIELE
---------
::

        /* Normalerweise sollte man es SO gerade nicht machen. Stattdessen
         * sollte die "kletter"-Funktion pruefen, ob die Luke geoeffnet ist, 
         * und sich im Fehlerfall beschweren.
         * So aber dient es als schoenes Beispiel fuer update_actions() ;)
         */
        int oeffne(string str)
        {
          if( str == "luke" ) {
            write( "Du oeffnest die Luke. Du kannst jetzt nach unten klettern.\n");
            add_action("kletter", "kletter", 1);
            update_actions();
            return 1;
          }
          return 0;
        }

SIEHE AUCH
----------
::

  add_action(E), AddCmd(L), RemoveCmd(L)

