ShowPropList()
==============

FUNKTION
--------
::

     void ShowPropList(string *props);

DEFINIERT IN
------------
::

     /std/thing/util.c

ARGUMENTE
---------
::

     props
          Array von Strings mit den Namen der Properties, die ausgegeben
          werden sollen.

BESCHREIBUNG
------------
::

     Gibt die Inhalte der in props angegebenen Properties aus.

RUeCKGABEWERT
-------------
::

     keiner

BEISPIELE
---------
::

     Sei test.c folgendes Programm:

     inherit "std/thing";
     inherit "std/thing/util";

     create() {
       ::create();

       SetProp(P_SHORT, "Kurz");
       SetProp(P_NAME, ({ "Name", "Namens", "Namen", "Namen" }));
       SetProp("me", this_object() );
     }

     Mit xcall test->ShowPropList( ({ P_SHORT, P_NAME, "me" }) ); erhielte
     man dann folgende Ausgabe:

     *short: "Kurz"
     *name: ({ "Name", "Namens", "Namen", "Namen" })
     *me: OBJ(/players/wargon/test#12345)

BEMERKUNGEN
-----------
::

     Mit dem Befehl xprops des MGtools lassen sich uebrigens saemtliche
     Properties eines Objekte auf einen Schlag ausgeben.

SIEHE AUCH
----------
::

     /std/ting/util.c


Last modified: Wed May 8 10:25:26 1996 by Wargon

