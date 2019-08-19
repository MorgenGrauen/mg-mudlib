P_MAX_POISON
============

NAME
----

    P_MAX_POISON                  "max_poison"                  

DEFINIERT IN
------------

    /sys/properties.h

BESCHREIBUNG
------------

     Maximale Vergiftung eines Lebewesens

ANMERKUNGEN
-----------

     Bei Lebewesen wird beim Setzen mittels SetProp die maximale Vergiftung
     seitens der Mudlib auf P_MAX_POISON begrenzt, wenn ein groesserer Wert
     gesetzt wird. Ein separater Check darauf ist daher nicht erforderlich.

SIEHE AUCH
----------
 
     :doc:`P_POISON`, :doc:`P_POISON_DELAY`,
     gift
