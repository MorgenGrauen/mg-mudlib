QueryGuest()
============

FUNKTION
--------
::

     int QueryGuest();

DEFINIERT IN
------------
::

     /std/player/base

BESCHREIBUNG
------------
::

     Auf der uid basierende Hilfsfunktion, um Gaeste zu identifizieren.

RUeCKGABEWERT
-------------
::

     1, wenn Spielerobjekt ein Gast ist; 0 sonst

BEISPIELE
---------
::

     if(this_interactive()->QueryGuest())
     {
       (this_interactive()->ReceiveMsg(
          "Wir bedienen hier nur ordentliche Charaktere.",
          MT_LISTEN, MA_SAY,
          "Der Wirt sagt: ") != MSG_SENSE_BLOCK) ||
       (this_interactive()->ReceiveMsg(
          "Der Wirt gestikuliert dich hinaus.",
          MT_LOOK, MA_LOOK) != MSG_SENSE_BLOCK) ||
       (this_interactive()->ReceiveMsg(
          "Irgendwer stupst dich an. Du sollst wohl gehen.",
          MT_FEEL, MA_FEEL));
       return 1;
     }

SIEHE AUCH
----------
::

     getuid()

14. Mai 2015 Gloinson

