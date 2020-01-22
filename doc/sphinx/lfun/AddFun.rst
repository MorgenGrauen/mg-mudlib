AddFun()
========

FUNKTION
--------
::

     void AddFun(string fun, int next);

DEFINIERT IN
------------
::

     /std/transport.c

ARGUMENTE
---------
::

     fun
          Name der Funktion.
     next
          Zeit bis zur naechsten Fahrplanstation.

BESCHREIBUNG
------------
::

     Dem Fahrplan wird der Aufruf der Funktion fun, die im Transporter
     definiert sein muss, hinzugefuegt. Nach Aufruf der Funktion vergehen
     next Sekunden, bis die naechste Station angefahren wird.

RUeCKGABEWERT
-------------
::

     keiner

HINWEISE
--------
::

     Die genannte Funktion <fun> wird per call_other() an this_object()
     gerufen, d.h. ihre Sichtbarkeit muss static oder public sein.

BEISPIELE
---------
::

     Wenn ein zufaellig ausgewaehlter Passagier eines Schiffes unterwegs
     seekrank werden soll, koennte man das wie folgt realisieren:

     protected void create() {
       ...
       AddFun("seekrank", 5);
       ...
     }

     seekrank() {
       // soll nicht immer passieren
       if (random(5))
         return;

       // Opfer auswaehlen
       object* passagiere = QueryPassengers();
       object opfer;
       if (sizeof(passagiere))
         opfer = passagiere[random(sizeof(passagiere))];

       tell_object(opfer,
         "Du wirst seekrank! Schnell stuerzt Du zur Reling, um Dich zu\n"
         "uebergeben.\n");
       tell_room(this_object(),
         sprintf("%s ueberkommt die Seekrankheit!\n"
                 "%s stuerzt an die Reling, um sich zu uebergeben.\n",
                 opfer->Name(WEN),
                 capitalize(opfer->QueryPronoun(WER))), ({ opfer }) );
     }

SIEHE AUCH
----------
::

     AddRoute(), AddMsg(), /std/transport.c


Last modified: 15.01.2020, Arathorn 

