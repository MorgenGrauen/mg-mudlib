AddExtraLook()
==============

FUNKTION
--------

    varargs int AddExtraLook(string look, [int duration, string key, string lookende, object ob]);

DEFINIERT IN
------------
    /std/living/description.c

ARGUMENTE
---------
    string look:

      - String, der in der Langbeschreibung des Lebewesens zusaetzlich
        ausgegeben wird.
      - zu rufender Funktionsname, wenn 'ob' angegeben ist

    int duration:

      - > 0: Dauer der Gueltigkeit des Extralooks in Sekunden.
      - 0:   Unbegrenzt gueltiger Eintrag. Rebootfest.
      - < 0: Bis Ende/Reboot bestehender Eintrag.

    string key:
      Schluesselwort, unter dem der Eintrag registriert wird und mit dem man
      diesen auch mittels RemoveExtraLook() entfernen kann. Sollte eindeutig
      sein!
      Ist 'key' nicht angeben, wird der Objektname (object_name()) des
      setzenden Objekts benutzt.

    string lookende:

      - String, der an das Lebewesen (nur bei Spielern) ausgegeben wird,
	wenn der eingetragene Extralook abgelaufen ist.
      - zu rufender Funktionsname, wenn 'ob' angegeben ist

    object ob:
      Ein Objekt, an dem die in 'look' und 'lookende' abgelegten Methoden
      bei Abfrage oder Ablauf aufgerufen werden.
      Die Methoden bekommen das Living uebergeben und muessen selbst
      umgebrochene Strings zurueckgeben.

BESCHREIBUNG
------------
    Der Extralook erscheint in der Langbeschreibung des Lebewesens.

    Texte dafuer koennen mit dieser Funktion hinzugefuegt und verwaltet werden.
    Wenn ihr nicht ohnehin unbedingt ein Objekt IM Spieler ablegt (wie zB
    eine Ruestung mit einem Extralook) (1), dann ist diese Methode bevorzugt zu
    verwenden.

    Ueber die Angabe eines 'ob' koennen Looks auch dynamisch erstellt werden,
    dabei werden dann 'look' bzw. 'lookende' als Methoden am Objekt gerufen.
    (1) Ein so fuer AddExtraLook verwendetes Objekt 'ob' muss nicht wie bisher
    ueblich im Inv des Spieler liegen!

    Direkt angegebene Texte (also nicht von einem Objekt 'ob' bezogen) werden
    durch replace_personal() gefiltert und unter Beibehaltung existierender
    Umbrueche auf 78 Zeichen umgebrochen.

BEMERKUNGEN
-----------
    - Die Meldung von <lookende> bzw. der Funktionsaufruf erfolgt, wenn der
      Extralook der Lebewesen das erste Mal nach Ablauf der Gueltigkeit
      aufgerufen wird.
    - Das uebergbene Objekt sollte fuer permanente Eintraege eine Blueprint
      sein, da Clones irgendwann (spaetestens mit Reboot) zerstoert werden
      und der Eintrag dann bei Abfrage automatisch geloescht wird.
    - Folgerung: Clone-Objekte koennen fuer selbst beschraenkt temporaere
      Extralooks benutzt werden.

RUECKGABEWERTE
--------------
    Siehe auch /sys/living/description.h fuer Konstanten.

    - 1, falls der Eintrag erfolgreich registriert wurde.
    - < 0 sonst.

      - -1: <key> war nicht gueltig und es konnte keiner ermittelt werden.
      - -2: <look> war kein gueltiger String.
      - -3: <duration> war kein Integer.
      - -4: unter <key> gibt es schon einen Eintrag.

BEISPIELE
---------

.. code-block:: pike

    // (1) einfacher Eintrag, "fuer die Ewigkeit"
    living->AddExtraLook("@WER1 hat den Drachengott der SSP besiegt.");

.. code-block:: pike

    // (2) Eintrag der nach 1h automatisch weg ist.
    living->AddExtraLook("@WER1 ist ganz mit Marmelade bedeckt.", 3600);

.. code-block:: pike

    // (3) Eintrag mit bestimmten Schluessel, damit man ihn wieder entfernen kann
    living->AddExtraLook("@WER1 ist ganz mit Marmelade bedeckt.", 3600,
                         "humni_marmeladen_look");

.. code-block:: pike

    // (4) Mit "Ende"-Meldung, aber kein eigener Schluessel
    living->AddExtraLook("@WER1 ist patschnass.", 1200, 0,
                         "Du bist endlich wieder trocken. Puuh.");

.. code-block:: pike

    // (5) Mit Objekt, welches den Extralook dynamisch erzeugt
    living->AddExtraLook("get_my_special_extralook", 3600, 0, 0,
                         this_object());
    // In diesem Fall muss this_object() natuerlich die Funktion
    // "get_my_special_extralook()" definieren, die einen String zurueckgibt

.. code-block:: pike

    // (6) Mit Objekt, welches den Extralook dynamisch erzeugt
    // Hier wird explizit die Blueprint uebergeben, der Extralook ist also
    // rebootfest.
    living->AddExtraLook("get_my_special_extralook", 3600, 0,
                         "extralookende", blueprint(this_object()));

.. code-block:: pike

    // Mit Objekt, was den Extralook und die Endemeldung dynamisch erzeugt
    // und keine festgelegte Existenzdauer hat, sondern sich aufgrund
    // eigener Konditionen entsorgt
    void set_extra_look(object living) {
      object dyntemplook = clone_object("/path/to/some/object");
      if(living->AddExtraLook("get_my_special_extralook", 0,
                              object_name(dyntemplook),
                              0, dyntemplook) == XL_OK)
        dyntemplook->SetProp(P_INTERNAL_EXTRA_LOOK, living);
      else
        dyntemplook->remove();
    }

    // entsprechendes Objekt:
    varargs int remove(int silent) {
      object ob = QueryProp(P_INTERNAL_EXTRA_LOOK);
      // wenn der Spieler da ist, entfernen wir den Look regulaer
      if(objectp(ob))
        ob->RemoveExtraLook(object_name(this_object()));
      return ::remove(silent);
    }

    void reset() {
      if(!random(10))
        remove();
      else
        ::reset();
    }

SIEHE AUCH
----------

   Verwandt:
     :doc:`RemoveExtraLook`, :doc:`../props/P_INTERNAL_EXTRA_LOOK`
   Sonstiges:
     :doc:`../sefun/replace_personal`, :doc:`../sefun/break_string`
   Fuer Spielerobjekte:
     :doc:`../props/P_EXTRA_LOOK`

5. Juni 2017 Gloinson
