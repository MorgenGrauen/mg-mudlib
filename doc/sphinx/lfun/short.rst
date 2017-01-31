short()
=======

FUNKTION
--------
::

     public varargs string short();

DEFINIERT IN
------------
::

     /std/thing/description.c

ARGUMENTE
---------
::

     keine

BESCHREIBUNG
------------
::

     Der Inhalt der Property P_SHORT wird ausgewertet, mit ".\n"
     abgeschlossen und zurueckgegeben.

RUeCKGABEWERT
-------------
::

     Die Kurzbeschreibung als String oder 0, falls das Objekt unsichtbar
     ist.

BEMERKUNGEN
-----------
::

     Durch Ueberladen von short() lassen sich noch weitere Eigenschaften des
     Objektes zeigen. Fackeln zeigen zum Beispiel an, ob sie brennen,
     Ruestungen, ob sie angezogen sind und Waffen, ob sie gezueckt sind.

SIEHE AUCH
----------
::

     Aehnliches:	long()
     Properties:	P_SHORT, P_INVIS
     Sonstiges:		make_invlist()


20.01.2015, Zesstra

