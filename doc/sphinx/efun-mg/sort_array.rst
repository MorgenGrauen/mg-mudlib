sort_array
==========

BEMERKUNGEN
-----------

   Achtung, die Elemente in 'arr' werden nicht tief kopiert, sind sie
   also selbst Arrays oder Mappings, so fuehrt eine Aenderung im Rueckgabe-
   Array zur Aenderung im Ursprungsarray.

BEISPIELE
---------

  1. Sortieren von Zahlen in aufsteigender Reihenfolge

    .. code-block:: pike

       int *arr = ({ 3, 8, 1, 3 })

       // Folgend identische Resultate, aber andere Ansaetze:
       #1: nutzt die 'Efun' > als Lfun-Closure (ideal hier):
           sort_array(arr, #'>);

       #2: mit Sortierfunktion im selben Objekt:
           int is_greater (int a, int b) {
             return a > b;
           }

       #2a: sortiert mittels der Lfun im selben Objekt die Elemente in das
            Rueckgabearray
            sort_array(arr, "is_greater", this_object())
            sort_array(arr, "is_greater")

       #2b: nutzt die Lfun is_greater() als Lfun-Closure (Funktionspointer)
            sort_array(arr, #'is_greater)

       #3: Nutzt eine Inline-Closure
           sort_array(arr, function int (int a, int b) {
             return a > b; } );

     Resultat in allen Faellen: ({1,3,3,8})

   2. Sortieren von geschachtelten Arrays

     .. code-block:: pike

       arr = ({ ({ "foo", 3 }), ({ "quux", 1 }), ... })

       // Vorgehen identisch, allerdings muss die Sortierfunktion
       // angepasst werden:

       int is_greater (<string|int> *a, <string|int> *b) {
         return a[1] > b[1];
       }

