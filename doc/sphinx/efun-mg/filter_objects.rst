filter_objects
==============

BEMERKUNGEN
-----------

  Werden Pfade angegeben, so wird versucht ein Objekt zu laden, falls
  dieses nicht existiert.

BEISPIELE
---------

  .. code-block:: pike

     // gibt alle tauben Personen wieder
     deaf=filter_objects(users(), "QueryProp", P_DEAF);

     // gibt alle blinden Personen wieder
     blind=filter_objects(users(), "QueryProp", P_BLIND);

     // filtert alle Objekte auf eine bestimmte ID (-> present)
     idmatch=filter_objects(arr, "id", "irgendwasID");

     // gibt alle Autoloader wieder
     al=filter_objects(all_inventory(this_player()),
                      "QueryProp", P_AUTOLOADOBJ);

