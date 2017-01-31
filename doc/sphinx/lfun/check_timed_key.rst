check_timed_key()
=================

FUNKTION
--------
::

       public int check_timed_key(string key)

       

DEFINIERT IN
------------
::

       /std/living/life.c    

ARGUMENTE
---------
::

       string key  : Eindeutiger Name

BESCHREIBUNG
------------
::

       Diese Funktion hat die Aufgabe, mittels check_and_update_timed_key()
       gespeicherte Zeitsperren zu pruefen, aber dabei nicht zu veraendern.

       Es MUSS bei der eigentlichen Aktion (z.B. Heilung des Spielers) der
       Rueckgabewert von check_and_update_timed_key() beruecksichtigt werden,
       sollte dieses nicht -1 geliefert haben, MUSS <key> als gesperrt
       betrachtet werden, d.h. check_timed_key() hat nur informativen
       Charakter.

RUeCKGABEWERT
-------------
::

       0    Die Zeitsperre <key> ist abgelaufen.

       >0   <key> ist noch gesperrt.
            Der Rueckgabewert ist der Zeitpunkt, ab dem <key> wieder frei ist,

SIEHE AUCH
----------
::

       check_and_update_timed_key, eat_food, drink_alcohol, drink_soft,
       heal_self, restore_spell_points, reduce_hit_point


08.01.2012, Zesstra

