P_READ_MSG
==========

********************* VERALTETE PROPERTY ******************************
* Diese Property ist veraltet. Bitte nicht mehr in neuem Code nutzen. *
***********************************************************************

NAME
----
::

    P_READ_MSG                "read_msg"                

DEFINIERT IN
------------
::

    /sys/properties.h

BESCHREIBUNG
------------
::

    Diese Property ist veraltet. Ihre Funktion wird von
    AddReadDetail(SENSE_DEFAULT, ...) uebernommen.

    

    Hier koennen Informationen gespeichert werden, die beim Lesen
    des Objektes ausgegeben werden.

     

    Fuer das Identifizieren des zu lesenden Objektes wird der gleiche
    Mechanismus benutzt wie fuer lesbare und andere Details.

    Die Benutzung von process_string() ist in dieser Prop nicht mehr erlaubt.

BEISPIEL
--------
::

    AddId(({"rolle", "schriftrolle"}));
    SetProp(P_READ_MSG,
       "Du oeffnest die Rolle und liest: LOREM IPSUM ...\n");

     

SIEHE AUCH
----------
::

    Details:   AddReadDetail(), RemoveReadDetail(), P_READ_DETAILS
    Sonstiges: break_string()

09.12.2012, Zesstra

