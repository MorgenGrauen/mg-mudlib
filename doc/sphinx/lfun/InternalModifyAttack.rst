InternalModifyAttack()
======================

InternalModifyAttack(L)
-----------------------
::

FUNKTION
--------
::

     protected void InternalModifyAttack(mapping ainfo)

DEFINIERT IN
------------
::

     /std/living/combat

ARGUMENTE
---------
::

     mapping ainfo		Werte aus der Attack

BESCHREIBUNG
------------
::

     Dient dazu noch Aenderungen am Verhalten der Attack() vornehmen zu
     koennen. Die Parameter werden alle per Referenz uebergeben, Aenderungen
     wirken also direkt in der Attack()!
     Einfach ueberschreiben (aber ::InternalModifyAttack(&ainfo) nicht
     vergessen!

     Aufbau von 'ainfo':
     ([ SI_ENEMY :            object Angreifer,			(-> Defend)
        SI_SPELL :           0/1/array Spellparameter,		(-> Defend)
        P_WEAPON :           - oder Waffe,
        SI_SKILLDAMAGE_MSG:  string Angriffsmeldungsende an Raum,
        SI_SKILLDAMAGE_MSG2: string Angriffsmeldungsende an Kaempfende,
        SI_SKILLDAMAGE:      int Schaden in Zehntel HP (Skills schon drin)
								(-> Defend),
        SI_SKILLDAMAGE_TYPE: string/string* Schadenstypen,	(-> Defend)
        P_WEAPON_TYPE:       string Waffentyp (combat.h),
        P_WC:		     - oder int WC der Waffe/Hand,
        P_NR_HANDS:	     - oder int Hands der Waffe/Hand,
     ]);

SIEHE AUCH
----------
::

     InternalModifyDefend(L)
     Attack(L)

28.03.2008, Zesstra

