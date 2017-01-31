MaterialName()
==============

FUNKTION
--------
::

     varargs string MaterialName(string mat, int casus, mixed idinf)

DEFINIERT IN
------------
::

     /p/daemon/materialdb.c (MATERIALDB)

ARGUMENTE
---------
::

     string mat	 - das zu erkennende Material
     int casus	 - der Fall
     mixed idinf - Dinge, welche die Faehigkeiten des Erkennens beeinflussen
		   (siehe "man MaterialList")

BESCHREIBUNG
------------
::

     Diese Funktion sucht unter Beruecksichtigung der Erkennungsbe-
     schraenkungen des Materials und Faehigkeiten des Spielers den
     Klarnamen des Materials heraus und gibt den zurueck.

RUECKGABEWERT
-------------
::

     string: Materialname oder genereller Name.

BEISPIELE
---------
::

     // der hier mag alle existierenden Juwelen, auch wenn welche ergaenzt
     // werden sollten
     // Parameter: 1. ein Juwel, 2. Casus, 3. 100% Erkennung - ob er sie
     // beim Empfang dann auch zu 100% erkennt, steht hier nicht!
     string* likeit;
     likeit=MATERIALDB->GetGroupMembers(MATGROUP_JEWEL);
     ...
     write("Der Alte sagt: Ich mag "+
	   MATERIALDB->MaterialName(likeit[random(sizeof(likeit))], WEN, 100)+
	   ".\n");
     ...

     // ein weiser schmied:
     int i;
     string *mat, mname, mgroup;
     mat=m_indices(ob->queryprop(p_material));
     i=sizeof(mat);

     write("der schmied sagt: "+ob->name(wer)+" besteht aus ...\n");
     while(i--) {
      // den namen erkennen/aussprechen:
      // materialien werden allgemein ganz schlecht erkannt (zu 5%), aber
      // alles aus metall wird zu +100% gut erkannt ...
      mname=materialdb->materialname(mat[i], wer,
	     ({5, ([material_symmetric_recognizability:
			({matgroup_metal, 100})])}));

      // und nur metalle analysieren ...
      if(materialdb->materialgroup(([mat[i]:100]),matgroup_metal)>=100) {
       int j;
       string *mgr;
       mgr=materialdb->getmatmembership(mat[i]);
       j=sizeof(mgr);
       mgroup=" gehoert zu ";
       while(j--) {
        mgroup+=materialdb->groupname(mgr[j]);
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
		  GroupName()
		  GetGroupMembers(), GetMatMembership()

7. Mai 2004 Gloinson

