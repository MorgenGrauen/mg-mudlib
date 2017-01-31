StopHuntFor()
=============

FUNKTION
--------
::

  varargs int StopHuntFor(object arg,int silent);

DEFINIERT IN
------------
::

  /std/living/combat.c

ARGUMENTE
---------
::

  arg
    Der Gegner, welcher nicht mehr bekaempft werden soll.
  silent
    Flag, welches gesetzt anzeigt, dass die beiden Ex-Streithaehne
    ueber das einseitige Friedensangebot nicht informiert werden
    sollen.

RUeCKGABEWERT
-------------
::

  Flag: Bei 0 war der Gegner nicht auffindbar, bei 1 Erfolg.

BESCHREIBUNG
------------
::

  Mit dieser Funktion kann man ein Lebewesen <arg> als Gegner
  austragen. Im Normalfall erhalten sowohl das aktuelle Objekt, als
  auch der Gegner eine Information darueber. Dies kann jedoch mit dem
  gesetzten Flag <silent> unterbunden werden.
  Es ist auch moeglich, auf diese Meldung Einfluss zu nehmen, indem
  man die Funktion StopHuntText() ueberschreibt, welche dafuer
  verantwortlich ist.
  Achtung: Um zwischen beiden Streithaehnen Frieden zu schliessen,
  muss der eine Gegner jeweils bei dem anderen ausgetragen werden. Ein
  einzelnes StopHuntFor() ist sozusagen nur ein einseitiges
  Friedensangebot.

BEMERKUNGEN
-----------
::

  Soll ein Viech unter bestimmten Umstaenden nicht angreifbar sein, ist in
  keinem Fall StopHuntFor() im Defend() zu verwenden, sondern P_NO_ATTACK.
  Grund: Stoppt man unliebsame Kaempfe jeweils am Anfang vom Defend, kann ein
  Gegner gefahrlos Angriffsspells ausfuehren (und ueben), ohne dass die Gefahr
  besteht, dass der NPC zurueckschlaegt.

BEISPIELE
---------
::

  Man will aus irgendeinem Grund den Kampf zwischen sich und Gegner enemy
  einstellen:
  ...
  StopHuntFor(enemy); // enemy nicht mehr bekaempfen
  enemy->StopHuntFor(this_object()); // enemy soll mich nicht bekaempfen.
  ...

SIEHE AUCH
----------
::

  StopHuntText(), SelectEnemy(), QueryEnemies(), IsEnemy()


16.03.2008, Zesstra 

