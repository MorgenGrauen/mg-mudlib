P_RACE
======

NAME
----

	P_RACE				"race"

DEFINIERT IN
------------

	/sys/living/description.h

BESCHREIBUNG
------------

	Die Rasse eines Lebewesens kann ueber diese Property ermittelt bzw.
	gesetzt werden. Es empfiehlt sich hierbei, Rassen nur in Form von
	grossgeschriebenen Strings zu setzen. Leichen erhalten mittels
	dieser Property automatisch die Rasse der Lebewesen, aus denen sie
	hervorgegangen sind.
	Der Sinn des Ganzen liegt darin, das Spiel differenzierter zu
	gestalten und auf Rassenspezifika einzugehen. Zum Beispiel koennen
	Elfen weniger Alkohol vertragen als Zwerge, was in '/std/pub'
	beruecksichtigt wurde.

BEISPIEL
--------

.. code-block:: pike

	void create()
	{ ::create();
	  // Definitionen
	  SetProp(P_RACE,"Ork");
	  // weitere Definitionen
	}

SIEHE AUCH
----------

  :doc:`P_REAL_RACE`
	/std/npc.c, /std/pub.c


Last modified: 15.10.2018 Zesstra
