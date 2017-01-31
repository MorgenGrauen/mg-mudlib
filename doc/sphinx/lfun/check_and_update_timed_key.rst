check_and_update_timed_key()
============================

FUNKTION
--------
::

       public int check_and_update_timed_key(int duration, string key)

       

DEFINIERT IN
------------
::

       /std/living/life.c    

ARGUMENTE
---------
::

       int duration: In wieviel Sekunden wird <key> wieder freigebeben,
                     (z.B. wann kann der Spieler an dieser Stelle eine neue 
                     Heilung bekommen).
       string key  : Eindeutiger Name, wird zusammen mit <duration>
                     gespeichert.

BESCHREIBUNG
------------
::

       Diese Funktion hat die Aufgabe, Zeitsperren verschiedenster Art
       einfach zu ermoeglichen (z.B. die Realisierung charakter-abhaengiger
       Heilstellen u.ae.).

       <key> muss eindeutig sein, am besten verwendet man den eigenen
       Magiernamen (und ggf. nen Gebietsnamen) als Teil des Strings.

       Die Funktion ist definiert in /std/living/life.c. Somit funktioniert
       sie auch bei NPCs. Die Daten werden in P_TIMING_MAP gespeichert, sind
       also gegen "ende" resistent. (werden allerdings nach Ablauf ggf.
       'aufgeraeumt')

       Das Mapping P_TIMING_MAP ist NICHT zur Abfrage und / oder Manipulation
       'per Hand' vorgesehen.

RUeCKGABEWERT
-------------
::

       0    Irgendein Fehler im Aufruf, evtl. existiert die Funktion (noch)
            nicht im jew. Objekt.

      -1    Alles okay. Neuer Zeitpunkt wird automatisch gespeichert. In
            diesem Fall darf der Spieler geheilt werden.

      >0    Key noch gesperrt, in dem Fall darf also nicht geheilt werden. 
            Der Rueckgabewert ist der Zeitpunkt, ab dem <key> wieder frei ist,
            laesst sich daher dazu nutzen, um dem Spieler einen Anhaltspunkt
            zu geben, wann er die Stelle wieder nutzen kann, etwa:

            "Die Schale ist erst halb voll, Du musst noch etwas warten."

BEISPIELE
---------
::

       Eine Heilstelle soll jedem Spieler alle 5min zur Verfuegung stehen:

       AddCmd(({"trink","trinke"}),"trink_cmd");

       int trink_cmd(string str){
         ...
         ...
         /*
         Der key sollte natuerlich eine etwas eindeutigere Kennzeichnung
         wie etwa "tilly_trinken" bekommen, auch wenn er durch einen
         anderen (gleichnamigen) nicht ueberschrieben werden kann.

         Trifft diese Abfrage hier zu, kann dem Spieler Heilung o.ae. zu-
         gefuehrt werden. Die neue Zeit (duration) wird automatisch gesetzt.
         */
         if(this_player()->check_and_update_timed_key(300,"jof_trinken")==-1){
           if(this_player()->drink_soft(2)){
             this_player()->heal_self(50);
             write("Du fuehlst Dich sichtlich erfrischt.\n");
             return 1;
            }
           else{
             write("Du hast schon zuviel getrunken.\n");
             return 1;
            }
          }
         else {
           write("Du trinkst und denkst . o O (Hmm, nicht schlecht).\n");
           return 1;
          }
         return 0;
        }

BEMERKUNGEN: 
       Auch bei dieser Funktion ist darauf zu achten, dass Properties wie
       P_FOOD, P_DRINK und P_ALCOHOL beruecksichtigt werden.
       Heilstellen sind dem zustaendigen Magier fuer Heilungs-Balance zu 
       melden. Wer dies momentan ist, kann dem Mailalias heilungs_balance
       entnommen werden.

SIEHE AUCH
----------
::

       check_timed_key, eat_food, drink_alcohol, drink_soft, heal_self,
       restore_spell_points, reduce_hit_point


08.01.2012, Zesstra

