UnregisterHelperObject()
========================

FUNKTION
--------
::

     int UnregisterHelperObject(object helper, int type);

DEFINIERT IN
------------
::

     /std/living/helpers.c

ARGUMENTE
---------
::

     object helper
       Das Objekt, das als Hilfsobjekt deregistriert werden soll.
     int type
       Helfertyp, einer der in /sys/living/helpers.h definierten Typen:
       - HELPER_TYPE_AERIAL fuer die Flug-/Segelunterstuetzung
       - HELPER_TYPE_AQUATIC fuer Tauchunterstuetzung

BESCHREIBUNG
------------
::

     Das als Hilfsobjekt fuer bestimmte Aktivitaeten wie zum Beispiel Tauchen
     oder Fliegen bei einem Lebewesen registrierte Objekt "helper" meldet
     sich bei diesem ab.
     Hinweis: fuer eine temporaer gueltige "Nicht-Zustaendigkeit" kaeme auch
     in Frage, in dieser Zeit einfach "0" zurueckzugeben, statt sich
     komplett abzumelden.

RUECKGABEWERTE
--------------
::

      1  Objekt wurde erfolgreich ausgetragen (HELPER_SUCCESS)
     -1  angegebenes Hilfsobjekt existiert nicht (HELPER_NO_CALLBACK_OBJECT)
     -3  angegebenes Hilfsobjekt war gar nicht angemeldet
         (HELPER_NOTHING_TO_UNREGISTER)

BEISPIEL
--------
::

     Eine luftgefuellte Blase hatte sich als Tauch-Helfer am Spieler
     angemeldet, ist jetzt aber verbraucht und meldet sich daher ab:

     // Austragen im Spielerobjekt
     void BlaseAustragen() {
       [...]
       if ( TP->UnregisterHelperObject(ME, HELPER_TYPE_AQUATIC)
            == HELPER_SUCCESS )
         remove();
     }

SIEHE AUCH
----------
::

     Funktionen:  RegisterHelperObject()
     Properties:  P_HELPER_OBJECTS, P_AERIAL_HELPERS, P_AQUATIC_HELPERS
     Sonstiges:   /sys/living/helpers.h

19.02.2013 Arathorn

