ReceiveMsg()
============

public varargs int ReceiveMsg(string msg, int msg_typ, string msg_action,
                              string msg_prefix, mixed origin)


DEFINIERT IN
------------
::

    /std/npc/comm.c
    /std/player/comm.c
    /std/room/items.c
    /sys/living/comm.h

ARGUMENTE
---------
::

    string msg
      String mit der uebermittelten Nachricht. Muss nicht umgebrochen sein,
      da ReceiveMsg() das ebenfalls erledigt.
    int msg_typ
      Nachrichtentyp(en) fuer Filterung und Flags fuer Behandlung/Umbruch.
      Siehe unten fuer mit | kombinierbare Konstanten.
    string msg_action (optional)
      Ausloesende Aktion, wird auch fuer Ignorieren verwendet.
      Default ist query_verb(). Siehe unten fuer alternative Konstanten.
    string msg_prefix (optional)
      String, welcher ggf. im break_string() als indent verwendet wird.
      Default ist 0 (kein Indent)
    mixed origin (<object>) (optional)
      Das die Meldung ausloesende Objekt, wird fuer Ignorieren verwendet.
      Default: previous_object()

BESCHREIBUNG
------------
::

    ReceiveMsg() wird benutzt, um einem Lebewesen eine Nachricht zu senden.
    Dabei ruft es das sonst uebliche tell_object() fuer Spieler bzw.
    catch_tell() im NPC auf.

    Gerade fuer Nachrichten an Spieler bietet die Methode weitere Features,
    die bisher sonst haendisch zu implementieren waren:
    1) Pruefung auf Empfangbarkeit, je nach 'msg_typ', zB
       MT_LOOK nur an Nichtblinde
       MT_LISTEN nur an Nichttaube
       MT_DEBUG nur an Magier/Testspieler
    2) Pruefen auf Ignorieren von
       - Aktion ('msg_action')
         - mit 'msg_action' || query_verb()
       - Urheber ('origin')
         - fuer sendende Spieler mit origin->query_realname()
         - fuer sendende NPCs mit origin->Name(RAW))
    3) Speicherung in der tmhist (Typ MT_COMM)
    4) Speicherung im Kobold (Typ MT_COMM + aktiver Kobold)
    5) Umbrechen unter Beruecksichtigung von indent, 'msg_typ'-Flags
       fuer Umbruch und P_PREPEND_BS

    Raeume definieren standardmaessig ebenfalls ein ReceiveMsg(), welches in
    jedem Objekt im Raum ReceiveMsg() mit den uebergebenen Argumenten aufruft.
    In diesem Fall ist der Rueckgabe der kleinste von allen gerufenen
    ReceiveMsg() zurueckgelieferte Wert.

    

RUeCKGABEWERT
-------------
::

    > 0 fuer Erfolg, < 0 fuer Misserfolg, s.u.
    MSG_DELIVERED    bei erfolgreicher Zustellung
    MSG_BUFFERED     bei Zwischenspeicherung durch den Kobold

    MSG_FAILED       nicht naeher spezifizierter Fehler bei Zustellung
    MSG_IGNORED      Nachricht wurde ignoriert (Absender, origin)
    MSG_VERB_IGN     Nachricht wurde ignoriert (Kommandoverb, msg_action)
    MSG_MUD_IGN      Absendendes Mud wurde ignoriert.
    MSG_SENSE_BLOCK  Passende Sinne des Empfaenger sind blockiert (z.B.
                     blind, taub)
    MSG_BUFFER_FULL  Kobold kann sich nichts mehr merken

BEMERKUNGEN
-----------
::

    Fuer saemtliche Alternativmeldungen im Falle einer nicht erfolgreich
    zugestellten Nachricht ist der Aufrufer von ReceiveMsg() verantwortlich.

    NPCs:
    * ReceiveMsg() ruft zwecks Abwaertskompatibilitaet catch_tell() auf
    * empfohlen ist, in NPCs nun ReceiveMsg() zu ueberschreiben.
      * catch_tell() muss weiterhin fuer andere tell_object()
        ueberschrieben  werden

BEISPIELE
---------
::

    #1.1 Nachricht an einen Spieler, zB in der Wueste
    this_player()->ReceiveMsg("Die Sonne brennt dir auf den Kopf.",
                              MT_FEEL|MT_LOOK);

    #1.2 Nachricht an einen Spieler von einem NPC mit Indent
    // bei aktivem Editor+Kobold landet dieser Text auch im Kobold
    this_player()->ReceiveMsg("Du haust ja ganz schoen rein!",
                              MT_COMM|MT_FAR|MSG_DONT_STORE,
                              MA_TELL,
                              "Arkshat teilt dir mit: ");

    #1.3 Nachricht an einen Spieler mit Fallback-Kaskade
    // Achtung, bei MT_COMM oder Ignorieren gibt es natuerlich auch
    // Misserfolgs-Rueckgaben. Bei einem normalen Kommando wie diesem
    // hier ist das unproblematisch und daher sinnvoll:
    if(this_player()->ReceiveMsg(
         "Du drueckst den Knopf und es oeffnet sich knirschend "
         "ein kleines Fach in der Wand.", MT_LOOK) < MSG_DELIVERED &&
       this_player()->ReceiveMsg(
         "Du drueckst den Knopf und irgend etwas scheint sich "
         "knirschend zu oeffnen. Das Geraeusch kam von der Wand.",
         MT_LISTEN) < MSG_DELIVERED) // leider blind UND taub ... also:
      this_player()->ReceiveMsg(
        "Du drueckst den Knopf und irgend etwas scheint zu passieren, "
        "aber leider siehst und hoerst du nichts.", MT_FEEL);


    #2.1 Im NPC als Empfaenger auf ein TM reagieren
    public varargs int ReceiveMsg(string msg, int msg_typ, string msg_action,
                                  string msg_prefix, mixed origin) {
      int ret = MSG_DELIVERED;  // Default

      // eine OOC-Kommunikation?
      if(msg_typ&MT_COMM) {
        if(strstr(msg, "hilfe")>=0)
          if(environment(origin)==environment()) {
            origin->ReceiveMsg("Ich werd dir gleich helfen!",
                               MT_COMM|MSG_DONT_STORE, MA_TELL,
                               "Arkshat teilt dir mit: ");
          } else {
            origin->ReceiveMsg("Hilf dir selbst, dann hilft dir Gott!",
                               MT_COMM|MT_FAR|MSG_DONT_STORE,
                               MA_TELL,
                               "Arkshat teilt dir mit: ");
          }
        else if(...)
        [...]
      } else if(msg_typ&MT_LISTEN && msg_action==MA_SAY) {
        [...]
      }

      return ret;
    }


    #3.1 als Sender an viele, Variante mit eigenem filter
    // Achtung: siehe 3.3. send_room() loest vieles.
    // Living nickt nur seinen Nichtgegnern zu
    object *all = filter(all_inventory(environment(this_player())),
                         #'living) - ({this_player()});
    all -= this_player()->PresentEnemies();
    all->ReceiveMsg(this_player()->Name()+
                    " nickt dir verstohlen zu und scheint bereit.",
                    MT_LOOK, MA_EMOTE);

    #3.2 als Sender an viele, Variante mit einzelnem Iterieren
    // Achtung: siehe 3.3. send_room() loest vieles.
    // Living trinkt etwas, jeder im Raum soll es sehen oder hoeren
    object ob = first_inventory(environment(this_player()));
    do {
      if(living(ob) && ob!=this_player())
        ob->ReceiveMsg(this_player()->Name()+" trinkt einen Schnaps aus.",
                       MT_LOOK|MT_LISTEN,
                       MA_DRINK);
      ob = next_inventory(ob);
    } while(ob);

    #3.3 als Sender an viele, Variante mit send_room
    // Living gruesst seine Freunde
    // send_room() ruft ReceiveMsg mit allen entsprechenden Parametern
    object *exclude = this_player()->PresentEnemies();
    send_room(this_object(),
              this_player()->Name()+" gruesst dich.",
              MT_LOOK|MT_LISTEN,
              MA_EMOTE,
              0,
              exclude);

    #3.4 als Sender an viele mit send_room und ReceiveMsg()
    // Living gruesst seine Freunde, seine Feinde sehen das
    // send_room() ruft ReceiveMsg mit allen entsprechenden Parametern
    object *exclude = this_player()->PresentEnemies();
    send_room(this_object(),
              this_player()->Name()+" gruesst dich.",
              MT_LOOK|MT_LISTEN, MA_EMOTE, 0, exclude);
    exclude->ReceiveMessage(
      this_player()->Name()+" gruesst, aber nicht dich.",
      MT_LOOK|MT_LISTEN, MA_EMOTE);

KONSTANTEN FUER PARAMETER
-------------------------
::

    Saemtlich in "/sys/living/comm.h". Hier nicht notwendigerweise
    immer aktuell oder vollstaendig.

    <msg_typ>
      MT_UNKNOWN      unspez. Nachrichtentyp (nicht verwenden). Es wird
                      versucht, aufgrund <msg_action> den Typ zu erraten.
      MT_LOOK         alles, was man sieht
      MT_LISTEN       alles, was man hoert
      MT_FEEL         alles, was man fuehlt
      MT_TASTE        alles, was man schmeckt
      MT_SMELL        alles, was man riecht
      MT_MAGIC        alle sonstigen (uebersinnlichen) Wahrnehmungen
      MT_NOTIFICATION Statusmeldungen, Kommandobestaetigungen
      MT_COMM         alle OOC-Kommunikation, d.h. nicht durch o.g. Sinne
                      abgedeckt.
      MT_FAR          alles, was aus der Ferne / einem anderen Raum kommt.
                      muss mit min. einem anderen Typ kombiniert werden
      MT_DEBUG        Debugmeldungen, sehen nur Magier im Magiermodus
      MT_NEWS         Mails & MPA

      MSG_DONT_BUFFER Nachricht darf nicht im Kobold gespeichert werden
      MSG_DONT_STORE  Nachricht darf nicht in die Comm-History
      MSG_DONT_WRAP   Nachricht nicht per break_string umbrechen
      MSG_DONT_IGNORE Nachricht kann nicht ignoriert werden

      MSG_BS_LEAVE_LFS    wie BS_LEAVE_MY_LFS fuer break_string()
      MSG_BS_SINGLE_SPACE wie BS_SINGLE_SPACE fuer break_string()
      MSG_BS_BLOCK        wie BS_BLOCK fuer break_string()
      MSG_BS_NO_PARINDENT wie BS_NO_PARINDENT fuer break_string()
      MSG_BS_INDENT_ONCE  wie BS_INDENT_ONCE fuer break_string()
      MSG_BS_PREP_INDENT  wie BS_PREPEND_INDENT fuer break_string()

    <msg_action> (optional)
      MA_UNKNOWN     Unspez. Aktion. Es wird der Default query_verb()
                     benutzt bzw. versucht, die Aktion zu erraten.
      MA_PUT         Jemand legt etwas hin und gibt jemanden etwas
      MA_TAKE        Jemand nimmt etwas
      MA_GIVE        Jemand gibt jemandem etwas
      MA_MOVE_IN     Jemand betritt den Raum
      MA_MOVE_OUT    Jemand verlaesst den Raum
      MA_MOVE        Jemand bewegt sich 
      MA_FIGHT       Jemand kaempft
      MA_WIELD       Jemand zueckt eine Waffe
      MA_UNWIELD     Jemand steckt eine Waffe weg
      MA_WEAR        Jemand zieht etwas an
      MA_UNWEAR      Jemand zieht etwas aus
      MA_EAT         Jemand isst etwas
      MA_DRINK       Jemand trinkt etwas
      MA_SPELL       Jemand wirkt einen Spell
      MA_LOOK        Jemand sieht etwas an, untersucht etwas
      MA_LISTEN      Jemand horcht oder lauscht an etwas
      MA_FEEL        Jemand betastet etwas
      MA_SMELL       Jemand schnueffelt herum
      MA_SENSE       Jemand macht eine uebersinnliche Wahrnehmung
      MA_READ        Jemand liest etwas
      MA_USE         Jemand benutzt etwas
      MA_SAY         Jemand sagt etwas
      MA_REMOVE      Etwas verschwindet
      // MA_CHAT        Chatkrams (z.B. teile-mit, Teamkampfchat)
      MA_CHANNEL     Ebenen
      MA_EMOTE       (r)Emotes, Soulverben (remotes mit Typ MT_COMM|MT_FAR)
      MA_SHOUT       Rufen (nicht: shout()!)
      MA_SHOUT_SEFUN Rufen ueber shout(SE)

SIEHE AUCH
----------
::

    Verwandt: send_room(SE)
    Lfuns:    TestIgnore(L)
    Efuns:    tell_object(E), catch_tell(L), catch_msg(L)
              query_verb(E), query_once_interactive(E), break_string(SE)

13.03.2016, Zesstra

