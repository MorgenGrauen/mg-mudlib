process_call()
==============

simul_efun::process_call(E)

FUNKTION
--------
::

     int process_call()

BESCHREIBUNG
------------
::

     Gibt zurueck, ob die Ausfuehrung zum derzeitigen Zeitpunkt durch
     process_string() ausgerufen wurde.

BEISPIELE
---------
::

     process_string("@@current_long@@");
     ...
     string current_long() {
      if(process_call())
       return("Dieser String wurde durch ein process_string eingefuegt.");
      else return("Du hast die Funktion direkt gerufen!");
     }

SIEHE AUCH
----------
::

     notify_fail(E), process_string(E), replace_personal(E)

28. Maerz 2004 Gloinson

