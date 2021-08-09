AddInfo()
=========

FUNKTION
--------
:: 

  public varargs void AddInfo(string|string* key, string|closure info,
                 string indent, int|string silent, string|closure casebased);

DEFINIERT IN
------------

     /std/npc/info.c

ARGUMENTE
---------

     string|string* frage
       Schluesselphrase(n), fuer die der NPC eine Antwort geben soll, wenn
       man ihn danach fragt

     string|closure meldung
       Information, die gegeben werden soll; wenn 'meldung' eine
       Closure ist, wird der gerufenen Funktion die vom Spieler gefragte
       Schluesselphrase uebergeben.

     string indent    (optional)
       Text, der sich bei mehrzeiligen Meldungen wiederholen soll.

     int|string silent    (optional)
       Ist silent gesetzt, so erfolgt Antwort nur an Fragenden. 

     string|closure casebased   (optional)
       Closure mit Returnwert string oder int. Der Funktion wird die vom
       Spieler gefragte Schluesselphrase uebergeben.

BESCHREIBUNG
------------

     Wenn ein Spieler ein NPC mittels "frage <monstername> nach <frage>" nach
     einer Information mit dem Schluessel 'frage' fragt, so wird die
     entsprechende 'meldung' ausgegeben (oder die Closure in 'meldung'
     gerufen und der zurueckgegebene Text ausgegeben). Der Meldung wird
     der Name des Monsters vorangestellt.

     Frage:
      Schluesselphrasen muessen kleingeschrieben sein, koennen aber
      Leerzeichen enthalten.

     Meldung:
      Wenn kein 'indent' angegeben ist, muss man die Meldung selbst
      umbrechen.

     Indent:
      Wird ein 'indent' angegeben so wird jeder Zeile hinter dem
      Monsternamen noch das 'indent' vorangesetzt.
      Ein typisches indent ist "sagt: ".

     Silent:
      Bei 'silent'==1 erfolgt keine Textausgabe der Antwortmeldung im Raum,
      ist 'silent' ein String, so wird jener an alle anderen Spieler ausser
      dem Fragesteller im Raum ausgegeben.
      Dem ausgegebenen Text wird der Name des NPCs als Indent vorangestellt.

     Casebased:
      Die als Closure angegebene Methode entscheidet, ob oder wie der NPC 
      auf diese Frage antworten soll:

      # return 0:    normale Antwort mit "meldung"

      # return 1:    keine Antwort/Antwort mit DEFAULT_NOINFO

      # return string: Antwort mit string unter Beruecksichtigung eines indent


     Die Strings von 'silent' und 'meldung' werden geparsed.
     Dabei koennen die @[...]-Tags von replace_personal() verwendet werden,
     Objekt 1 ist this_player(). Ersetzte Strings am Satzanfang werden
     automatisch gross geschrieben.
     AddInfo() konvertiert die alten Schluesselworte @WER, @WESSEN, @WEM,
     @WEN zu denen von replace_personal(), jedoch nicht in den Rueckgabe-
     werten von Closures.

     Mittels der in <npc.h> definierten Frage DEFAULT_INFO kann eine
     Meldung gesetzt werden, die gegeben werden soll, wenn der Spieler
     etwas fragt, auf das keine Antwort vorgegeben ist (das loest
     SetProp(P_DEFAULT_INFO, <text>) ab).

BEISPIELE
---------
     Siehe auch: /doc/beispiele/AddInfo/

.. code-block:: pike

     // Beispiel 1: ### eine Standardantwort setzen ###
     AddInfo(DEFAULT_INFO, "starrt Dir boese in die Augen.\n");
     // identisch zu
     // obsolet: SetProp(P_DEFAULT_INFO, "starrt Dir boese in die Augen.\n");

.. code-block:: pike

     // Beispiel 2: einfache Beispiele, auch mit casebased
     AddInfo(({"knete","kohle"}),
         "sagt: ich habe so etwas nicht.\n");
     AddInfo("geld",
         "Ich habe zwar kein Geld, aber [...] blablabla [...]",
         "sagt: ");
     AddInfo("muenzen",
         "fluestert: Du willst Geld?\n",
         0,
         "fluestert @WEM1 etwas zu.\n");

     // "frage monster nach geld": alle im Raum hoeren
     //  Das Monster sagt: Ich habe zwar kein Geld, aber [...]
     //  Das Monster sagt: [...] blablabla [...]

     // "frage monster nach muenzen":
     // - der Fragensteller hoert:
     //   "Das Monster fluestert: Du willst Geld?"
     // - alle andere hoeren:
     //   "Das Monster fluestert <Fragenstellernamen> etwas zu."

.. code-block:: pike

     // Beispiel 3: dynamisch
     // ein Prototyp, damit wir die Methode bekannt machen
     protected string query_kekse(string key);

     AddInfo(({"keks","kekse"}),
         #'query_kekse,        // ein Verweis auf die Funktion
         "sagt: ");

     protected string query_kekse(string key) {
       if(present("keks", this_object()))
         return("Ich hab noch welche. Aetsch!");
       else if(present("keks", environment()))
         return("Da liegt einer!");
       return("Menno. Keine mehr da!");
     }
     // "frage monster nach keks":
     // - wenn es noch Kekse hat, hoeren alle:
     //   "Das Monster sagt: Ich hab noch welche. Aetsch!
     // - sonst:
     //   "Das Monster sagt: "Menno. Keine mehr da!

.. code-block:: pike

     // Beispiel 4: dynamischer
     // ein Prototyp, damit wir die Methode bekannt machen
     protected string query_kekse(string key);
     protected mixed case_fighting(string key);
     
     AddInfo(({"keks","kekse"}),
         #'query_kekse,"        // ein Verweis auf die Funktion
         "sagt: ",
         0,                     // nicht silent :)
         #'case_fighting);      // noch ein Funktionsverweis

     protected string query_kekse(string key) {
       if(present("keks"))
         return("Ich hab noch welche. Aetsch!");
       return("Menno. Keine mehr da!");
     }

     protected mixed case_fighting(string key) {
       if(InFight())
         return("Keine Zeit fuer Kekse. Muss kaempfen.");
       return 0;
     }

     // "frage monster nach keks":
     // - wenn es kaempft, hoeren alle:
     //   "Das Monster sagt: Keine Zeit fuer Kekse. Muss kaempfen.
     // - sonst, wenn es noch Kekse hat, hoeren alle:
     //   "Das Monster sagt: Ich hab noch welche. Aetsch!
     // - sonst:
     //   "Das Monster sagt: "Menno. Keine mehr da!

.. code-block:: pike

     // Beispiel 5: ### dynamisch und komplex ###
     // ein Prototyp, damit wir die Methode bekannt machen
     protected string question_gold(string key);

     // "gold" wird eine Closure auf die Methode question_gold()
     // zugewiesen, ausserdem soll es still bleiben (wir informieren
     // den Restraum selbst)
     AddInfo("gold", #'question_gold, "murmelt: ", 1);

     // los gehts, wir generieren unsere Antwort selbst
     protected string question_gold(string key) {
       // wieviel Kohle hat der Spieler
       int money = this_player()->QueryMoney();
       string* valuables = ({});
       
       // Wertgegenstaende suchen, d.h. Material kein Gold, 
       // Objekt kein Geld, und auch nicht unsichtbar
       foreach(object o : deep_inventory(this_player())) 
       {
         if ( o->QueryMaterial(MAT_GOLD)>0 && 
              load_name(o) != "/items/money" &&
              o->QueryProp(P_SHORT) &&
              !o->QueryProp(P_INVIS) )
         {
           valuables += ({o->Name(WER,1)});
         }
       }

       // das geht an alle anderen im Raum, silent bietet sich hier
       // nicht an, weil es mehrere Moeglichkeiten gibt
       send_room(environment(),
         sprintf("%s murmelt %s etwas zu%s.",
                 Name(WER,1),
                 this_player()->name(WEM,1),
                 (money || sizeof(valuables))?
                   " und glotzt "+this_player()->QueryPronoun(WEN)+
                   " gierig an" : ""),
         MT_LOOK|MT_LISTEN,
         MA_EMOTE, 0, ({this_player()}));

       // Und hier die Antwort an den Spieler selbst, mit diversen 
       // Verzweigungen fuer dessen Besitztum.
       string reply = "Ich hab kein Gold bei mir.";
       string verb = sizeof(valuables) ? "ist" : "sind";
       if ( money ) 
       {
         reply += " Aber Du hast ja jede Menge Kohle bei dir, so etwa "+
                  money+" Muenzen.";
         if ( sizeof(valuables) ) 
           reply += sprintf(" Ausserdem %s auch noch %s aus Gold.",
                      verb, CountUp(valuables));           
       }
       else if ( sizeof(valuables) )
       {
           reply += sprintf(
             " Aber Du hast ja ein paar Wertsachen dabei: %s %s aus Gold.",
             CountUp(valuables), verb);
       }
       return reply;
     }

     // "frage monster nach gold"
     // - der Fragesteller hoert zB:
     //   Das Monster murmelt: Ich hab kein Gold bei mir. Aber Du hast ja
     //   Das Monster murmelt: jede Menge Kohle bei Dir, so etwa <number>
     //   Das Monster murmelt: Muenzen. Ausserdem ist/sind noch <object1>
     //   Das Monster murmelt: und <object2> aus Gold."
     // - die Umstehenden hoeren:
     //   "Das Monster murmelt @WEM1 etwas zu."
     //   oder
     //   "Das Monster murmelt @WEM1 etwas zu und glotzt ihn/sie gierig an."


SIEHE AUCH
----------

     Verwandt:
       :doc:`AddSpecialInfo`, :doc:`RemoveInfo`
     Props:
       :doc:`../props/P_PRE_INFO`
     Files:
       /std/npc/info.c
     Loggen:
       :doc:`../props/P_LOG_INFO`
     Interna:
       :doc:`GetInfoArr` , :doc:`do_frage`

09.08.2021, Zesstra
