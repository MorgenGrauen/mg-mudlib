P_NOGET
=======

NAME
----
::

	P_NOGET				"noget"

DEFINIERT IN
------------
::

	/sys/thing/moving.h

BESCHREIBUNG
------------
::

	Ist diese Property in einem Objekt gesetzt, so kann ein Lebewesen
	das Objekt nicht nehmen.
	Als Standardmeldung kommt in diesem Fall beispielsweise:
	  Du kannst <Objektname> nicht nehmen.
	  Du kannst <Objektname> so nirgendwo reinstecken.
	Man kann auch eine alternative Meldung angeben, wobei selbstaendig
	auf einen korrekten Zeilenumbruch zu achten ist.

BEISPIELE
---------
::

	Ein Objekt, welches fest im Raum verankert ist, kann natuerlich
	nicht entfernt werden, z.B. ein angebundenes Seil:
	  SetProp(P_NOGET,"Das Seil ist fest am Baum verknotet!\n");
	In einem Kommando zum Losknoten koennte man die Property dann
	loeschen, um ein Wegnehmen zu ermoeglichen.

SIEHE AUCH
----------
::

     Aehnliches: P_NODROP, P_TOO_HEAVY_MSG, P_ENV_TOO_HEAVY_MSG,
                 P_TOO_MANY_MSG, P_NOINSERT_MSG, P_NOLEAVE_MSG 
     Erfolg:     P_PICK_MSG, P_DROP_MSG, P_GIVE_MSG, P_PUT_MSG,
                 P_WEAR_MSG, P_WIELD_MSG
     Sonstiges:  replace_personal(E), /std/living/put_and_get.c


Last modified: Thu Jun 14 22:26:29 2001 by Patryn

