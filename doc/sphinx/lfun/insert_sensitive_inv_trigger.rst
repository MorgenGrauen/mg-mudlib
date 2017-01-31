insert_sensitive_inv_trigger()
==============================

FUNKTION
--------
::

     void insert_sensitive_inv_trigger(object ob, string key,
				       int threshold, mixed *opt)

DEFINIERT IN
------------
::

     /std/container/inventory.c

BESCHREIBUNG
------------
::

     Diese Funktion sucht, ob ein sensitives Objekt im Inventory ist,
     das durch dieses (soeben eingefuegte) Objekt ausgeloest wird.

SIEHE AUCH
----------
::

     P_SENSITIVE
     InsertSensitiveObject, RemoveSensitiveObject
     insert_sensitive_inv
     P_SENSITIVE_ATTACK, P_SENSITIVE_INVENTORY,
     P_SENSITIVE_INVENTORY_TRIGGER
     CheckSensitiveAttack

28.Jan.2001, Gloinson@MG

