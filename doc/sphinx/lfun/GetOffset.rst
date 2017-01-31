GetOffset()
===========

FUNKTION
--------
::

	varargs int GetOffset(string vname, mapping map, object pl) 

ARGUMENTE
---------
::

	vname	: name des parameters aus dem spellmapping
	map   	: spellmapping
	pl 	: caster

BESCHREIBUNG
------------
::

	'Berechnet' den Offset des Parameters in spellmapping.

RUECKGABEWERT
-------------
::

	Berechneter Offset aus dem Spellmapping.

BEMERKUNGEN
-----------
::

	Beschraekung auf -10000 bis +10000. Werte groesser oder kleiner 
	als diese werden 'abgeschnitten'. Mehr als 100% Bonus oder Malus
	gibts nicht ;-).

BEISPIEL
--------
::

	xxx=GetOffset(SI_SKILLDAMAGE,sinfo,caster);

Siehe auch:

	"GetValue", "GetFactor", "GetFValue", "GetValueO", "GetFValueO"

	Ausfuehrliches Beispiel siehe "GetFValueO".

