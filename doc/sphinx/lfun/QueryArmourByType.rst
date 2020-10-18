QueryArmourByType()
===================

QyeryArmourByType()
-------------------
::

FUNKTION
--------
::

     object|object*|mapping QueryArmourByType(string type)

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


RUECKGABEWERTE
--------------
::
    
    Zurueckgegeben wird abhaengig vom Argument <type> folgendes:

    1) Ist <type> ein Typ, von dem man nur eine Ruestung tragen kann:
       *  0, falls das Lebewesen die gesuchte Ruestungsart nicht traegt,
       *  ansonsten das Ruestungsobjekt

    2) Ist <type> AT_MISC:
       *   ein Array mit allen AT_MISC-Ruestungen
       *   ({}), wenn das Lebewesen keine AT_MISC-Ruestung traegt

    3) Ist <type> 0:
       * Ein Mapping mit den Ruestungstypen als Schluessel der folgenden 
       Form:
          ([AT_MISC:  ({object misc1, ... }),
            AT_CLOAK: object cloak,
            AT_...:   object ...,
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

