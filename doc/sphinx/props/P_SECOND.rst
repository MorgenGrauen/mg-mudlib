P_SECOND
========

NAME
----

    P_SECOND "second"

DEFINIERT IN
------------

    /sys/player/base.h

BESCHREIBUNG
------------

     Wenn diese Prop gesetzt ist, ist der Spieler ein Zweitie. Inhalt der
     Prop ist ein String mit dem (lowercase) Namen des Ersties.
     Ob anderen Spielern angezeigt wird, dass jemand Zweitie ist, kann mittels
     der Property P_SECOND_MARK gesteuert werden.

     Bitte auch das Konzept der Familie beachten (s. :doc:`../wiz/familien`)
     und speziell bei Aenderung von P_SECOND ggf. die Familie eintragen
     lassen.

     Soll ein Spieler kein Zweitie mehr sein, bitte zusaetzlich zum Loeschen
     dieser Property den Zweitieeintrage auch in /secure/zweities loeschen
     (lassen).

BEISPIEL
--------

.. code-block:: pike

     if (this_player()->QueryProp(P_SECOND)=="notstrom") {
       tell_object(this_player(), "Nicht alles aufessen!\n");
     }

SIEHE AUCH
----------

     Familien:
       :doc:`../wiz/familien`
     Kommandos:
       :doc:`../pcmd/zweitiemarkierung`
     Properties:
       :doc:`P_SECOND_MARK`
     Sonstiges:
       /secure/zweities.c

Last modified: 08.01.2019, Zesstra

