DeleteTimedAttrModifier()
=========================

FUNKTION
--------
::

     int DeleteTimedAttrModifier(string key)

DEFINIERT IN
------------
::

     /std/living/attributes.c

ARGUMENTE
---------
::

     key	-	aus P_TIMED_ATTR_MOD zu loeschender Eintrag

BESCHREIBUNG
------------
::

     Der zu key gehoerende Eintrag in P_TIMED_ATTR_MOD wird geloescht und
     update_max_sp_and_hp ausgefuehrt.

RUeCKGABEWERT
-------------
::

     TATTR_INVALID_ARGS      -     Im Falle eines fehlenden key-Arguments 
     TATTR_NO_SUCH_MODIFIER  -     Falls der Modifier mit diesem Key nicht
                                   existiert
     TATTR_OK                -     Im Erfolgsfall

     

     Die Rueckgabewerte sind in /sys/living/attributes.h definiert.

    

SIEHE AUCH
----------
::

	QueryAttribute(), QueryRealAttribute(), QueryAttributeOffset(),
	SetAttribute(), SetRealAttribute(), UpdateAttributes(),
	SetTimedAttrModifier(), QueryTimedAttrModifier(), 
	P_ATTRIBUTES, P_ATTRIBUTES_OFFSETS, P_TIMED_ATTR_MOD,
	P_X_ATTR_MOD, P_M_ATTR_MOD, /std/living/attributes.c

Last modified: Tue Jul 27 20:00:20 2004 by Muadib

