P_PLURAL
========

NAME
----
::

    P_PLURAL                      "plural"

DEFINIERT IN
------------
::

    /sys/thing/language.h

BESCHREIBUNG
------------
::

    Mit Hilfe von P_PLURAL koennen auch nicht Unit Objekte als Pluralobjekte
    markiert werden. Bei einem Wert > 1 wird der Wert ausserdem auch noch in
    den Namen eingefuegt. Sollte man in eigenem Code zulassen wollen, das
    etwas mit bestimmten Objekten geschieht, dann sollte man die Verben
    entsprechen konjugieren.

BEMERKUNGEN
-----------
::

    Wirkt nicht auf Todesmeldungen -> siehe dafuer P_KILL_MSG

BEISPIELE
---------
::

    SetProp(P_NAME, "Stiefel"); SetProp(P_PLURAL, 2);
    name(WER, 1) -> "die zwei Stiefel"

    SetProp(P_NAME, "Stiefel"); SetProp(P_PLURAL, 1);
    name(WER, 1) -> "die Stiefel"

    // Ein Beispiel fuer das konjugieren von Verben
    static int cmd_opfer(string str)
    {
       int i;
       object *obs;
       notify_fail("Was moechtest Du opfern?\n");
       if (!str || !sizeof(obs=PL->find_obs(str))) return 0;
       for (i=sizeof(obs)-1; i>=0; i--)
          if (obs[i]->QueryProp(P_VALUE)<=0)
            write(obs[i]->Name(WER)+" "
                 +(ob->QueryProp(P_PLURAL) ? "sind" : "ist")
                 +" doch gar nichts wert.\n");
          else obs[i]->remove();
    }

26. Juni 2004 Gloinson

