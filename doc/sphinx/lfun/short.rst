short()
=======

FUNKTION
--------

     public string short();

DEFINIERT IN
------------

     /std/thing/description.c

ARGUMENTE
---------

     keine

BESCHREIBUNG
------------

     Der Inhalt der Property P_SHORT wird ausgewertet, mit ".\\n"
     abgeschlossen und zurueckgegeben.

     Durch Ueberladen von short() lassen sich ggf. weitere Eigenschaften des
     Objektes zeigen. Fackeln zeigen zum Beispiel an, ob sie brennen,
     Ruestungen, ob sie angezogen sind und Waffen, ob sie gezueckt sind.

     In jedem Fall muss die Kurzbeschreibung aber *kurz* sein, empfohlen
     werden max. 60 Zeichen.

RUeCKGABEWERT
-------------

     Die Kurzbeschreibung als String oder 0, falls das Objekt nicht
     wahrnehmbar ist (d.h. nicht in Inventaren angezeigt wird).

BEMERKUNGEN
-----------

     Zu der mit dem Rueckgabewert von 0 vermittelten Unsichtbarkeit s.
     :doc:`../props/P_SHORT`.

SIEHE AUCH
----------

     Aehnliches
       :doc:`long`
     Properties
       :doc:`../props/P_SHORT`, :doc:`../props/P_INVIS`,
       :doc:`../props/P_INT_SHORT`

:Letzte Aenderung: 18.06.2020, Zesstra

