QueryDu()
=========

FUNKTION
--------
::

     varargs string QueryDu(int casus, int gender, int anzahl);

DEFINIERT IN
------------
::

     /std/thing/language.c

ARGUMENTE
---------
::

     casus
          Der Fall fuer die Anrede.

     gender
          Das Geschlecht des anzuredenden Objektes.

     anzahl
          Ist nur ein Objekt anzusprechen oder mehrere?

BESCHREIBUNG
------------
::

     Diese Funktion liefert die dem Anlass entsprechende Anrede fuer ein
     Objekt.

RUeCKGABEWERT
-------------
::

     Ein String mit der Anrede.

BEISPIELE
---------
::

     printf("%s setzt %s auf den Boden.\n",
           capitalize(QueryDu(WER, TP->QueryProp(P_GENDER), SINGULAR),
           QueryDu(WEN, TP->QueryProp(P_GENDER), SINGULAR));

     (In den meisten Faellen kann man hier auch direkt "Du" und "dich"
     einsetzen.)

SIEHE AUCH
----------
::

     /std/thing/language.c
     QueryPossPronoun(), QueryOwn(), QueryPronoun()

Last modified: Wed May 8 10:22:27 1996 by Wargon

