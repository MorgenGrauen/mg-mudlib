P_NEXT_DISABLE_ATTACK
=====================

PROPERTY
--------
::

  P_NEXT_DISABLE_ATTACK    "next_diable_attack"

DEFINIERT IN 
-------------
::

  combat.h

BESCHREIBUNG
------------
::

  Diese Property gibt an, wann der NPC das naechste Mal paralysiert
  werden kann. Ueblicherweise wird sie automatisch beim Setzen
  von P_DISABLE_ATTACK gesetzt. Sie gibt einen Zeitpunkt wie
  die Funktion time() an, an dem zum ersten Mal wieder Paralyse
  moeglich ist.

  Will man einen NPC schreiben, der immer paralysierbar ist und nicht erst
  nach einer gewissen Wartezeit nach der letzten Paralyse, laesst sich dies
  durch eine Set-Methode auf P_NEXT_DISABLE_ATTACK erreichen:

    

  Set(P_NEXT_DISABLE_ATTACK, function int () {return 0;}, F_SET_METHOD);

  Diese Set-Methode verhindert das Setzen von P_NEXT_DISABLE_ATTACK mittels
  eines SetProp-Aufrufes.

BEMERKUNGEN
-----------
::

  Die Zeit zum Schutz vor erneuter Paralyse existiert absichtlich. Bitte
  waegt sorgfaeltig ab, bevor ihr diese Property an Gegnern/Spielern
  manipuliert.

SIEHE AUCH
----------
::

  P_DISABLE_ATTACK

21.Jul 2014 Gloinson

