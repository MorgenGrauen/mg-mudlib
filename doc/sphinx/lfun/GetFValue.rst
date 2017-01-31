GetFValue()
===========

FUNKTION
--------
::

	varargs int GetFValue(string vname, mapping map, object pl) 

ARGUMENTE
---------
::

	vname	: name des parameters aus dem spellmapping
	map   	: spellmapping
	pl 	: caster

BESCHREIBUNG
------------
::

	Berechnet den Wert und den Factor des Parameters in spellmapping.

RUECKGABEWERT
-------------
::

	Berechneter Wert*Factor aus dem Spellmapping.

BEMERKUNGEN
-----------
::

	Ruft GetValue(vname,map,pl)*GetFactor(vname,map,pl)/100 auf.

BEISPIEL
--------
::

	xxx=GetFValue(SI_SKILLDAMAGE,sinfo,caster);

Siehe auch:

	"GetValue", "GetFactor", "GetOffset", "GetValueO", "GetFValueO"

	Ausfuehrliches Beispiel siehe "GetFValueO".

