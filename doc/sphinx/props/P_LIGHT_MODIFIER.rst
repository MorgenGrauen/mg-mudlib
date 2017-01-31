P_LIGHT_MODIFIER
================

NAME
----
::

    P_LIGHT_MODIFIER                     "light_modifier"

DEFINIERT IN
------------
::

    /sys/properties.h

BESCHREIBUNG
------------
::

    Veraendert das Lichtlevel das von einem Lebewesen wahrgenommen wird.
    Der Wert dieser Property wird additiv in P_PLAYER_LIGHT beruecksichtigt.
    Es ist hiermit z.B. moeglich eine Sonnenbrille zu programmieren, diese
    veraendert ja nicht das tatsaechliche Lichtlevel, sondern verdunkelt nur
    die Sicht.

ANMERKUNG
---------
::

    Damit NPCs in der Lage sind solche Gegenstaende richtig einzuschaetzen,
    sollte diese Property in jedem Gegenstand der einen Light-Modifier setzt,
    auch gesetzt sein. Das veraendern dieser Property in Spielern durch NPCs
    oder Gegenstaende ist selbstverstaendlich genehmigungspflichtig.

BEISPIELE
---------
::

       // Ein NPC der auch in relativ dunklen Raeumen mit Lichtlevel -2
       // noch sehen kann...
       create_default_npc(10);
       SetProp(P_LIGHT_MODIFIER, 3);

       // Eine Sonnenbrille, die das Lichtlevel um eins senkt.

       create()
       {

          :

          SetProp(P_ARMOUR_TYPE, AT_GLASSES);
          SetProp(P_LIGHT_MODIFIER, -1);

          :

       }

       // Achtung: Falls pl Query- oder Set-Methoden auf P_LIGHT_MODIFIER hat,
       // wird diese Methode hier furchtbar schief gehen und im besten Fall
       // nichts veraendern. In realen Objekten empfiehlt sich zumindest eine
       // Pruefung im Vorfeld, ob eine Query-/Set-Methode gesetzt ist.
       protected void InformWear(object pl, int silent, int all) {
           pl->SetProp(P_LIGHT_MODIFIER, pl->QueryProp(P_LIGHT_MODIFIER) -1);
       }

       protected void InformUnwear(object pl, int silent, int all) {
           pl->SetProp(P_LIGHT_MODIFIER, pl->QueryProp(P_LIGHT_MODIFIER) +1);
       }

SIEHE AUCH
----------
::

    P_TOTAL_LIGHT, P_INT_LIGHT, P_PLAYER_LIGHT, P_LIGHT_MODIFIER, CannotSee()

