P_KILLER
========

NAME
----
::

    P_KILLER                      "killer"                      

DEFINIERT IN
------------
::

    /sys/properties.h

BESCHREIBUNG
------------
::

   Diese Property enthaelt das Objekt, welches das Lebewesen als letztes
   getoetet hat. Sie wird von do_damage(), heart_beat() (Vergiftungen) und
   die() (bei direkten Aufrufen) automatisch gesetzt. Ein manuelles
   Setzen vor Aufruf von do_damage() oder die() hat keinerlei Wirkung!
   Sinnvollerweise liest man diese Property im NotifyPlayerDeath() aus,
   spaeteres Auslesen ist unzuverlaessig, da der Killer inzwischen zerstoert
   sein koennte.
   Diese Property sollte _nicht_ per Hand gesetzt werden, schon gar nicht
   waehrend eines NotifyPlayerDeath(), weil es evtl. noch andere Objekte gibt,
   die sich dafuer interessieren!

BEMERKUNGEN
-----------
::

   Normalerweise steht hier ein Objekt drin (s.o.). Es gibt allerdings eine
   Ausnahme: Stirbt ein Lebewesen an Gift, enthaelt P_KILLER den String
   "gift".

SIEHE AUCH
----------
::

   do_damage()

29.08.2008, Zesstra

