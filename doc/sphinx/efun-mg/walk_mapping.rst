walk_mapping
============

BEISPIELE
---------

  In einem Mapping (Keys: Spielerobjekte) soll auf alle Werte etwas drauf
  addiert werden:

  .. code-block:: pike

     // Liste mit Spielern durchgehen ...
     mapping x=([ [/human:liafar]:  20,
                  [/dwarf:mesirii]: 50,
                  [/elf:zarniya]:   40,
                  [/feline:turbo]:  30]);

     // ... und Werte aendern:
     void add_val(object key, int val, int add) {
       if(key->InFight())
         val+=add;
       else
         val-=add;
     }

     // verschiedene Aufrufarten, identisches Resultat:
     walk_mapping(x, "add_val", 0, 10);
     walk_mapping(x, "add_val", this_object(), 10
     walk_mapping(x, "add_val", "/players/jof/addierobjektmitmethode", 10);

     walk_mapping(x, #'add_val, 10);

