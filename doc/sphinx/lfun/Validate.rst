Validate()
==========

FUNKTION
--------
::

   string Validate(string oname);

DEFINIERT IN
------------
::

   /std/virtual/v_compiler.c

ARGUMENTE
---------
::

   oname
       Objektname, der geprueft werden soll 

RUeCKGABEWERT
-------------
::

   

BESCHREIBUNG
------------
::

   Diese Funktion hat die Aufgabe zu ueberpruefen ob ein Objekt welches
   geladen werden soll, in dem VC  ueberhaupt erlaubt ist. Dieser 
   Funktion wird nur der reine Filename uebergeben, ohne Pfad!
   Diese Funktion macht im Standard-VC in /std/ nichts weiter, als
   das '.c' am File Namen abzuschneiden.
   Sollte der Dateiname gueltig sein liefert die Funktion als Rueckgabewert
   den Filenamen ohne .c und sonst 0.

BEMERKUNGEN
-----------
::

   Am besten ruft man in seinem Validate() das ::Validate(), was einem die
   Arbeit abnimmt, ein .c am Ende zu entfernen.

BEISPIEL
--------
::

   string Validate(string oname) {
     string raum, spieler;
     //.c abschneiden
     oname=::Validate(oname);

     

     // folgt der Raum dem Muster "arena|name"? Wenn nein -> ungueltig,
     // 0 zureckgeben, sonst den Filenamen.
     if(sscanf(oname,"%s|%s",raum,spieler)<2 || raum!="arena")
        return 0;
     return oname;
   }

SIEHE AUCH
----------
::

     virtual_compiler
     CustomizeObject(), Validate(), NoParaObjects(), 
     P_COMPILER_PATH, P_PARA
     /std/virtual/v_compiler.c

27.10.2007, Zesstra

