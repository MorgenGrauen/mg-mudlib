MaterialList()
==============

MaterialList(L)
---------------
::

FUNKTION
--------
::

     varargs string MaterialList(int casus, mixed idinf)

DEFINIERT IN
------------
::

     /std/thing/description.c

ARGUMENTE
---------
::

     int casus	 - der Fall, in dem die Materialien dekliniert werden sollen
     mixed idinf - Dinge, welche die Faehigkeiten des Erkennens beeinflussen:
		   Einzelne Werte:
                   * x: allgemeine Erkennung -100 ... 100
                   * who: der Spieler - P_MATERIAL_KNOWLEDGE wird abgefragt
                   * fun: wird evaluiert
                   * what, kann folgendes enthalten:
                     - Eintrag fuer Materialien ([MAT_XXX:-100...100])
                     - Eintrag fuer Materialiengruppen (dito)
                     - ([MATERIAL_SYMMETRIC_RECOGNIZABILITY: mixed mg])
                       * mg ein Array:
                         ({MATGROUP_X1,int z1, MATGROUP_X2, int z2, ...})
                         wobei bei Zugehoerigkeit von string mat zu Gruppe
                         z<n> auf die Faehigkeit addiert, andernfalls davon
                         subtrahiert wird
		   Array mit obigen Werten:
                   - alle Parameter des Arrays sind optional und additiv
                   - ({int x, object who, mapping what, closure fun})

BESCHREIBUNG
------------
::

     Listet die Materialien auf, aus denen ein Objekt besteht.
     Dabei haengt die Genauigkeit der Materialerkennung von idinf ab. D.h.
     je nach den Faehigkeiten/der angegebenen Faehigkeit wird zB Wolfram
     als "Wolfram" oder nur als "Metall" erkannt.

     Wenn ein Spieler etwas identifiziert, sollte auch TP uebergeben werden,
     bei NPCs koennte das anders aussehen.

RUECKGABEWERT
-------------
::

     String mit Liste der Materialien.

BEMERKUNGEN
-----------
::

     - es werden nur die Materialien angegeben, nicht die Menge.
     - ruft ConvMaterialList() an der MATERIALDB

BEISPIELE
---------
::

     // simpel
     write("Der Gegenstand besteht aus"+ob->MaterialList(WEM,TP)+".\n")
     // -> "Der Gegenstand besteht aus Gold, Silber und Rubin.\n"

     // komplexer
     ob->SetProp(P_MATERIAL, ([P_NITROGLYCERINE:90,P_GUNPOWDER:10]));
     write("Das enthaelt "+ob->MaterialList(WER,TP)+".\n");
     // -> "Das enthaelt Schwarzpulver und Nitroglycerin."

SIEHE AUCH
----------
::

     Konzepte:	  material, materialerkennung
     Grundlegend: P_MATERIAL, /sys/thing/material.h
     Methoden:    QueryMaterial(), QueryMaterialGroup()
     Listen:	  AllMaterials(), AllGroups(), Dump()
		  materialliste, materialgruppen
     Master:	  AddMaterial(), ConvMaterialList(), MaterialGroup(),
		  GroupName(), MaterialName(),
		  GetGroupMembers(), GetMatMembership()
     Sonstiges:	  P_MATERIAL_KNOWLEDGE

7. Mai 2004 Gloinson

