P_TECHNIQUE
===========

********************* UNGENUTZTE PROPERTY *****************************
* Diese Property wird von der Mudlib NICHT ausgewertet und kann       *
* als veraltet gelten.                                                *
* Momentan ist auch keine Gilde bekannt, die mehr macht, als sie      *
* auszugeben.                                                         *
***********************************************************************

NAME
----
::

	P_TECHNIQUE				"technique"

DEFINIERT IN
------------
::

	/sys/weapon.h

BESCHREIBUNG
------------
::

        Die Technik(en), mit denen eine Waffe im Kampf eingesetzt werden
        kann. Folgende Techniken stehen zur Verfuegung:

                TQ_STROKE       Streichtechnik
                TQ_THRASH       Schlagtechnik
                TQ_THRUST       Stosstechnik
                TQ_WHIP         Peitschtechnik

        Die Techniken sind ebenfalls in <weapon.h> definiert und auf der
        man-page 'techniken' naeher erlaeutert.

BEMERKUNGEN
-----------
::

        Man kann einer Waffe eine oder mehrere Techniken zuweisen.

BEISPIELE
---------
::

        a) Eine Waffe, die nur mit einer Peitschtechnik eingesetzt wird,
           also typischerweise eine Peitsche, aber auch ein Morgenstern:

            SetProp(P_TECHNIQUE,TQ_WHIP);

        b) Eine Waffe, die sowohl mit der Schlag- als auch der Stosstechnik
           eingesetzt wird, also z.B. eine Hellebarde:

            SetProp(P_TECHNIQUE,({TQ_THRASH,TQ_THRUST}));

SIEHE AUCH
----------
::

        techniken, P_BALANCED_WEAPON, /std/weapon/combat.c

LETZTE AeNDERUNG
----------------
::

15.02.2009, Zesstra

