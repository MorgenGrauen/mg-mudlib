MaterialGroup()
===============

FUNKTION
--------
::

     int MaterialGroup(mapping mats, string grp)

DEFINIERT IN
------------
::

     /p/daemon/materialdb.c (MATERIALDB)

ARGUMENTE
---------
::

     mapping mats - Materialienmapping
     string grp   - Materialiengruppe

BESCHREIBUNG
------------
::

     Die Materialien im Mapping werden auf Zugehoerigkeit zu der Gruppe
     untersucht und der Gesamtanteil dieser Materialiengruppe am Mapping
     in Prozent zurueckgegeben (wenn das Mapping sich auf 100% aufaddiert).

RUECKGABEWERT
-------------
::

     int - prozentualer Anteil der Materialiengruppe -100 ... 100 %

BEISPIELE
---------
::

     if(MATERIALDB->MaterialGroup(
		([MAT_MISC_STONE:40,MAT_AMETHYST:50,MAT_MISC_METAL:10]),
		MATGROUP_JEWEL)>50)
      write("Oh ja, darin sind sehr viele Edelsteine!\n");

BEMERKUNGEN
-----------
::

     Wird von /std/thing/description::QueryMaterialGroup() gerufen.
     Bitte an Objekten auch QueryMaterialGroup() verwenden.

SIEHE AUCH
----------
::

     Konzepte:	  material, materialerkennung
     Grundlegend: P_MATERIAL, /sys/thing/material.h
     Master:	  AddMaterial(), ConvMaterialList()
		  GroupName(), MaterialName(),
		  GetGroupMembers(), GetMatMembership()

7. Mai 2004 Gloinson

