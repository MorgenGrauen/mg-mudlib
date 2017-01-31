create_super()
==============

FUNKTION
--------
::

     protected void create_super();

DEFINIERT IN
------------
::

     allen Standardobjekten

ARGUMENTE
---------
::

     keine

BESCHREIBUNG
------------
::

     Diese Funktion wird immer dann aufgerufen, wenn ein Objekt implizit durch
     ein 'inherit' in einem erbenden Objekte geladen wird.
     Normalweise muss man dieses so geladene Objekte nicht konfigurieren, weil
     nur das Programm dieses Objektes fuer die erbenden Objekte interessant
     ist, nicht seine Daten.
     Was hier aber z.B. sinnvoll ist, ist das Abschalten des Resets, denn ein
     Objekt, was nur dazu dient, dass es von anderen geerbt wird, braucht
     keinen Reset, auch wenn es ein reset() definiert (was in erbenden Objekte
     benutzt wird).
     Die create_super() in den Standardobjekten tun momentan genau dieses.

RUeCKGABEWERT
-------------
::

     keiner

BEMERKUNGEN
-----------
::

     Wenn ihr von /std/ erbt, braucht ihr euch in aller Regel um diese
     Funktion keine Gedanken machen.
     Ihr koennt diese Funktion aber in Objekten selber definieren, die nur zum
     Erben gedacht sind. Dies kann sinnvoll ein, wenn ihr nicht von /std/ erbt
     und ein reset() im Objekt habt oder was machen wollt, was ueber das
     Abschalten des Resets hinausgeht.

BEISPIELE
---------
::

     Gegeben sei folgende Vererbungskette:
     /std/room -> /d/inseln/zesstra/stdraum -> /d/inseln/zesstra/raum

     Wird nun 'raum' geladen und 'stdraum' ist noch nicht geladen, wird der
     Driver implizit von selbst 'stdraum' laden (weil 'raum' das Programm von
     'stdraum' braucht). Bei diesem Laden wird das create() in 'stdraum' nicht
     ausgefuehrt, sondern stattdessen create_super().

    

SIEHE AUCH
----------
::

     create(L), reset(L)
     hook(C), create(H), create_ob(H), create_super(H, reset(H)
     create(A), reset(A)

22.10.2007, Zesstra

