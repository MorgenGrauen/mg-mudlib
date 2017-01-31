QueryAttribute()
================

FUNKTION
--------
::

     int QueryAttribute(string attr)

DEFINIERT IN
------------
::

     /std/living/attributes.c

ARGUMENTE
---------
::

     attr       - interessierendes Gesamtattribut

BESCHREIBUNG
------------
::

     Das Attribut samt seiner Offsets (Modifier) wird zurueckgegeben.

RUeCKGABEWERT
-------------
::

     Wert des Attributes, bei Spielern nicht groesser als 30.

BEISPIELE
---------
::

     if (this_player()->QueryAttribute(A_CON) > 20)
       write("Du schaffst es den Berg hoch. Aber nur muehsam.\n");

BENERKUNGEN
-----------
::

     Wenn es um Attributabfragen geht, bitte diese Methode verwenden!

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

