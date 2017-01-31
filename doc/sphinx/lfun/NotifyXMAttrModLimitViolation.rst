NotifyXMAttrModLimitViolation()
===============================

FUNKTION
--------
::

      void NotifyXMAttrModLimitViolation()

DEFINIERT IN
------------
::

     /std/thing/restrictions.c

BESCHREIBUNG
------------
::

     Wird gerufen wenn die Summe der in P_X_ATTR_MOD oder P_X_ATTR_MOD
     angegebenen Modifikatoren die Summe aller Modifikatoren im Spieler 
     ueber den zugelassenen Grenzwert hebt und somit nicht mehr in die
     Berechnung eingeht.

     

SIEHE AUCH
----------
::

     QueryAttribute(), QueryRealAttribute(), QueryAttributeOffset(),
     SetAttr(), SetAttribute(), SetRealAttribute(), UpdateAttributes(),
     P_ATTRIBUTES, P_ATTRIBUTES_OFFSETS, P_ATTRIBUTES_MODIFIER,
     P_X_ATTR_MOD, P_M_ATTR_MOD, /std/living/attributes.c

13.Jun.2004, Muadib

