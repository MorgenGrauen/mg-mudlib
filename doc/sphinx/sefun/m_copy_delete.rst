m_copy_delete()
===============

FUNKTION
--------
::

	mapping m_copy_delete(mapping map, mixed key)

ARGUMENTE
---------
::

	map - das Mapping, aus dem geloescht werden soll.
	key - der zu loeschende Eintrag

BESCHREIBUNG
------------
::

	Aus dem Mapping map wird der Eintrag key geloescht (wenn er in map vor-
	handen ist). map wird dabei nicht veraendert.

RUECKGABEWERT
-------------
::

	Eine (flache !) Kopie von map ohne den Eintrag key, d.h. enthaltene
	Mappings/Arrays werden nicht kopiert.

BEMERKUNGEN
-----------
::

	Das urspruengliche Mapping wird bei dieser Operation nicht veraendert!
	Wenn man nur einen Wert aus dem Mapping loeschen will und die Kopie nicht
	braucht, bietet sich efun::m_delete(mapping,key) sehr an, da die Erstellung
  einer Kopie sehr aufwendig sein kann.

BEISPIELE
---------
::

	mapping m1, m2;

        m1 = ([ "a":1, "b":2, "c":3 ]);

        m2 = m_copy_delete(m1, "b");
           => m1 = ([ "a":1, "b":2, "c":3 ])
	      m2 = ([ "a":1, "c":3 ])

        m_copy_delete(m1, "a");
           => (es hat sich nichts geaendert)

        m1 = m_copy_delete(m1, "a");
           => m1 = ([ "b":2, "c":3 ])

        Im letzten Fall sollte aber besser efun::m_delete(m1, "a") benutzt 
        werden, da ansonsten das Mapping unnoetig kopiert wird und Rechen-
        leistung frisst. 

SIEHE AUCH
----------
::

  efun::m_delete(E), mappingp(E), mkmapping(E), m_indices,(E) m_values(E),
  sizeof(E), widthof(E)

