IsGuildMember()
===============

FUNKTION
--------

    varargs int IsGuildMember(object pl)

DEFINIERT IN
------------

    /std/gilden_ob.c

RUeCKGABEWERT
-------------

    1
      wenn Gildenmitglied
    0
      sonst

BESCHREIBUNG
------------

    Gibt 1 zurueck, wenn der Spieler Gildenmitglied ist, d.h. P_GUILD
    entsprechend den Gildennamen gesetzt ist. Weitere Pruefungen auf
    Properties oder Skills werden nicht vorgenommen.

    Kann fuer Spieleraktionen im Gildenraum benutzt werden. Setzt bereits
    ein _notify_fail() fuer Nichtgildenmitglieder.

BEISPIEL
--------

.. code-block:: pike
    
    protected int clown_weihe(string str)
    {
      if(IsGuildMember()) {
        tortenweihe+=getuid(this_player());
        this_player()->ReceiveMsg(
          "Du kniest nieder und empfaengt die Torte des Meisterclown.",
          MT_NOTIFICATION);
        send_room(
          this_object(),
          this_player()->Name()+" kniet nieder und wird wuerdig getortet.",
          MT_LOOK|MT_LISTEN,
          MA_EMOTE, 0, ({this_player()}));
        return 1;
      }
      return 0;
    }

SIEHE AUCH
----------

    Verwandt:
      :doc:`GuildName`
    Props:
      :doc:`../props/P_GUILD`
