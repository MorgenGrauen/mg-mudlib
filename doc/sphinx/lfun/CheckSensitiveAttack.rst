CheckSensitiveAttack()
======================

FUNKTION
--------
::

     void CheckSensitiveAttack(int dam, string *dam_type, mixed spell,
			       object enemy)

DEFINIERT IN
------------
::

     /std/living/inventory.c

ARGUMENTE
---------
::

     dam	- an Defend uebergebener Schaden
     dam_type	- Schadenstyp(en)
     spell	- spell, int oder mapping
     enemy	- Feindesobjekt

BESCHREIBUNG
------------
::

     Wird von Defend() aufgerufen und prueft die Listen in
     P_SENSITIVE_ATTACK durch.
     Wenn die Schluessel und Threshold-Werte stimmen, wird
     trigger_sensitive_attack(enemy,key,dam,spell,options)
     im Objekt gerufen.

BEMERKUNGEN
-----------
::

     Objekte mit P_SENSITIVE mit Schluessel SENSITIVE_ATTACK bitte vorsichtig
     verwenden, da rechenintensiv.

SIEHE AUCH
----------
::

     P_SENSITIVE
     InsertSensitiveObject, RemoveSensitiveObject
     insert_sensitive_inv_trigger, insert_sensitive_inv
     P_SENSITIVE_ATTACK, P_SENSITIVE_INVENTORY,
     P_SENSITIVE_INVENTORY_TRIGGER

28.Jan.2001, Gloinson@MG

