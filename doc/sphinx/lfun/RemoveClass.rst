RemoveClass()
=============

FUNKTION
--------
::

     void RemoveClass(string|string* class);

DEFINIERT IN
------------
::

     /std/thing/description.c

ARGUMENTE
---------
::

     string/string* class	- String oder Stringarray der Klasse(n)

BESCHREIBUNG
------------
::

     Dem Objekt werden einige der mit AddClass() gesetzten Klassen wieder 
     entzogen.
     Die allgemein verfuegbaren Klassen sind unter /sys/class.h definiert.

SIEHE AUCH
----------
::

     AddClass(), is_class_member()
     P_CLASS


20.01.2015, Zesstra

