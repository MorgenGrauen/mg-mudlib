P_REJECT
========

NAME
----
::

	P_REJECT			"reject"

DEFINIERT IN
------------
::

	/sys/properties.h

BESCHREIBUNG
------------
::

	Diese Property zeigt standardmaessig nur in NPCs eine Wirkung. Mit
	ihr laesst sich sehr einfach einstellen, wie sich ein solcher NPC
	gegenueber Gegenstaenden verhalten soll, welche ihm zugesteckt
	werden. Hierbei besteht die Property aus 2 Elementen, welche
	bestimmt, was der NPC mit Dingen tuen soll, die ihm gegeben werden.
	Standardmaessig behaelt der NPC die Sachen einfach.
	Folgende Moeglichkeiten gibt es ausserdem:
	  1. Arrayelement: Art der Handlung. (aus "/sys/moving.h")
	    REJECT_GIVE: Der NPC gibt das Objekt zurueck.
	    REJECT_DROP: Der NPC laesst das Objekt fallen.
	    REJECT_KEEP: Der NPC behaelt das Objekt doch.
	    REJECT_LIGHT_MODIFIER: Der NPC nimmt keine Gegenstaende an, die
	      sein Lichtlevel veraendern und damit Einfluss auf sein
	      Kampfverhalten haben koennten.
	  2. Arrayelement: Meldung, mit welcher der NPC die Handlung
	                   kommentiert.
	    Der Meldung wird nichts automatisch vorangestellt und ein
	    abschliessender Zeilenumbruch ist ebenfalls selbstaendig
	    vorzunehmen. Mit einem Leerstring ist somit auch gar keine
	    Rueckmeldung moeglich.

BEISPIEL
--------
::

	Der NPC schmeisst alle ihm gegebenen Gegenstaende einfach weg:
	  void create()
	  { ::create();
	    ...
	    SetProp(P_REJECT,({REJECT_GIVE,
	    Name(WER)+" murmelt: Was soll ich denn damit?!\n"}));
	    ...
	  }
	Manchmal ist das recht nuetzlich, z.B. kann man so eigentlich schwer
	zu toetende NPCs dagegen schuetzen, dass man ihnen angezuendetes
	Dynamit oder aehnliches ueberreicht.

BEMERKUNGEN
-----------
::

	Innerhalb von NPCs ist die Funktion give_notify() fuer die
	automatische Auswertung dieser Property verantwortlich; das sollte
	man insbesondere beim Ueberschreiben dieser Funktion beachten!

SIEHE AUCH
----------
::

	give_notify(), /std/npc/put_and_get.c


Last modified: Mon Apr 23 16:54:07 2001 by Patryn

