SetChats()
==========

FUNKTION
--------

    void SetChats(int chance, mixed strs);

DEFINIERT IN
------------

    /std/npc/chat.c

ARGUMENTE
---------

    int chance
      Prozentuale Wahrscheinlichkeit einer Ausgabe
    <string|closure|<string|closure|int>* >* strs
      Array mit den verschiedenen Moeglichkeiten der Monsterchats
      (optional)

BESCHREIBUNG
------------

    Der NPC gibt mit der Wahrscheinlichkeit <chance> pro Heartbeat einen
    zufaellig gewaehlten Text aus dem Array <strs> in den Raum aus. Wird
    kein <strs> uebergeben, wird nur die Chat-Chance geaendert und die
    bisherigen Chats werden beibehalten.
    
    Per Default wird send_room() ohne erneutes Umbrechen mit den Messagetypen
    MT_LOOK|MT_LISTEN|MT_FEEL|MT_SMELL zur Ausgabe verwendet.

    Die einzelnen Arrayelemente koennen sein:

      * Strings
      * Closures, deren Rueckgabe ausgegeben wird und die zusaetzlich
        einen aenderbaren und in send_room() verwendeten 'msg_typ' per
        Referenz uebergeben bekommen
      * Arrays mit der Struktur
        `({<string|closure msg >, <int msg_typ>})`, fuer
        die obige Regeln auf 'msg' angewendet werden und bei denen 'msg_typ'
        im send_room() verwendet wird

    Fuer keine Ausgabe muss man einen Leerstring "" zurueckgeben oder
    verwenden. In allen Funktionen ist this_player() das Monster selbst.
    Normalerweise muss man die Nachrichten selbst umbrechen, ausser man
    uebergibt die Message-Typen explizit und uebergibt kein MSG_DONT_WRAP.

BEMERKUNGEN
-----------

    * im Kampf werden keine Chats ausgegeben, man muss dann SetAttackChats()
      verwenden
    * die strings werden (noch) durch process_string() geschickt, 
      dieses sollte man aber nicht mehr verwenden
    * 'chance' gilt sowohl fuer Attack- als auch normale Chats
    * 'chance' wird in der Property P_CHAT_CHANCE abgelegt. Um einen NPC
      voruebergehend 'stillzulegen', kann man P_CHAT_CHANCE auf 0 setzen
    * Spieler koennen P_CHAT_CHANCE temporaer auf 0 setzen ('stillen')
    * NPC haben bei Abwesenheit von Spielern in der Regel keinen Heartbeat,
      weswegen dann auch die Chats ausgeschaltet sind
    * send_room() bekommt immer MSG_DONT_STORE|MSG_DONT_BUFFER uebergeben

BEISPIELE
---------

.. code-block:: pike

    // Ein einfaches Beispiel:
    SetChats(20,
     ({"Der Ork sagt: Hau ab, bevor ich Dich fresse.\n",
       "Der Ork grinst Dich unverschaemt an.\n",
       "Der Ork wedelt mit seinem Saebel vor dir herum.\n",
       "Der Ork stupst Dich mit dem Finger hart.\n"}));

.. code-block:: pike

    // Ein Beispiel mit send_room-Typen ohne MSG_DONT_WRAP
    SetChats(20,
     ({({"Der Ork sagt: Hau ab, bevor ich Dich fresse.", MT_LISTEN}),
       ({"Der Ork grinst Dich unverschaemt an.", MT_LOOK}),
       ({"Der Ork wedelt mit seinem Saebel vor dir herum.", MT_LOOK}),
       ({"Der Ork stupst Dich mit dem Finger hart.", MT_LOOK|MT_FEEL})}));

.. code-block:: pike

    // Laengeres Beispiel mit Closures
    protected string chat_flightinfo(int msg_typ);
    protected string chat_trysteal(int msg_typ);

    void create() {
      SetChats(20,
       ({({"Der Ork sagt: Hau ab, bevor ich Dich fresse.\n", MT_LISTEN}),
         #'chat_flightinfo,
         #'chat_trysteal}));
      // [...]
    }

    protected string chat_flightinfo(int msg_typ) {
      msg_typ = MT_LISTEN;
      return ("Der Ork sagt: "+
              (QueryProp(P_HP)<QueryProp(P_WIMPY)?
                "Ich hab Angst!":
                "Guck mich nicht so an, Schwaechling!"));
    }

    protected string chat_trysteal(int msg_typ) {
      object *pls = filter(all_inventory(environment()), #'interactive);
      if(sizeof(pls)) {
        object pl = pls[random(sizeof(pls))];
        if(!IS_LEARNER(pl)) {
          object *objs = all_inventory(pl);
          if(sizeof(objs)) {
            object ob = objs[random(sizeof(objs))];
            if(ob->move(this_object(),
                        M_NO_SHOW|M_GIVE|M_MOVE_ALL)==MOVE_OK) {
              if(pl->ReceiveMsg(Name(WER)+" stiehlt dir "+ob->name(WEN, 0)+".",
                                MT_FEEL|MT_LOOK)<0)
                pl->ReceiveMsg("Irgendwie scheint dir jetzt etwas zu fehlen.",
                               MT_FEEL|MT_LOOK|MSG_DONT_IGNORE);
              send_room(environment(),
                Name(WER, 1)+" bestiehlt "+pl->name(WEN)+".",
                MT_LOOK, 0, 0, ({pl}));
              return "";
            }
          }
        }
      }
      msg_typ = MT_LOOK;
      return Name(WER, 1)+" schaut sich verstohlen um.";
    }


SIEHE AUCH
----------

     Verwandt:
       :doc:`SetAttackChats`
     Props:
       :doc:`../props/P_CHAT_CHANCE`, :doc:`../props/P_ACHATS`, :doc:`../props/P_ACHAT_CHANCE`
     Sonstiges:
       :doc:`../sefun/send_room`, :doc:`../sefun/process_string`

03. April 2017 Gloinson
