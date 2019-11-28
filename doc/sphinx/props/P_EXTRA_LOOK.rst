P_EXTRA_LOOK
============

NAME
----
::

  P_EXTRA_LOOK:                          "extralook"

DEFINIERT IN
------------
::

  /sys/living/description.h

BESCHREIBUNG
------------

  Diese Property enthaelt einen String. Sie wird entweder in Lebewesen
  direkt oder in Objekten gesetzt, die im Besitz von Lebewesen
  sein koennen.
  Diese Strings erscheinen dann zusaetzlich in der Langbeschreibung
  des Lebewesens bzw. des Besitzers (wenn das Objekt sich direkt im
  Lebewesen befindet, jedoch nicht in einem Behaelter im Lebewesen).

  P_EXTRA_LOOK nutzt replace_personal() um @WER1 etc. zu ersetzen.

  Ist das letzte Zeichen im String kein Zeilenumbruch, wird auf 78 Zeichen
  umgebrochen. Wenn die Platzhalter von replace_personal() verwendet werden,
  sollte man das Umbrechen der Mudlib ueberlassen.

  **Nur** dann benutzen, wenn ihr auch unabhaengig vom Extralook ein
  Objekt im Spieler benoetigt, ansonsten **immer**
  :doc:`../lfun/AddExtraLook` verwenden.

  .. note::
  
    Der Extralook taucht an einem Lebewesen nur auf, wenn
    dieser schon existiert, wenn das Objekt in das Lebewesen bewegt wird
    (oder im Fall von F_QUERY_METHOD-Methoden: diese bereits gesetzt
    ist.)

BEISPIEL
--------

  Ein Spieler hat eine gefaehrliche Krankheit. In dem Krankheitsobjekt setzt
  man z.B. folgendes:  

  .. code-block:: pike

    SetProp(P_EXTRA_LOOK,"@WER1 sieht leichenblass aus.");

SIEHE AUCH
----------

  - :doc:`../lfun/long`
  - :doc:`../lfun/AddExtraLook`, :doc:`../lfun/RemoveExtraLook`
    :doc:`../lfun/HasExtraLook`
  - /std/living/description.c, /std/player/base.c
  - :doc:`../sefun/replace_personal`

