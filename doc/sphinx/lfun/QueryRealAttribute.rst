QueryRealAttribute()
====================

FUNKTION
--------
::

     int QueryRealAttribute(string attr)

DEFINIERT IN
------------
::

     /std/living/attributes.c

ARGUMENTE
---------
::

     attr       -       das interessierende Attribut

BESCHREIBUNG
------------
::

     Das reale Attribut (ohne Offsets) wird zurueckgegeben.

BEISPIELE
---------
::

     if(this_player()->QueryRealAttribute(A_INT)>15)
      write("Du bist auch so ganz schoen clever.\n");

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

