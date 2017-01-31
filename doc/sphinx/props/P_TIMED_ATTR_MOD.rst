P_TIMED_ATTR_MOD
================

NAME
----
::

    P_TIMED_ATTR_MOD         "timed_attr_mod"

DEFINIERT IN
------------
::

    /sys/living/attributes.h

BESCHREIBUNG
------------
::

    In dieser Property werden Attribut-Modifikatoren gespeichert, die
    nicht ueber laengere Zeit wirksam sein sollen.
    Die Wirksamkeit der Modifikatoren kann an Zeit und Objekte
    gebunden werden.

    Intern werden die Modifikatoren in einer Datenstruktur der Form

    ({
       ({ Ablaufzeiten }),
       ([ Key : Ablaufobjekt ]),
       ([ Key : ([ Mapping mit den Modifikatoren ]);
         Ablaufzeit ; Ablaufobjekt ; Nachrichtenempfaenger
       ])
    })

    gespeichert mit:
    * Ablaufzeiten:  Zeit in Sekunden seit 1. Jan 1970, 0.0:0 GMT
    * Ablaufobjekte: Objekte, an deren Existenz die Attribut-
                     veraenderungen gebunden sind
    * Nachrichtenempfaenger:
      Objekte/Klassen, welche ueber abgelaufene Attributveraenderung
      durch den Aufruf von "NotifyTimedAttrModExpired" (mit key als
      Argument) benachrichtigt werden.

    Das Setzen der Werte erfolgt NUR ueber die Methoden SetTimedAttrModifier
    und DeleteTimedAttrModifier.

    Die Daten zu einem Key koennen ueber QueryTimedAttrModifier abgefragt
    werden. Die Abfrage mittels QueryProp liefert eine Kopie der gueltigen
    Datenstruktur, die per Query nicht (siehe Bemerkungen).

    Die Bedingungen fuer die ueber P_TIMED_ATTR_MOD gesetzten
    Attributveraenderungen werden im Heartbeat in der Funktion
    attribute_hb ueberprueft. Eine verminderte Funktionalitaet im
    Falle von Magiern ist somit kein Fehlerfall.

BEMERKUNGEN
-----------
::

    Keine echte Property. Die Methode _query_timed_attr_mod() in
    /std/living/attributes.c stellt die Daten zusammen.

    ACHTUNG: Bitte nur die bereitgestellten Methoden zur Manipulation
             benutzen! Setzen als Property hat keinen Effekt.

SIEHE AUCH
----------
::

    QueryAttribute(), QueryRealAttribute(), QueryAttributeOffset(),
    SetAttribute(), SetRealAttribute(), UpdateAttributes(),
    SetTimedAttrModifier(), QueryTimedAttrModifier(),
    DeleteTimedAttrModifier(),
    P_ATTRIBUTES, P_ATTRIBUTES_OFFSETS, P_ATTRIBUTES_MODIFIER,
    P_X_ATTR_MOD, P_M_ATTR_MOD, /std/living/attributes.c

Last modified: Tue Jul 27 20:00:20 2004 by Muadib

