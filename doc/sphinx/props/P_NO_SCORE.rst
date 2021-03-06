P_NO_SCORE
==========

NAME
----
::

     P_NO_SCORE               "no_score"

DEFINIERT IN
------------
::

     /secure/scoremaster.h

BESCHREIBUNG
------------
::

     Die Property stellt ein Flag innerhalb von Lebewesen dar, welches
     standardmaessig nicht gesetzt ist. In diesem Fall werden
     Erstkillstufenpunkte an den Angreifer vergeben, sofern er ein Opfer
     toetet.

     Innerhalb eines Teams koennen Erstkillstufenpunkte auch an
     Mitglieder vergeben werden, die das Lebewesen nicht selbst getoetet
     haben. Voraussetzung hierfuer ist, dass derjenige, der den letzten
     Schlag ausfuehrte, den Kill schon hat. Danach werden Mitglieder des
     Teams gesucht, welche den Kill noch nicht haben und in der Formation
     moeglichst weit vorne stehen.

     Mit der gesetzten Property P_NO_SCORE im Opfer erreicht man nun,
     dass diese Gutschrift fuer den/die Angreifer unterbunden wird.

BEISPIEL
--------
::

     Folgendermassen unterbindet man die Vergabe von
     Erstkillstufenpunkten fuer den Tod eines NPC's:

       include "/secure/scoremaster.h"
       inherit "std/npc";
       void create() {
         ::create();
         ...
         SetProp(P_NO_SCORE,1);
       }

     Damit kann P_XP einen Wert haben, der eigentlich zum automatischen
     Eintragen von Erstkillstufenpunkten fuer ein Lebewesen fuehrt, und
     trotzdem wird dieser Eintrag nicht vorgenommen.
     Sinnvoll ist dies insbesondere bei Lebewesen, die nicht jeder
     Spieler erreichen kann (man moechte doch eine gewisse
     Chancengleichheit fuer das Erreichen von Stufenpunkten bieten).

BEMERKUNGEN
-----------
::

     Auch die Vergabe von Erfahrungspunkten kann explizit unterbunden
     werden. Hierfuer gibt es die aehnlich geartete Property P_NO_XP.

SIEHE AUCH
----------
::

     Funktionen:  GiveKillScore(), do_damage()
     Verwandt:    P_NO_XP
     Sonstiges:   P_XP

14.Feb 2007 Gloinson

