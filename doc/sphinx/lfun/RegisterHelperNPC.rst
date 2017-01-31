RegisterHelperNPC()
===================

FUNKTION
--------
::

     public int RegisterHelperNPC(object npc, int flags);

DEFINIERT IN
------------
::

     /std/player/combat.c
     /sys/living/combat.h

ARGUMENTE
---------
::

     object npc
       Objekt des helfenden NPC, der angemeldet werden soll.

     int flags
       ver-oder-te Konstanten, die die Art des Helfer-NPC beschreiben (s.u.)

BESCHREIBUNG
------------
::

     Mit dieser Funktion wird ein einem Spieler helfender NPC im Spieler
     angemeldet. Hierdurch kann spaeter herausgefunden werden, welche NPC
     einem Spieler helfen und ggf. den von diesen verursachten Schaden im
     Kampf dem Spieler zuzurechnen.

     Die Flags sind eine der folgenden Konstanten oder eine beliebige durch
     ver-oder-ung gebildete Kombination.

     Momentan gibt es 2 Klassen von Helfer-NPC:
     + GUILD_HELPER: der Helfer-NPC ist ein Gilden-NPC
     + MISC_HELPER:  der Helfer-NPC ist irgendein NPC, der nicht zu einer Gilde
                     gehoert.

     Zusaetzlich zu den Klassen gibt es noch weitere Flags, die etwas ueber
     den Helfer-NPC sagen:

     + EXCLUSIVE_HELPER: dieser Helfer-NPC duldet keinen weiteren NPC der
                         gleichen Klasse.
     + ACTIVE_HELPER: ist dieses Flag gesetzt, ist der NPC mehr als nur reiner
                      Schlagfaenger.

     Wird EXCLUSIVE_HELPER gesetzt und es ist in der gleichen Klasse schon ein
     NPC angemeldet, schlaegt die Registrierung fehl.
     Ist in der gleichen Klasse bereits ein NPC mit EXCLUSIVE_HELPER
     angemeldet, schlaegt die Registierung ebenfalls fehl, auch wenn der neue
     NPC kein EXCLUSIVE_HELPER setzt.

RUeCKGABEWERT
-------------
::

     1, wenn die Registrierung erfolgreich war.
     0 sonst. In diesem Fall darf der NPC dem Spieler NICHT helfen, weder
       passiv (Schlagfaenger), noch aktiv.

BEMERKUNGEN
-----------
::

     Diese Funktion setzt bei der Erfolg die Property P_HELPER_NPC in <npc>
     auf passende Werte.
     Bitte auf gar keinen Fall die numerischen Werte der Konstanten fuer
     <flags> nutzen, diese koennen sich jederzeit aendern.

BEISPIELE
---------
::

     1. Ein nicht-exklusiver Gilden-NPC, der dem Spieler folgt.
     if (spieler->RegisterHelperNPC(this_object(), GUILD_HELPER) == 1) {
       move(environment(spieler), M_GO);
       spieler->AddPursuer(this_object());
       // meldung ausgebene...
     }

     2a. Ein exklusiver Nicht-Gilden-NPC
     if (spieler->RegisterHelperNPC(this_object(),
                                    MISC_HELPER|EXCLUSIVE_HELPER) == 1) {
       move(environment(spieler), M_GO);
     }

     2b. Ein nicht-exklusiver Nicht-Gilde-NPC, der nach 2a. kommt.
     if (spieler->RegisterHelperNPC(this_object(), MISC_HELPER) == 1) {
       // ... wenn der NPC aus 2a noch existiert, trifft dies hier nie zu.
     }

     3. Ein exklusiver NPC, der weitere Gilden- und sonstige NPC ausschliesst
        (Solche Kombination bitte mit der Gilden-Balance abstimmen.)
     if (spieler->RegisterHelperNPC(this_object(), 
                            MISC_HELPER|GUILD_HELPER|EXCLUSIVE_HELPER) == 1) {
       move(environment(spieler), M_GO);
     }

     4. Die Registrierung ohne Klasse schlaegt fehl, z.B.:
       spieler->RegisterHelperNPC(this_object(), 0);
       spieler->RegisterHelperNPC(this_object(), EXCLUSIVE_HELPER);

SIEHE AUCH
----------
::

    UnregisterHelperNPC()
    P_HELPER_NPC

