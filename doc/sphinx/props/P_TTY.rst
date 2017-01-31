P_TTY
=====

NAME
----
::

    P_TTY                         "tty"

DEFINIERT IN
------------
::

    /secure/telnetneg.h

BESCHREIBUNG
------------
::

     Name der Terminalemulation, die der Spieler nutzt.
     Es existieren bisher "dumb", "vt100" und "ansi".

	

ANMERKUNG
---------
::

     Farben duerfen ausschliesslich bei P_TTY=="ansi" benutzt werden.
     Bei nicht farbfaehigen Terminals koennen ANSI-Codes die gesamte
     Ausgabe zerschiessen!

     Die Attribute fett, unterstrichen, blinkend und invers koennen auch
     schon von vt100-Terminals dargestellt werden. Aber nicht ueberall
     sind alle Attribute/Farben implementiert.

     Bei allen ANSI-Codes sind drei Sachen zu beachten:

     

        1) Sparsam benutzen! Aufgezwungene Hervorhebungen koennen
	   Spieler ganz schnell nerven.

	2) Nicht jeder benutzt dieselbe Hintergrundfarbe!

	3) Sparsam benutzen! Beser noch: nur im Notfall!

