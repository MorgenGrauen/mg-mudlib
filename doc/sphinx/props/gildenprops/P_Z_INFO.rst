P_Z_INFO
========

NAME
----
::

     P_Z_INFO						"z_info"

DEFINIERT IN
------------
::

     /p/zauberer/wiznpc.h

BESCHREIBUNG
------------
::

     Diese Property muss gesetzt werden, wenn man den
     Zauberergilden-Standard-NPC nutzt. Sie setzt die
     Grundeinstellungen des NPCs und generiert das 
     Newskills-Mapping. 

     Die Property ist wie folgt aufgebaut:

     * MIN (minimale Skillability im Bereich von 0 - 10000)
     * MAX (maximale Skillability im Bereich von 0 - 10000)
     * LEV (Gildenlevel)
     * ZWEIG (Gildenzweig)

BEMERKUNGEN
-----------
::

     Fuer die Argumente LEV und ZWEIG stehen folgende Auswahl-
     moeglichkeiten zur Verfuegung.

     LEV:

        Z_ANFAENGER	0
	Z_LEHRLING	1
	Z_MEISTER	2
	Z_ERZMEISTER	3

     ZWEIG:

        ZZW_ANGRIFF		  1
	ZZW_ABWEHR		  2
	ZZW_ILLUSION		  4
	ZZW_BEHERRSCHUNG	  8
	ZZW_HELLSICHT		 16
	ZZW_VERWANDLUNG		 32
	ZZW_SCHWARZ		 64	INAKTIV
	ZZW_WEISS		128
	ZZW_ALLE		511

BEISPIEL
--------
::

     SetProp(P_Z_INFO, ({9000, 9500, Z_ERZMEISTER, ZZW_ANGRIFF|ZZW_ABWEHR}));
     erzeugt einen Erzmagister-PC, der alle Lehrlings- sowie die Magister und
     Erzmagister-Sprueche Angriff und Abwehr mit 90-95% beherrscht.

SIEHE AUCH
----------
::

     /p/zauberer/text/wiznpc.doku

21.Okt 2006 Chagall

