GetValue()
==========

FUNKTION
--------
::

	varargs int GetValue(string vname, mapping map, object pl) 

ARGUMENTE
---------
::

	vname	: name des parameters aus dem spellmapping
	map   	: spellmapping
	pl 	: caster

BESCHREIBUNG
------------
::

	'Berechnet' den Wert des uebergebenen Parameters aus dem 
	Spellmapping.

RUECKGABEWERT
-------------
::

	Berechneter Wert aus dem Spellmapping.

BEMERKUNGEN
-----------
::

	

BEISPIEL
--------
::

	xxx=GetValue(SI_SKILLDAMAGE,sinfo,caster);

Siehe auch:

	"GetFactor", "GetOffset", "GetFValue", "GetValueO", "GetFValueO"

	Ausfuehrliches Beispiel siehe "GetFValueO".

