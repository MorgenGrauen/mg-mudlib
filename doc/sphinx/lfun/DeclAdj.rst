DeclAdj()
=========

FUNKTION
--------
::

     varargs string DeclAdj( string adj, int casus, int demon);

DEFINIERT IN
------------
::

     /std/thing/language.c

ARGUMENTE
---------
::

     adj
          Das zu deklinierende Adjektiv.

     casus
          Der Fall, in den es dekliniert werden soll.

     demon
          Bezieht sich das Adjektiv auf einen bestimmten oder einen
          unbestimmten Gegenstand?

BESCHREIBUNG
------------
::

     Dekliniert das uebergebene Adjektiv in den angegebenen Fall. Ist demon
     ungleich Null, so wird das Adjektiv so behandelt, als wuerde es sich
     auf einen bestimmten Gegenstand beziehen, ansonsten bezieht es sich auf
     einen unbestimmten Gegenstand.

RUeCKGABEWERT
-------------
::

     Das deklinierte Adjektiv. Es wird zusaetzlich noch ein Leerzeichen
     hinten angefuegt!

BEISPIELE
---------
::

     Zunaechst ein bestimmtes Adjektiv:

     printf("Der %sBall.\n", ball->DeclAdj("gruen", WER, 1);

     Nun ein unbestimmtes Adjektiv:

     printf("Ein %sBall.\n", ball->DeclAdj("gruen", WER, 0);

     Da DeclAdj() "gruene " bzw. "gruener " zurueckgibt, darf zwischen dem
     "%s" und dem "Ball" kein Leerzeichen stehen!

SIEHE AUCH
----------
::

     /std/thing/language.c


Last modified: Wed May 8 10:18:05 1996 by Wargon

