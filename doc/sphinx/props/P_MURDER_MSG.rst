P_MURDER_MSG
============

NAME
----
::

     P_MURDER_MSG			"murder_msg"

DEFINIERT IN
------------
::

     /sys/properties.h

BESCHREIBUNG
------------
::

     In dieser Property kann man einen String oder eine Closure ablegen
     dessen Wert bzw. deren Resultat beim Tod des NPCs auf dem
     Moerder-Kanal erscheint.
     Normalerweise ist die Property nicht gesetzt, woraufhin zufaellig
     eine Meldung generiert wird.

     Ob der Tod eines NPCs auf dem Moerder-Kanal erscheint, haengt davon ab,
     wie oft und welche Art von NPCs in der letzten Zeit getoetet wurden. Zum
     Beispiel ist es eher selten, dass ein schwacher NPC auf dem Kanal
     erscheint, wenn kuerzlich viele starke NPCs getoetet wurden. Allerdings
     kann man auf diese Regelung mittels der Property P_FORCE_MURDER_MSG
     Einfluss nehmen.

     Wird in einen String der Platzhalter %s eingefuegt, so erscheint an der
     Stelle spaeter der Name des Moerders.

BEISPIELE
---------
::

     // Zum Beispiel koennte man ja bei einer Ratte, die getoetet wird,
     // folgendes auf dem Moerder-Kanal ausgeben lassen:
     SetProp(P_MURDER_MSG,
       "Ratten aller MUDs, vereinigt euch gegen %s!");


     // Um auch mal eine Closure zu zeigen: die Ratte koennte auch ihre
     // Meldung erst bei ihrem Tod erstellen lassen:
     private string moerder_meldung() {
       return ({"Achweh!", "Au!", "Weia!"})[random(3)];
     }

     SetProp(P_MURDER_MSG, #'moerder_meldung);

BEMERKUNGEN
-----------
::

     - P_NOCORPSE:
       Ist in dem NPC die Property P_NOCORPSE gesetzt, so wird die
       Moerdermeldung nicht auf dem Kanal gesendet, da diese Ausgabe ueber
       /std/corpse laeuft.
       Will man dennoch eine Meldung, so sollte man /std/corpse im die()
       selbst clonen, daran Identify(this_object()) rufen und das geclonte
       Objekt wieder entsorgen.

     - Closures:
       Closures bieten sich an, wenn ein zentrales Objekt fuer mehrere NPCs
       bestimmte Moerdermeldungen generieren soll. Dann muss nur noch bei
       den NPCs die Closure, die auf die erstellende Methode zeigt gesetzt
       werden.

SIEHE AUCH
----------
::

     Tod:		die(L)
     Verwandt:		P_FORCE_MURDER_MSG
     Todesmeldungen:	P_KILL_NAME, P_KILL_MSG, P_DIE_MSG
			P_ZAP_MSG, P_ENEMY_DEATH_SEQUENCE
     Sonstiges:		P_CORPSE, P_NOCORPSE, /std/corpse.c

30. Mai 2006, Gloinson

