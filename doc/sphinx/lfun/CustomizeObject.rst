CustomizeObject()
=================

FUNKTION
--------
::

   string CustomizeObject();

DEFINIERT IN
------------
::

   /std/virtual/v_compiler.c

ARGUMENTE
---------
::

   keine

RUeCKGABEWERT
-------------
::

   Den Objektnamen, den das zuletzt erzeugte Objekt (welches gerade die
   Funktion aufruft) spaeter vom Driver bekommen wird.

BESCHREIBUNG
------------
::

   Diese Funktion ist aus dem Grunde da, da zum Zeitpunkt des Clonens des
   VC-Objektes (P_STD_OBJECT) dieses Objekt ja noch nicht weiss Wer
   oder Was es spaeter mal sein wird.
   Deshalb kann dieses VC-Objekt im create() (und nur da!) die Funktion
   CustomizeObject() in dem virtual_compiler aufrufen, welches das Objekt
   geclont hat und bekommt von diesem den Objektnamen zureck, welches es
   spaeter mal bekommen wird.
   Da das VC-Objekt vom VC geclont wurde, ist previous_object() im create()
   des VC-Objektes der VC, in dem man CustomizeObject() ruft.

BEMERKUNGEN
-----------
::

   Das CustomizeObject() im Standard-VC gibt nur den zukuenftigen Objektnamen
   zurueck und macht sonst nix.

BEISPIELE
---------
::

   create() eines VC-Objektes:

   

   protected void create() {
     ...

     

     // wer bin ich denn eigentlich?
     string myname = previous_object()->CustomizeObject();
     switch(myname) {
       // Kram konfigurier, ja nach myname... 
     }

     

     ...
   }

SIEHE AUCH
----------
::

     virtual_compiler
     CustomizeObject(), Validate(), NoParaObjects(), 
     P_COMPILER_PATH, P_PARA
     /std/virtual/v_compiler.c

21.10.2007, Zesstra

