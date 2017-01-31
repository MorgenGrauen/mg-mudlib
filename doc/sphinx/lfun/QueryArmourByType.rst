QueryArmourByType()
===================

QyeryArmourByType()
-------------------
::

FUNKTION
--------
::

     mixed QueryArmourByType(string type)

DEFINIERT IN
------------
::

     /std/living/combat.c

ARGUMENTE
---------
::

     string type
        Typ der Ruestung aus /sys/combat.h, auf die getestet werden soll.

BESCHREIBUNG
------------
::

     Abfrage, ob das Lebewesen eine Ruestung des angegebenen Typs traegt.

     Zurueckgegeben wird je nach Tragestatus und <type>:
     * 0, falls das Lebewesen die gesuchte Ruestungsart nicht traegt
     * im Erfolgsfall das Ruestungsobjekt
     * falls <type> AT_MISC ist:
       * ({}), wenn es keine AT_MISC-Ruestung traegt
       * ein Array von AT_MISC-Ruestungen
     * falls <type> 0 ist: ein Mapping, das diese Informationen mit dem
       Ruestungstypen als Schluessel enthaelt:
       ([AT_MISC: ({[object], ...}),
         AT_...: <object>,
         ... ])

BEMERKUNG
---------
::

     Ist <type> AT_MISC, so wird auf jeden Fall ein Array zurueckgegeben!

BEISPIELE
---------
::

     Wir wollen wissen, ob this_player() Handschuhe traegt:

     if (objectp(this_player()->QueryArmourByType(AT_GLOVE)))
       ...


     Wir bauen einen Tuersteher, der auf AT_MISC-Kleidung achtet:

     if (sizeof(this_player()->QueryArmourByType(AT_MISC)) > 3) {
       if(this_player()->ReceiveMsg(
            "Du darfst nicht passieren, Du hast zuviele "
            "unpassende Dinge an!",
            MT_LISTEN|MSG_DONT_STORE, MA_TELL,
            "Der Waechter teilt Dir mit: ")<MSG_DELIVERED && // taub?
          this_player()->ReceiveMsg(
            "Der Waechter haelt dich auf.", MT_LOOK)<MSG_DELIVERED) // blind?
            this_player()->ReceiveMsg(
              "Jemand haelt dich auf.", MT_FEEL); // nu aber!
       // Aufhalten!
     } else this_player()->ReceiveMsg(
              "Du darfst passieren, viel Spass im Casino!",
              MT_LISTEN|MSG_DONT_STORE, MA_TELL,
              "Der Waechter teilt Dir mit: ");
       // im Erfolgsfall ist es uns egal, wenn es der Spieler nicht
       // liest: er wird dann eben "wortlos" durchgelassen

SIEHE AUCH
----------
::

     Wear(), WearArmour(), WearClothing(), Unwear(), UnwearArmour(),
     UnwearClothing(), FilterClothing(),
     P_ARMOUR_TYPE, P_CLOTHING, P_ARMOURS,
     /std/living/combat.c

02.02.2016, Gloinson

