access_rights()
===============

FUNKTION
--------
::

    int access_rights(string euid, string file);

DEFINIERT IN
------------
::

    access_rights.c (muss man selber schreiben)

PARAMETER
---------
::

    euid
        Die euid desjenigen, der auf das Verzeichnis schreiben will
    file
        Name der Datei, auf die zugegriffen werden soll

BESCHREIBUNG
------------
::

    access_rights() wird immer dann aufgerufen, wenn jemand, der nicht
    sowieso schon schreibberechtigt ist, in die Datei/das Verzeichnis file
    schreiben oder loeschen will.

    Anhand von euid kann man dann entscheiden, ob der Schreibzugriff erlaubt
    wird oder nicht.

RUECKGABEWERT
-------------
::

    0, wenn der Zugriff verweigert wird,
    1, wenn der Zugriff erlaubt wird.

BEISPIELE
---------
::

    /* /d/inseln/wargon/access_rights.c */

    int access_rights(string euid, string file)
    {
      string dir, rest;

      // Catweazle darf auf alles zugreifen (*argl* ;^)
      if (euid == "catweazle")
        return 1;

      // Rechte auf einzelne Verzeichnisse ermitteln:
      if (sscanf(file, "%s/%s", dir, rest) != 2)
        rest = file;

      // Jof und Boing duerfen an Tarko Reub rumpfuschen:
      if (dir == "tarko" && (euid == "jof" || euid == "boing"))
        return 1;

      // Anthea darf die Karten von Aurora und der Piratenhoehle bearbeiten:
      if (dir == "MAPS" && 
          member( ({"Aurora", "Piratenhoehle" }), rest) >= 0 && 
          euid == "anthea")
        return 1;

      // alle anderen duerfen nicht!
      return 0;
    }

BEMERKUNGEN
-----------
::

    file ist immer relativ zu dem Verzeichnis, in dem das access_rights.c
    liegt! Will also jemand auf /d/inseln/wargon/tarko/macros.h schreiben,
    wird file "tarko/macros.h" uebergeben.

    In Verzeichnissen von Magiern mit einem Level >= ELDER_LVL wird das
    access_rights.c NICHT ausgewertet (da damit andere Magier zB. an
    Erzmagierrechte gelangen koennten).

    Es wird immer nur EIN access_rights.c ausgewertet, naemlich das in der
    tiefsten Verzeichnisebene.

    Man kann sowohl in seinen Regionsverzeichnissen als auch in seinen
    Homeverzeichnissen access_rights.c-Dateien anlegen.

    GANZ WICHTIG!!!
    Fuer die Dateien, die evtl. von anderen angelegt werden, ist man immer
    noch selbst verantwortlich! Wenn jemand also ein Gebiet bei Dir an-
    schliesst, muss es erst von den verantwortlichen Regionsmagiern abgesegnet
    sein!

