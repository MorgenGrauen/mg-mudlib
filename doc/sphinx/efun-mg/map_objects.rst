map_objects
===========

BEMERKUNGEN
-----------

 Werden Pfade angegeben, so wird versucht ein Objekt zu laden, falls
 dieses nicht existiert.

BEISPIELE
---------

  .. code-block:: pike

   // ersetze alle Objekte durch ihre Namen
   arr=map_objects(inputarr, "name");

   // ersetze alle Objekte durch ihre Namen im Genitiv
   arr=map_objects(inputarr, "name", WESSEN);

   // AeQUIVALENZCODE (nicht empfohlen, nur zum Verstaendnis!):
   int i;
   object *ret; mixed *input;

   i=sizeof(input);
   ret=allocate(i);
   while(i--)
     ret[i]=input[i]->fun([extra1, extra2, ...]);

