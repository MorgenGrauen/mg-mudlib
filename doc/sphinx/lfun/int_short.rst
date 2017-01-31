int_short()
===========

FUNKTION
--------
::

     string int_short(mixed viewer, mixed viewpoint);

DEFINIERT IN
------------
::

     /std/room/description.c

ARGUMENTE
---------
::

     mixed viewer	- der Betrachter des Raumes
     mixed viewpoint	- 0/Objekt/Array der/die Ausgangspunkt(e) des
			  Betrachtens (und damit nicht sichtbar!)

BESCHREIBUNG
------------
::

     Es wird eine kurze  Beschreibung des Rauminneren erstellt. Dabei wird
     die Kurzbeschreibung des Raumes, die enthaltenen Objekte (exklusive
     aller viewpoints (normalerweise nur der Betrachter)) und Ausgaenge,
     wenn vom Viewer eingeschaltet dargestellt.

     Ist Viewer ein Magier mit eingeschaltetem Magiermodus, so wird der
     Beschreibung der Dateiname des Raumes vorangestellt.

RUeCKGABEWERT
-------------
::

     Die Kurzbeschreibung des Raumes von innen.

BEMERKUNGEN
-----------
::

     Die Trennung von viewer und viewpoint hat durchaus ihren Sinn. So ist
     es zum Beispiel moeglich, einen Raum "mit den Augen eines Anderen" zu
     betrachten. Dabei saehe man sich selbst, wenn man im Raum waere.

BEISPIELE
---------
::

     // in diesem Raum sieht man keine Mitspieler im "schau" oder beim
     // Betreten (vielleicht ist es zu neblig)
     // dazu werden einfach alle Interactives zu den viewpoints addiert
     string int_long(object viewer, mixed viewpoints, int flags) {
      if(!pointerp(viewpoints)) viewpoints=({viewpoints});
      return ::int_long(&viewer,
			viewpoints+
			filter(all_inventory(this_object()),
				     #'interactive),
			&flags);
     }

     string int_short(object viewer, mixed viewpoints) {
      if(!pointerp(viewpoints)) viewpoints=({viewpoints});
      return ::int_short(&viewer,
			 viewpoints+
			 filter(all_inventory(this_object()),
				      #'interactive));
     }

SIEHE AUCH
----------
::

     Aehnliches:	int_long()
     Properties:	P_INT_SHORT, P_SHORT
			P_HIDE_EXITS, P_SHOW_EXITS
     Kommandokette:	make_invlist(), short()
     Sonstiges:		P_REFERENCE_OBJECT, P_WANTS_TO_LEARN

11. Mai 2004 Gloinson

