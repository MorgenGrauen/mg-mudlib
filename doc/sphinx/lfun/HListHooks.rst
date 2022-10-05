HListHooks()
============

FUNKTION
--------

  int* HListHooks()

DEFINIERT IN
------------

  /std/hook_provider.c
  /sys/hook.h

ARGUMENTE
---------

  keine

BESCHREIBUNG
------------

  Diese Methode liefert eine Liste von Hooktypen, fuer die das Objekt
  Registrierungen akzeptiert. Standardmaessig bieten die Mudlib-Basis-
  objekte folgende Hooks an:
  Spielerobjekte: H_HOOK_MOVE, H_HOOK_DIE, H_HOOK_DEFEND, H_HOOK_ATTACK,
                  H_HOOK_HP, H_HOOK_SP, H_HOOK_ATTACK_MOD, H_HOOK_ALCOHOL 
                  H_HOOK_FOOD, H_HOOK_DRINK, H_HOOK_POISON, H_HOOK_CONSUME,
                  H_HOOK_TEAMROWCHANGE ,H_HOOK_INSERT
  NPCs: H_HOOK_MOVE, H_HOOK_DIE, H_HOOK_DEFEND, H_HOOK_ATTACK, 
        H_HOOK_ATTACK_MOD, H_HOOK_ALCOHOL, H_HOOK_FOOD, H_HOOK_DRINK, 
        H_HOOK_POISON, H_HOOK_CONSUME, H_HOOK_TEAMROWCHANGE
  Raeume: H_HOOK_EXIT_USE, H_HOOK_INIT
  Dinge: keine

RUECKGABEWERTE
--------------

  Integer-Array der angebotenen Hook-IDs

BEISPIEL
--------

.. code-block:: pike

  // Bietet das Objekt einen Consume-Hook an?
  if(H_HOOK_CONSUME in ob->HListHooks())
  {
    do_something();
  }

SIEHE AUCH
----------

  :doc:`HRegisterToHook`, :doc:`HUnregisterFromHook`, :doc:`HIsHookConsumer`

Letzte Aenderung: 06.10.2022, Bugfix
