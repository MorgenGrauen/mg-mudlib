set_living_name()
=================

SYNOPSIS
--------
::

        void set_living_name(string name)

BESCHREIBUNG
------------
::

        Setzt einen "Lebewesennamen" fuer das Objekt, indem Name und Objekt in
        eine Tabelle eingetragen werden, welche von find_living() durchsucht 
        wird. Nach Setzen des Namens kann das Objekt per find_living() 
        gefunden werden.

        Das Objekt muss ausserdem per enable_commands() als Lebewesen
        markiert worden sein. Dies ist fuer alle von /std/npc erbenden NPCs
        _automatisch_ der Fall und sollte daher nicht nochmal explizit gemacht
        werden.

        Alle von /std/npc erbenden NPCs setzen ebenfalls automatisch einen
        LivingName, der lower_case(P_NAME) entspricht.

        Ein Objekt kann nur einen Namen haben, mit dem es per find_living()
        gesucht werden kann.

SIEHE AUCH
----------
::

        find_living(E), find_livings(E), find_player(E), enable_commands(E)

LETZTE AeNDERNG
---------------
::

19.10.2015, Arathorn

