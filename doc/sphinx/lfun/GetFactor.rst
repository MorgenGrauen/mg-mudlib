GetFactor()
===========

FUNKTION
--------
::

	varargs int GetFactor(string vname, mapping map, object pl) 

ARGUMENTE
---------
::

	vname	: name des parameters aus dem spellmapping
	map   	: spellmapping
	pl 	: caster

BESCHREIBUNG
------------
::

	'Berechnet' den Factor des Parameters in spellmapping.

RUECKGABEWERT
-------------
::

	Berechneter Factor aus dem Spellmapping.

BEMERKUNGEN
-----------
::

	Beschraekung auf 10-1000. Werte groesser oder kleiner als diese
	werden 'abgeschnitten'.

BEISPIEL
--------
::

	xxx=GetFactor(SI_SKILLDAMAGE,sinfo,caster);

Siehe auch:

	"GetValue", "GetOffset", "GetFValue", "GetValueO", "GetFValueO"

	Ausfuehrliches Beispiel siehe "GetFValueO".

