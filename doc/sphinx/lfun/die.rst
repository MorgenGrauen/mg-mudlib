die()
=====

FUNKTION
--------

  public varargs void die(int poisondeath,int extern);

DEFINIERT IN
------------

  /std/living/life.c

ARGUMENTE
---------

  poisondeath
    Dieses Flag sollte bei einem Gifttod (P_POISON) gesetzt sein.
  extern
    Ex- oder interner Aufruf.

BESCHREIBUNG
------------

  Das Lebewesen stirbt, meist automatisch von do_damage() ausgeloest, wenn
  0 HP unterschritten werden. In diesem Fall wird der Kampf beendet, Gift,
  Alkohol, Trink- und Esswerte, Blindheit, Taubheit u.s.w. auf Null
  gesetzt oder geloescht.

  Es wird automatisch eine Leiche (siehe auch P_CORPSE, P_NOCORPSE) nebst
  Todesmeldungen (siehe auch P_DIE_MSG) erzeugt, und fuer Spieler werden
  Killstupse vergeben, sofern notwendig.

  Ueber den Hook H_HOOK_DIE kann man jedoch auf den Automatismus
  Einfluss nehmen, z.B. koennte ein temporaerer Todesbann-Zauber das
  Sterben fuer kurze Zeit verhindern.

RUeCKGABEWERT
-------------

  keiner

BEMERKUNGEN
-----------

  Diese Funktion sollte nur selten direkt verwendet werden. Meist ist der
  uebliche Weg ueber Defend() -> do_damage() -> die() die logisch bessere
  und balancetechnisch guenstigere Loesung.

  Diese Funktion sollte nur ueberschrieben werden,  wenn tatsaechlich einfluss
  auf das Sterben genommen werden soll. Wird nur ein Item hinzugefuegt, ist
  es sinnvoller :doc:`second_life` zu verwenden.

  Wird die() ueberschrieben, sollte man nicht nur das Argument extern
  uebergeben, sondern mit extern_call() verodern (siehe Beispiel), weil das
  geerbte die() den externen Aufruf nicht mehr erkennen kann.

BEISPIEL
--------

.. code-block:: pike

  public varargs void die(int poisondeath, int extern)
  {
    // Dieser NPC soll nicht an Gift sterben.
    if(poisondeath) return;
    
    // Das geerbte die() aufrufen, dabei die Argumente uebergeben und ggf.
    // extern setzen.
    ::die(poisondeath, extern||extern_call());
  }

SIEHE AUCH
----------

  :doc:`Defend`, :doc:`do_damage`, :doc:`second_life`, :doc:`../props/P_POISON`,
  :doc:`../props/P_DEADS`,
  :doc:`../props/P_KILL_NAME`, :doc:`../props/P_KILL_MSG`, 
  :doc:`../props/P_MURDER_MSG`, :doc:`../props/P_DIE_MSG`,
  :doc:`../props/P_ZAP_MSG`, :doc:`../props/P_ENEMY_DEATH_SEQUENCE`,
  :doc:`../props/P_CORPSE`, :doc:`../props/P_NOCORPSE`, 
  extern_call,
  /std/corpse, /std/hooks


Letzte Aenderung: 17.03.2019, Bugfix
