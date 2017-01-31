P_PARA
======

NAME
----
::

    P_PARA                        "para"                        

DEFINIERT IN
------------
::

    /sys/properties.h

BESCHREIBUNG
------------
::

    Nummer der Parallelwelt, in der sich ein Spieler befindet.

    Ist die Property P_PARA auf Null gesetzt, so befindet sich der Spieler in
    der 'Normalwelt'. Gibt es bei einer Bewegung dieses Spielers mehrere
    moegliche Zielraeume mit identischem Namen aber unterschiedlichen Endungen
    'name.c', 'name^1.c', 'name^2.c' etc., so wird der Spieler in den Raum
    'name.c' bewegt. 

    Wird die Property P_PARA auf einen Wert n>0 gesetzt, so landet der Spieler
    bei einer Bewegung im Raum 'name^n.c'. Ist kein Raum mit entsprechender
    Endung vorhanden, wird der Spieler stattdessen in den Normalweltraum
    bewegt.

    Diese Prop kann auch in einem Virtual Compiler gesetzt werden. In diesem
    Fall schraenkt sie die Dimensionen ein, in denen der VC Objekte erzeugt.
    Die Prop kann eine einzelne Ziffer (Int) oder ein Array von Ints 
    aufnehmen, dann ist der VC fuer alle angegeben Dimensionen zustaendig. 
    Ein leeres Array erlaubt gar keine Para-Objekte.

ANMERKUNG
---------
::

    Die Endung '^0' kennzeichnet _nicht_ die Normalwelt. So lange kein Ausgang
    explizit auf den Raum 'name^0.c' verweist, wird kein Spieler den Raum
    betreten koennen. Deshalb kann man die Endung '^0' z.B. dazu benutzen, um
    eigene Standardraeume fuer ein Gebiet zu schreiben, die dann sowohl von
    den Normal- als auch von den Parallelweltraeumen inheritet werden.

    Raeume mit Endungen '^n.c', bei denen 'n' keine positive ganze Zahl ist,
    werden nicht beachtet.

    Fuer die Entscheidung, in welchem Raum ein Spieler in Abhaengigkeit von
    P_PARA landet, ist die Funktion move() zustaendig. Als Magier muss man sich
    darum nicht gesondert kuemmern. Das heisst aber auch, dass beim Anschluss
    eines Normalweltraumes automatisch alle in dem Verzeichnis mit gleichem
    Namen vorhandenen Parallelweltraeume mit angeschlossen werden.

    Sollen einzelne Parallelweltraeume noch nicht angeschlossen werden, so muss
    in ihnen die Property P_NO_PLAYERS gesetzt werden. Diese Raeume sind dann
    nur durch Magier und Testspieler zu betreten (und zu testen).

    In Paraweltraeumen liefert P_PARA 'n' zurueck.
    Man kann also z.B. in NPCs einfach ueber environment()->QueryProp(P_PARA) 
    abfragen, in welcher Parawelt sich dieser gerade befindet.

SIEHE AUCH
----------
::

    P_NO_PLAYERS, move, pararaeume

    

25.Jan 2015 Gloinson

