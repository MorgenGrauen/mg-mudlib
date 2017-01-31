PresentEnemies()
================

FUNKTION
--------
::

	object *PresentEnemies();

DEFINIERT IN
------------
::

	/std/living/combat.c

ARGUMENTE
---------
::

	keine

RUeCKGABEWERT
-------------
::

	anwesende Feinde

BESCHREIBUNG
------------
::

	Diese Funktion liefert genau die Feinde zurueck, die sich derzeit im
	selben Raum befinden. Die Feinde unterliegen hierbei noch gewissen
	Einschraenkungen:
	  (1) Sie duerfen als NPC nicht gestorben sein, das heisst, sie
	      muessen als Objekt noch existieren.
	  (2) Sie duerfen als Spieler nicht gestorben sein, das heisst, sie
	      duerfen keine Geister sein (Property P_GHOST nicht gesetzt).
	  (3) Sie muessen angreifbar sein, das heisst, die Property
	      P_NO_ATTACK darf nicht gesetzt sein.
	Wird eine dieser Bedingungen verletzt, so wird der Gegner aus der
	internen Gegnerliste entfernt. Zusaetzlich gilt:
	  Netztote werden nur als Gegner erkannt, wenn keine anderen Gegner
	  zur Verfuegung stehen.

BEISPIEL
--------
::

	Im Folgenden erblinden alle Gegner im Raum:
	  string blindThemAll()
	  { ob*livList;
	    if(!livList=PresentEnemies())
	      return break_string(
	 "Mist...keiner da, der blind werden moechte.",77,
	 Name(WER)+" grummelt: ");
	    for(i=sizeof(livList);i--;)
	    { if(livList[i]->QueryProp(P_BLIND))
	      { tell_object(this_object(),break_string(
	 livList[i]->Name(WER)+" ist schon blind.",77));
	        continue;
	      }
	      tell_object(this_object(),break_string(
	 "Du kratzt "+livList[i]->name(WEM)+" die Augen aus.",77));
	      tell_object(livList[i],break_string(
	 Name(WER)+" kratzt Dir die Augen aus!",77));
	      tell_room(environment(this_object()),break_string(
	 Name(WER)+" kratzt "+livList[i]->name(WEM)
	+" die Augen aus.",77),({this_object(),livList[i]}));
	      livList[i]->SetProp(P_BLIND,1);
	    }
	    return"";
	  }
	Aufgerufen wird das ganze am Besten in einem Chat:
	  void create()
	  { ::create();
	    ...
	    SetProp(P_CHAT_CHANCE,10);
	    SetChats(20,({break_string(
	 "Nicht angreifen, sonst wirst Du noch blind!",77,
	 Name(WER)+" bruellt: "),
	                  "@@blindThemAll@@"}));
	  }
	Natuerlich sind auch noch mehr Funktionen und Meldungen als Chats
	moeglich: Die zwei im Beispiel sind im Normalfall etwas wenig.

SIEHE AUCH
----------
::

	SelectEnemy(), QueryEnemies(), IsEnemy(), P_GHOST, P_NO_ATTACK,
	SetChats, P_CHAT_CHANCE


Last modified: Thu May 27 15:01:48 1999 by Patryn

