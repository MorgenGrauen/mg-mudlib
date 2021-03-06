P_DISABLE_COMMANDS
==================

NAME
----
::

     P_DISABLE_COMMANDS             "p_lib_disablecommands"

DEFINIERT IN
------------
::

     /sys/player/command.h

BESCHREIBUNG
------------
::

    Mit dieser Prop kann man verhindern, dass Kommandos eines Spielers
    verarbeitet werden. Dies ist z.B. in Sequenzen nuetzlich, wo der Spieler
    rein passiv konsumieren soll.
    In diese Property muss ein Array mit 2 oder 3 Elementen geschrieben 
    werden:
       1) Endzeitpunkt in Sekunden seit 1.1.1970 (int)
       2) Meldung (String oder Closure)
       3) (optional) Array von trotzdem erlaubten Verben (string*)
         (nur ausgewertet, wenn die Meldung ein String ist)

    

    Ist die Meldung ein String, wird dieser einfach bei jedem Kommando so wie
    er ist an den Spieler ausgegeben und das Kommando abgebrochen.
    Ist die Meldung eine Closure wird diese bei jedem Kommando aufgerufen und
    das Kommando abgebrochen, wenn sie einen Rueckgabewert != 0 zurueckgibt.
    In diesem Fall ist die gerufene Funktion dafuer verantwortlich, geeignete
    Meldungen an den Spieler auszugeben!
    Der Funktion wird der vom Spieler eingebene Befehl (string) als erstes
    Argument uebergeben. Zu diesem Zeitpunkt wurde alle Aliase schon
    ausgewertet. Die Funktion kann den Kommandogeber via this_player()
    ermitteln.
    Fuer weitere Informationen steht auch command_stack() zur Verfuegung,
    allerdings ist dort die Alias-Ersetzung nicht beruecksichtigt.

    Die Ausnahmeliste wird nur fuer simple Strings als Meldung ausgewertet,
    wird eine Closure verwendet, kann diese besser selber die Ausnahmen
    bestimmen.

    

    Fragt man diese Prop ab, erhaelt man ein Array mit 4 Elementen: setzendes
    Objekt (object), Ablaufzeit (int), Meldung (String oder Closure) und
    Liste von Ausnahmen (string*).

BEMERKUNGEN
-----------
::

    1. Die Prop wird fuer Magier mit eingeschaltetem P_WANTS_TO_LEARN
       ignoriert.
    2. Wenn das Objekt zerstoert wird, was die Prop gesetzt hat, wird der
       Eintrag unwirksam.
    3. Wenn diese Prop gesetzt und nicht abgelaufen ist, kann nur das gleiche
       Objekt sie mit neuen Daten ueberschreiben. Alle anderen Objekte koennen
       die Prop nur loeschen. Dies soll verhindern, dass Magier unabsichtlich
       einfach anderer Leute Eintraege ueberschreiben. Dementsprechend: Lasst
       die Finger davon, wenn die schon gesetzt ist. ;-)
    4. Diese Prop darf _ausschliesslich_ mit SetProp() und QueryProp() benutzt
       werden, Set() und Query() funktionieren _nicht_.
    5. Es gibt einige Verben, die NIE blockiert werden. Zur Zeit sind dies
       "mrufe", "mschau", "bug", "idee", "typo" und "detail".
    6. Bitte nicht missbrauchen, speziell nicht dazu, die Kommandos von
       Spieler zu ueberwachen/mitzuschreiben. Das Setzen dieser Prop wird 
       geloggt.

BEISPIEL
--------
::

   In einem Raum startet eine Sequenz, in der der Spieler nichts machen soll:

   if (!pl->QueryProp(P_DISABLE_COMMANDS))
      pl->SetProp(P_DISABLE_COMMANDS,
            ({ time() + 120, "Du bist tief in Deinem Traum gefangen!\n" }) );
   else // ... Fehlerbehandlung, falls Prop schon gesetzt ...

   

   Soll die Prop aus irgendeinem Grund (vorzeitig) geloescht werden:
   pl->SetProp(P_DISABLE_COMMANDS, 0);

SIEHE AUCH
----------
::

     command(), query_command(), command_stack(), modify_command(), 
     this_player()

01.12.2012, Zesstra

