Query()
=======

FUNKTION
--------
::

     public varargs mixed Query(string name, int Type);

DEFINIERT IN
------------
::

     /std/thing/properties.c

ARGUMENTE
---------
::

     string name - Property, deren Wert(e) ausgelesen werden
     int type  - Art der gewuenschten Information.

BESCHREIBUNG
------------
::

     Der Wert einer der inneren Eigenschaften der Property 'name' wird
     zurueckgegeben.  'Type' ist dabei einer der in /sys/thing/properties.h
     und folgend aufgelisteten F_XXX-Werte:

     F_VALUE (==0, Default)
         Unter Umgehung einer eventuell vorhandenen Abfragemethode oder
         _query_'name'() wird der Datenwert der Property 'name'
         zurueckgegeben.
     F_MODE
          Die internen Flags der Property werden zurueckgegeben.Dies koennen
          (logisch mit & verknuepft) sein:
          SAVE  - Property soll bei save_object() gespeichert werden
          PROTECTED - Objekt selbst/EM/Root kann Property manipulieren
          SECURED  - wie PROTECTED, das Flag kann aber nicht
                     zurueckgesetzt werden (immer SECURED)
          NOSETMETHOD - niemand kann Property manipulieren
                        (auch kein F_SET_METHOD oder _set_'name'())
     F_SET_METHOD
          Ein eventuell fuer die Property eingetragene F_SET_METHOD wird
          zurueckgegeben.
          (_set_'name'()-Methoden werden so nicht aufgefuehrt!)
     F_QUERY_METHOD
          Ein eventuell fuer die Property eingetragene F_QUERY_METHOD wird
          zurueckgegeben.
          (_query_'name'()-Methoden werden so nicht aufgefuehrt!)

RUeCKGABEWERT
-------------
::

     Die gewuenschte Eigenschaft, abhaengig von 'Type'.

BEMERKUNGEN
-----------
::

     - Query() sollte nicht zum regulaeren Auslesen des Inhalt einer
       Property verwendet werden, da sowohl F_QUERY_METHOD als auch
       libinterne _query_'name'()-Methoden umgangen werden und das Ergebnis
       fuer so veraenderte Propertys undefiniert ist
     - _set_'name'() und _query_'name'() sind alte Propertymethoden und
       sollten nicht in normalen Objekten benutzt werden ->
       F_SET_METHOD/F_QUERY_METHOD (ggf. mit PROTECTED) nutzen
     - F_SET/F_QUERY_METHODs koennen 'protected' (empfohlen) oder 'static'
       sein. _set_/_query_ duerfen momentan _nicht_ 'protected' sein, fuer
       diese geht nur 'static' (in diesem Fall empfohlen).

BEISPIELE
---------
::

     // Auslesen des Wertes unter Umgehung einer Abfragemethode
     Query(P_XYZ, F_VALUE);

     // Auslesen der Flags erfaehrt man mit:
     Query(P_XYZ, F_MODE);

     // sauberes Programmieren, wir wollen eine F_QUERY_METHOD setzen,
     // pruefen vorher auf Existenz:
     if(this_player()->Query(P_FROG, F_QUERY_METHOD) {
      write(break_string(
       "Ich kann dich nicht weiter vor Froschsein schuetzen!",
       "Der Magier winkt ab: ", 78));
      say(break_string(
       "Ich kann dich nicht weiter vor Froschsein schuetzen!",
       "Der Magier sagt zu "+this_player()->name(WEM)+": ", 78));
     } else {
      this_player()->Set(P_FROG, #'query_protect_frog, F_QUERY_METHOD);
      ...
     }

SIEHE AUCH
----------
::

     Aehnliches: SetProp(L), QueryProp(L), Set(L)
     Generell:  SetProperties(L), QueryProperties(L)
     Konzept:  properties, /std/thing/properties.c
     Sonstiges:  P_AUTOLOADOBJ

28.03.2008, Zesstra

