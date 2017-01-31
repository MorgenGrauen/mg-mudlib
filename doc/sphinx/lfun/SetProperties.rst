SetProperties()
===============

FUNKTION
--------
::

     void SetProperties(mapping props);

DEFINIERT IN
------------
::

     /std/thing/properties.c

ARGUMENTE
---------
::

     props	- Mapping mit den Daten fuer alle Properties

BESCHREIBUNG
------------
::

     Diese Funktion setzt angegebene Properties auf einen Schlag.
     Mapping muss wie folgt aufgebaut sein:
	([ name: wert; flags; set_method; query_method,
	   name2: ... ]);

BEMERKUNGEN
-----------
::

     - diese Funktion wird von restore_object() verwendet, um nicht alle
       restaurierten Properties einzeln setzen zu muessen.
     - SECURED/PROTECTED/NOSETMETHOD Properties werden beruecksichtigt

SIEHE AUCH
----------
::

     Aehnliches:	SetProp(L), QueryProp(L), Set(L), Query(L)
     Generell:		QueryProperties(L)
     Konzept:		properties, /std/thing/properties.c

1.Mai 2004 Gloinson

