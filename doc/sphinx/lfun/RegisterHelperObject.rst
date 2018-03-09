RegisterHelperObject()
======================

FUNKTION
--------
::

     int RegisterHelperObject(object helper, int type, 
                              string|closure callback);

DEFINIERT IN
------------
::

     /std/living/helpers.c

ARGUMENTE
---------
::

     object helper
       Das Objekt, das bei einem Lebewesen als Hilfsobjekt registriert 
       werden soll. Das Objekt muss sich dazu nicht im Inventar des
       Lebewesens befinden.
     int type
       Helfertyp, einer der in /sys/living/helpers.h definierten Typen:
       - HELPER_TYPE_AERIAL fuer die Flug-/Segelunterstuetzung
       - HELPER_TYPE_AQUATIC fuer Tauchunterstuetzung
     string|closure callback
        Closure oder Funktionsname als String; dies ist die Funktion, die im
        Objekt helper gerufen wird, um abzufragen, ob es sich aktuell fuer
        zustaendig erklaert oder nicht.

BESCHREIBUNG
------------
::

     Das Objekt "helper" wird als Hilfsobjekt fuer bestimmte Aktivitaeten wie
     zum Beispiel Fliegen oder Tauchen registriert. 

     

     Die als Callback uebergebene Funktion wird bei der Abfrage der 
     P_*_HELPERS-Properties (siehe unten) aufgerufen und deren 
     Rueckgabewert in der Property vermerkt. 

     

     Der Rueckgabewert der Callback-Methode muss im Wertebereich 
     0..10000 liegen, wobei ein Wert von 0 bedeutet, dass das Hilfsobjekt 
     sich gerade nicht zustaendig fuehlt. Bei den Werten >0 ist es dem 
     abfragenden Objekt ueberlassen, wie es diese Daten bewertet.

     Die Funktion muss existieren und public sein, d.h. von aussen gerufen 
     werden koennen.

     

     Die Funktion bekommt das Spielerobjekt und das abfragende Objekt als
     Parameter uebergeben.

HINWEIS
-------
::

     Es ist ein Unterschied, ob sich ein Objekt via UnregisterHelperObject()
     als Helfer austraegt, oder ob der Aufruf der Callback-Funktion eine
     0 zurueckgibt. Im letzteren Fall ist das Objekt nach wie vor 
     registriert und kann trotzdem vom abfragenden Objekt als 
     funktionsfaehig angesehen werden.

RUECKGABEWERTE
--------------
::

      1  Objekt wurde erfolgreich registriert (HELPER_SUCCESS)
     -1  angegebenes Hilfsobjekt existiert nicht (HELPER_NO_CALLBACK_OBJECT)
     -2  angegebenes Hilfsobjekt ist bereits fuer diese Art der
         Unterstuetzung registriert (HELPER_ALREADY_LISTED)

BEISPIELE
---------
::

     a) Eine luftgefuellte Blase will sich als Tauch-Helfer am Spieler
     anmelden und ist zustaendig, solange sich noch Luft in ihr befindet:

     int luft = 5; // globale Variable z.B. fuer Luftinhalt von 5 Atemzuegen

     // Registrierung im Spielerobjekt
     if ( TP->RegisterHelperObject(ME, HELPER_TYPE_AQUATIC,
          "DivingCallback") == HELPER_SUCCESS ) {
       tell_object(TP, "Du kannst jetzt mit Hilfe der Luftblase unter Wasser "
         "atmen.\n");
     }

     // hier koennen natuerlich auch noch komplexere Dinge ablaufen, z.B.
     // wenn ein Wert zurueckgegeben werden soll, der nicht nur 0 oder 1 ist.
     public int DivingCallback(object player, object caller) {
       return (environment(ME)==player && luft>0);
     }

     b) Ein Spieler befindet sich in einem Raum mit einem steilen Abhang, den
     man hinunterspringen kann. Dies geht aber nur dann gefahrlos, wenn es
     Hilfsobjekte gibt, die den (Segel)flug erlauben:

     // cmd_springen() sei die Funktion, die bei der Eingabe des Befehls durch
     // den Spieler aufgerufen wird.
     static int cmd_springen(string str) {
       [...]
       // Property abfragen
       mapping helpers = TP->QueryProp(P_AERIAL_HELPERS);
       // Liste der Werte fuer die einzelnen Unterstuetzungsobjekte ermitteln
       int *values = m_values(helpers,0);
       // Spieler schonmal runterbewegen, die Folgen seines Handelns spuert
       // er dann, wenn er unten angekommen ist.
       TP->move(zielraum, M_GO|M_SILENT);
       // "helpers" ist immer ein Mapping, das pruefen wir nicht extra.
       // Wenn die Liste der Objekte noch mindestens ein Element enthaelt,
       // nachdem alle unzustaendigen Hilfsobjekte (Rueckgabewert der
       // Callback-Funktion == 0) rausgerechnet wurden, existiert mindestens
       // ein geeignetes Hilfsobjekt.
       if ( sizeof(values-({0})) ) {
         tell_object(TP, "Sanft segelst Du den Hang hinab.\n");
       }
       else {
         tell_object(TP, BS("Todesmutig springst Du den Hang hinab und "
           "schlaegst hart unten auf. Du haettest vielleicht daran denken "
           "sollen, Dir geeignete Hilfsmittel fuer so eine Aktion zu "
           "besorgen.");
         TP->Defend(800, ({DT_BLUDGEON}), ([SP_NO_ACTIVE_DEFENSE:1]), ME);
       }
       return 1;
     }

SIEHE AUCH
----------
::

     Funktionen:  UnregisterHelperObject(), GetHelperObject()
     Properties:  P_HELPER_OBJECTS, P_AERIAL_HELPERS, P_AQUATIC_HELPERS
     Sonstiges:   /sys/living/helpers.h

05.02.2015 Arathorn

