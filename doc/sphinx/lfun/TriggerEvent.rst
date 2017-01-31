TriggerEvent()
==============

FUNKTION
--------
::

     varargs int TriggerEvent(string eid, mixed data);

DEFINIERT IN
------------
::

     /p/daemon/eventd.c

DEKLARIERT IN
-------------
::

     /sys/events.h

ARGUMENTE
---------
::

     string eid,
       Die ID des Events, der ausgeloest werden soll.
       Da dieser String fuer alle Events jeweils eindeutig sein muss, 
       empfiehlt es sich, fuer eigene Events z.B. als Praefix den eigenen 
       Magiernamen zu nehmen, z.B. "zesstra_vulkanausbruch".
       ACHTUNG: IDs, die mit 'evt_lib_' beginnen, sind AUSSCHLIESSLICH der
       Mudlib vorbehalten!

     mixed data,
       Daten, die jeder Lauscher uebergeben bekommt. Kann ein beliebiger
       Datentyp sein. Ggf. ein Mapping oder Array benutzen.

BESCHREIBUNG
------------
::

     Der Event mit der ID 'eid' wird ausgeloest. Mit kurzer Verzoegerung
     (meist 0-2s) werden alle fuer 'eid' registrierten Lauscher durch Aufruf
     einer Funktion in ihnen informiert:
     listener->fun(eid, triggerob, data);
     'triggerob' ist hierbei das Objekt, welche TriggerEvent() gerufen hat,
     'data' das, was das triggernde Objekte als Daten weiterverteilen moechte.
     Die einzelnen fun() in den lauschenden Objekten muessen wissen, was sie
     mit 'data' anfangen sollen. ;-)

RUeCKGABEWERT
-------------
::

     1 fuer Erfolg, <=0 fuer Misserfolg.
     1   - Erfolg, Event 'eid' wurde ausgeloest.
     -1  - falsche Argumente wurden uebergeben
     -2  - nicht-oeffentlicher Event und das triggernde Objekt wurde nicht
           fuer diesen Event freigegeben (momentan gibt es noch keine
           nicht-oeffentlichen Events)
     -3  - Event 'eid' existiert nicht, d.h. es gibt keine Lauscher.
     -4  - Es gibt zuviele nicht verarbeitete Events.

BEMERKUNGEN
-----------
::

BEISPIELE
---------
::

     1. Ein Waechter wird angegriffen:
        EVENTD->TriggerEvent("xand_holzfaellerlager_angriff", 
                             (["angreifer": enemy,
                               "ort": environment(this_object()) ]) );
        Alle anderen angemeldeten Waechter der Lagers werden nun informiert
        und koennen ihrerseits reagieren (dem ersten Waechter zuhilfe kommen
        oder auch die Lagertore schliessen).

SIEHE AUCH
----------
::

     events, eventd, UnregisterEvent(), RegisterEvent()


Last modified: 15.08.2007, Zesstra

