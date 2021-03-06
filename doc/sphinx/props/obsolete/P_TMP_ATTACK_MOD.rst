P_TMP_ATTACK_MOD
================

********************* VERALTETE PROPERTY ******************************
* Diese Property ist veraltet. Bitte nicht mehr in neuem Code nutzen. *
***********************************************************************
P_TMP_ATTACK_MOD

NAME
----
::

     P_TMP_ATTACK_MOD              "attack_mod"

DEFINIERT IN
------------
::

     /sys/new_skills.h

BESCHREIBUNG
------------
::

     Mittels dieser Property koennen die Attacken eines Livings temporaer
     veraendert werden.

     Es wird an dem Living die Property als mindestens 3-elementiges Array
     ({zeitpunkt, objekt, methode, ...})
     gesetzt und die Methode 'methode' wird dann waehrend des Attack() des
     Lebewesens in 'objekt' aufgerufen, solange time()<'zeitpunkt'.

     Der Methode wird beim Aufruf ein Kopie des Mappings uebergeben, in dem
     die bisherigen Werte des Angriffes vermerkt sind.
     Aufbau von Mapping 'ainfo':
     ([ SI_ENEMY :           object Angreifer,			(-> Defend)
        SI_SPELL :           0/1/array Spellparameter,		(-> Defend)
        P_WEAPON :           - oder Waffe,
        SI_SKILLDAMAGE_MSG:  string Angriffsmeldungsende an Raum,
        SI_SKILLDAMAGE_MSG2: string Angriffsmeldungsende an Kaempfende,
        SI_SKILLDAMAGE:      int Schaden in Zehntel HP (Skills bis auf Rasse
			     drin!)				(-> Defend),
        SI_SKILLDAMAGE_TYPE: string/string* Schadenstypen,	(-> Defend)
        P_WEAPON_TYPE:       string Waffentyp (combat.h),
        P_WC:		     - oder int WC der Waffe/Hand,
        P_NR_HANDS:	     - oder int Hands der Waffe/Hand,
     ]);

     Gibt die Methode:
     - 0 oder kein Mapping zurueck, fuehrt das zum Abbruch der Attacke
       -> da inzwischen Waffen abgefragt wurden, koennen schon Meldungen
          von diesen ausgegeben worden sein
     - ein leeres Mapping ( ([]) ) zurueck, fuehrt das zu keiner Modifikation
     - ein Mapping mit veraenderten Werten zurueck, werden diese in das
       Angriffsmapping kopiert
       Die geaenderten Werte werden teilweise von SkillResTransfer() in
       den eigentlichen Angriff uebernommen.

BEMERKUNGEN
-----------
::

     - falls die Zeit abgelaufen oder das Objekt zerstoert ist, wird die
       Property auf 0 gesetzt
     - vor dem Setzen immer auf die Existenz eines gueltigen Modifiers
       pruefen, einfaches Ueberschreiben fuehrt einerseits zu Fehlern
       im Code anderer und ist andererseits unfair gegenueber ihnen

BEISPIELE
---------
::

     *** ein besonder heisser Segen modifiziert die Attacken des Spielers ***
     int action_segen() {
       ...
       mixed *tmp;

       // pruefen, ob nicht ein anderer Modifier existiert
       if(!pointerp(tmp=TP->QueryProp(P_TMP_ATTACK_MOD)) ||
          sizeof(tmp)<3 || tmp[0]<=time()) {

         object wield;
         wield=TP->QueryProp(P_WEAPON);

         write(break_string(
           "Der Priester der Flamme weiht "+
           (wield?wield->name(WEN,1):"deine Haende")+".", 78));

         // setzen des Modifiers .. 30-40 Sekunden gueltig
         TP->SetProp(P_TMP_ATTACK_MOD,
	              ({time()+30+random(10), this_object(), "modfun"}));
        } else ...
        ...
      return 1;
     }

     // die eigentlich Methode, die waehrend des Angriffs gerufen wird
     mapping modfun(mapping ainfo) {
       mapping ret;

       // Returnwert ist immer ein Mapping, damit die Attacke weitergeht
       ret=m_allocate(0,1);

       // magische Waffen oder Sprueche werden nicht verbessert
       if(ainfo[P_WEAPON_TYPE]!=WT_MAGIC) {
         // sonst Verbesserungen ... Feuer addieren ...
         ret[SI_SKILLDAMAGE_TYPE]=(ainfo[SI_SKILLDAMAGE_TYPE]||({}))+
				({DT_FIRE});
	 // ... und bei Waffe Meldungen anpassen
         if(ainfo[P_WEAPON]) {
           ret[SI_SKILLDAMAGE_MSG]=
             " mit sengendem "+ainfo[P_WEAPON]->name(RAW);
           ret[SI_SKILLDAMAGE_MSG2]=
             " mit sengendem "+ainfo[P_WEAPON]->name(RAW);
         }
       }

       return ret;
     }

SIEHE AUCH
----------
::

     Angriff:	Attack(L)
     Schutz:    Defend(L)
     Verwandt:  InternalModifyAttack(L)
		P_TMP_ATTACK_HOOK
		P_TMP_DEFEND_HOOK
     Sonstiges: SkillResTransfer(L)
     Hooks:	P_TMP_DIE_HOOK, P_TMP_MOVE_HOOK

10.Feb 2005 Gloinson

