AddItem()
=========

FUNKTION
--------
::

	varargs object AddItem(mixed filename,int refresh,mixed props);

DEFINIERT IN
------------
::

	/std/room/items.c
	/std/npc/items.c

ARGUMENTE
---------
::

	filename
	  String mit dem Namen des zu erzeugenden Objektes oder Array von
	  Strings mit den Namen der zu erzeugenden Objekte. Bei einem Array
	  wird ein Name zufaellig ausgewaehlt.
	refresh
	  Wann und wie soll das Objekt erneuert werden:
	  - REFRESH_NONE      - Kein Refresh bis zum Neuladen des Raums
                        - oder NPCs.
	  - REFRESH_DESTRUCT  - Refresh bei Reset, wenn Item zerstoert
	                        wurde.
	  - REFRESH_REMOVE    - Refresh bei Reset, wenn Item entfernt wurde.
	  - REFRESH_ALWAYS    - Neuer Clone bei jedem Reset.
	  - REFRESH_MOVE_HOME - Objekt wird bei Reset automatisch
	                        zurueckgeholt, wenn es wegbewegt wurde.
	                        (nur in Raeumen!)
	  Bei NPC's gilt zusaetzlich:
	  - CLONE_WEAR       - Item Anziehen, wenn es eine Ruestung ist.
	  - CLONE_WIELD      - Item zuecken, wenn es eine Waffe ist.
	  - CLONE_NO_CHECK   - Zuecken oder Anziehen ohne Ueberpruefungen.
	props (optional)
	  Mapping mit denen in einem geclonten Objekt zu setzenden
	  Properties oder 1 fuer die Erzeugung einer Blueprint.

RUeCKGABEWERT
-------------
::

	Innerhalb von Raeumen wird das erzeugte Objekt zurueckgeliefert. Bei
	NPC's klappt dies leider nicht, da dort die Objekte nicht sofort
	erzeugt werden, sondern erst, nachdem der NPC an seinen
	Bestimmungsort transferiert wurde. Daher wird bei NPC immer 0 
	zurueckgegeben.

BESCHREIBUNG
------------
::

	Abhaengig von <filename> und <props> wird ein Objekt erzeugt und in
	den Raum bzw. NPC bewegt. Dabei gibt es folgende Moeglichkeiten:
	- <filename> ist ein Dateiname.
	  Es wird ein Clone dieser Datei erstellt oder (wenn <props>=1 ist)
	  deren Blueprint verwendet.
	- <filename> ist ein Array von Dateinamen.
	  Es wird eine Datei zufaellig aus dem Array ausgewaehlt und von
	  dieser Datei ein Clone erstellt oder (wenn <props>=1 ist) deren
	  Blueprint verwendet.
	Uebergibt man fuer <props> ein Mapping mit dem Aufbau
	  ([prop_name:prop_wert,...]),
	so werden diese Properties im erzeugten Objekt gesetzt.
	Der Parameter <refresh> gibt an, was waehrend eines Resets im Raum
	bzw. in NPC's oder was beim Erzeugen von NPC's geschehen soll:
	In <rooms.h> sind dazu folgende Moeglichkeiten definiert:
	- REFRESH_NONE
            Das Objekt wird niemals erneuert; falls es zerstoert wurde, wird
	    es erst dann wieder erzeugt, wenn der Raum erneut geladen bzw.
	    der NPC neu erzeugt wird. Man beachte, dass nicht jeder NPC
	    wirklich refreshende Objekte benoetigt, REFRESH_NONE spart
	    hierbei sowohl Rechenzeit als auch Speicher!
	- REFRESH_DESTRUCT
	    Das Objekt wird nur dann erneuert, wenn es in der Zwischenzeit
	    zerstoert wurde (bei NPC's ist das zum Beispiel der Fall, wenn
	    sie getoetet wurden).
	    REFRESH_NONE & REFRESH_DESTRUCT + Blueprint-Objekt bedeutet bei
	    NPC's ein Unique-Objekt, es wird also nicht beim Neuerzeugen des
	    NPC's zurueckgesetzt.
	- REFRESH_REMOVE
	    Das Objekt wird erneuert, wenn es sich nicht mehr im Raum bzw.
	    im NPC befindet. Das kein sein, weil es zerstoert wurde, aber
	    auch zum Beispiel in folgenden Faellen:
	    * weil es jemand mitgenommen hat
	       (in Raeumen bei Gegenstaenden)
	    * weil es fortgegangen ist
	       (in Raeumen bei NPC's, die herumlaufen)
	    * weil es weggeworfen wurde
	       (in NPC's bei Gegenstaenden)
	- REFRESH_ALWAYS
	    Das Objekt wird immer erneuert. Von dieser Refreshmethode sollte
	    man allerdings Abstand nehmen, da sich sonst mit der Zeit
	    gewaltige Mengen von Objekten ansammeln koennen!
	- REFRESH_MOVE_HOME
	    Das Objekt wird in einen Raum zurueckbewegt, sofern es noch
	    existiert, jedoch nicht mehr in dem Raum ist. Sinnvoll ist dies
	    eigentlich nur fuer Lebewesen, funktioniert aber auch bei
	    beliebigen Objekten. Hauptsaechlich geht es hierbei darum,
	    herumlaufende NPCs oder bei erzwungenen Bewegungen nicht von
	    P_GUARD zurueckgehaltene NPCs wieder an einen definierten
	    Ausgangsort zurueckzubringen.
	Hat man in Raeumen als <filename> ein Array von Dateinamen
	uebergeben, so wird beim Reset jedesmal aufs Neue ein zufaelliges
	Objekt aus der Liste ausgewaehlt (nicht in NPC's).
	In NPC's gilt der Grundsatz der Vermeidung von ueberfluessigen
	Objekten im MUD. Neu erzeugt werden Objekte beim Erzeugen eines
	NPC's oder bei einem Reset im selbigen. Anstatt die Objekte gleich
	neu zu erschaffen, wird erst geschaut, ob sich identische Objekte
	schon im Raum befinden. Ist dies der Fall, so nimmt der NPC sie auf,
	ruft jedoch vorher nochmals create() in ihnen auf!
	  (noetig wegen moeglicher Veraenderungen an den Objekten)
	Was dann passiert, haengt von weiteren Angaben in <refresh> ab.
	Folgende weitere Moeglichkeiten sind in <npc.h> definiert:
        - CLONE_WEAR
	  Ist das hinzugefuegte Item eine Ruestung, so wird sie nach
	  Aufnahme oder Neuerzeugung angezogen.
        - CLONE_WIELD
	  Ist das hinzugefuegte Item eine Waffe, so wird sie nach Aufnahme
	  oder Neuerzeugung gezueckt.
        - CLONE_NO_CHECK
	  Hiermit verhindert man eine Ueberpruefung, ob eine Ruestung
	  angezogen oder eine Waffe gezueckt werden kann. Es ist jedoch
	  Vorsicht geboten: So kann es ohne weiteres passieren, dass ein NPC
	  mehrere Ruestungen gleichen Typs angezogen oder mehrere Waffen
	  gezueckt hat.
	Benutzt man Blueprints (<props>=1) mit REFRESH_REMOVE oder
	REFRESH_ALWAYS, so kann es zu ungewollten Ueberraschungen kommen, da
	die Blueprint dann unabhaengig von ihrem momentanen Aufenthaltsort
	wieder in den Raum bzw. NPC bewegt wird, von dem sie erzeugt wurde!

BEMERKUNGEN
-----------
::

	Wenn man Blueprints benutzt, sollte man daran denken, dass sich von
	dieser dann keine Clones mehr erstellen lassen!
	RemoveItem() zum Entfernen von Items ist nur fuer Raeume definiert!

	Die Option CLONE_NEW ist veraltet. Die Objekte werden nun immer
	neu erzeugt. Die Option darf noch angegeben werden, hat aber keine
	Bedeutung mehr.

BEISPIELE
---------
::

	// Ein Wuerfel, der sich nach Entfernen erneuert:
	  AddItem("/obj/misc/wuerfel",REFRESH_REMOVE);
	// Ein etwas veraenderter Wuerfel:
	  AddItem("/obj/misc/wuerfel",
	          REFRESH_REMOVE,
	          ([P_SHORT :"Ein schwerer Wuerfel",
	            P_WEIGHT:100]));
	// Eine Blueprint, die nur einmal im MUD existiert. Wenn sie
	// zerstoert wurde, wird sie bei Reset neu erzeugt:
	  AddItem("/mon/angsthase",REFRESH_DESTRUCT,1);
	// Eine Blueprint, die nur einmal im MUD existiert. Wenn sie aus dem
	// Raum entfernt wurde, wird sie bei Reset zurueckgeholt:
	  AddItem("/mon/angsthase",REFRESH_MOVE_HOME,1);
	// Ein zufaelliges Objekt:
	  AddItem(({"/obj/misc/lolli",
	            "/obj/misc/bonbon",
	            "/obj/misc/bier"}),REFRESH_REMOVE);
	// Eine Ruestung, die auch angezogen wird (nur in NPC's):
	  AddItem("/ruestung/sommerkleid",REFRESH_REMOVE|CLONE_WEAR);
	// Eine Unique-Waffe, die auch gezueckt wird (nur in NPC's):
	  AddItem("/waffe/zapper",REFRESH_DESTRUCT|CLONE_WIELD,1);

SIEHE AUCH
----------
::

	RemoveItem(), replace_program(), create(), P_GUARD,
	/std/room/items.c, /std/npc/items.c,
	/sys/rooms.h, /sys/npc.h


Last modified: Thu Nov 23 13:43:30 CET 2006 by Rumata

