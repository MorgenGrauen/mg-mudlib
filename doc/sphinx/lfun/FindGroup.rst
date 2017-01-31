FindGroup()
===========

FUNKTION
--------
::

    object*FindGroup(object pl,int who);

DEFINIERT IN
------------
::

    /std/spellbook.c

ARGUMENTE
---------
::

    pl
      Lebewesen, von welchem die Freunde oder Feinde in der Umgebung
      gefunden werden sollen.
    who
      Flag, welches anzeigt, ob Freunde oder Feinde gefunden werden
      sollen (Konstanten definiert in '/sys/new_skills.h'):
        FG_ENEMIES - (Wert -1) Feinde sollen gefunden werden
        FG_FRIENDS - (Wert  1) Freunde sollen gefunden werden
        FG_ALL     - (Wert  0) alle Lebewesen sollen gefunden werden

RUeCKGABEWERT
-------------
::

    Array mit gefundenen Lebewesen

BESCHREIBUNG
------------
::

    Bei Spells, die sich auf mehrere Gegner auswirken oder bei denen man
    per Hand ein Opfer auswaehlen moechte, laesst sich mittels der
    Funktion FindGroup() eine Liste von Lebewesen ermitteln, welche in
    der Umgebung von <pl> zu finden sind.
    Je nachdem, was man denn genau vorhat, kann man sich von der
    Funktion freundlich oder feindlich gesinnte Lebewesen heraussuchen
    lassen.
    Will man die freundlich gesinnten Lebewesen ermitteln, so uebergibt
    man in <who> die Konstante FG_FRIENDS, bei feindlich gesinnten die
    Konstante FG_ENEMIES, und wenn man alle Lebewesen bekommen moechte
    schliesslich FG_ALL.
    Bei der Auswahl gelten folgende Regeln:
      (1) Lebewesen, mit denen <pl> im Kampf ist, sind grundsaetzlich
          feindlich gesinnt.
      (2) Teammitglieder von <pl> sind grundsaetzlich freundlich
          gesinnt.
      (3) Spieler sind gegenueber Spielern freundlich gesinnt, NPCs
          gegenueber NPCs. NPCs kann man hierbei mit Hilfe der Property
          P_FRIEND den Spielern zuordnen.
      (4) Daraus folgt natuerlich, dass Spieler und NPCs grundsaetzlich
          eine feindliche Einstellung gegenueber haben, sofern die NPCs
          nicht die Property P_FRIEND gesetzt haben
           (was standardmaessig natuerlich nicht der Fall ist).
      (5) Netztote werden nicht erkannt.
      (6) Magier werden nicht erkannt, wenn sie unsichtbar sind.
      (7) Ein Magier wird als feindlich gesinnt nur dann erkannt, wenn
          <pl> mit ihm im Kampf ist.
      (6) Sucht man feindlich gesinnte Lebewesen, so werden die, welche
          eine von den Properties P_NO_ATTACK oder P_NO_GLOBAL_ATTACK
          gesetzt haben, nicht erkannt.
    Die Property P_FRIEND sollte man in NPCs setzen, die dem Spieler
    hilfreich beiseite stehen, z.B. vom Spieler beschworene HilfsNPCs.

BEISPIELE
---------
::

    Wenn man einen Feuerball nach jemandem wirft, so trifft dieser unter
    Umstaenden auch andere, wenn er gross genug ist. Man nimmt hierbei
    an, dass sich die freundlich gesinnten Lebewesen des Gegners auch
    naeher bei ihm befinden als die feindlich gesinnten:
      victim->Defend(500,DT_FIRE,([SP_SHOW_DAMAGE:1]),caster);
      victimList=FindGroup(victim,FG_FRIENDS);
      map_objects(victimList,
                      "Defend",
                      100,
                      DT_FIRE,
                      ([SP_SHOW_DAMAGE:1]),
                      caster);
    Hiermit trifft man also auch die Freunde von <victim>.

SIEHE AUCH
----------
::

    FindGroupN(), FindGroupP(), P_FRIEND, P_NO_GLOBAL_ATTACK


Last modified: Mon Jan 28 21:45:00 2002 by Tiamak

