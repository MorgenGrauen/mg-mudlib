TestLimitViolation()
====================

FUNKTION
--------
::

      status TestLimitViolation(mapping check)

DEFINIERT IN
------------
::

     /std/living/attributes.c

PARAMETER
---------
::

     check	- Mapping mit Attributen: ([<attr>:<wert>])

BESCHREIBUNG
------------
::

     Prueft, ob die Summe der in check enthaltenen Modifikatoren die Summe
     aller Modifikatoren im Spieler ueber den zugelassenen Grenzwert hebt.

     

SIEHE AUCH
----------
::

     QueryAttribute(), QueryRealAttribute(), QueryAttributeOffset(),
     SetAttr(), SetAttribute(), SetRealAttribute(), UpdateAttributes(),
     P_ATTRIBUTES, P_ATTRIBUTES_OFFSETS, P_ATTRIBUTES_MODIFIER,
     P_X_ATTR_MOD, P_M_ATTR_MOD, /std/living/attributes.c

13.Jun.2004, Muadib

