RemoveInsertHook()
==================

OBSOLETE LFUN
-------------

    Diese Efun existiert nicht mehr. Bitte stattdessen den Hook
    H_HOOK_INSERT benutzen. (s. /doc/std/hooks)


FUNKTION
--------

     void RemoveInsertHook(object ob);

ARGUMENTE
---------

     ob
       Das Objekt, das als Listener aus der Liste ausgetragen werden soll

BESCHREIBUNG
------------

     Diese Funktion wird im Spielerobjekt aufgerufen, um ein als Listener
     eingetragenes Hook-Objekt ob wieder auszutragen.

RUeCKGABEWERT
-------------

     keiner

SIEHE AUCH
----------

    NotifyInsert(), AddInsertHook(), QueryInsertHooks()


Last modified: 18.10.2019 Zesstra
