P_HELPER_OBJECTS
================

NAME
----
::

     P_HELPER_OBJECTS "lib_p_helper_objects"

DEFINIERT IN
------------
::

     <living/helpers.h>

BESCHREIBUNG
------------
::

     Diese Property enthaelt eine Liste von Hilfsobjekten, die das Lebewesen,
     in dem sie gesetzt ist, bei bestimmten Aktivitaeten wie Tauchen oder
     Fliegen/Segeln unterstuetzt. Die Property enthaelt ein Mapping der Form
     ([ HELPER_TYPE : ({ Callback-Closures }) ]).

BEMERKUNGEN
-----------
::

     Externe Manipulationen an dieser Property bitte unterlassen, zum Ein-
     und Austragen von Objekten gibt es die unten aufgefuehrten Methoden.

SIEHE AUCH
----------
::

     Methoden:    RegisterHelperObject(L), UnregisterHelperObject(L)
     Properties:  P_AERIAL_HELPERS, P_AQUATIC_HELPERS


18.02.2013, Arathorn

