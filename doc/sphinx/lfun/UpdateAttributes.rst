UpdateAttributes()
==================

FUNKTION
--------
::

     void UpdateAttributes()

DEFINIERT IN
------------
::

     /std/living/attributes.c

BESCHREIBUNG
------------
::

     Rechnet damit alle Attributmodifier der im Inventory befindlichen 
     (P_X_ATTR_MOD, P_X_HEALTH_MOD) und getragenen/gezueckten 
     (P_M_HEALTH_MOD, P_M_ATTR_MOD) Objekte und aller Attributoffsets 
     zusammen und speichert sie in einer intern fuer Attribute 
     verwendete Variablen.
     Berechnet darauf basierend HP und SP neu.

     

     Die Bedingungen fuer die ueber P_TIMED_ATTR_MOD gesetzten 
     Attributveraenderungen werden im Heartbeat in der Funktion
     attribute_hb ueberprueft.

BEMERKUNGEN
-----------
::

     Sollte nach Einbringen neuer Modifikatorobjekte am Living gerufen
     werden.

SIEHE AUCH
----------
::

	QueryAttribute(), QueryRealAttribute(), QueryAttributeOffset(),
	SetAttribute(), SetRealAttribute(), UpdateAttributes(),
	SetTimedAttrModifier(), QueryTimedAttrModifier(), 
	DeleteTimedAttrModifier(),
	P_ATTRIBUTES, P_ATTRIBUTES_OFFSETS, P_TIMED_ATTR_MOD,
	P_X_ATTR_MOD, P_M_ATTR_MOD, /std/living/attributes.c

09.05.2007 by Zesstra

