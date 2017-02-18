depointer_adj()
=========

FUNKTION
--------
::

     private string depointer_adj(<string|string*>* adj, int casus, int demon);

DEFINIERT IN
------------
::

     /std/thing/description.c

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

     Ruft fuer jeden Eintrag von adj DeclAdj() auf und baut aus den 
     Rueckgabewerten einen String zusammen.

RUeCKGABEWERT
-------------
::

     Die aneinandergehaengten und kommagetrennten Namensadjektive.

SIEHE AUCH
----------
::

     /std/thing/description.c, DeclAdj(), P_NAME_ADJ


Letzte Aenderung: 18.02.2017, Bugfix

