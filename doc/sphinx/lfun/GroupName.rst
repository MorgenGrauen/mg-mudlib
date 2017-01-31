GroupName()
===========

FUNKTION
--------
::

     string GroupName(string grp)

DEFINIERT IN
------------
::

     /p/daemon/materialdb.c (MATERIALDB)

ARGUMENTE
---------
::

     string grp - ein Gruppenname

BESCHREIBUNG
------------
::

     Gibt die Langbeschreibung des Gruppennamens zurueck.

RUECKGABEWERT
-------------
::

     Die Gruppenbeschreibung oder "Unbekanntes"

BEISPIELE
---------
::

     // simpel
     tmp=m_indices(ob->QueryProp(P_MATERIAL));
     write("Dieses Objekt gehoert u.a. zur Gruppe "+
           MATERIALDB->GroupName(MATERIALDB->GetMatMembership(tmp[0])[0])+
           ".\n");
     // gibt die erste Gruppenzugehoerigkeit des erste Materials in
     // P_MATERIAL zurueck (bei MATGROUP_METAL z.B. "... zur Gruppe Metalle.")

     // ein weiser Schmied:
     int i;
     string *mat, mname, mgroup;
     mat=m_indices(ob->QueryProp(P_MATERIAL));
     i=sizeof(mat);

     write("Der Schmied sagt: "+ob->Name(WER)+" besteht aus ...\n");
     while(i--) {
      // den Namen erkennen/aussprechen:
      // Materialien werden allgemein ganz schlecht erkannt (zu 5%), aber
      // alles aus Metall wird zu +100% gut erkannt ...
      mname=MATERIALDB->MaterialName(mat[i], WER,
	     ({5, ([MATERIAL_SYMMETRIC_RECOGNIZABILITY:
			({MATGROUP_METAL, 100})])}));

      // und nur Metalle analysieren ...
      if(MATERIALDB->MaterialGroup(([mat[i]:100]),MATGROUP_METAL)>=100) {
       int j;
       string *mgr;
       mgr=MATERIALDB->GetMatMembership(mat[i]);
       j=sizeof(mgr);
       mgroup=" gehoert zu ";
       while(j--) {
        mgroup+=MATERIALDB->GroupName(mgr[j]);
        if(j>0) mgroup+=", ";
       }
      } else mgroup=" kenne ich nicht";
      printf("%-12.12s: %s\n",mname, mgroup);
     }

SIEHE AUCH
----------
::

     Konzepte:	  material, materialerkennung
     Grundlegend: P_MATERIAL, /sys/thing/material.h
     Master:	  AddMaterial(), ConvMaterialList(), MaterialGroup(),
		  MaterialName(),
		  GetGroupMembers(), GetMatMembership()

7. Mai 2004 Gloinson

