SetAttackChats()
================

FUNKTION
--------
::

     void SetAttackChats(int chance, mixed strs);

DEFINIERT IN
------------
::

     /std/npc/chat.c

ARGUMENTE
---------
::

     chance
          Prozentuale Wahrscheinlichkeit einer Ausgabe
     strs
          Stringarray mit den Monsterchats

BESCHREIBUNG
------------
::

     Der NPC gibt mit der Wahrscheinlichkeit 'chance' einen zufaellig
     gewaehlten Text aus 'strs' von sich. Die Arrayelemente koennen 
     auch Funktionen ("@@func@@") oder Closures enthalten, die dann
     aufgerufen werden und deren Rueckgabewerte das Monster dann ausgibt.

RUECKGABEWERT
-------------
::

     keiner

     

BEMERKUNGEN
-----------
::

     AttackChats werden nur im Kampf ausgegeben. Ansonsten ist SetChats()
     zu verwenden.

     

     'chance' wird in der Property P_ACHAT_CHANCE abgelegt. Um einen NPC
     voruebergehend 'stillzulegen', kann man P_ACHAT_CHANCE auf 0 setzen.

     

     Man kann AttackChats nutzen, um Monsterspells zu realisieren. Besser
     ist es aber, dafuer 'AddSpell' zu verwenden.

     

BEISPIELE
---------
::

     SetAttackChats( 20,
       ({ "Der Ork tritt Dir in den Hintern.\n",
          "Der Ork bruellt: Lebend kommst Du hier nicht raus!\n",
          "Der Ork blutet schon aus mehreren Wunden.\n",
          "@@knirsch@@" }) );

          

     string knirsch()
     {
       object *ruestung, *tmp, helm;

       

       ruestung = this_player()->QueryProp(P_ARMOURS); // getragene Ruestung
       tmp = filter_objects(ruestung, "id", AT_HELMET);
       // Wenn der Spieler einen Helm traegt, enthaelt 'tmp' jetzt
       // ein Array mit dem Helmobjekt als einzigem Element
       if (sizeof(tmp)) helm = tmp[0];
	 if (objectp(helm))
	 {
	   // AC nur dann senken, wenn sie nicht schon 0 ist.
	   if (helm->QueryProp(P_AC)>0) {
	     helm->Damage(1);
	     return "Der Ork beschaedigt Deinen Helm!\n";
	   }
	   return "";
	 }
	 else
	   return ""; // keine Meldung
     }

SIEHE AUCH
----------
::

     P_ACHAT_CHANCE, SetChats(), /std/npc/chat.c, AddSpell()

LETZTE AENDERUNG
----------------
::

	Don, 27.12.2007, 16:35:00 von Arathorn

