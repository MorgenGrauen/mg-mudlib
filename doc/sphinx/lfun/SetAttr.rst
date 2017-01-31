SetAttr()
=========

FUNKTION
--------
::

     public int SetAttr(string attr, int val)

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

     Filtert den Wert durch _filterattr(). Dadurch werden STR, INT, CON, DEX
     auf 20 begrenzt. Setzt dann das Attribut.
     Offsets werden hier nicht beruecksichtigt, QueryAttribute() gibt also
     val + etwaige Offsets/Modifier zurueck.

RUeCKGABEWERT
-------------
::

     Tatsaechlich gesetzter Wert.

BEMERKUNGEN
-----------
::

     Wird von SetAttribute() und SetRealAttribute() aufgerufen.

SIEHE AUCH
----------
::

     QueryAttribute(), QueryRealAttribute(), QueryAttributeOffset(),
     SetAttribute(), SetRealAttribute(), UpdateAttributes(),
     TestAttributeLock(),
     P_ATTRIBUTES, P_ATTRIBUTES_OFFSETS, P_ATTRIBUTES_MODIFIER,
     P_X_ATTR_MOD, P_M_ATTR_MOD, /std/living/attributes.c

Letzte Aenderung: 22.12.2016, Bugfix

