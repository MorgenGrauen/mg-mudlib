check_restrictions()
====================

FUNKTION
--------
::

    string check_restrictions(object pl, mapping restr)

DEFINIERT IN
------------
::

    /std/restriction_checker.c

ARGUMENTE
---------
::

    object pl        geprueftes Lebewesen
    mapping restr    Mapping mit Restriktionen, s.u.

BESCHREIBUNG
------------
::

    Die Methode wird verwendet, um Restriktionen (zum Beispiel fuer das
    Casten eines Spells) zu pruefen. Sie wird von Spellbook und
    Gildenobjekt direkt genutzt.

    Ihr wird dabei ein Spielerobjekt sowie ein Mapping mit den jeweiligen
    Restriktionen uebergeben. Die aktuell moeglichen Keys des Mappings sind
    weiter unten gelistet.

BEMERKUNGEN
-----------
::

    Es wird bei der Rasse P_REAL_RACE geprueft. Der Tarnhelm funktioniert
    also nicht.

    Bei Erweiterungsvorschlaegen wendet euch bitte an einen EM oder
    inheritet im Zweifelsfall nach Absprache.
    NIEMALS solchen Code einfach KOPIEREN. Spaeter muss nur irgendwer
    eurem alten Code hinterherraeumen.

    Aktuelle Liste der pruefbaren Parameter:

    P_LEVEL
      Mindeststufe, die das Lebewesen besitzen muss, um die Aktion
      auszufuehren.
    P_GUILD_LEVEL
      Gildenlevel, das das Lebewesen mindestens erreicht haben muss, um die
      Aktion auszufuehren.
    SR_SEER
      Ist gesetzt, wenn das Lebewesen Seher sein muss.
      Auswertung nur fuer Interactives, NSC ignorieren das Flag.
    P_XP
      Mindestmenge an Erfahrungspunkten, die ein Lebewesen besitzen muss,
      um die Aktion auszufuehren.
    P_QP
      Mindestmenge an Abenteuerpunkten, die das Lebewesen haben muss.
    P_ALCOHOL
      Menge an Alkohol, unter der der Alkoholspiegel des Lebewesen liegen
      muss, um die Aktion noch ausfuehren zu koennen.
    P_DRINK
      Menge an Fluessigkeit, unter der der Fluessigkeitsspiegel des
      Lebewesen liegen muss, um die Aktion noch ausfuehren zu koennen.
    P_FOOD
      Beinhaltet die Menge an Nahrung, unter der der Nahrungsspiegel des
      Spielers liegen muss, um die Aktion noch ausfuehren zu koennen.
    P_DEAF
      Ist gesetzt, falls der Spieler nicht taub sein darf.
    P_FROG
      Ist gesetzt, falls der Spieler kein Frosch sein darf.
    P_BLIND
      Ist gesetzt, falls der Spieler nicht blind sein darf.
      Achtung: das ist nicht gleichbedeutend mit dem Umstand, dass er evtl.
      nichts mehr sehen kann. Auch andere Gruende (zum Beispiel Dunkelheit)
      koennen bewirken, dass ein Spieler nichts mehr sieht.
    A_INT, A_DEX, A_CON, A_STR
      Jeweilige Mindesthoehe eines Attribut, um eine Aktion ausfuehren zu
      koennen.
    SR_BAD, SR_GOOD
      Gibt an, wie [minimal] boese bzw. wie [maximal] gut ein Charakter sein
      darf, um eine Aktion ausfuehren zu koennen.
    SR_MIN_SIZE, SR_MAX_SIZE
      Gibt die minimale, bzw. die maximale Groesse an, die ein Charakter
      maximal haben darf, um eine Aktion ausfuehren zu koennen.
    SR_FREE_HANDS
      Gibt an, wieviele freie Haende ein Charakter fuer diese Aktion
      besitzen muss.
    SR_EXCLUDE_RACE
      Mitglieder aller in dieser Liste aufgefuehrten Rassen koennen
      diese Aktion nicht ausfuehren.
    SR_INCLUDE_RACE
      Mitglieder aller NICHT in dieser Liste aufgefuehrten Rassen koennen
      diese Aktion nicht ausfuehren.
    SM_RACE
      Hier kann pro Rasse ein Mapping mit besonderen (nur) fuer diese Rasse
      geltenden Einschraenkungen vorgenommen werden. Als Keys sind die
      in dieser Manpage beschriebenen Keys erlaubt, wobei SM_RACE nicht
      rekursiv ausgewertet wird.
      Der Rassenname ist gross geschrieben und "*" steht fuer alle Rassen.
    SR_EXCLUDE_GUILD
    SR_INCLUDE_GUILD
      Diese beiden Keys verhalten sich wie SR_*_RACE, nur dass hier Gilden
      genannt werden.
    SR_FUN
      Hier kann eine Funktion angegeben werden, die aufgerufen wird, um sie
      die Restriktionen zu pruefen zu lassen. Folgende Formen sind moeglich:
      - Funktionsname als String; Funktion wird an dem Objekt gerufen, das
        die Restriktion prueft, d.h. an der Ruestung/Waffe/Kleidung. Soll
        die Funktion an einem anderen Objekt gerufen werden, ist eine
        der beiden alternativen Formen zu verwenden.
      - eine Closure, wird per funcall() gerufen
      - ein Array mit dem folgenden Aufbau:
        ({ Objekt/Objektname, Funktionsname, arg_1, arg_2, ... , arg_n })
     
      Der aufgerufenen Funktion wird das Spielerobjekt immer als erstes
      Argument uebergeben, d.h. bei der Array-Form ggf. vor dem ersten
      Extra-Argument arg_1 eingeschoben.
      SR_FUN kann nuetzlich sein, um Restriktionen zu pruefen, die sich mit
      den anderen Optionen nicht abbilden lassen.
      Ist der Test nicht bestanden, muss die Funktion einen String zurueck-
      geben, ansonsten 0.
      Eine Besonderheit besteht beim Aufruf per call_other(), d.h. wenn
      restriction_checker.c nicht geerbt wurde und nur ein Funktionsname
      uebergeben wird. In diesem Fall, der auch bei Verwendung von
      P_RESTRICTIONS zum Tragen kommt, wird die Funktion immer am
      aufrufenden Objekt, d.h. previous_object(), gerufen.
    SR_PROP
      Hier kann ein Mapping mit Properties und zugehoerigen Werten angegeben
      werden, die jeweils auf Identitaet geprueft werden. Zusaetzlich sollte
      eine Meldung angegeben werden, die als Fehlermeldung ausgegeben wird,
      wenn der Spieler die Bedingung nicht erfuellt. Es sollte immer eine
      passende Meldung fuer den Spieler eingebaut werden. Beispiel:
      ([ SR_PROP: ([P_AUSGANG_ENTDECKT: 1; "Dein Schwert fluestert "
          "veraergert: Ich werde Dir erst dann zu Diensten sein, wenn Du "
          "Dich als wuerdig erwiesen hast!"]) ])
      Aufgrund der Meldung wird empfohlen, SR_PROP nicht in Restriktionen
      einzusetzen, die massenweise in Savefiles landen (z.B.
      Spielersavefiles).
    SR_QUEST
      Hier kann ein String-Array mit den Namen (Keys) der Quest(s) angegeben
      werden, die der Spieler bestanden haben muss, um die Aktion ausfuehren
      zu koennen.
    SQ_MINIQUEST
      Hier kann entweder ein String-Array mit den Ladenamen der vergebenden
      Objekte oder ein Int-Array mit den Index-Nummern (IDs) der
      Miniquest(s) (empfohlen!) angegeben werden, die der Spieler bestanden
      haben muss, um die Aktion ausfuehren zu koennen.

BEISPIELE
---------
::

    // #1 Levelbeschraenkung in der Abenteurergilde
    AddSpell("feuerball",20,
             ([SI_SKILLRESTR_LEARN:([P_LEVEL:15]), ...

    // #2 Glaubenstest im Klerus
    AddSpell("bete",
             ([SI_SKILLRESTR_LEARN: ([P_GUILD_LEVEL : LVL_NOVIZE,
                                      SR_FUN : #'glaubensTest ]), ...
    // mit
    static string glaubensTest(object pl) {
      if (pl->QueryProp(K_STRENGTH) < 8000)
        return ("Deine Glaubensstaerke laesst zu wuenschen uebrig!\n");
      return 0;
    }

    // #3 SM_RACE-Modifikation der Restriktionen:
    //    haertere Restriktionen fuer Zwerge
    //    - hoeheres Level
    //    - zusaetzlich A_STR pruefen
    ([P_LEVEL:15,
      A_INT:10,
      SM_RACE: (["Zwerg": ([P_LEVEL:17, A_STR:20])])])
    // ist identisch zu
    ([SM_RACE: (["*":     ([P_LEVEL:15, A_INT:10]),
                 "Zwerg": ([P_LEVEL:17, A_INT:10, A_STR:20])])])

SIEHE AUCH
----------
::

    execute_anything(L), AddSpell (Gilde), P_RESTRICTIONS

03. Januar 2014, Arathorn

