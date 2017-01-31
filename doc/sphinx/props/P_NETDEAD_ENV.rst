P_NETDEAD_ENV
=============

NAME
----
::

    P_NETDEAD_ENV                  "netdead_env"

DEFINIERT IN
------------
::

    /sys/player/base.h

BESCHREIBUNG
------------
::

    Diese Property kann in netztoten Spielern abgefragt werden,
		um herauszufinden, in welchem Raum sie netztot geworden sind.

		Es wird der selbe Wert zurueckgegeben, den die Mudlib benutzen
		wuerde, um die Spieler beim reconnect wieder an den richtigen
		ort zu bringen. Das ist normalerweise das Raumobjekt oder der
		Dateiname des Raums (zum Beispiel so dieser ein clone war).

		Bei nicht netztoten Spielern wird 0 zurueckgegeben.

BEMERKUNGEN
-----------
::

    Diese Property ist read-only.

SIEHE AUCH
----------
::

    P_NETDEAD_INFO

2009-08-04 Rumata

