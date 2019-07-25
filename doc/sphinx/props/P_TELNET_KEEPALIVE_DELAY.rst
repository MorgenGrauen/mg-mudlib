P_TELNET_KEEPALIVE_DELAY
========================

NAME
----

     P_TELNET_KEEPALIVE_DELAY "p_lib_telnet_alive_delay"

DEFINIERT IN
------------

     <player/telnetneg.h>

BESCHREIBUNG
------------

     In dieser Property kann der zeitliche Abstand zwischen zwei Aussendungen
     der "Telnet Timing Mark", welche fuer das Telnet keep-alive benutzt wird,
     bestimmt werden. Die Angabe ist in Heartbeats.
     Eine 0 fuehrt zu einer Standardverzoegerung (zur Zeit 240 s).

     Wenn diese Property in einem Magier (oder Spieler) gesetzt wird, sollte
     experimentell ermittelt werden, welcher Wert sinnvoll ist. Dies kann z.B.
     dadurch erfolgen, dass von 120 (Heartbeats) eine stufenweise Reduktion
     erfolgt, bis kein Disconnect mehr erfolgt. Helfen sehr niedrige Werte
     nicht, sollte die Verzoegerung wieder erhoeht werden bzw. das Keepalive
     ganz abgeschaltet werden.

BEMERKUNGEN
-----------

     Die Property sollte nicht auf weniger als 30 gesetzt werden.

SIEHE AUCH
----------

     :doc:`../pcmd/telnet`

25.07.2019, Zesstra
