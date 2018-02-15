P_HARBOUR
=========

NAME
----

    P_HARBOUR                                  "harbour_name"                   

DEFINIERT IN
------------

    /sys/transport.h

BESCHREIBUNG
------------

    Array mit eindeutiger Bezeichnung des 'Hafens'

BEMERKUNGEN
-----------

    Diese Property wird in Raeumen gesetzt, die als Anleger fuer Transporter
    dienen sollen. Sie enthaelt ein Array aus zwei Elementen, einem String
    und einem String-Array, zum Beispiel:

    Hafen der Sonneninsel: ({ "zur Sonneninsel", ({"sonneninsel"}) }) 
    oder
    Hafen von Titiwu:      ({ "nach Titiwu", ({"titiwu"}) })

    Damit bekommt der Spieler bei einer Abfrage seiner Reiseroute mittels 
    "reise route", sofern er schon eine gesetzt hat, eine Ausgabe wie
    ..

      'Du reist mit dem Floss nach Titiwu' oder
      'Du reist mit dem Wal zur Sonneninsel'.

    Das zweite Element der Property enthaelt eine Liste der IDs, mit denen
    sich der Hafen mit dem Befehl "reise nach <ID>" ansprechen laesst. Im
    Beispiel oben wuerde also "reise nach sonneninsel" und 
    "reise nach titiwu" akzeptiert werden. Der erste Eintrag in dieser ID-
    Liste wird in der Ausgabe des Befehls "reise route" verwendet, sollte
    also den Anlegeort korrekt bezeichnen und nicht z.B. eine Abkuerzung
    enthalten.
    Es bietet sich an, bei bestimmten, deklinierbaren Bezeichnungen alle
    Varianten einzutragen, z.B. "verlorenes land" und zusaetzlich
    "verlorene land" und "verlorenen land", damit alle Varianten von 
    "reise nach ..." funktionieren.

    Ist in einem Hafen diese Property nicht gesetzt, so bekommt der 
    Spieler keinerlei Hinweis darauf, wohin ihn ein bestimmter Trans-
    porter befoerdert. 
    Stattdessen erhaelt er die Bezeichnung 'unbekannt'.

HINWEISE
--------

    Wird der zweite Parameter in dieser Property, d.h. die Liste der 
    Anleger-IDs, nicht korrekt gesetzt, kann das dazu fuehren, dass Spieler
    den hier anlegenden Transporter nicht benutzen koennen, weil es
    keine sinnvolle Syntax gibt, um den gewuenschten Zielhafen zu finden.

    Zu achten ist auch darauf, das der erste Eintrag unveraendert in einer 
    Meldung an den Spieler ausgegeben wird, d.h. Gross-und Kleinschreibung
    sollte korrekt sein.

HISTORIE
--------

    Frueher war der zweite Eintrag in dieser Property ein einzelner String.
    Es existiert eine SetMethode auf dieser Property, die solche Daten in
    altem Code auf die neue Datenstruktur umbiegt. Dies fuehrt dazu, dass
    bei solchen Objekten die im geladenen Raum enthaltenen Daten nicht mit
    den Daten im File uebereinstimmen. Dies moege aber bitte niemand 
    zum Anlass nehmen, in neuem Code veraltete Daten in die Property zu 
    schreiben!
    

SIEHE AUCH
----------

  Properties:     :doc:`P_NO_TRAVELING`, :doc:`P_TRAVEL_INFO`
  Funktionen:     :doc:`../lfun/AddRoute`
  Spielerbefehle: reise
  weitere Doku:   /d/inseln/schiffe/HowTo

LETZTE AENDERUNG
----------------

04.02.2018, Arathorn
