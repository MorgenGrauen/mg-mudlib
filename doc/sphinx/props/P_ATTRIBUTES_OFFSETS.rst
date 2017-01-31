P_ATTRIBUTES_OFFSETS
====================

NAME
----
::

	P_ATTRIBUTES_OFFSETS		"attributes_offsets"

DEFINIERT IN
------------
::

	/sys/living/attributes.h

BESCHREIBUNG
------------
::

	Diese Property enthaelt ein Mapping mit Offsets, die zu den
	Attributen eine Lebewesens addiert werden. Diese koennen auch
	negativ sein! Realisiert werden damit beispielsweise Rassenboni.
	Es gibt noch weitere Moeglichkeiten, Attributoffsets anzugeben.
	Fuer weiteres siehe P_ATTRIBUTES.

BEMKERUNGEN
-----------
::

        Keine echte Property, _query_attributes_offsets() und 
        _set_attributes_offsets() sind in /std/living/attributes.c 
        implementiert.

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

