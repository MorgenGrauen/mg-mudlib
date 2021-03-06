RemoveExtraLook()
=================

FUNKTION
--------

    int RemoveExtraLook(string key);

DEFINIERT IN
------------
    /std/living/description.c

ARGUMENTE
---------
    - string key:
      Schluessel, unter dem der Extralook registriert wurde ODER
      Objektname des registrierenden Objektes

BESCHREIBUNG
------------
    Der Extralook erscheint in der Langbeschreibung des Lebewesens.

    Eintraege koennen mit dieser Funktion (vorzeitig) wieder entfernt
    werden. Dazu wird entweder der selbst festgelegte Schluessel oder
    der implizite Schluessel `object_name()` des setzenden Objekts
    benoetigt.

BEMERKUNGEN
-----------
    Beim Entfernen mit dieser Funktion wird die "Endemeldung" des entfernten
    Eintrages nicht ausgegeben.

RUECKGABEWERTE
--------------
    Siehe auch /sys/living/description.h fuer Konstanten.

    - 1, falls der Eintrag erfolgreich entfernt wurde.
    - < 0 sonst.

      - -1: keinen (gueltigen) <key> uebergeben.
      - -2: kein Eintrag fuer <key> gefunden.

BEISPIELE
---------

.. code-block:: pike

    // (1) Loeschen ueber expliziten Key
    living->AddExtraLook(
      "@WER1 wird von einer Horde Daemonen verfolgt.",
      1800, "ennox_daemonenhordenverfolgerlook");

    // Nun kann der Eintrag auch wieder entfernt werden:
    living->RemoveExtraLook("ennox_daemonenhordenverfolgerlook");

.. code-block:: pike

    // (2) Loeschen ueber impliziten Objektnamen-Schluessel
    living->AddExtraLook("Eine Sonnenblume ragt aus dem Ohr.")
    // das ist nur aus dem gleichen Objekt heraus moeglich:
    living->RemoveExtraLook(0);

.. code-block:: pike

    // (3) Loeschen ueber impliziten Objektnamen-Schluessel
    string implizite_id = object_name(this_object());
    living->AddExtraLook("Eine Sonnenblume ragt aus dem Ohr.")
    // diese ID kann man natuerlich durch die Gegend schicken
    living->RemoveExtraLook(implizite_id);

SIEHE AUCH
----------

   Verwandt:
     :doc:`AddExtraLook`, :doc:`../props/P_INTERNAL_EXTRA_LOOK`, :doc:`../lfun/HasExtraLook`
   Sonstiges:
     :doc:`../sefun/replace_personal`, :doc:`../sefun/break_string`
   Fuer Spielerobjekte:
     :doc:`../props/P_EXTRA_LOOK`

15. Jun 2017 Gloinson
