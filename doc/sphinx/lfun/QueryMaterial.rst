QueryMaterial()
===============

QueryMaterial(L)
----------------
::

FUNKTION
--------
::

     int QueryMaterial(string mat)

DEFINIERT IN
------------
::

     /std/thing/description.c

ARGUMENTE
---------
::

     string mat -	Material, auf das getestet werden soll

BESCHREIBUNG
------------
::

     Testet, ob ein Gegenstand aus dem angegebenen Material besteht
     und gibt dessen Anteil zurueck.
     Die Rueckgabe ist im Wertebereich -100 (Antigruppen) bis +100 (%).

RUECKGABEWERT
-------------
::

     Anteil in Prozent.

BEISPIELE
---------
::

     if(ob->QueryMaterial(MAT_IVORY)<=0)
       write("Daraus kannst Du keine Billiardkugeln schnitzen!\n");

SIEHE AUCH
----------
::

     Konzepte:	  material, materialerkennung
     Grundlegend: P_MATERIAL, /sys/thing/material.h
     Methoden:    QueryMaterialGroup(), MaterialList(),
     Listen:	  AllMaterials(), AllGroups(), Dump()
		  materialliste, materialgruppen
     Master:	  AddMaterial(), ConvMaterialList(), MaterialGroup(),
		  GroupName(), MaterialName(),
		  GetGroupMembers(), GetMatMembership()
     Sonstiges:	  P_MATERIAL_KNOWLEDGE

7. Mai 2004 Gloinson

