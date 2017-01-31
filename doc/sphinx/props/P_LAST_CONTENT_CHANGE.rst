P_LAST_CONTENT_CHANGE
=====================

NAME
----
::

    P_LAST_CONTENT_CHANGE         "last_content_change"         

DEFINIERT IN
------------
::

    /sys/properties.h

BESCHREIBUNG
------------
::

     Wann wurde zum letzten Mal was ins Obj gestopft oder rausgenommen?
     Wichtig fuer den Weight-Cache

ANMERKUNG
---------
::

     Die Property kann nur ueber QueryProp() und SetProp() ausgelesen bzw.
     gesetzt werden. Query() und Set() funktionieren *nicht*.

     Ausserdem fuehrt ein Setzen per SetProp() zu einer Erhoehung des 
     Wertes um eins - unabhaengig vom gesetzten Wert.

