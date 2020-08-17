AddToMenu()
===========

FUNKTION
--------
::

        varargs string AddToMenu(string  menuetext,
                                 mixed   ids,
                                 mapping minfo,
                                 mixed   rate,
                                 mixed   msg,
                                 mixed   refresh,
                                 mixed   delay,
                                 mixed   d_msg);

DEFINIERT IN
------------
::

        /std/pub.c

ARGUMENTE
---------
::

        Die Erlaeuterung der Parameter beschraenkt sich im Folgenden
        zunaechst auf die Grundfunktionalitaet bei Verwendung fester
        Werte. Die Moeglichkeiten zur Realisierung dynamischen Verhaltens
        sind in den Fussnoten zu den einzelnen Parametern beschrieben.

        menuetext
          Der Text steht als kurze Beschreibung im Menue.
        ids
          String oder Array von Strings, mit denen sich die Speise bzw. das
          Getraenk beim Bestellen ansprechen laesst.
        minfo
          Mapping mit Eintraegen fuer:
          P_HP      LP-Heilung in Punkten [*]
          P_SP      KP-Heilung in Punkten [*]
          P_FOOD    Saettigungswirkung der Speise [*]
          P_DRINK   Saettigungswirkung des Getraenks [*]
          P_ALCOHOL Alkoholgehalt des Getraenks [*]
          P_VALUE   Preis der Speise bzw. des Getraenks [*]
        rate [*]
          Heilrate in Punkten pro HeartBeat.
        msg [**]
          Meldung beim Essen:
          Array mit 2 Strings: (1) fuer Empfaenger, (2) fuer Andere.
          Verfuegbare Platzhalter sind weiter unten im Abschnitt "Beispiel"
          dokumentiert.
        refresh
          Mapping mit Eintraegen fuer:
            PR_USER (Kontingent fuer einzelnen Spieler) [*]
            PR_ALL  (Zusatzkontingent fuer alle) [*]
          Alternativ: 0 fuer unbegrenzte Verfuegbarkeit
          Einem Key muessen dabei zwei Werte zugeordnet werden:
          Der Erste gibt die Hoehe des Kontingents an, der Zweite legt
          fest, alle wieviel reset()s das Kontingent wieder aufgefuellt
          wird.
          Verwendung des Mappings erfordert Inkludieren von pub.h.
        delay [*]
          Zahl der Sekunden, um die verzoegert die Heilung eintritt,
          z.B. weil das Essen erst zubereitet werden muss.
        d_msg [**]
          Meldung beim Bestellen, falls die Heilung verzoegert wird
          Array mit 2 Strings: (1) fuer Empfaenger, (2) fuer Andere.

          

        [*] Dieser Parameter kann statt eines festen Zahlenwerts mit 
            folgenden Werten gefuellt werden:

            1) Mapping <racemodifier> der Form:
                   ([ 0 : <standardwert> ,
                    <rasse_1> : <wert_1>, 
                    ... , 
                    <rasse_n> : <wert_n> ]).
               Die Eintraege in diesem Mapping werden gegen die Rasse des
               bestellenden Spielers geprueft und entsprechend die 
               zugehoerigen Werte verwendet.
            2) string <func>: Aufruf erfolgt mittels 
                 call_other(this_object(), func, empfaenger);
               gerufen (aber: siehe Hinweise).
            3) closure <func>: Aufruf erfolgt mittels
                 funcall(func, empfaenger);
            4) Array der Form  ({string <obj>, string <func>}) oder
                               ({object <obj>, string <func>})
               Aufruf erfolgt mittels
                 call_other(obj, func, empfaenger);
               (aber: siehe Hinweise). <obj> ist folglich als Objektpointer 
               oder dessen object_name() anzugeben. <func> wird mittels 
               function_exists() auf Existenz ueberprueft.

               HINWEISE: im Falle von Lieferverzoegerung ("delay") und
               Preis (P_VALUE) wird bei allen Funktionsaufrufen NICHT der 
               Empfaenger, sondern der Zahler uebergeben.
               Im Falle der Kontingent-Liste ("refresh") kann nur die
               verfuegbare Menge modifiziert werden, nicht die Zeit
               bis zum Wieder-Auffuellen.

        [**] Zur Erzeugung variabler Meldungen koennen folgende Werte
             eingetragen werden:

             1) closure <func>: Aufruf erfolgt mittels 
                  funcall(func, zahler, empfaenger, ident, minfo);
             2) string <func>: Aufruf erfolgt mittels
                  call_other(this_object(), func, zahler, empfaenger, 
                             ident, minfo);
             <func> bekommt Zahler und Empfaenger als Objektpointer,
             ident als String und minfo als Mapping mit den 
             jeweiligen Heilwerten uebergeben. minfo entspricht hierbei
             den Daten, die als dritter Parameter an AddToMenu()
             uebergeben wurden.
             HINWEIS: wenn in das minfo-Mapping keine int-Festwerte 
             eingetragen wurden, werden diese gemaess den Regeln unter [*]
             geprueft; Funktionen/Closures werden ggf. ausgewertet und 
             deren Rueckgabewerte an die Funktion <func> uebergeben.
             WICHTIG: Die Rueckgabewerte der Funktion werden nicht 
             ausgewertet. Jeder, der anstatt einer Meldung einen 
             Funktionsaufruf programmiert, muss fuer die Ausgabe der
             Meldungen selbst sorgen.

                   

BESCHREIBUNG
------------
::

        Mit dieser Funktion werden Speisen oder Getraenke in die Karte
        von Kneipen und Restaurants eingefuegt.

RUECKGABEWERT
-------------
::

        Rueckgabewert ist ein String "menuentry%d", wobei %d eine Nummer
        ist, die darueber Auskunft gibt, den wievielten Eintrag in die
        interne Karte der Kneipe diese Speise bzw. dieses Getraenk
        darstellt. Im Prinzip handelt es sich bei dem String um einen Key
        fuer ein Mapping, in dem die Speisen bzw. Getraenke gespeichert
        sind.

BEMERKUNGEN
-----------
::

        Die aelteren Funktionen 'AddDrink' bzw. 'AddFood' werden zwar mithilfe
        dieser maechtigeren Funktion aus Gruenden der Abwaertskompatibilitaet
        simuliert, sollen aber nicht mehr eingesetzt werden.

        

        Die alten Platzhalter && etc. (s.u.) werden weiterhin unterstuetzt,
        sollten aber fuer bessere Wartbarkeit nicht mehr verwendet werden.

        

        Fuer das Testen der Kneipe gibt es in jeder Kneipe den Befehl
        'pubinit'. Hiermit lassen sich die Speisen und Getraenke durch-
        checken. Steht in der Ausgabe bei einem Getraenk/Essen ein FAIL,
        so wird die entsprechende Speise (oder Getraenk) NICHT an Spieler
        verkauft. Ausnahmen fuer Speisen/Getraenke mit hoeheren maximalen
        Werten sind durch Balance-EM zu genehmigen.

BEISPIEL
--------
::

        include <pub.h>

        create()
        {
        AddToMenu("'Opa's Drachenkeule'",({"drachenkeule","keule"}),
        ([P_HP:63,P_SP:63,P_FOOD:9,P_VALUE:528]), 5,
        ({"Du isst die Keule mit einem schlechten Gewissen.",
          "@WER1 isst die Keule mit einem schlechten Gewissen."}),
        ([ PR_USER : 4; 1 , PR_ALL : 20; 3 ]), 9,
        ({"Der unsichtbare Kneipier schneidet einem Rentner ein grosses "
          "Stueck aus dessen Keule und bereitet sie Dir zu. Komisch, muss "
          "wohl ein Tippfehler auf der Karte gewesen sein.",
          "Der unsichtbare Kneipier schneidet einem hilflosen Opa ein "
          "Stueck aus dessen Keule und braet diese fuer @WEN1."}) );
        }

        1) Name der Speise (des Getraenks) auf der Karte (bei menue).

           AddToMenu("'Opa's Drachenkeule'",     

        2) ids mit denen sich bestellen laesst (z.B. "kaufe keule").

           ({"drachen","drachenkeule","keule"}),

        3) Heilung fuer LP und KP, Saettigung (P_FOOD oder P_DRINK,
           P_ALCOHOL nach Belieben setzen), Preis (P_VALUE). 
           HP und SP muessen nicht gleich sein. Speisen und Getraenke,
           die nur eines von beiden heilen, sind auch moeglich.

           ([P_HP:63,P_SP:63,P_FOOD:9,P_VALUE:528]),

        4) Heilung pro Heartbeat (in diesem Beispiel je 5 KP/LP).

           5,

        5) Meldungen fuer Spieler und Umstehende die bei Genuss ausgege-
           ben werden (also NICHT der Bestell-Text).

           ({"Du isst die Keule mit einem schlechten Gewissen.",
             "@WER1 isst die Keule mit einem schlechten Gewissen."}),

           Die Ausgabe-Strings werden vor der Ausgabe mit dem Empfaenger
           als Objekt an replace_personal() uebergeben. Fuer die
           moeglichen Platzhalter siehe dort.

        6) Die Speise ist in ihrer Anzahl begrenzt. Fuer jeden Spieler
           sind 4 Keulen pro reset() da. Ausserdem gibt es noch einen
           "Notvorrat" von 20 Keulen, der alle 3 reset()s aufgefuellt
           wird. Aus diesem (so noch vorhanden) werden die Spieler
           versorgt, wenn ihr "persoenlicher Vorrat" aufgebraucht ist.

           ([ PR_USER : 4; 1 , PR_ALL : 20; 3 ]),

           HINWEIS: bei Benutzung des Mappings muss <pub.h> inkludiert 
           werden!

           Wenn man keine reset-abhaengigen Speisen haben moechte, traegt
           man hier eine 0 ein.


        7) Die Zahl ist die Wartezeit in Sekunden, die der Wirt z.B. fuer
           die Zubereitung und Auslieferung an den Spieler braucht.

           9,

        8) Letztendlich die Meldungen an Spieler und Umstehende, die bei Be-
           stellung (hier 'kaufe keule') ausgegeben werden.

           ({"Der unsichtbare Kneipier schneidet einem Rentner ein grosses "
           "Stueck aus dessen Keule und bereitet sie Dir zu. Komisch, muss "
           "wohl ein Tippfehler auf der Karte gewesen sein.",
           "Der unsichtbare Kneipier schneidet einem hilflosen Opa ein "
           "Stueck aus dessen Keule und braet diese fuer @WEN1."}));


LISTE DER ALTEN PLATZHALTER (DEPRECATED):
-----------------------------------------
::

           &&  - pl->name(WER,2)
           &1& - pl->name(WER,2)
           &2& - pl->name(WESSEN,2)
           &3& - pl->name(WEM,2)
           &4& - pl->name(WEN,2)
           &1# - capitalize(pl->name(WER,2))
           &2# - capitalize(pl->name(WESSEN,2))
           &3# - capitalize(pl->name(WEM,2))
           &4# - capitalize(pl->name(WEN,2))
           &!  - pl->QueryPronoun(WER)
           &5& - pl->QueryPronoun(WE);
           &6& - pl->QueryPronoun(WESSEN)
           &7& - pl->QueryPronoun(WEM)
           &8& - pl->QueryPronoun(WEN)
           &5# - capitalize(pl->QueryPronoun(WER))
           &6# - capitalize(pl->QueryPronoun(WESSEN))
           &7# - capitalize(pl->QueryPronoun(WEM))
           &8# - capitalize(pl->QueryPronoun(WEN))

SIEHE AUCH
----------
::

        AddFood(), AddDrink(), /sys/pub.h
        RemoveFromMenu(), replace_personal()

Last modified: Sam, 01. Okt 2011, 23:40 by Arathorn

