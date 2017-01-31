P_ENEMY_DAMAGE
==============

NAME
----
::

     P_ENEMY_DAMAGE "enemy_damage"

DEFINIERT IN
------------
::

     <living/life.h>

BESCHREIBUNG
------------
::

     In dieser Property ist vermerkt, welches Wesen diesem Wesen wieviel
     Schaden zugefuegt hat.

     Die Property enthaelt ein Mapping, das den Angreifern den
     errechten Schaden zuordnet:
     ([ <obname1>: <damage>; <owner>, ... ])

       <obname1>: Name des Objekts, das den Schaden verursacht hat (string),
                  z.B. "/magier:zesstra"
       <damage> : Schadensmenge (int)
       <owner>  : wenn das schadensverursachende Wesen ein NPC war/ist,
                  welches P_HELPER_NPC gesetzt hatte und somit einem Spieler
                  hilft, steht hier das Objekt des jeweiligen Spielers
                  (object)

BEMERKUNGEN
-----------
::

     Diese Property laesst sich nur abfragen!

SIEHE AUCH
----------
::

     P_HELPER_NPC

26.10.2009, Zesstra

