AddAction()
===========

AddAction(L)
------------
::

FUNKTION
--------
::

     varargs void AddAction(mixed fun, mixed cmd, int flag, int lvl);

DEFINIERT IN
------------
::

     /std/player/command.c

ARGUMENTE
---------
::

     fun	zu rufende Methode im Spieler oder eine Closure
     cmd	ausloesendes Kommandoverb
     flag	unscharf ausfuehren
     lvl	ab welchem (Magierlevel) funktioniert das Kommando

BESCHREIBUNG
------------
::

     Dem Spieler wird ein neues Kommando definiert. Dieses kann eine Methode
     in ihm sein, so wie bei allen Spielerkommandos ueblich, kann aber auch
     eine Closure (Lfun-Closure oder Lambda) enthalten.

     Mittels "flag" kann man die Kommandoerkennung unscharf halten, d.h. wie
     bei AddCmd(L) und add_action(E) wird ein 'cmd' bei 'flag = 1' auch
     schon von Praefix-Strings (hier ohne Leerzeichen) getriggert:
     AddAction([...], "muh", 1, 0) wird zB auch von 'muhtens' oder 'muh4'
     ausgeloest.

     Mit "lvl" begrenzt man die Ausfuehrbarkeit. Spieler haben ein
     Magierlevel von 0, Seher von 1.

     Das Kommando wird in P_LOCALCMDS eingetragen. Diese Property wird
     nicht gespeichert! Effektiv kann man mit AddAction() ein kommando-
     gebendes Objekt im Spieler einsparen.

BEMERKUNGEN
-----------
::

     - es gibt _noch_ kein RemoveAction! Per Hand in P_LOCALCMDS editieren
       kann zu ernsten Fehlern fuehren.
     - echte Spielerkommandos kann man damit _nicht_ ueberschreiben,
       ein AddAction(...,"sag",1,0); funktioniert nicht
     - ein generelles AddAction(...,"",1,0); geht nicht

BEISPIELE
---------
::

     ...
     this_player()->AddAction(symbol_function("zeige_mysterium",
                                              find_object(".../mystzeiger")),
			      "knorfula",0,0);
     write(break_string("Wann immer du jetzt das Kommando \"knorfula\" "
			"eingibst, werden dir Mysterien enthuellt!",78));
     ...

     // im Objekt "knorfula" ...
     int zeige_mysterium(string str) {
       string myst;
       myst=environment(TP)->QueryMysterium(str);
       if(myst) {
         write("Du hast ein Mysterium entdeckt!\n");
	 write(break_string(myst,78));
	 say(break_string(
		TP->Name(WER)+" scheint nach kurzer Konzentration etwas "
		"entdeckt zu haben!",78));
       } else {
         write(break_string(
		"Leider entdeckst du trotz deiner magischen Faehigkeit "
		"kein Mysterium in deiner Umgebung.",78));
	 say(break_string(
		TP->Name(WER)+" konzentriert sich sichtbar, sieht dann "
		"jedoch etwas enttaeuscht aus.",78));
       }
       return 1;
     }

SIEHE AUCH
----------
::

			P_LOCALCMDS
     Fehlermeldungen:	notify_fail(E), _notify_fail(E)
     Argumentstring:	query_verb(E), _unparsed_args(L)
     Sonstiges:		replace_personal(E), enable_commands(E)
     Alternativen:	AddCmd(L), add_action(E)

24. Maerz 2004 Gloinson

