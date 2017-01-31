NotifyDestruct()
================

FUNKTION
--------
::

     public int|string NotifyRemove(object zerstoerer);

ARGUMENTE
---------
::

     zerstoerer
          Das Objekt, welches destruct() auf dieses Objekt anwendet.

BESCHREIBUNG
------------
::

     Diese Funktion wird vom Master der Mudlib in jedem Objekt gerufen,
     welches zerstoert wird, um Objekten eine letzte Chance zu geben, sich
     aufzuraeumen.
     Wenn ihr diese Funktion selber definiert, achtet bittet darauf, dass sie
     in keinem Fall Fehler ausloesen sollte, d.h. testet entsprechend
     gruendlich.
     Wenn ihr aus /std/ erbt und diese Funktion ueberschreibt,, _muesst_ ihr
     die geerbte NotifyDestruct() aufrufen, da sonst das Lichtsystem nicht
     aktualisiert wird.

     Privilegierte Objekte (z.B. Root-Objekte, spezielle Ausnahmen wie der
     Netztotenraum, Armageddon) koennen die Zerstoerung in letzter Sekunde
     verhindern, indem sie hier einen Wert != 0 zurueckliefern. Wird ein
     string zurueckgeliefert, wird dieser die Fehlermeldung des
     prepare_destruct() im Master sein.
     Bei nicht-privilegierten Objekten (also fast alle) ist an dieser Stelle
     _kein_ Abbruch der Zerstoerung moeglich!

RUeCKGABEWERT
-------------
::

     Der Rueckgabewert muss ein string oder ein int sein. Allerdings wird der
     Master den Rueckgabewert nur fuer privilegierte Objekte auswerten.

BEMERKUNGEN
-----------
::

     Wie gesagt, bitte keine Fehler ausloesen (auch wenn der Master
     grundsaetzlich damit klar kommt). Speziell ist ein raise_error() zur
     Verhinderung eines destructs nutzlos.
     Bitte macht in dieser Funkion nur das, was _unbedingt_ notwendig ist.
     Wenn jemand ein destruct() anwendet statt ein remove() zu rufen, hat das
     in der Regel einen Grund (z.B. um buggende remove() zu umgehen). Insb.
     ist ein save_object() in remove() und NotifyDestruct() vollstaendig
     ueberfluessig.

SIEHE AUCH
----------
::

    NotifyLeave(), NotifyInsert(), NotifyRemove()


Last modified: 25.02.2009, Zesstra

