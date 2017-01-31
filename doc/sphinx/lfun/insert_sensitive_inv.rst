insert_sensitive_inv()
======================

FUNKTION
--------
::

     void insert_sensitive_inv(object ob, string key,
			       int threshold, mixed *opt)

DEFINIERT IN
------------
::

     /std/container/inventory.c

BESCHREIBUNG
------------
::

     Diese Funktion sucht, ob beim Hinzufuegen eines sensitiven Objekts
     schon ein Objekt da ist, dass dieses ausloest.

SIEHE AUCH
----------
::

     P_SENSITIVE
     InsertSensitiveObject, RemoveSensitiveObject
     insert_sensitive_inv_trigger
     P_SENSITIVE_ATTACK, P_SENSITIVE_INVENTORY,
     P_SENSITIVE_INVENTORY_TRIGGER
     CheckSensitiveAttack

28.Jan.2001, Gloinson@MG

