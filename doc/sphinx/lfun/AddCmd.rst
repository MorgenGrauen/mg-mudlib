AddCmd()
========

FUNKTION
--------

    varargs void AddCmd(mixed cmd, mixed func, [mixed flag, [mixed id]]);

DEFINIERT IN
------------


    /std/thing/commands.c

ARGUMENTE
---------

    cmd
       Verben, auf die reagiert werden soll

       ODER

       Regel mit Triggerverb und noetigen Keywords/Synonymen
    func
       Funktionsname im selben Objekt oder Closure (Funktionspointer)
    flag (optional)
       Unscharfe Ausfuehrung == 1

       ODER

       Fehlermeldungen bei Nutzung von Regeln
    id (optional)
       eine ID, ueber die das Kommando eindeutig wieder geloescht
       werden kann

BESCHREIBUNG
------------

    Wenn ein Spieler im Einflussbereich des Objektes das Verb benutzt,
    wird die entsprechende Funktion im Objekt aufgerufen:

    * die Verben sollten Imperative sein
    * die Funktion muss 1 fuer ERFOLG (und FPs!) und 0 sonst zurueckgeben
      (sonst werden evtl. weitere Funktionen mit diesem Kommandoverb gerufen)
    * innerhalb der Funktionen koennen Fehlermeldungen fuer den totalen
      Misserfolg des Kommandos mit notify_fail gesetzt werden
      (anstatt "Wie bitte?")

    AddCmd ist ein dynamischer Ersatz fuer add_action - im Gegensatz
    zu add_action koennen auch ohne erneuten Aufruf der init() neue
    Kommandos hinzugefuegt werden.

    AddCmd kann das Leben einfacher machen mit:

    ### REGELN: ###
    
    Bei komplexen Syntaxen kann ein String angegeben werden, der die
    _notwendigen_ (nicht die erlaubten!) Schluesselworte und deren
    zulaessige Synonyme beschreibt. Damit kann man sich einen grossen
    Teil eigener Auswertung ersparen.

    Nur wenn in richtiger Reihenfolge aus JEDER der durch & getrennten
    Synonymgruppen ein Wort im Spielerkommando enthalten ist, wird
    die zugehoerige Funktion ausgefuehrt.

    Trenner sind: | fuer Alternativen
                  & fuer Konjunktionen

.. code-block:: pike

    Beispiel:
      "ritz|ritze|schnitz|schnitze&mit&messer|schnitzmesser&"
      "herz|herzchen&rinde|baumrinde"
    wuerde z.B. durch
      "ritz mit dem Messer ein Herz in die Rinde des Baumes"
      "schnitz mit Messer Herzchen Baumrinde"
      "schnitz mit meinem Messer Herzchen in die harte Baumrinde"
    erfuellt werden.

 
    Spezialregelteile sind:
    
    * @PRESENT: entspricht einem Objekt in Inv oder Env des Spielers
    * @ID:      entspricht der ID des kommandobesitzenden Objektes
                (wo die Kommandomethode definiert ist, ist noch unwichtig)
    * @PUT_GET_DROP, @PUT_GET_TAKE, @PUT_GET_NONE:
                entsprechend den Filteroptionen fuer find_obs()
    ACHTUNG: Zusaetzliche Ziffern in Verbindung mit den @-Spezialregeln
             sind schlecht. @-Regeln versuchen gierig, Objekte exakt im
             Inventory zu matchen ("objekt 3" anstatt "objekt") und miss-
             interpretieren daher zB die 4 in "stopf objekt 3 in loch 4" als
             Teil des Objekt-ID-Strings.
             Interna: 3 Substrings fuer @PRESENT/@ID ("gruener kristall 2")
                      5 fuer @PUT... ("kristall 2 in beutel 3")

    ### FEHLERMELDUNGEN (bei Anwendung von Regeln): ###
    
    Als dritter Parameter koennen auch Fehlermeldungen fuer jeweils
    fehlende Synonymgruppen (ausser der ersten - den Kommandoverben)
    angegeben werden. Sie werden in derselben Reihenfolge (!) wie die
    Synonymgruppen angegeben.

    Mit nicht von Spielern erfuellbaren Regeln und ^-Fehlermeldungen
    kann man auch ohne Ausfuehrung einer Funktion Texte an Spieler
    und Umgebung ausgeben. Siehe dazu AddCmd_bsp.

    Trenner sind: | zum Trennen der einzelnen Fehlermeldungen
                  ^ um
                     - die Auswertung (ab dieser Fehlermeldung!) mit
                       "return 1;" zu beenden und
                     - eine write^say-Meldung zu trennen und
                     - (fuer funktionslose AddCmd auch FPs zu vergeben!)

    Beispielfehlermeldungen fuer obige Regel:
      "Womit willst Du schnitzen?|Was willst Du schnitzen?|"
      "Wohinein willst Du das schnitzen?"

    Es koennen in den Fehlermeldungen folgende Platzhalter benutzt
    werden:

    * @verb (ersetzt durch query_verb() ohne beendendes 'e')
    * @VERB (ersetzt durch capitalize(query_verb()) ohne beendendes 'e')
    * @WERx, @WESSENx, @WEMx, @WENx: siehe alles aus replace_personal()
      * @WE..1 ist immer der aktive Spieler
      * alle folgenden sind die matchenden Parameter der Spielereingabe
        * (x-1)<=Fehlermeldung (da x=1 Spieler und
                                (x-1)>Fehlermeldungsobjekt nicht existent)

    Ausfuehrungsbeispiel:
      AddCmd("ritz|ritze|schnitz|schnitze&mit&messer|schnitzmesser&"
             "herz|herzchen&rinde|baumrinde",#'fun,
            "Willst Du mit etwas @verben?|Womit willst du @verben?|"
            "Was willst du mit dem @WEM3 @verben?|"
            "Wohinein willst Du das @WEN4 schnitzen?");
      1. "ritze" == "Willst Du mit etwas ritzen?"
      2. "schnitz mit" == "Womit willst du schnitzen?"
      3. "ritz mit messer" == "Was willst du mit dem messer ritzen?"
      4. "ritze mit dem messer ein herz" ==
           "Wohinein willst Du das herz schnitzen?"
      5. "ritze mit dem messer ein herzchen in die baumrinde"
           == Erfolg!

    ### UNSCHARFER AUSFUEHRUNG: ###

    Bei unscharfer Ausfuehrung wird die zugehoerige Methode auch dann
    ausgefuehrt, wenn das verwendete Verb ein Superstring ist und
    bisher noch nicht behandelt wurde.
    Dieses Verhalten sollte nur beim generellen Abfangen von
    Befehlsgruppen benutzt werden und ist ansonsten veraltet. Es
    entsprich add_action("fun","kommando",1).


    Beispiel:
      1. AddCmd("klett","fun",1);
      2. AddCmd("kletter|klettere&hoch",#'fun2,"Wohin klettern?");

      a) "klett"
      b) "kletter"
      c) "klettere hoch"

      Ausgefuehrte Funktion bei: 1a, 1b, 1c; 2c
      (1 wuerde also immer ausgefuehrt)
      Fehlermeldung bei: 2b ("Wohin klettern?")

BEMERKUNGEN
-----------

    * Methoden der put_and_get (nimm/nehme) sollten so nicht versucht
      werden zu ueberschreiben - dazu sind invis Container da
    * benutzt man fuer <function> eine Closure, kann man die Fkt. auch
      protected oder private deklarieren _und_ sie kann in einem
      anderen Objekt sein
    * bei Regeln wird an die ggf. gerufene Methode als zweiter Parameter
      ein Array der erfuellenden Eingabeteile uebergeben:
      "steck&@PRESENT&in&loch" bei Erfuellung -> ({<Objekt>,"in","loch})
      
      ** bei Nutzung von @PUT_GET_XXX koennen die Parameter wiederum
         Arrays sein ("jede Hose" -> mehrere gueltige Objekte)
    * juengere AddCmd ueberschreiben aeltere, bzw. werden vor diesen
      ausgewertet
    * @PUT_GET_XXX kosten sehr viel Auswertungszeit

BEISPIELE (SIEHE AUCH ADDCMD_BSP):
----------------------------------

.. code-block:: pike

    // SIMPEL: ganz simpel, beinahe wie add_action
    AddCmd("befiehl","action_befehlen");
    ...
    int action_befehlen(string str) {
      if(!str || !strlen(str))
        // Fehlermeldung, falls gar keine Funktion 1 dafuer zurueckgibt
        notify_fail("Was willst du befehlen?!\n");
      else {
        write("Du befiehlst \""+str+"\", und alle folgen!\n");
        say(TP->Name(WER)+" befiehlt \""+str+"\", und du folgst!\n");
        return 1;  // ERFOLG - Abbruch der Kommandoauswertung
      }
      return 0;  // MISSERFOLG - Fehlermeldung oben gesetzt
    }

    // SIMPEL .. weitere Beispiele
    AddCmd(({"kletter","klettere"}),"action_klettern" );
    AddCmd(({"renn","renne"}),#'action_rennen);

    // REGELN: eine komplexere Regel
    AddCmd("loesch|loesche|ersticke&feuer|brand|flammen&decke|wolldecke",
           "action_loeschen",
           "Was willst du loeschen?|Womit willst du loeschen?");

    // REGELN: mit Platzhaltern im Fehlerstring
    AddCmd("spring|springe|huepf|huepfe&von|vom&baum|ast|eiche",
           #'action_huepfe,
           "Willst du von etwas @verben?|Von wo willst du @verben?");

    // SCHLECHT: eine unscharfe Regel - sie sollten eine Ausnahme sein (!)
    AddCmd("kletter","fun_klettern",1);

    // FALSCH: sehr schlecht, kein Imperativ verwendet
    // ausserdem sollte man fuer solche Syntaxen AddReadDetail benutzen
    AddCmd("lese","eval_lesen");

    // SIMPLE REGEL OHNE METHODE
    // mit Regeln kann man auch Aktivitaeten im Raum erlauben, ohne eine
    // Funktion aufrufen zu muessen: die letzte Regel ist fuer Spieler
    // unmoeglich zu erfuellen, die dazugehoerige Fehlermeldung wird mit
    // dem ^ (write-Flag) versehen und entsprechend an den Spieler
    // (und den Raum (hinter dem ^)) ausgegeben
    AddCmd("spring|springe&herunter|runter&\n\bimpossible",0,
           "Wohin oder wovon willst Du springen?|"
           "Du springst vom Baum und kommst hart auf.^"
           "@WER1 springt vom Baum und kommt hart auf.");


SIEHE AUCH
----------

    AddCmd_bsp, RemoveCmd(L), init(E)
    Fehlermeldungen: notify_fail(E), _notify_fail(E)
    Argumentstring: query_verb(E), _unparsed_args(L)
    Sonstiges:  replace_personal(E), enable_commands(E)
    Alternativen: AddAction(L), add_action(E)

30. Aug 2013 Gloinson

