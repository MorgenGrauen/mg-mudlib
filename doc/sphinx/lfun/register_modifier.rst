register_modifier()
===================

FUNKTION
--------
::

     void register_modifier(object modifier)

DEFINIERT IN
------------
::

     /std/living/attributes.c

PARAMETER
---------
::

     modifier	- Objekt mit P_X_ATTR_MOD oder P_M_ATTR_MOD

BESCHREIBUNG
------------
::

     Registriert einen Modifier im Spieler. Wird durch InsertSensitiveObject
     beziehungsweise beim Anziehen oder Zuecken gerufen.
     Muss nicht direkt gerufen werden. Bei Veraenderungen von Modifikatoren
     sollte stattdessen UpdateAttributes gerufen werden.     

SIEHE AUCH
----------
::

     QueryAttribute(), QueryRealAttribute(), QueryAttributeOffset(),
     SetAttr(), SetAttribute(), SetRealAttribute(), UpdateAttributes(),
     deregister_modifier(), P_ATTRIBUTES, P_ATTRIBUTES_OFFSETS, 
     P_ATTRIBUTES_MODIFIER,P_X_ATTR_MOD, P_M_ATTR_MOD, 
     /std/living/attributes.c

13.Jun.2004, Muadib

