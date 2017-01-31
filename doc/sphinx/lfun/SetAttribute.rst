SetAttribute()
==============

FUNKTION
--------
::

     int SetAttribute(string attr, int val)

DEFINIERT IN
------------
::

     /std/living/attributes.c

ARGUMENTE
---------
::

     attr       - zu setzendes Attribut
     val        - Wert

BESCHREIBUNG
------------
::

     Nimm val als Gesamtwert des Attributes, d.h. zieht etwaige Offsets vor
     Setzen ab. (QueryAttribute gibt dann also val zurueck)

RUeCKGABEWERT
-------------
::

     Den durch SetAttr gefilterten gesetzten Wert.
     Bitte nicht auf Spieler anwenden!

SIEHE AUCH
----------
::

	QueryAttribute(), QueryRealAttribute(), QueryAttributeOffset(),
	SetAttribute(), SetRealAttribute(), UpdateAttributes(),
	SetTimedAttrModifier(), QueryTimedAttrModifier(), 
	DeleteTimedAttrModifier(),
	P_ATTRIBUTES, P_ATTRIBUTES_OFFSETS, P_TIMED_ATTR_MOD,
	P_X_ATTR_MOD, P_M_ATTR_MOD, /std/living/attributes.c

Last modified: Tue Jul 27 20:00:20 2004 by Muadib

