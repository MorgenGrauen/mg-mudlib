P_NODROP
========

NAME
----
::

	P_NODROP			"nodrop"                      

DEFINIERT IN
------------
::

	/sys/thing/moving.h

BESCHREIBUNG
------------
::

	Ist diese Property in einem Objekt gesetzt, so kann ein Lebewesen
	das Objekt nicht weglegen.
	Als Standardmeldung kommt in diesem Fall beispielsweise:
	  Du kannst <Objektname> nicht wegwerfen!
	  Du kannst <Objektname> nicht weggeben.
	Man kann auch eine alternative Meldung angeben, wobei selbstaendig
	auf einen korrekten Zeilenumbruch zu achten ist.

BEMERKUNGEN
-----------
::

	Soll ein Objekt beim Tod des Lebewesens oder bei Ende eines Spielers
	nicht in der Leiche bzw. im Raum zurueckgelassen werden, so ist
	die Property P_NEVERDROP zu nutzen.
	Beide Properties zusammen stellen sicher, dass ein Objekt nicht
	weitergegeben werden kann.

BEISPIELE
---------
::

	Ein schwer zu erkaempfender Dolch koennte folgendes beinhalten,
	um nicht weitergegeben werden zu koennen:
	  SetProp(P_NODROP,1);
	Informativer jedoch ist eine eigene Meldung:
	  SetProp(P_NODROP,
	 "Den Dolch hast Du Dir hart erkaempft, nicht wegwerfen!\n");

SIEHE AUCH
----------
::

     Aehnliches: P_NOGET, P_NEVERDROP, P_TOO_HEAVY_MSG, P_ENV_TOO_HEAVY_MSG,
                 P_TOO_MANY_MSG, P_NOINSERT_MSG, P_NOLEAVE_MSG,  
     Erfolg:     P_PICK_MSG, P_DROP_MSG, P_GIVE_MSG, P_PUT_MSG,
                 P_WEAR_MSG, P_WIELD_MSG
     Sonstiges:  replace_personal(E), /std/living/put_and_get.c


Last modified: Thu Jun 14 22:26:29 2001 by Patryn

