P_BALANCED_WEAPON
=================

********************* UNGENUTZTE PROPERTY *****************************
* Diese Property wird von der Mudlib NICHT ausgewertet und kann       *
* als veraltet gelten.                                                *
* Momentan ist auch keine Gilde bekannt, die mehr macht, als sie      *
* auszugeben.                                                         *
***********************************************************************

NAME
----
::

        P_BALANCED_WEAPON  "balanced_weapon"

DEFINIERT IN
------------
::

        /sys/weapon.h

BESCHREIBUNG
------------
::

        Die Property gibt an, ob eine Waffe ausbalanciert ist oder nicht.
        Die beiden moeglichen Werte sind logischerweise:

                WP_BALANCED       balanciert
                WP_UNBALANCED     unbalanciert

        Die WP_* sind ebenfalls in <weapon.h> definiert.

BEISPIELE
---------
::

        a) Eine ausbalancierte Waffe ist z.B. ein Kampfstab.

            SetProp(P_BALANCED_WEAPON,WP_BALANCED);

        b) Eine nicht ausbalancierte Waffe ist z.B. eine Keule.

            SetProp(P_BALANCED_WEAPON,WP_UNBALANCED);

SIEHE AUCH
----------
::

        P_TECHNIQUE, /std/weapon/combat.c

LETZTE AeNDERUNG
----------------
::

15.02.2009, Zesstra

