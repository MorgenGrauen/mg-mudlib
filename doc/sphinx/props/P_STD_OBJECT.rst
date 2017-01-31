P_STD_OBJECT
============

NAME
----
::

    P_STD_OBJECT                  "std_object"                  

DEFINIERT IN
------------
::

    /sys/v_compiler.h

BESCHREIBUNG
------------
::

   Enthaelt den Namen eines Files welches als Standard-Objekt fuer den 
   Virtual Compiler gelten soll.

   In diesem File werden das generelle Aussehen, Ausgaenge, Funktionen
   usw. der VC-generierten Raeume / Objekte festgelegt.

   Dieses File ist ein 'normales' .c - File, welches geclont wird und
   anschliessend umbenannt wird.

   

   Ganz wichtig: Falls euer Standardobjekt (direkt oder indirekt) von
   /std/room.c erbt, solltet ihr darauf achten, dass euer Objekt ausser dem
   create() noch eine weitere (beliebige) Funktion hat.  
   Ansonsten wuerde das Programm eures Standardobjekts automatisch durch
   /std/room.c ersetzt, was in der Regel zu schwer zu findenen Bugs fuehrt.

BEISPIEL
--------
::

   (create eines VCs)
   protected void create() {
     ...
     SetProp(P_STD_OBJECT,"/d/region/magier/vc/std_raum");
     ...
   }

   Was in diesem std_raum.c nun steht, wird in jedem VC-Clone
   verfuegbar. Sei es Details, Gerueche, auch Objekte die per 
   AddItem eingebunden sind, ...

SIEHE AUCH
----------
::

   P_COMPILER_PATH, virtual_compiler

Letzte Aenderung: 22.10.07 von Zesstra

