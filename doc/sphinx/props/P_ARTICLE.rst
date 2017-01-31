P_ARTICLE
=========

NAME
----
::

    P_ARTICLE                     "article"                     

DEFINIERT IN
------------
::

    /sys/thing/language.h

BESCHREIBUNG
------------
::

     Gibt an, ob in der Beschreibung ein Artikel ausgegeben werden soll
     oder nicht.

     Wenn ein Artikel angegeben werden soll, so wird 1 gesetzt, sonst 0.
     Diese Property ist aus historischen Gruenden auf 1 voreingestellt
     und intern invertiert. (d.h., beim Auslesen per Query kommt als 
     Ergebnis genau das falsche heraus). Bitte beachtet das bei Set- bzw.
     Query-Funktionen.

