InternalModifyDefend()
======================

InternalModifyDefend(L)
-----------------------

FUNKTION
--------

  protected void InternalModifyDefend(int dam, string* dam_types,
      mapping si_spell, object enemy)

DEFINIERT IN
------------

  /std/living/combat

ARGUMENTE
---------

  int dam
    Staerke des abzuwehrenden Angriffs (in HP/10)
  string* dam_types
    Art(en) des Schadens, der angerichtet werden soll
  mapping si_spell
    Mapping mit zusaetzlichen Informationen ueber den Angriff
  object enemy
    der Feind/Schadenverursacher

BESCHREIBUNG
------------

  Dient dazu noch Aenderungen am Verhalten der Defend() vornehmen zu
  koennen. Die Parameter werden alle per Referenz uebergeben, Aenderungen
  wirken also direkt im Defend()!

  Einfach ueberschreiben, aber ::InternalModifyDefend(&..., &....) nicht
  vergessen!

SIEHE AUCH
----------

  :doc:`InternalModifyAttack`
  :doc:`Defend`
