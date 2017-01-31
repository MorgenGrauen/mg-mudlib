P_M_ATTR_MOD
============

NAME
----
::

    P_M_ATTR_MOD                  "magic_attributes_modifier"

DEFINIERT IN
------------
::

    /sys/living/attributes.h

BESCHREIBUNG
------------
::

    Mapping, das die Attribute des Spielers veraendert, der diese Ruestung
    bzw. Waffe traegt bzw. benutzt.

    Zu beachten:
    P_M_ATTR_MOD kann problemlos durch ein SetProp() gesetzt werden. Es wird
    nur dann beruecksichtigt, wenn die Ruestung/Waffe getragen/benutzt wird.
    Beim Tragen/Ausziehen/Zuecken/Wegstecken wird im Spieler automatisch
    UpdateAttributes() aufgerufen.

    Fuer Krankheiten etc. oder Objekte, deren *Besitz* allein schon die
    Attribute veraendern sollen, verwendet man besser P_X_ATTR_MOD.

    P_X_ATTR_MOD und P_M_ATTR_MOD duerfen einen gemeinsamen kumulierten
    positiven Grenzwert nicht ueberschreiten. Dieser Grenzwert,
    CUMULATIVE_ATTR_LIMIT, ist in /sys/living/attributes.h definiert.

BEMERKUNGEN
-----------
::

    Die Werte sollten moeglichst nicht dynamisch geaendert werden.
    Wenn doch, muss mit TestLimitViolation() am Spieler auf Validitaet
    geprueft und ggf. mit UpdateAttributes() an ihm upgedatet werden.

BEISPIELE
---------
::

    // Dem Spieler, der das Objekt benutzt, wird 2 von A_INT abgezogen und
    // dafuer 1 auf A_STR aufaddiert.
    SetProp(P_M_ATTR_MOD, ([A_INT:-2, A_STR:1]) );

SIEHE AUCH
----------
::

    QueryAttribute(), QueryRealAttribute(), QueryAttributeOffset(),
    SetAttribute(), SetRealAttribute(), UpdateAttributes(),
    SetTimedAttrModifier(), QueryTimedAttrModifier(),
    DeleteTimedAttrModifier(),
    P_X_HEALTH_MOD, P_M_HEALTH_MOD, P_ATTRIBUTES, P_ATTRIBUTES_OFFSETS,
    P_TIMED_ATTR_MOD, P_X_ATTR_MOD, P_M_ATTR_MOD, /std/living/attributes.c

02.02.2016, Gloinson

