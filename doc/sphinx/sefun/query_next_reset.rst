query_next_reset()
==================

simul_efun::query_next_reset(E)

FUNKTION
--------
::

     varargs int query_next_reset(object ob)

ARGUMENTE
---------
::

     ob - das interessierende Objekt; wenn nicht angegeben, wird 
          this_object() verwendet

BESCHREIBUNG
------------
::

     Diese sefun gibt den Zeitpunkt des naechsten Resets des Objektes ob 
     zurueck. Die Angabe erfolgt in Sekunden, die seit dem 01. Januar 1970 
     00:00:00 GMT verstrichen sind, analog zu time().

     In der Regel erfolgt der Reset im naechsten Backend-Zyklus nach dem 
     Faelligkeitszeitpunkt,  d.h. momentan in den nachfolgenden 2s.
     Allerdings kann dies auch mal nen Zyklus laenger dauern (4s), wenn der
     Driver viel zu tun hat.

BEMERKUNGEN
-----------
::

     Diese sefun ist object_info()-Abfragen vorzuziehen, da die Parameter und
     Rueckgabewerte von object_info() bei verschiedenen Gamedriverversionen
     variieren koennen.

SIEHE AUCH
----------
::

     call_out(E), object_info(E), reset(L), set_next_reset(E), time(E)

28.07.2014 Arathorn

