P_ALLOWED_SHADOW
================

NAME
----
::

    P_ALLOWED_SHADOW              "allowed_shadow"

DEFINIERT IN
------------
::

    /sys/player/base.h

BESCHREIBUNG
------------
::

     Normalerweise koennen nur Shadows an Spieler gebunden werden, die in 
     /std/player/shadows/ liegen. 

     

     Zu Testzwecken kann in dieser Property der Pfad eines Shadows eingetragen
     werden. Damit wird die oben beschriebene Regel fuer diesen Spieler und 
     diesen Shadow ausser Kraft gesetzt.

BEMERKUNG: 

     Der Spieler muss ein Testspieler sein. Ansonsten wird diese Property
     ignoriert.

     Die Property ist secured gesetzt. Das heisst, nur EM+ koennen
     diese Property setzen und loeschen.

Last modified: Sun Mar 21 00:27:46 2004 by Vanion

