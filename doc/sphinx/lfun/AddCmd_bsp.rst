ADDCMD() - BEISPIELE
********************

FUNKTION
========
::

    varargs void AddCmd(mixed cmd, mixed func, mixed flag);

BEMERKUNGEN
===========
::

    Die hier aufgefuehrten Komplexbeispiele sind zum Verstaendnis gedacht,
    daher fuehren sie oft Alternativen auf. Die letzte Variante ist dann
    jeweils diejenige, welche am leichtesten das Problem loesen koennte.
    Falls die einem zu komplex ist, hilft vielleicht die vorletzte.

BEISPIELE
=========

Einfache Beispiele
------------------

1. SIMPEL: ganz simpel, beinahe wie add_action()

.. code-block:: pike

   AddCmd("befiehl","action_befehlen");
   // ...
   int action_befehlen(string str) {
     if(!str || !strlen(str))
       // Fehlermeldung, falls gar keine Funktion 1 dafuer zurueckgibt
       notify_fail("Was willst du befehlen?!\n");
     else {
       write("Du befiehlst \""+str+"\", und alle folgen!\n");
       say(TP->Name(WER)+" befiehlt \""+str+"\", und du folgst!\n");
       return 1;  // ERFOLG - Abbruch der Kommandoauswertung
     }
     return 0;    // MISSERFOLG - Fehlermeldung oben gesetzt
   }

2. SIMPEL .. weitere Beispiele

.. code-block:: pike

   AddCmd(({"kletter","klettere"}),"action_klettern" );
   AddCmd(({"renn","renne"}),#'action_rennen);

3. REGELN: eine komplexere Regel

.. code-block:: pike

   AddCmd("loesch|loesche|ersticke&feuer|brand|flammen&decke|wolldecke",
          "action_loeschen",
          "Was willst du loeschen?|Womit willst du loeschen?");

4. REGELN: mit Platzhaltern im Fehlerstring

.. code-block:: pike

   AddCmd("spring|springe|huepf|huepfe&von|vom&baum|ast|eiche",
          #'action_huepfe,
          "Willst du von etwas @verben?|Von wo willst du @verben?");

5. SCHLECHT: eine unscharfe Regel - sie sollten eine Ausnahme sein (!)

.. code-block:: pike

   AddCmd("kletter","fun_klettern",1);

6. FALSCH: sehr schlecht, kein Imperativ verwendet 

.. code-block:: pike

   AddCmd("lese","eval_lesen");

   ausserdem sollte man fuer Lese-Syntaxen AddReadDetail benutzen

7. SIMPLE REGEL

.. code-block:: pike

   static int action_jump(string str);        // Prototype (wegen closure)
   // ...
   AddCmd("spring|springe|huepf|huepfe&von&baum|ast",#'action_jump,
          "Willst Du von etwas @verben?|Wovon willst Du @verben?");
   // ...
   static int action_jump(string str) {
     write(break_string("Du springst vom Baum und kommst hart auf!",78));
     this_player()->move((XXXROOM+"boden"), M_GO, 0,
                         "springt unelegant vom Baum","faellt vom Baum");
     this_player()->Defend(random(100),({DT_BLUDGEON}),([SP_RECURSIVE:1]),
                           this_object());
     return 1;
   }

8. SIMPLE REGEL OHNE METHODE

   mit Regeln kann man auch Aktivitaeten im Raum erlauben, ohne eine
   Funktion aufrufen zu muessen: die letzte Regel ist fuer Spieler
   unmoeglich zu erfuellen, die dazugehoerige Fehlermeldung wird mit
   dem ^ (write-Flag) versehen und entsprechend an den Spieler
   (und den Raum (hinter dem ^)) ausgegeben

.. code-block:: pike

   AddCmd("spring|springe&herunter|runter&\n\bimpossible",0,
          "Wohin oder wovon willst Du springen?|"
          "Du springst vom Baum und kommst hart auf.^"
          "@WER1 springt vom Baum und kommt hart auf.");


Komplexbeispiel: Regeln mit Fehlermeldungen
-------------------------------------------

Variante 1a, OHNE REGELN
^^^^^^^^^^^^^^^^^^^^^^^^
    
  Wenn man keine Regeln verwendet, muss man die Eingabe selbst auswerten.

.. code-block:: pike

   AddCmd(({"bohr","bohre"}),#'action_bohren);
   // ...
   private int action_bohren(string str) {
     string *tmp;
     notify_fail("Wo willst (etwas) Du bohren?\n");
     if(!str) return 0;       // Tja, keine Argumente ...
     tmp=explode(str," ");    // nach " " in Argument-Array aufspalten
     if((i=member(tmp,"loch"))>=0) { // aha, ab jetzt uebernehmen wir :)
       if((j=member(tmp[(i+1)..],"in"))<0 &&
          (j=member(tmp[(i+1)..],"durch"))<0)
         write("Willst Du das Loch in etwas bohren?\n");
       else if((i=member(tmp[(j+1)..],"boden"))<0 &&
               (i=member(tmp[(j+1)..],"erde"))<0)
         write("In/Durch was willst du das Loch bohren?\n");
       else {
         write("Du bohrst ein Loch in den Boden.\n");
         say(this_player()->Name(WER)+" bohrt ein Loch in den Boden.\n");
       }
       return 1;  // "bohre loch" war so eindeutig, dass nur diese
                  // Methode gemeint sein konnte, also brechen wir die
                  // weitere Auswertung auf jeden Fall ab (und geben
                  // eine write-Fehlermeldung)
     } // end if(..."loch")
     return 0;    // "bohre" allein muss nicht diese Methode meinen,
                  // also nur obige notify_fail()-Meldung, falls
                  // sich nach dieser Methode gar keine sonst
                  // angesprochen fuehlt
   } // end fun

Variante 1b, OHNE REGELN, Alternative
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

  prinzipiell koennte die Methode action_bohren() auch so
  aussehen, ist aber nicht ganz so flexibel:

.. code-block:: pike

  private int action_bohren(string str) {
    string unused;
    if(!str || (sprintf(str,"loch in erde%s", unused)!=1 &&
                sprintf(str,"loch durch erde%s", unused)!=1 &&
                sprintf(str,"loch in boden%s", unused)!=1 &&
                sprintf(str,"loch durch boden%s", unused)!=1))
      notify_fail("Willst Du in irgendwas ein Loch bohren?\n");
    else {
      // ...
      return 1;
    }
    return 0;
  }

Variante 2, MIT REGEL
^^^^^^^^^^^^^^^^^^^^^
  
  das gleiche in etwa mal als einfache Regel

.. code-block:: pike

  AddCmd("bohr|bohre&loch&in|durch&erde|boden",#'action_bohren,
         "Was willst du (wohin) bohren?|"
         "Willst du das Loch in etwas bohren?|"
         "Wohin willst du das Loch bohren?");
  // ...
  private int action_bohren(string str, mixed *param) {
    write("Du bohrst ein Loch in den Boden.\n");
    say(this_player()->Name(WER)+" bohrt ein Loch in den Boden.\n");
    // ...
    return 1;
  }

Variante 3, MIT REGEL UND FEHLERMELDUNG
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
   
  und nun mit Fehlermeldungen mit Ersetzungen, so dass wir mehr
  auf die Eingaben des Spielers eingehen

.. code-block:: pike

   AddCmd("bohr|bohre&loch&in|durch&erde|boden",#'action_bohren,
          "Was willst du (wohin) @verben?|"
          "Willst du das Loch in etwas @verben?|"
          "@WER3 was willst du das Loch @verben?");
   // ...
   private int action_bohren(string str, mixed *param) // ...

Variante 4, MIT REGEL, FEHLERMELDUNG UND RETURN 1
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

  In Variante 1 kam sinnvollerweise sehr frueh der Abbruch mit
  "return 1;" und mit Ausgabe von write-Fehlermeldungen, das koennen
  wir auch direkt und ohne eigene Methode.

.. code-block:: pike

  AddCmd("bohr|bohre&loch&in|durch&erde|boden",#'action_bohren,
         "Was willst du (wohin) @verben?|"
         "Willst du das Loch in etwas @verben?^|"
         "@WER3 was willst du das Loch @verben?^");
   // ...
   private int action_bohren(string str, mixed *param) // ...

Variante 5, MIT REGEL, FEHLERMELDUNG, RETURN 1, OHNE FUN
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: pike

  // und falls in action_bohren() nichts ausser Ausgaben passiert, koennen
  // wir uns die auch ganz sparen indem wir eine nichterfuellbare Regel
  // samt Fehlermeldung bauen
  AddCmd("bohr|bohre&loch&in|durch&erde|boden&\nimpossible",0,
         "Was willst du (wohin) @verben?|"
         "Willst du das Loch in etwas @verben?^|"
         "@WER3 was willst du das Loch @verben?^|"
         "Du @verbst ein Loch @WER3 den Boden.^@WER1 @verbt "
         "ein Loch @WER3 den Boden.");

Komplexbeispiel: Spezialregeln @PRESENT und @ID
-----------------------------------------------

Variante 1, OHNE REGELN
^^^^^^^^^^^^^^^^^^^^^^^

  Oft agieren Kommandos auf Objekten im Raum, diese muessen dabei per
  present() identifiziert werden:
  Beispiel ist ein Geldautomat 
  (Hinweis: dieses Beispiel dient der Illustration, die Funktionalitaet an
  sich sollte man besser mit einem Container mit PreventInsert() erzeugen.)

.. code-block:: pike

   AddCmd(({"stopf","stopfe"}),#'action_stopf);
   // ...
   private int action_stopf(string str) {
     string was, unused;
     if(str && (sprintf("%s in automat%s", was, unused)==2 ||
                sprintf("%s in geldautomat%s", was, unused)==2 ||
                sprintf("%s in bankomat%s", was, unused)==2) {
       object o = present(was, this_player());
       if(o) {
         if(o->QueryProp(...)) {
           write(break_string(
             "Du stopfst "+o->name(WEN,1)+" in den Automaten.",78));
           say(...);
         } else {
           write(break_string(
             "Du versuchst "+o->name(WEN,1)+" in den Automaten zu stopfen, "
             "aber "+o->QueryPronoun(WER)+" passt nicht hinein.",78));
           say(...);
         }
       } else {
         write("Was willst du in den Automaten stopfen?\n");
         say(....);
       }
       return 1;
     }
     notify_fail("Was willst du wohin stecken?\n");
     return 0;
   }

Variante 2, MIT REGEL
^^^^^^^^^^^^^^^^^^^^^

  einerseits koennen wir auf diese Weise das Finden von Objekten in Inv 
  und Env in die AddCmd()-Regel integrieren und uns andererseits das 
  Aufzaehlen aller IDs des Automaten ersparen.

  Wie immer werden die gefundenen Matches als Parameterarray an die
  angegebene Methode uebergeben. Das Array enthaelt die mit @PRESENT und 
  @ID gefundenen Treffer praktischerweise als Objekte.

.. code-block:: pike

  AddCmd("steck|stecke&@PRESENT&in&@ID",#'action_stopf,
         "Was willst du wohin stopfen?|"
         "Willst du @WEN2 in etwas stopfen?|"
         "Wohinein willst du @WEN2 stopfen?");
  // ...
  private int action_stopf(string str, mixed *param) {
    if(param[0]->QueryProp(...)) {
      write(break_string(
        "Du stopfst "+param[0]->name(WEN,1)+" in den Automaten.",78));
      say(...);
    } else {
      write(break_string(
        "Du versuchst "+param[0]->name(WEN,1)+" in den Automaten zu "
        "stopfen, aber "+param[0]->QueryPronoun(WER)+" passt nicht "
        "hinein.",78));
      say(...);
    }
    return 1;
  }

Komplexbeispiel: gleiches Verb, mehrere Regeln
----------------------------------------------

  Das Problem mehrerer Regeln fuer ein Kommandoverb besteht darin, dass
  letztlich nur eine der Fehlermeldungen zum Tragen kommt - welche
  genau, ist etwas vage.
  Dabei kann man sich auf eines verlassen: juengere AddCmd werden
  zuerst ausgewertet. Wenn sich das aendert, tretet euren EM.

Problem 1: Mehrere Regeln weil mehrere Zwecke
---------------------------------------------

Variante 1 - GLEICHLAUTENDE FEHLERMELDUNG
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
  
  Fuer alles wird eine identische Fehlermeldung gesetzt, das ist
  natuerlich nicht sehr flexibel oder schoen
  oder man versucht eine bessere Regel zu schaffen, was hier durch
  die Moeglichkeit von zwei oder drei Parameter unmoeglich ist

.. code-block:: pike

  AddCmd("kriech|krieche&hoch|hinauf|hinaus|heraus|raus",#'result_kriech,
         "Wohin willst Du kriechen?");
  AddCmd("kriech|krieche&nach&oben",#'result_kriech,
         "Wohin willst Du kriechen??|Wohin willst Du kriechen?");
  AddCmd("kriech|krieche&aus&loch|grube|falle",#'result_kriech);
         "Wohin willst Du kriechen?|Wohin willst Du kriechen?");


Variante 2 - EIGENE AUSWERTUNG
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
    
  Statt der Verwendung mehrerer aehnlicher AddCmd() bietet sich die eigene 
  Weiterauswertung an, was durch die Uebergabe der getriggerten Argumente
  erleichtert wird.

.. code-block:: pike

    AddCmd("kriech|krieche&hoch|hinauf|hinaus|heraus|raus|aus|nach",
           #'result_kriech,
           "Wohin willst Du kriechen?");
    // ...
    private int result_kriech(string str, mixed *extra) {
      if(member(extra,"aus")>=0 &&
         !sizeof(({str}),"*.\\<(hoehle|grube|falle)\\>.*"))
        notify_fail("Woraus willst Du kriechen?\n");
      else if(member(extra,"nach")>=0 && strstr(str,"oben")<0)
        notify_fail("In welche Richtung willst Du kriechen?\n");
      else if(this_player()->QueryAttribute(A_DEX)>10 ||
              member(holding_root,this_player())) {
        write("Du kriechst mit Muehe heraus.\n");
        this_player()->move((XXXROOM+"draussen"), M_GO, 0,
                            "kriecht mit Muehe aus der Grube",
                            "kriecht aus einer Grube");
        return 1;
      } else
        write("Du bist zu ungeschickt, halt Dich irgendwo fest.\n");
        return 1;
      }
      return 0;
    }

Problem 2: mehrere Regeln aufgrund von optionalen Parametern
------------------------------------------------------------

  Manchmal will man optionale Parameter erlauben, die aber eine
  Wirkung zeigen sollen. Hierbei ist die Reihenfolge der AddCmd()-
  Anweisungen und ggf. deren Aufbau entscheidend.

.. code-block:: pike

  AddCmd("schlag|schlage&@ID&hart",#'action_schlag_hart,
         "Was oder wen willst du @verben?|"
         "Wie willst du @WEN2 schlagen?");
  AddCmd("schlag|schlage&@ID",#'action_schlag,
         "Was oder wen willst du @verben?");

  Da juengere AddCmd aelteren vorgehen, wird die komplexere Regel samt
  ihrer Fehlermeldung nie ausgewertet, da ein "schlag ball hart" auch
  die zweite Regel triggert.

  Anders herum:

.. code-block:: pike

  AddCmd("schlag|schlage&@ID",#'action_schlag,
         "Was oder wen willst du @verben?");
  AddCmd("schlag|schlage&@ID&hart",#'action_schlag_hart,
         "Was oder wen willst du @verben?|"
         "Wie willst du @WEN2 schlagen?");

  Jetzt wird die komplexere Regel zuerst ueberprueft und triggert
  auch die richtige Funktion.
  Leider kommt die Fehlermeldung nie zum Tragen, denn was durch Regel 2
  durchfaellt, triggert entweder Regel 1 oder faellt auch durch Regel 1
  durch und ueberschreibt dabei die Meldung.

.. code-block:: pike

  AddCmd("schlag|schlage&@ID",#'action_schlag,
         "Was oder wen willst du wie @verben?");
  AddCmd("schlag|schlage&@ID&hart",#'action_schlag_hart);

  Das ist zwar auch nur fast perfekt, besser wird es aber nicht.

Letzte Aenderung: 20.11.2019, Arathorn

