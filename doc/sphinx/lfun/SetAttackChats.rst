SetAttackChats()
================

FUNKTION
--------

    void SetAttackChats(int chance, mixed strs);

DEFINIERT IN
------------

    /std/npc/chat.c

ARGUMENTE
---------

    int chance
      Prozentuale Wahrscheinlichkeit einer Ausgabe
    mixed strs
      Array mit den verschiedenen Moeglichkeiten der Monsterchats


BESCHREIBUNG
------------

    Der NPC gibt mit der Wahrscheinlichkeit <chance> waehrend des Kampfes
    pro Heartbeat einen zufaellig gewaehlten Text aus dem Array <strs>
    in den Raum aus. Dabei wird per Default send_room() ohne erneutes
    Umbrechen mit den Messagetypen MT_LOOK|MT_LISTEN|MT_FEEL|MT_SMELL
    verwendet.

    Die einzelnen Arrayelemente koennen:
    
      * Strings sein
      * Closures sein, deren Rueckgabe ausgegeben wird und die zusaetzlich
        einen aenderbaren und in send_room() verwendeten 'msg_typ' per
        Referenz uebergeben bekommen
      * Arrays mit der Struktur
        `({<string|closure msg >, <int msg_typ>})` sein, fuer
        die obige Regeln auf 'msg' angewendet werden und bei denen 'msg_typ'
        im send_room() verwendet wird

    Fuer keine Ausgabe muss man einen Leerstring "" zurueckgeben oder
    verwenden. In allen Funktionen ist this_player() das Monster selbst.
    Normalerweise muss man die Nachrichten selbst umbrechen, ausser man
    uebergibt die Message-Typen explizit und uebergibt kein MSG_DONT_WRAP.

BEMERKUNGEN
-----------

    * ausserhalb des  Kampf werden keine AttackChats ausgegeben, man muss
      dann SetChats() verwenden
    * die strings werden (noch) durch process_string() geschickt, 
      dieses sollte man aber nicht mehr verwenden
    * 'chance' gilt sowohl fuer Attack- als auch normale Chats
    * 'chance' wird in der Property P_CHAT_CHANCE abgelegt. Um einen NPC
      voruebergehend 'stillzulegen', kann man P_CHAT_CHANCE auf 0 setzen
    * Spieler koennen P_CHAT_CHANCE temporaer auf 0 setzen ('stillen')
    * NPC haben bei Abwesenheit von Spielern in der Regel keinen Heartbeat,
      weswegen dann auch die Chats ausgeschaltet sind
    * send_room() bekommt immer MSG_DONT_STORE|MSG_DONT_BUFFER uebergeben
     
    Man kann AttackChats nutzen, um Monsterspells zu realisieren. Besser
    ist es aber, dafuer 'AddSpell' zu benutzen, ausser man moechte zB
    die Spielerfaehigkeit des Abschaltens der Chats verwenden.

BEISPIELE
---------

.. code-block:: pike

    protected string knirschschrei(int msg_typ);

    SetAttackChats( 20,
     ({"Der Ork tritt Dir in den Hintern.\n",
       ({"Der Ork bruellt: Lebend kommst Du hier nicht raus!\n", MT_LISTEN}),
       ({"Der Ork blutet schon aus mehreren Wunden.\n", MT_LOOK}),
       #'knirschschrei}));

    protected string knirsch(int msg_typ) {
      object helm = this_player()->QueryArmourByType(AT_HELMET);
      if (objectp(helm)) {
        // AC nur dann senken, wenn sie nicht schon 0 ist.
        if (helm->QueryProp(P_AC)>0) {
          helm->Damage(1);
          msg_typ = MT_LISTEN|MT_FEEL;
          return ("Der Ork schreit dich so laut an, dass dabei "+
                  helm->QueryOwn(WER)+" "+
                  helm->name(RAW)+" beschaedigt wird.");
        }
      }
      return ""; // keine Meldung
    }

SIEHE AUCH
----------

     Verwandt:
       :doc:`SetChats`, :doc:`AddSpell`
     Props:
       :doc:`../props/P_ACHAT_CHANCE`, :doc:`../props/P_CHATS`, :doc:`../props/P_CHAT_CHANCE`
     Sonstiges:
       :doc:`../sefun/send_room`, :doc:`../sefun/process_string`

03. April 2017 Gloinson
