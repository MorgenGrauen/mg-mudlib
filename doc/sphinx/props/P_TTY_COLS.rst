P_TTY_COLS
==========

NAME
----
::

    P_TTY_COLS                                  "tty_cols"

DEFINIERT IN
------------
::

    /secure/telnetneg.h

BESCHREIBUNG
------------
::

    In dieser Property steht die Anzahl der Spalten, die das
    Terminalfenster des Spielers derzeit hat, sofern das Telnet des
    Spielers Telnet-Negotiations unterstuetzt. 
    
    Wenn das nicht der Fall ist, ist sie leer, d.h. auf den Wert 0 gesetzt.
    Ansonsten enthaelt die Property die Fensterbreite, mindestens aber
    einen Wert von 35.
    
    Das Setzen der Property aendert die Fenstergroesse des Spielers
    natuerlich nicht.

HINWEIS
-------
::
    
    Die Property ist technisch nicht dagegen geschuetzt, auf unpraktisch
    kleine, grosse oder auch negative Werte gesetzt zu werden. Das heisst 
    insbesondere, dass man sich bei der Abfrage der Property nur dann auf
    den Mindestwert von 35 verlassen kann, wenn sichergestellt ist, dass 
    er nur im Rahmen der automatischen Aushandlung veraendert wurde.
  

SIEHE AUCH
----------

    :doc:`P_TTY_ROWS`, :doc:`P_TTY_TYPE`, :doc:`P_TTY_SHOW`

2019-Sep-25, Arathorn

