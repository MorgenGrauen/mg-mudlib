Dump()
======

FUNKTION
--------
::

     void Dump()

DEFINIERT IN
------------
::

     /p/daemon/materialdb.c (MATERIALDB)

BESCHREIBUNG
------------
::

     Schreibt alle Materialien samt ihren Gruppen und alle Gruppen mit
     dazugehoerenden Materialien in DUMPFILE (/p/daemon/save/MATERIALS)
     Wird in create() der Materialiendatenbank automatisch aufgerufen.
     Das Dumpfile ist zum Recherchieren der Materialien gedacht.

SIEHE AUCH
----------
::

     Konzepte:	  material, materialerkennung
     Grundlegend: P_MATERIAL, /sys/thing/material.h
     Listen:	  AllMaterials(), AllGroups()
		  materialliste, materialgruppen
     Master:	  AddMaterial(), ConvMaterialList(), MaterialGroup(),
		  GroupName(), MaterialName(),
		  GetGroupMembers(), GetMatMembership()

7. Mai 2004 Gloinson

