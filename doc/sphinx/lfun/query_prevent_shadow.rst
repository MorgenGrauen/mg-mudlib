query_prevent_shadow()
======================

query_prevent_shadow(L)

FUNKTION
--------
::

     varargs int query_prevent_shadow(object shadower)

PARAMETER
---------
::

     object shadower	- da Objekt, das eine Beschattung beantragt

BESCHREIBUNG
------------
::

     Diese Methode kann in Objekten definiert werden, die nicht beschattet
     werden wollen oder anhand des Objektes shadower entscheiden wollen ob
     sie beschattet werden wollen.

     Gibt die Funktion 0 zurueck, wird ein Shadow auf das Objekt erlaubt,
     sonst schlaegt es fehl.

BEISPIEL
--------
::

     // generell keine Beschattung
     int query_prevent_shadow(object who) {
      return 1;
     }

     // Beschattung durch offizielle Objekte erlaubt
     int query_prevent_shadow(object who) {
      if(who && !strstr(object_name(who),"/std/player"))
       return 0;
      return 1;
     }

SIEHE AUCH
----------
::

     Rechte:	     query_allow_shadow(M)
     Generell:	     shadow(E), unshadow(E)
     Informationen:  query_shadowing(E)

20. Mai 2004 Gloinson

