GetGroupMembers()
=================

FUNKTION
--------
::

     string *GetGroupMembers(string grp)

DEFINIERT IN
------------
::

     /p/daemon/materialdb.c (MATERIALDB)

ARGUMENTE
---------
::

     string grp - Gruppenname

BESCHREIBUNG
------------
::

     Gibt alle dieser Gruppe zugehoerigen Materialien zurueck. Dazu gut, sich
     einen Ueberblick ueber die aktuelle Liste zu verschaffen.

RUECKGABEWERT
-------------
::

     Array von Strings mit Materialien oder ({})

BEISPIELE
---------
::

     // wir wollen irgend ein Metall haben, nennen dies aber beim Namen
     int ind;
     string* likeit;
     likeit=MATERIALDB->GetGroupMembers(MATGROUP_METAL);
     ind=random(sizeof(likeit));
     ...
     write("Der Schmied sagt: Mir fehlt noch "+
	   MATERIALDB->MaterialName(likeit[ind], WER, 100)+".\n");
     ...

SIEHE AUCH
----------
::

     Konzepte:	  material, materialerkennung
     Grundlegend: P_MATERIAL, /sys/thing/material.h
     Master:	  AddMaterial(), ConvMaterialList(), MaterialGroup(),
		  GroupName(), MaterialName(),
		  GetMatMembership()

7. Mai 2004 Gloinson

