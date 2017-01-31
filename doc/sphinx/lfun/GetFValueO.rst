GetFValueO()
============

FUNKTION
--------
::

	varargs int GetFValueO(string vname, mapping map, object pl) 

ARGUMENTE
---------
::

	vname	: name des parameters aus dem spellmapping
	map   	: spellmapping
	pl 	: caster

BESCHREIBUNG
------------
::

	'Berechnet' den Wert, den Factor und den Offset des Parameters 
	in spellmapping.

RUECKGABEWERT
-------------
::

	Berechneter (Wert*Factor)/100+Offset aus dem Spellmapping.

BEMERKUNGEN
-----------
::

	Ruft (GetValue(vname,map,pl)*GetFactor(vname,map,pl))/100+
	GetOffset(vname,map,pl) auf.

BEISPIEL
--------
::

	AddSpell("egal",10,
	([
	OFFSET(SI_COST):([SM_RACE:(["Zwerg":4]) ]),
	FACTOR(SI_COST):([SM_RACE:(["Mensch":90]) ]),
	SI_SKILLDAMAGE:100,
	OFFSET(SI_SKILLDAMAGE):25,
	SI_SKILLDAMAGE_TYPE:DT_EXAMPLE,
	FACTOR(SI_SKILLDAMAGE):([SM_RACE:(["Zwerg":80,"Elf":120]) ]) 
	]));

	So, was sollen uns diese Zeilen sagen?

	Es wird ein Spruch Names 'egal' ins Spellbook eingetragen. Er kostet
	regulaer 10 MP. Fuer Zwerge allerdings wird ein Offset von 4 MP 
	aufgeschlagen. Ausserdem machen Zwerge nur 80% Schaden, Elfen 
	hingegen 120%. Der Grundschaden betraegt 100 Schadenspunkte, der 
	Offset des Schadens nochmal 25. Menschen bezahlen fuer diesen 
	Spruch nur 90% der Kosten.

	Nun die Rechenbeispiele:

	Fuer die Kosten:
		Value	ValueO	FValue	FValueO
	Mensch	   10	    10	     9	      9
	Elf   	   10	    10	    10       10
	Hobbit	   10	    10	    10	     10
	Zwerg	   10	    14	    10	     14

	Fuer den Schaden:
			Value	ValueO	FValue	FValueO
	Mensch		100	125	100	125
	Elf  		100	125	120	150
	Hobbit		100	125	100	125
	Zwerg		100	125	 80	100

	An diesem Beispiel sieht man deutlich, wie man mit ein paar 
	Offsets und Faktoren die Wirkung eines Spruches deutlich 
	veraendern kann. Es sollte bei eigenen Berechnungen immer 
	GetFValueO benutzt werden.

Siehe auch:

	"GetValue", "GetFactor", "GetOffset", "GetFValue", "GetValueO"

