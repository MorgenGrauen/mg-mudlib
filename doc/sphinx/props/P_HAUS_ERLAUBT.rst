P_HAUS_ERLAUBT
==============

NAME
----
::

    P_HAUS_ERLAUBT                "haus_erlaubt"                

DEFINIERT IN
------------
::

    /sys/room/description.h

BESCHREIBUNG
------------
::

     Hier darf ein Seherhaus abgestellt werden

BEMERKUNGEN
-----------
::

     Diese Property sollte nicht per default in Raeumen auf einen Wert > 0
     gesetzt werden um einem Wildwuchs an Seherhaus(-ruinen) vorzubeugen.
     Auch sei darauf geachtet, dass in Raeumen die P_INDOORS auf einen 
     Wert > 0 haben, per default nicht gebaut werden kann.     
     Hier lieber die Property per Hand auf 1 setzen und nach dem Aufstellen
     des Hauses wieder loeschen.

      

     xcall $h->SetProp(P_HAUS_ERLAUBT,1);

     Angemerkt sei noch, dass der Magier dem der Raum gehoert ueber Hausbau
     entscheidet und nicht der Regionsmagier. In jedem Fall Ruecksprache 
     halten falls der entsprechende Magier zumindest ab und an anwesend sein
     sollte.

     Unter /doc/beispiele/misc liegt ein File, in dem dokumentiert ist,
     wie nach Aenderungen am Seherhaus zu verfahren ist.


Last modified: Fri Nov 26 15:41:47 1999 by Tilly

