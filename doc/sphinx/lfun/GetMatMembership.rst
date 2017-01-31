GetMatMembership()
==================

FUNKTION
--------
::

     string *GetMatMembership(string mat)

DEFINIERT IN
------------
::

     /p/daemon/materialdb.c (MATERIALDB)

ARGUMENTE
---------
::

     string mat - ein Material

BESCHREIBUNG
------------
::

     Gibt alle Gruppen, denen das Material angehoert zurueck. Geeignet, um
     die Eigenschaften eines Materials zu ueberpruefen.

RUECKGABEWERT
-------------
::

     Array von Strings mit Materialiengruppen oder ({})

BEISPIELE
---------
::

     // ein weiser Schmied:
     int i;
     string *mat, mname, mgroup;
     mat=m_indices(ob->QueryProp(P_MATERIAL));
     i=sizeof(mat);

     write("Der Schmied sagt: "+ob->Name(WER)+" besteht aus ...\n");
     while(i--) {
      // den Namen erkennen/aussprechen:
      // Materialien werden allgemein etwas besser erkannt (zu 5%), aber
      // alles aus Metall wird zu +100% besser erkannt ...
      mname=MATERIALDB->MaterialName(mat[i], WER,
	     ({5, ([MATRGROUP_METAL, 100])}));

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
		  GroupName(), MaterialName(),
		  GetGroupMembers()

7. Mai 2004 Gloinson

