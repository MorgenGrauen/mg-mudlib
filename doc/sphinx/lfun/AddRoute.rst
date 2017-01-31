AddRoute()
==========

FUNKTION
--------
::

    public varargs void AddRoute(string room, int stay, int next, 
        string harbour_desc, mixed dest_ids, string deststr)

DEFINIERT IN
------------
::

    /std/transport.c

ARGUMENTE
---------
::

    string room
        Filename der Haltestelle (des Ziels)
    int stay
        Aufenthaltszeit an der Haltestelle.
    int next
        Fahrtzeit von dort bis zum naechsten Fahrplanpunkt
    string harbour_desc
        Name der Haltestelle (fuer QueryArrived)
    mixed dest_ids
        kleingeschriebene IDs der Haltestelle
    string destrstr
        Unbenutzt / Undefiniert.

BESCHREIBUNG
------------
::

    Dem Fahrplan des Transporters wird eine neue Haltestelle hinzugefuegt.

    Bei Erreichen der Haltestelle wird der Transporter in den Raum 'room'
    bewegt und sichtbar gemacht. Nun kann man ihn fuer 'stay' Sekunden
    betreten oder verlassen, bevor er ablegt (unsichtbar gemacht wird).
    Nach 'next' Sekunden wird dann der naechste Punkt des Fahrplans
    ausgefuehrt.

     

    'harbour_desc' ist ein String, den QueryArrived() zurueckgibt, wenn sich
    der Transporter an einer Haltestelle befindet. In der Regel ist das ein
    String, der die Haltestelle naeher beschreibt.

    

    'dest_ids' ist ein Array von Strings, die als ID-Code fuer diese 
    Haltstelle dienen. Das wird zB bei der Ermittlung einer Haltestelle bei 
    "reise nach" benutzt. Wenn 'dest_ids' nicht gesetzt ist, und auch 
    P_HARBOUR des Zielhafens nicht korrekt gesetzt ist, werden 
    grossgeschriebene Begriffe aus 'harbour_desc' verwendet.

BEISPIELE
---------
::

    #1 Hier ein Auszug aus /d/inseln/schiffe/jolle.c:

      AddRoute("/d/ebene/room/PortVain/po_haf2", 40,
               10, "Hafen von Port Vain");

    Die Jolle wird also beim Anlegen in den Port Vainer Hafen bewegt und
    laesst sich dort 40 Sekunden lang betreten oder verlassen.
    QueryArrived() liefert waehrend dieser Zeit den String "Hafen von Port
    Vain" zurueck. Nach den 40 Sekunden legt die Jolle ab, und nach
    weiteren 10 Sekunden wird die naechste Station in ihrem Fahrplan
    angefahren.

    #2 Die Galeere nach Orkhausen:
      AddRoute(INSEL("steg"), 30, 20, "Verlorene Land ( Orkhausen )" );
      - haelt 30 Sekunden
      - reist 20 Sekunden
      - reist nach INSEL("steg"), bekannt als "Verlorene Land ( Orkhausen )"
      - da keine 'dest_ids' angegeben sind, waere eine so definierte 
        Haltstelle nur mit den IDs ansprechbar, die in P_HARBOURS im Zielraum
        angegeben sind.

    Wenn man nun erreichen wollte, dass das Ziel auch mit dem Kuerzel "vland"
    ansprechbar ist, kann man zum einen explizite 'dest_ids' eintragen:
      AddRoute(INSEL("steg"), 30, 20, "Verlorene Land ( Orkhausen )",
               ({"verlorenes", "land", "orkhausen", "vland"}));
    Dies laesst sich im Zielhafen aber auch durch Eintragen des Kuerzels
    "vland" in P_HARBOUR erreichen. Dies hat den Vorteil, dass dieser Hafen
    dann von allen Transportern, die dort anlegen, unter demselben Namen
    erreicht werden kann.

    

HINWEISE
--------
::

    Dadurch, dass die Eintraege aus P_HARBOUR und 'dest_ids' gleichberechtigt
    fuer "reise nach <ziel>" verwendet werden koennen, ist es moeglich,
    dass die Reiseziele auf einem Schiff unter zusaetzlichen Bezeichnungen 
    bekannt sind, als an Land (zum Beispiel koennte eine fernwestliche
    Besatzung die Ziele anders nennen).

SIEHE AUCH
----------
::

Funktionen  AddMsg(L), AddFun(L), /std/transport.c
Properties  P_HARBOUR, P_NO_TRAVELING, P_TRAVEL_INFO

2015-Jan-18, Arathorn.
-------------------------------------------------------------------------------------------------------------------------------
::

