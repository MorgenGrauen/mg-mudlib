Homemud
=======

Abhaengigkeiten
---------------

    Um den Treiber mit allen Features, die unsere Mudlib benutzt, zu bauen,
    benoetigt man folgende Pakete (die Namen koennen je nach Distribution
    abweichen):

    libjson0/json-c, libgnutls30, libsqlite3, libpcre3, libxml2, libgcrypt,
    libpcre3, libiconv (diese ist bei neueren Distributionen in libc6-dev
    enthalten).

    Beachte, dass man auf debianbasierten System auch die entsprechenden
    Entwicklerpackete (`*-dev`) mitinstallieren muss, weil sie die notwendigen
    Header-Dateien der Files enthalten.

Anleitung fuer ein Homemud, unter Linux
---------------------------------------

    Mit den folgenden Schritten installiert man die gegenwaertige mudlib und
    einen Driver der Version LDMUD-3.6.4 oder https://github.com/zesstra/ldmud/releases/tag/mgreboot-20210507

    1. Lade den aktuellen Driver unter https://github.com/ldmud/ldmud herunter.
       Du kannst auch das Repo via git clonen.

    2. Lade die aktuelle Mudlib von
       https://mg.mud.de/gerrit/gitweb?p=mudlib-public.git herunter, entweder
       durch einen Klick auf "snapshot" oder mittels git clonen. Dabei kann
       sich git an unserem selbsterstellten SSL-Zertifikat stoeren, wenn Du
       Dich darum nicht kuemmern willst, nutze die Option
       :code:`-c http.sslVerify=false`.

    3. Gehe in das `src/`-Verzeichnis des drivers

    4. Im `src/`-Verzeichnis, fuehre :code:`./autogen.sh` aus (Hierfuer ist das Paket autoconf empfehlenswert.)

    5. Im src/-Verzeichnis, passe die Pfade in settings/morgengrauen an, und
       fuehre settings/morgengrauen aus

    6. im src/-Verzeichnis, kompiliere und installiere den Treiber mit
       :code:`make all` und :code:`make install-all`. (Hierfuer wird das Paket bison benoetigt.)

    7. Falls noch nicht geschehen, schiebe die Mudlib in den Ordner, den Du in
       `settings/morgengrauen` eingestellt hast

    Nun laesst sich das Homemud mit der installierten ldmud-binary starten.
    Du kannst als Argument noch einen alternativen Port angeben.

    8. Starte das mud mit der installierten ldmud-Binary. Mittels
       :code:`ldmud <port>` kannst Du auch den Port bestimmten, auf welchem
       das Homemud laeuft.

    9. Einloggen als "jof".

    Optional kannst Du *anschliessend* den Namen des Mudgottes von Jof auf
    etwas anderes aendern, das geht wie folgt:

.. code-block:: shell

      clone /obj/tools/MGtool
      xcall $me->SetProp(P_START_HOME, "/players/thomas/workroom");
      ende

    Auf der Shell:

.. code-block:: shell

      mv data/save/j/jof.o data/save/t/thomas.o
      mv data/secure/save/j/jof.o data/secure/save/t/thomas.o
      # (beachte den Namen des Unterverzeichnisses, es ist der erste Buchstabe
      # Deines Namens. )
      sed -i 's/jof/thomas/' data/secure/save/t/thomas.o
      mkdir -p players/thomas
      mv players/jof/workroom.c players/thomas/workroom.c

    Beachte hierbei allerdings, dass in Homemuds immer automatisch
    sichergestellt wird, dass "Jof" existiert. Falls noetig, wird diesr Char
    ohne Passwort angelegt. Ist Dein Homemud erreichbar, gibt "jof" bitte ein
    Passwort und lasse den Char bestehen. (Alternativ muss Du das File
    /secure/testmud.c anpassen.)

Beispielinstallation
--------------------

.. code-block:: shell

       cd <mudhome>
       git clone https://github.com/ldmud/ldmud.git
       cd ldmud.git/src
       ./autogen.sh
       settings/morgengrauen
       make all && make install-all
       cd <mudhome>
       # hier wurde der bin/ Ordner nicht angepasst, wir verschieben noch die
       # Binary
       mv bin.install/ldmud bin/ldmud
       tar xvzf <mudlib-snapshot.tgz>
       # Alternative
       git clone https://mg.mud.de/gerrit/mudlib-public mudlib
       cd <mudhome>
       bin/ldmud
       # oder
       bin/ldmud -m <alternative path to mudlib> <alternative port>
       # Einloggen als Jof.

Letzte Aenderung: 06.07.2021, Bugfix

