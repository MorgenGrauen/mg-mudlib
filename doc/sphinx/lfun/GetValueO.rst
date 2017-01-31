GetValueO()
===========

FUNKTION
--------
::

	varargs int GetValueO(string vname, mapping map, object pl) 

ARGUMENTE
---------
::

	vname	: name des parameters aus dem spellmapping
	map   	: spellmapping
	pl 	: caster

BESCHREIBUNG
------------
::

	Berechnet den Wert und den Offset des Parameters in spellmapping.

RUECKGABEWERT
-------------
::

	Berechneter Wert+Offset aus dem Spellmapping.

BEMERKUNGEN
-----------
::

	Ruft GetValue(vname,map,pl)+GetOffset(vname,map,pl) auf.

BEISPIEL
--------
::

	xxx=GetValueO(SI_SKILLDAMAGE,sinfo,caster);

Siehe auch:

	"GetValue", "GetFactor", "GetOffset", "GetFValue", "GetFValueO"

	Ausfuehrliches Beispiel siehe "GetFValueO".

