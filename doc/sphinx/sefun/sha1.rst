sha1()
======

DEPRECATED
----------
::

SYNOPSIS
--------
::

        string sha1 (string arg)
        string sha1 (int *  arg)

BESCHREIBUNG
------------
::

        Berechnet den SHA1-Hashwert von <arg>.
        Das Argument kann ein String, oder ein Array von Zahlen sein (von
        welchen nur das unterste Byte betrachted wird).

BEISPIEL
--------
::

        string s;

        s = sha1("Hello");
        s = sha1( ({ 'H', 'e', 'l', 'l', 'o' })

HISTORY
-------
::

        Eingefuehrt in LDMud 3.3.523.
        LDMud 3.3.712 fuehrte Zaehlenarrays als Argument ein.

SIEHE AUCH
----------
::

        crypt(E), md5(E)

