QueryProp()
===========

FUNKTION
--------
::

     mixed QueryProp(string name);

DEFINIERT IN
------------
::

     /std/thing/properties.c

ARGUMENTE
---------
::

     string name	- abzufragende Property

BESCHREIBUNG
------------
::

     Der Datenwert der Property 'name' wird zurueckgegeben.

     Existiert eine F_QUERY_METHOD oder eine _query_'name'()-Methode fuer
     diese Property, so wird diese aufgerufen und ihr 'Value' uebergeben.
     Eine F_QUERY_METHOD hat dabei Vorrang vor _query_'name'(), d.h.
     _query_'name'() wird nach erfolgreicher F_QUERY_METHOD nicht mehr
     gerufen.

     (Diese Methoden nutzen dann Set(), um auf den Datenwert der Property
      'name' zurueckzugreifen. Teilweise werden aber auch interne Variablen
      so oeffentlich gemacht und sind nicht in der ueber Set/Query
      verfuegbaren Property 'name' abgelegt.)

RUeCKGABEWERT
-------------
::

     Der Datenwert der Property.
     0, falls diese nicht existiert.

BEISPIELE
---------
::

     // wie hoch sind die aktuelle LP des Spielers?
     hp = this_player()->QueryProp(P_HP);

SIEHE AUCH
----------
::

     Aehnliches:	SetProp(L), Set(L), Query(L)
     Generell:		SetProperties(L), QueryProperties(L)
     Konzept:		properties, /std/thing/properties.c
     Sonstiges:		P_AUTOLOADOBJ

15.Dez 2004 Gloinson

