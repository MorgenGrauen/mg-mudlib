md5()
=====

DEPRECATED
----------
::

SYNOPSIS
--------
::

        string md5 (string arg [ , int iterations ] )
        string md5 (int *  arg [ , int iterations ] )

BESCHREIBUNG
------------
::

        Berechnet den MD5-Hashwert von <arg>.
        Das Argument kann ein String, oder ein Array von Zahlen sein (von
        welchen nur das unterste Byte betrachted wird).

        Das Ergebnis wird als 32-stelliger Hexadezimalwert geliefert.

        Ist das <iterations> Argument eine Zahl groesser 0, berechnet der
        Driver den Digest mit diese Anzahl an Wiederholungen. Fehlt die
        Angabe, fuehrt der Driver die Digest-Berechnung einmal aus.

BEISPIEL
--------
::

        string s;

        s = md5("Hallo");
        s = md5( ({ 'H', 'e', 'l', 'l', 'o' }) )
        s = md5( ({ 'H', 'e', 'l', 'l', 'o' }), 2 )

AENDERUNGEN
-----------
::

        Eingefuehrt in LDMud 3.2.9
        LDMud 3.2.12 fuehrte Zaehlenarrays als Argument ein, also auch
          die Anzahl der Wiederholungen.

SIEHE AUCH
----------
::

        crypt(E), md5_crypt(E), sha1(E), hash(E), hmac(E)

