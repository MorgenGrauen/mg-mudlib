P_TMP_DEFEND_HOOK
=================

********************* VERALTETE PROPERTY ******************************
* Diese Property ist veraltet. Bitte nicht mehr in neuem Code nutzen. *
***********************************************************************
P_TMP_DEFEND_HOOK

NAME
----
::

     P_TMP_DEFEND_HOOK             "defend_hook"

DEFINIERT IN
------------
::

     /sys/new_skills.h

BESCHREIBUNG
------------
::

     Mittels dieser Property koennen die Abwehren eines Livings temporaer
     veraendert werden.

     Es wird an dem Living die Property als mindestens 3-elementiges Array

        ({int zeitpunkt, object objekt, string methode, ...})

     gesetzt und die Methode 'methode' wird dann waehrend des Defend() des
     Lebewesens in 'objekt' aufgerufen, solange time()<'zeitpunkt'.

     Der Methode werden die Parameter der Defend() uebergeben:
     int dam, string* dam_type, int|mapping spell, object enemy
     - spell ist definitiv ein Mapping - ein an Defend() uebergebener
       int-Wert wurde aequivalent umgesetzt.
     - dam_type ist definitiv ein Array von Schadenswerten oder einem Wert

     Zudem findet der Aufruf der Methode nach dem Abarbeiten der P_DEFENDERS
     statt, diese koennen also weitere Modifikationen vorgenommen haben.

     Gibt die Methode:
     - 0 zurueck, wird Defend() abgebrochen (und damit der Schaden voellig
       abgefangen) - nur noch die Flucht wird geprueft
     - ein 3-elementiges Array ({dam, dam_type, spell}) zurueck,
       werden diese Werte in der Defend() an Stelle der uebergebenen Werte
       verwendet
     - weder noch zurueck, wird das Ergebnis ignoriert und die Defend laeuft
       regulaer weiter

BEMERKUNGEN
-----------
::

     - Bitte das neuere Hooksystem (s. Manpage std/hooks) benutzen.
     - Der korrekte Union-Datentyp fuer die Property ist
       <int|object|string>* fuer die normale Nutzung mit 3 Elementen.
       Ein weit verbreiteter Anwendungsfall verwendet 2 weitere Elemente,
       die vom Typ int und string|string* sind. Alle Stellen, die die Property
       abfragen, sollten daher den Union-Datentyp <int|object|string|string*>*
       verwenden. Das Beispiel beruecksichtigt diesen Fall bereits.
     - Falls die Zeit abgelaufen oder das Objekt zerstoert ist, wird die
       Property auf 0 gesetzt.
     - Vor dem Setzen immer auf die Existenz eines gueltigen Hooks
       pruefen, einfaches Ueberschreiben fuehrt einerseits zu Fehlern
       im Code anderer und ist andererseits unfair gegenueber ihnen.

BEISPIEL
--------
::

     *** ein gruener Schutzzauber ***
     // Vor dem Setzen der Prop pruefen, ob schon ein Hook gesetzt ist.

     <int|object|string|string*>* tmp = TP->QueryProp(P_TMP_DEFEND_HOOK);

     // Ein etwas ausfuehrlicherer Check, ob wir ueberschreiben koennen,
     // Existenz && noch nicht abgelaufen?
     if(sizeof(tmp) && intp(tmp[0]) && time() < tmp[0]) {
       write("Der Zauber klappt nicht!\n");
     }
     else {
       // der Zauber gilt eine Stunde
       TP->SetProp(P_TMP_DEFEND_HOOK,({time()+3600,TO,"abwehrfun");
       write("Ein Zauber legt sich auf Dich.\n");
     }
     ...

     // die gerufene Methode
     int|<int|string*|mapping>* abwehrfun(int dam, string* dam_type,
                                      mapping spell, object en)
     {
       // keine rekursiven Schaeden abfangen ... mindestens ein magischer
       // muss auch dabei sein
       if(!spell[SP_RECURSIVE] &&
          sizeof(filter(dam_type,MAGICAL_DAMAGE_TYPES))) {
         // mit 10% Whkeit schuetzt der Zauber total
         if(!random(10)) {
           tell_object(previous_object(),
             "Dein Zauber gleisst rund um Dich gruen auf.\n");
           tell_room(environment(previous_object()), break_string(
             previous_object()->Name(WESSEN)+" Haut gleisst rund um "+
             previous_object()->QueryPronoun(WEN)+" gruen auf.",78),
             ({previous_object()}));

           // manchmal geht der Zauber dabei endgueltig kaputt
           if(!random(10))
             previous_object()->SetProp(P_TMP_DEFEND_HOOK, 0);

           return 0;			// volles Abfangen des Angriffs
         }

         // der Zauber schuetzt auf jeden Fall immer ein bischen
         tell_object(previous_object(),
           "Dein Zauber flackert rund um dich gruen auf.\n");
         tell_room(environment(previous_object()), break_string(
           previous_object()->Name(WESSEN)+" Haut flackert rund um "+
           previous_object()->QueryPronoun(WEN)+" gruen auf.",78),
           ({previous_object()}));
         dam = (7+random(2))*dam/10;	// Schaden reduzieren

         return(({dam, dam_type, spell}));
       }

       // der Zauber schuetzt dann gar nicht ...
       return 1;
     }

SIEHE AUCH
----------
::

     Angriff:	   Attack(L)
     Schutz:     Defend(L)
     Verwandt:	 InternalModifyDefend(L), P_TMP_ATTACK_MOD
     Hooks:	     P_TMP_DIE_HOOK, P_TMP_MOVE_HOOK, P_TMP_ATTACK_HOOK
     neue Hooks: std/hooks

29.12.2020, Arathorn

