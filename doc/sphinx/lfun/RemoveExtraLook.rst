RemoveExtraLook()
=================

RemoveExtraLook()

int RemoveExtraLook(string key);
----------------------------------------------------
::

DEFINIERT IN
------------
::

  /std/living/description.c

BESCHREIBUNG
------------
::

  Der Extralook erscheint in der Langbeschreibung des Lebewesens.
  Eintraege koennen mit dieser Funktion (vorzeitig) wieder entfernt werden.

ARGUMENTE
---------
::

  - string key:
    Schluesselwort, unter dem der Eintrag, den man entfernen moechte, von
    AddExtraLook() registriert wurde.

RUECKGABEWERTE
--------------
::

  > 0, falls der Eintrag erfolgreich entfernt wurde.
  < 0 sonst.
    -1: keinen (gueltigen) <key> uebergeben.
    -2: kein Eintrag fuer <key> gefunden.

BEMERKUNGEN
-----------
::

  Beim Entfernen mit dieser Funktion wird die "Endemeldung" des entfernten
  Eintrages nicht ausgegeben.

BEISPIELE
---------
::

  // Extralook registrieren.
  living->AddExtraLook(
    "@WER1 wird von einer Horde Daemonen verfolgt.",
    1800,
    // Ohne das gehts nicht, sonst gibt es keine ID, die man zum Loeschen
    // verwenden koennte.
    "ennox_daemonenhordenverfolgerlook");
  // Nun kann der Eintrag auch wieder entfernt werden:
  living->RemoveExtraLook("ennox_daemonenhordenverfolgerlook");

SIEHE AUCH
----------

  :doc:`AddExtraLook`
  :doc:`../props/P_INTERNAL_EXTRA_LOOK`
  :doc:`../props/P_EXTRA_LOOK`
