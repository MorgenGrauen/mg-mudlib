set_next_reset
==============

BEISPIELE
---------

  .. code-block:: pike

     // ein Objekt mit verkuerzter reset()-Zeit
     void create() {
       ...
       set_next_reset(15*60);	// ~ 15 Minuten
       ...
     }

     void reset() {
       set_next_reset(900);	// die muss im reset() immer wieder
       ::reset();           // neu gesetzt werden
     }

     // ein Objekt, dessen Blueprint keinen reset() bekommen soll
     void create() {
       if(!clonep(this_object())) {
         set_next_reset(-1);
         return;
       }
       ::create();
       ...
     }

