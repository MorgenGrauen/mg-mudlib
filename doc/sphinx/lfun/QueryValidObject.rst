QueryValidObject()
==================

FUNKTION
--------
::

     public int QueryValidObject(string oname);

DEFINIERT IN
------------
::

     /std/virtual/v_compiler.c

ARGUMENTE
---------
::

     oname
       Objektname, der geprueft werden soll (kompletter Pfad mit / am Anfang) 

RUeCKGABEWERT
-------------
::

     <=0 - falls VC nicht zustaendig ist.
     >0 - falls der VC sich fuer das Objekt zustaendig erklaert.

BESCHREIBUNG
------------
::

     Ueber die Funktion laesst sich herausfinden, ob ein VC sich fuer das
     gewuenschte Objekt zustaendig fuehlt. Dabei wird Validate(),
     P_COMPILER_PATH, NoParaObjects() und P_PARA im VC ausgewertet:
     1. Zuerst wird mit Validate() geprueft, ob der Filename (ohne Pfad) ok ist.
     2. wird geguckt, ob das angefragte Objekt im richtigen Pfad liegt 
        (P_COMPILER_PATH).
     3. wenn das angefragte Objekt ein Para-Objekt ist:
       a) wird NoParaObjects() geprueft, wenn das !=0 ist, sind gar keine Para-
          Objekte erlaubt.
       b) wird P_PARA _im VC_ abgefragt, dort kann man ein Array aller 
          erlaubten Para-Dimensionen reinschreiben. Fuer alle anderen erklaert 
          sich der VC fuer nicht zustaendig. Wenn P_PARA nicht gesetzt ist, 
          sind alle erlaubt. Ein leeres Array ({}) wuerde einem 
          NoParaObjects() {return 1;} entsprechen.

BEMERKUNGEN
-----------
::

     Diese Funktion wird vom move abgefragt. Bitte auf jeden Fall P_PARA oder
     NoParaObjects() passend definieren, sonst buggts.

     Wenn jemand mit dem oben beschrieben Standardverhalten nicht gluecklich
     ist, kann man die Funktion passend ueberschreiben.

SIEHE AUCH
----------
::

     virtual_compiler
     CustomizeObject(), Validate(), NoParaObjects(), 
     P_COMPILER_PATH, P_PARA
     /std/virtual/v_compiler.c

21.10.2007, Zesstra

