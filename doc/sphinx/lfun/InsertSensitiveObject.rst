InsertSensitiveObject()
=======================

FUNKTION
--------
::

     void InsertSensitiveObject(object ob, mixed *arg)

DEFINIERT IN
------------
::

     /std/container/inventory.c
     generalizes /std/living/inventory.c

BESCHREIBUNG
------------
::

     Fuegt "ob" in die Benachrichtigungslisten des Containers ein.
     Wird von thing/moving.c im Ziel-Environment gerufen, wenn
     P_SENSITIVE gesetzt ist.

BEMERKUNGEN
-----------
::

     Setzt man P_SENSITIVE nicht als Default sondern situationsabhaengig,
     dann muss man auch InsertSensitiveObject() im Environment
     auch selbst rufen!

BEISPIEL
--------
::

     // Fackel (inheriting lightsource)
     // wenn angezuendet, aendert es die Eigenschaften und wird zum
     // aktiven Objekt - das muss man dem environment() mitteilen
     static int light(string str) {
      int i;
      i=::light(str);
      if(i && QueryProp(P_LIGHT)>0) {
       SetProp(P_SENSITIVE,
        ({({SENSITIVE_INVENTORY_TRIGGER,DT_FIRE,120})}));
       if(environment())
        environment()->InsertSensitiveObject(this_object(),
					     QueryProp(P_SENSITIVE));
      }
      return i;
     }

     - falls ein empfindliches Objekt im environment() ist, dann wird
       in diesem nun eventuell (Treshold) trigger_sensitive_inv()
       gerufen

SIEHE AUCH
----------
::

     P_SENSITIVE
     RemoveSensitiveObject
     insert_sensitive_inv_trigger, insert_sensitive_inv
     P_SENSITIVE_ATTACK, P_SENSITIVE_INVENTORY, P_SENSITIVE_INVENTORY_TRIGGER
     CheckSensitiveAttack

25.Apr.2001, Gloinson@MG

