SkillResTransfer()
==================

FUNKTION
--------
::

     protected void SkillResTransfer(mapping from_M, mapping to_M)

DEFINIERT IN
------------
::

     /std/living/skill_utils

ARGUMENTE
---------
::

     mapping from_M: Mapping mit zu kopierenden Werten
     mapping to_M:   Zielmapping

BESCHREIBUNG
------------
::

     Interne Methode, die zB (!) waehrend der Ausfuehrung von Attack() durch
     Skills oder P_TMP_ATTACK_MOD geaenderte Werte selektiert in das
     Hauptangriffsmapping uebertraegt.

     Derzeit werden folgende Werte kopiert:
     SI_SKILLDAMAGE, SI_SKILLDAMAGE_MSG, SI_SKILLDAMAGE_MSG2,
     SI_SKILLDAMAGE_TYPE, SI_SPELL

BEMERKUNGEN
-----------
::

     * wird an mehreren Stellen, nicht nur der living/combat verwendet

SIEHE AUCH
----------
::

     P_TMP_ATTACK_MOD, UseSkill (Waffenfaehigkeiten)

18.Jul 2014 Gloinson

