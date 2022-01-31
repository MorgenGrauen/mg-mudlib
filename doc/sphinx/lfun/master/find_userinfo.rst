find_userinfo()
===============

FUNCTION
--------

  public int find_userinfo(string user)

DEFINIERT IN
------------

  /secure/master/userinfo.c

ARGUMENTE
---------

  - user
    UID des abzufragenden users

BESCHREIBUNG
------------

  Gibt zurueck, ob user existiert und traegt die Anfrage ggf. in den Cache
  ein.

RUECKGABEWERT
-------------

  1, wenn user existiert, sonst 0.

SIEHE AUCH
----------

  :doc:`query_userlist`

Letzte Aenderung: 31.01.2022, Bugfix
