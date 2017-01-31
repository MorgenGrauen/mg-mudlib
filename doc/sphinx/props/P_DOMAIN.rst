P_DOMAIN
========

NAME
----
::

    P_DOMAIN                                        "lib_p_domain"

DEFINIERT IN
------------
::

    /sys/room/description.h

BESCHREIBUNG
------------
::

     Diese Property enthaelt die Region, in der ein Raum liegt, sofern er
     in /d/ liegt.

     Falls ein Raum nicht in /d/ liegt, aber es eigentlich ein Gebiet ist,
     welches eindeutig in einer Region zugeordnet ist, kann der Magier
     hier gezielt einen anderen Wert setzen.

     

     Bitte keine Regionen faelschen!

BEISPIEL
--------
::

     In /d/inseln/zesstra/vulkanweg/room/r1 liefert
     QueryProp(P_DOMAIN)
     ein "Inseln" zurueck.

     In einem Raum der Chaosgilde:
     SetProp(P_DOMAIN, "Polar");
     damit der Raum als Raum der Region Polar angezeigt wird.

SIEHE AUCH
----------
::

     gmcp


23.02.2013, Zesstra

