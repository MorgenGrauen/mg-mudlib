P_FORCE_MURDER_MSG
==================

NAME
----
::

	P_FORCE_MURDER_MSG		"force_murder_msg"

DEFINIERT IN
------------
::

	/sys/properties.h

BESCHREIBUNG
------------
::

	Ob der Tod eines NPCs auf dem Moerder-Kanal erscheint, haengt davon
	ab, wie oft und welche Art von NPCs in der letzten Zeit getoetet
	wurden. Zum Beispiel ist es eher selten, dass ein schwacher NPC auf
	dem Kanal erscheint, wenn kuerzlich viele starke NPCs getoetet
	wurden. Mittels der Property P_FORCE_MURDER_MSG kann man auf diese
	Regelung Einfluss nehmen.
	Ein Wert groesser Null erzwingt die Meldung auf dem Moerder-Kanal,
	ein Wert kleiner Null unterdrueckt sie generell. Letzteres ist
	insbesondere fuer allzuoft getoetete Monster sinnvoll, um den
	Moerder-Kanal nicht uebermaessig zu belasten. Mit dem Erzwingen der
	Meldungen sollte man somit vorsichtig sein: Weniger ist oftmals
	besser als zu viel!
	Die Defaulteinstellung von P_FORCE_MURDER_MSG ist natuerlich Null
	und fuehrt zur bereits beschriebenen opferabhaengigen Meldung.

BEISPIELE
---------
::

	Ein grosser starker NPC, der getoetet wurde, sollte schon eine tolle
	Meldung auf dem Moerder-Kanal erzeugen. In der Property
	P_MURDER_MSG koennen hierzu alternative Texte zu den normal per
	Zufall ausgewaehlten angegeben werden:
	  SetProp(P_MURDER_MSG,
                  "Nicht schlecht, aber das schafft eh nur einer!");
	  SetProp(P_FORCE_MURDER_MSG,1);
	Zwar ist es bei grossen NPCs hinreichend wahrscheinlich, dass es
	infolge derer Staerke zur automatischen Ausgabe einer Moerdermeldung
	kommt, aber mit der letzten Zeile wurde dies absolut sichergestellt.

SIEHE AUCH
----------
::

	P_MURDER_MSG, P_ZAP_MSG, P_KILL_MSG, P_DIE_MSG, P_CORPSE, P_NOCORPSE


Last modified: Tue Nov 10 02:15:26 1998 by Patryn

