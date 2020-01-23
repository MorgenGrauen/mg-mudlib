filter
======

BEMERKUNGEN
-----------

  1. Achtung, die Elemente in 'arr' werden nicht tief kopiert, sind sie
     also selbst Arrays oder Mappings, so fuehrt eine spaetere Aenderung
     im Rueckgabe-Arrays zur Aenderung im Ursprungsarray:

     .. code-block:: pike

       int *i, *j;
       i=({({1,2,3}),({4,5,6})});
       j=filter(i, #'sizeof);     // filtert leere Arrays heraus
       j[0][0]=8;

    fuehrt zu: i==j==({({8,2,3}),({4,5,6})});

  2. Das Kopieren in das Rueckgabemapping erfolgt fuer jedes Element nach
     Ausfuehrung der Filtermethode. Aenderungen der Werte im Array in dieser
     Methode (globale Variable/Uebergabe als Referenz an filter)
     schlagen sich also im Rueckgabearray nieder.

  3. Fuer Arrays wirkt filter() wie filter_array(), fuer Mappings stellt
     filter() eine Verallgemeinerung von filter_indices() dar.

BEISPIELE
---------

::

    ### Filtere alle Lebewesen in einem Raum in ein Array ###
    filter(all_inventory(this_object()),#'living);

    ### Filtere alle tauben Spieler im Raum in ein Array ###
    static int filter_isdeaf(object who) {
      return (interactive(who) && who->QueryProp(P_DEAF));
    }

    filter(all_inventory(this_object()), #'filter_isdeaf);


    ### Filtern von Idlern (>=1 Sekunde idle) ###
    // Folgend identische Resultate, aber andere Ansaetze:

    #1: nutzt die Efun query_idle() als Lfun-Closure (ideal hier)
        idle_usr = filter(users(), #'query_idle );

    #2: mit Filtermethode
        int check_if_idle(object user) {
          return query_idle(user);
        }

        #2a: filtert mittels der Lfun im selben Objekt die Idler in das
             Rueckgabearray
             idle_usr = filter(users(), "check_if_idle");
             idle_usr = filter(users(), "check_if_idle", this_object());

        #2b: ruft die Lfun check_if_idle() als Lfun-Closure (Funktions-
             pointer)
             idle_usr = filter(users(), #'check_if_idle );

    #3: Nutzt eine Inline-Closure
        idle_usr = filter(users(), function int (object user) {
                     return query_idle(user);
                   } );

    ### Filtern von Idlern (>=20 Sekunden idle) mit Extraparameter ###
    // Folgend identische Resultate, aber andere Ansaetze:

    #1: die Efun koennen wir nicht mehr direkt nutzen, weil sie
        diesen Parameter nicht unterstuetzt
       // idle_usr = filter(users(), #'query_idle );

    #2: mit separater Filtermethode ... mit neuem Parameter
        int check_if_idle(object user, int length) {
          return query_idle(user)>length;
        }

        #2a: filtert mittels der Lfun im selben Objekt die Idler in das
             Rueckgabearray ... mit drittem Parameter!
             idle_usr = filter(users(), "check_if_idle", this_object(), 20);

        #2b: ruft die Lfun check_if_idle() als Lfun-Closure (Funktions-
             pointer)
             idle_usr = filter(users(), #'check_if_idle, 20);

    #3: Nutzt eine Inline-Closure
        idle_usr = filter(users(), function int (object user, int length) {
                     return (query_idle(user) > length); 
                     }, 20);

----------------------------------------------------------------------------
23.09.2019, Zesstra
