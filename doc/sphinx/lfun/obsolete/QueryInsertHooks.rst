QueryInsertHooks()
==================

OBSOLETE LFUN
-------------

    Diese Efun existiert nicht mehr. Bitte stattdessen den Hook
    H_HOOK_INSERT benutzen. (s. /doc/std/hooks)


FUNKTION
--------

     object \*QueryInsertHooks();

ARGUMENTE
---------

     keine

BESCHREIBUNG
------------

     Diese Funktion gibt die aktuell beim Spielerobjekt angemeldeten
     Listener-Objekte zurueck.

RUeCKGABEWERT
-------------

     Array aus Objektpointern oder leeres Array

SIEHE AUCH
----------

    NotifyInsert(), AddInsertHook(), RemoveInsertHook()


Last modified: 18.10.2019 Zesstra

