InformDefend()
==============

FUNKTION
--------

  void InformDefend(object enemy);

DEFINIERT IN
------------

  /std/living/combat.c

ARGUMENTE
---------

  enemy
    Der Feind, der ein zu verteidigendes Lebewesen angegriffen hat.

BESCHREIBUNG
------------

  Es ist moeglich, dass Objekte ueber Angriffe auf Lebewesen
  informiert werden, sofern diese Objekte bei dem angegriffenen
  Lebewesen mittels AddDefender() angemeldet wurden und sich der
  selben Umgebung befinden.
  Zumeist wird es sich bei den Objekten natuerlich ebenfalls um
  andere Lebewesen handeln, die das Lebewesen, bei dem sie angemeldet
  sind, verteidigen sollen.
  Bei einem Angriff auf das Lebewesen werden alle Objekte per Aufruf
  von InformDefend() darueber informiert, wobei der Angreifer als
  Parameter uebergeben wird.
  Standardmaessig ist diese Funktion in Lebewesen bereits definiert,
  wobei der Skill SK_INFORM_DEFEND, sofern vorhanden, aufgerufen wird.

BEISPIEL
--------

  Sehr beliebt sind in Gilden NPCs, die den Beschwoerer begleiten und
  verteidigen, z.B. beschworene Daemonen:

.. code-block:: pike

  inherit "/std/npc";

  #include <properties.h>

  object owner;

  protected void create()
  {
    ::create();
    SetProp(P_NAME,"Daemon");
    ...
  }

  // nach Clonen des Daemons folgende Funktion mit Beschwoerer als
  // Parameter aufrufen

  void Identify(object caster)
  { 
    if(!objectp(caster))
      call_out(#'remove,0);
    owner=caster;
    owner->AddDefender(this_object());
  }

  // folgende Funktion wird automatisch aufgerufen, wenn der
  // Beschwoerer angegriffen wird

  public void InformDefend(object enemy)
  { 
    if(!IsEnemy(enemy)&&enemy!=owner)
      Kill(enemy);
  }

  Soll der Daemon sich auch in ein Team einordnen, in welchem sich der
  Beschwoerer eventuell befindet, so ist zusaetzlich AssocMember() in
  diesem Beschwoerer aufzurufen, wobei der Daemon als Parameter
  uebergeben wird.

SIEHE AUCH
----------

  :doc:`AddDefender`, :doc:`RemoveDefender`, :doc:`QueryDefenders`, 
  :doc:`QueryPresentDefenders`, :doc:`QueryNearDefenders`, 
  :doc:`DefendOther`,
  :doc:`../props/P_DEFENDER`, /std/living/combat.c

Letzte Aenderung: 28.10.2022, Bugfix
