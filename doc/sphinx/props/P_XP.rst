P_XP
====

NAME
----
::

     P_XP                    "xp"

DEFINIERT IN
------------
::

     /sys/living/life.h

BESCHREIBUNG
------------
::

     Diese Property enthaelt die Anzahl der Erfahrungspunkte, die ein
     Lebewesen erreicht hat. Dies geschieht insbesondere durch
     Kampfhandlungen, wobei es sowohl fuer Einzelschlaege als auch fuer
     das Toeten eines Opfers Punkte gibt.

     Bei einzelnen Schlaegen ist die Vergabe von Erfahrungspunkten davon
     abhaengig, wie stark man das Opfer getroffen hat, und welche
     Gesamtwaffenklasse es hat (damage*P_TOTAL_WC/10).

     Beim Todesschlag erhaelt man zusaetzlich die Erfahrungspunkte des
     Opfers geteilt durch 100 (P_XP/100). Dieser Wert wird allerdings
     gegebenenfalls auf ein Team aufgeteilt, sofern der Angreifer sich in
     einem solchigen befindet.

BEISPIEL
--------
::

     NPC's gibt man im allgemeinen einen levelabhaengigen Sockelwert an
     Erfahrungspunkten mit, da sie nicht allzuoft selbst Gegner toeten
     und somit kaum die Moeglichkeit haben, diese Punkte selbst
     anzusammeln. Trotzdem sollen sie ja dem Spieler eine gewisse Anzahl
     an Erfahrungspunkten liefern, wenn sie getoetet werden:

       include "/sys/living/life.h"
       inherit "std/npc";
       void create() {
         ...
         SetProp(P_XP,25000000);
       }

     Beim Toeten gibt es nun 25.000.000/100 = 250.000 Erfahrungspunkte.
     Damit wird der NPC sogar automatisch fuer die Vergabe von
     Erstkillstufenpunkten vorgesehen.

     Die Funktion create_default_npc() setzt P_XP und andere Eigenschaften
     auf geeignete Werte.

BEMERKUNGEN
-----------
::

     Die Vergabe von Erstkillstufenpunkten kann man ueber die Property
     P_NO_SCORE unterbinden, die Vergabe von Erfahrungspunkten ueber
     P_NO_XP. Automatisch werden Lebewesen fuer Erstkillstufenpunkte
     vorgesehen, sofern sie eine der folgenden Grenzen ueberschritten
     haben:
       SCORE_LOW_MARK:  200000 (1 Stufenpunkt)
       SCORE_HIGH_MARK: 600000 (2 Stufenpunkte)
     Definiert sind die Konstanten in "/secure/scoremaster.h".

SIEHE AUCH
----------
::

     Funktionen:  AddExp(), do_damage()
     Verwandt:    P_NO_XP, P_LAST_XP
     Sonstiges:   P_NO_SCORE, create_default_npc()
                  P_TOTAL_WC

14.Feb 2007 Gloinson

