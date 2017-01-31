CheckLightType()
================

FUNKTION
--------
::

	varargs int CheckLightType(int lighttype, int mode);

DEFINIERT IN
------------
::

	/std/thing/description.c

ARGUMENTE
---------
::

	lighttype
	  Auf diesen Lichttyp wird getestet.
	mode
	  Die Methode, nach der der Lichttyp ueberprueft wird.

BESCHREIBUNG
------------
::

        Die Funktion prueft, ob der uebergebene lighttype mit dem in
        P_LIGHT_TYPE definierten Lichttyp uebereinstimmt. 

        Dabei kann in verschiedenen Modi getestet werden:

        LT_CHECK_ANY
          Es wird geprueft, ob mindestens einer der in lighttype ueber
          gebenen Lichttypen im Objekt vorhanden ist. Dies ist das
          Standardverhalten (default) der Funktion.

        LT_CHECK_ALL     
          Es wird geprueft, ob alle in lighttype definierten Lichttypen
          vorhanden sind. Es koennen aber auch mehr Lichttypen definiert
          sein.

        LT_CHECK_MATCH   
          Es wird geprueft, ob genau die in lighttype definierten Licht-
          tyen definiert sind, sind mehr oder weniger vorhanden, gibt die
          Funktion 0 zurueck.

        LT_CHECK_NONE    
          Es wird geprueft, ob keiner der uebergebenen Lichttypen vorhanden
          sind. Ob sonst noch andere Lichttypen vorhanden sind, ist dabei 
          egal.

RUeCKGABEWERT
-------------
::

	0 wenn die geprueften Bedingungen nicht korrekt sind, sonst 
        ein Wert ungleich 0.

BEISPIELE
---------
::

        In einem Raum scheint die Sonne, ausserdem gibt es dort ein Lager-
        feuer und ein Objekt mit magischem Gluehen (meine Phantasie streikt
        grad):

        raum->SetProp( P_LIGHT_TYPE, LT_SUN|LT_OPEN_FIRE|LT_GLOWING );

        Es soll getestet werden, ob an in dem Raum Tageslicht herrscht:

        raum->CheckLightType(LT_DAYLIGHT, LT_CHECK_ANY);
        raum->CheckLightType(LT_DAYLIGHT); // gleichwertig

        Die Funktion ergibt wahr, da LT_DAYLIGHT unter anderem LT_SUN ent-
        haelt (vgl man P_LIGHT_TYPES).

        Es soll getestet werden, dass weder Mond noch Sterne im Raum sind:

        raum->CheckLightType(LT_MOON|LT_STARS, LT_CHECK_NONE);

        Die Funktion ergibt wahr, da die beiden nicht gesetzt sind.

        Es soll geprueft werden, ob Mond und Sterne im Raum leuchten:

        raum->CheckLightType(LT_MOON|LT_STARS, LT_CHECK_ALL);

        Die Funktion ergibt falsch, da keins der beiden Lichter vorhanden
        ist. Sie ergaebe aber auch falsch, wenn einer der beiden Typen
        vorhanden waer. Nur wenn beide vorhanden sind, gibt LT_CHECK_ALL
        wahr.

BEMERKUNG
---------
::

        Lighttypes haben nichts mit dem Lichtsystem zu tun. Sie dienen 
        nur der Beschreibung der Lichtverhaeltnisse an/in einem Objekt.
        Objekte mit verschiedenen Lichtverhaeltnissen beeinflussen sich
        gegenseitig nicht.

SIEHE AUCH
----------
::

        /std/thing/description.c, /std/thing/lighttypes.h, P_LIGHT_TYPE

Last modified: Fri Jun 11 20:47:33 2004 by Vanion

