P_RESTRICTIONS
==============

NAME
----
::

    P_RESTRICTIONS                                "restrictions"

DEFINIERT IN
------------
::

    /sys/combat.h
    (Die SR_*-Parameter sind in /sys/new_skills.h definiert.)

BESCHREIBUNG
------------
::

    Enthaelt ein Mapping mit den zu pruefenden Einschraenkungen.

    In dieser Property lassen sich Restriktionen setzen, die vor dem
    Zuecken einer Waffe / Anziehen einer Ruestung oder Kleidung geprueft
    werden und dies gegebenfalls verhindern, ohne gleich auf eine evtl.
    vorhandene WieldFunc / WearFunc zuzugreifen.

    Die Auswertung erfolgt ueber den Aufruf von check_restrictions()
    in /std/restriction_checker.c per call_other().

    Die im Mapping verwendbaren Eintraege finden sich in der Manpage
    zu check_restrictions().
    
    Die Funktionalitaet ist identisch mit Ausnahme von SR_FUN. Bei
    Verwendung von SR_FUN im Mapping wird die Funktion, wenn sie als
    Funktionsname angegeben wird, immer am aufrufenden Objekt gerufen.
    Soll die Funktion an einem anderen Objekt gerufen werden, ist eine
    der beiden alternativen Formen (Closure oder Array) zu verwenden,
    um den Funktionsnamen anzugeben.

BEMERKUNGEN
-----------
::

    Diese Property eignet sich hervorragend dafuer, einige Grundbedingungen
    fuer das Nutzen der Waffe / Ruestung / Kleidung zu stellen, ohne gleich
    eine Wield- oder WearFunc setzen und auswerten zu muessen.

    Denkbar waere der Einsatz bei hochwertigen Waffen / Ruestungen / Kleidung,
    z.B. aus der Para-Welt oder solchen, die sich nah am Limit der geltenden
    Grenzwerte fuer P_WC / P_AC bewegen oder sogar (nach Genehmigung durch
    die Balance) darueber.

BEISPIEL
--------
::

    Mindeststufe 25: SetProp(P_RESTRICTIONS,([P_LEVEL:25]));
    Keine Menschen:  SetProp(P_RESTRICTIONS,([SR_EXCLUDE_RACE:({"Mensch"})]));
    Alignment >499:  SetProp(P_RESTRICTIONS,([SR_GOOD:500]));

    Komplexeres Beispiel

    Quest "Diamond Club" bestanden, magiereigene Property P_AUSGANG_GEFUNDEN
    muss gesetzt sein, Stufe 10, nicht taub, max. 45 Food:
    SetProp(P_RESTRICTIONS, ([ P_LEVEL: 10, P_DEAF: 1, P_FOOD: 45,
      SR_PROP: ([P_AUSGANG_GEFUNDEN:1]), SR_QUEST:({"Diamond Club"}) ]));


SIEHE AUCH
----------
::

    check_restrictions(L), execute_anything(L)
    WieldFunc(L), WearFunc(L), RemoveFunc(L), UnwieldFunc(L),
    P_WIELD_FUNC, P_WEAR_FUNC, P_REMOVE_FUNC, P_UNWIELD_FUNC
    /std/armour/wear.c, /std/weapon/combat.c, clothing, armours, weapon

LETZTE AeNDERUNG
----------------
::

    03. Januar 2014, Arathorn

