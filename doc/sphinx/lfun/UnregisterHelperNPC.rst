UnregisterHelperNPC()
=====================

FUNKTION
--------
::

     public int UnregisterHelperNPC(object npc);

DEFINIERT IN
------------
::

     /std/player/combat.c

ARGUMENTE
---------
::

     object npc
       Objekt des helfenden NPC, der abgemeldet werden soll.

BESCHREIBUNG
------------
::

     Mit dieser Funktion wird ein einem Spieler helfender NPC im Spieler
     wieder abgemeldet, wenn dieser dem Spieler ab jetzt nicht mehr hilft.

     Wenn ein Helfer-NPC zerstoert wird, ist der Aufruf nicht noetig.
     Bleibt das Objekt des NPC aber existent, bitte auf jeden Fall wieder
     ordentlich abmelden, da ansonsten ggf. der Spieler unnoetig blockiert
     wird.

RUeCKGABEWERT
-------------
::

     1, wenn die Abmeldung erfolgreich war.
     0 sonst, z.B. wenn der NPC gar nicht als Helfer registriert war.

BEMERKUNGEN
-----------
::

     Diese Funktion setzt bei der Erfolg die Property P_HELPER_NPC in <npc>
     auf 0.

BEISPIELE
---------
::

     1. Ein NPC, der dem Spieler nicht mehr helfen will und normalerweisen im
        Raum verbleiben soll:
     tell_object(spieler, "Ich mag Dich nicht mehr, Du bist doof!\n");
     if (spieler->UnregisterHelperNPC(this_object()) != 1) {
       // das ist ja bloed...
       remove(0);
     }
     else {
       tell_room(environment(),
           Name()+" dreht " +spieler->Name(WEM) + " schmollend den Ruecken "
           "zu.\n");
     }

SIEHE AUCH
----------
::

    UnregisterHelperNPC()
    P_HELPER_NPC

