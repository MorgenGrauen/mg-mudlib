TestAttributeLock()
===================

********************* OBSOLETE LFUN ***********************************
TestAttributeLock()

FUNKTION
--------
::

     string TestAttributeLock(mapping check)

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

     Prueft, ob eines der im Mapping enthaltenen Attribute blockiert
     ist (bereits durch einen anderen Modifier belegt wurde).

     

     Da Modifier nicht mehr direkt blockieren ist diese Funktion obsolet
     und in Livings inzwischen nicht mehr existent.

SIEHE AUCH
----------
::

     QueryAttribute(), QueryRealAttribute(), QueryAttributeOffset(),
     SetAttr(), SetAttribute(), SetRealAttribute(), UpdateAttributes(),
     P_ATTRIBUTES, P_ATTRIBUTES_OFFSETS, P_ATTRIBUTES_MODIFIER,
     P_X_ATTR_MOD, P_M_ATTR_MOD, /std/living/attributes.c

11.05.2007, Zesstra

