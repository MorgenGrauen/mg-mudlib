create_default_npc()
====================

FUNKTION
--------
::

     varargs void create_default_npc( int level, int maxhp );

DEFINIERT IN
------------
::

     /std/npc.c
     /sys/npc.h

FUNKTION
--------
::

     Setze die Daten eines Monsters auf einen gewissen Level.

     Der Level sollte zwischen 1 und 20 liegen. Die Stats werden auf diesen
     Level gesetzt und mehrere andere Werte, so dass das Monster von der
     Staerke her einem Spieler gleichen Levels entspricht.

     Wird der (optionale) Parameter maxhp weggelassen, wird dieser berechnet
     nach:
          maxhp = 42 + 8 * level

     Die genauen Werte sind:
          P_LEVEL : level
          P_MAX_HP: maxhp
          P_MAX_SP: maxhp
          P_HANDS : 10 * level
          P_BODY  : (20/3) * level
          P_XP    : 50 * level * maxhp (== 5 * P_HANDS * max_hp)

          A_STR, A_INT, A_DEX, A_CON : level

BEMERKUNG
---------
::

     Diese Funktion sollte nur im create() eines Monsters benutzt werden.
     Oben beschriebene Werte, die vor dem Aufruf der Funktion gesetzt
     wurden, werden durch die neuen Werte ersetzt.

     Ab einem Aufruf mit Level 20 werden P_XP = 202000 gesetzt, also ein
     Kill-Stups vergeben (siehe P_XP).

     P_HP und P_SP werden auf dieselben Werte wie P_MAX_HP bzw. P_MAX_SP 
     eingestellt.

     Die Attribute werden ueber SetAttr() gesetzt und damit auf 20 limitiert.
     Sollen hoehere Attribute als 20 gesetzt werden, ist im Anschluss an
     den Aufruf von create_default_npc() P_ATTRIBUTES direkt zu setzen.


BEISPIEL
--------
::

     create_default_npc(17) ergibt:

          P_LEVEL : 17
          P_MAX_HP: 178
          P_MAX_SP: 178
          P_HANDS : 170
          P_BODY  : 113
          P_XP    : 151300

          A_STR, A_INT, A_DEX, A_CON : 17

SIEHE AUCH
----------
::

     Funktionen:  AddExp(), GiveKillScore()
     Properties:  P_XP
                  P_LEVEL, P_MAX_HP, P_MAX_SP, P_HANDS, P_BODY, P_ATTRIBUTES
     Sonstiges:   npcs

14.Feb 2007 Gloinson

