P_FISH
======

NAME
----
::

    P_FISH                        "fish"

DEFINIERT IN
------------
::

    /sys/fishing.h

BESCHREIBUNG
------------
::

    Enthaelt Einstellungen zu allem, was mit Fischen zu tun hat. 
    Kann in Fischen, Raeumen und Koedern gesetzt werden. Die verfuegbaren 
    Optionen und Funktionsweisen sind in den nachfolgenden Abschnitten 
    aufgefuehrt.

    Fische:
    *******
    Die Property legt zusaetzliche Eigenschaften fest:

      F_NOROTTEN
        Fisch fault nicht; ggf. sollte hier auch gleich F_NOHEAL gesetzt 
        werden, weil sonst eine unverderbliche tragbare Tanke erzeugt wuerde.
        Wenn sie verderben koennen, beginnen Fische nach 10 min zu verderben
        und werden danach alle 2 Minuten schlechter, d.h. reduzieren ihren
        Heilwert. Nach vier Schritten ist ein Fisch komplett vergammelt.
      F_NOTHUNGRY
        Fisch frisst Koeder nur, wenn er auch wirklich nachher an der Angel
        haengt. Ist er zu schwer fuer die Angel und reisst ab, bleibt der
        Koeder erhalten.
      F_REPLACE
        Fisch soll sich beim Einholen der Angel verwandeln, technisch:
        wenn er geclont und in die Angel bewegt wurde. Hierzu ist die 
        Funktion ReplaceFish() im Fisch zu definieren, die sich um die
        Verwandlung kuemmert (z.B. Monster clonen und Fisch zerstoeren,
        Beispiel siehe unten).
      F_NOHEAL
        Fisch heilt nicht bei Verzehr. 
        Wenn er heilt, ist die Heilung ist abhaengig vom Gewicht des Fisches
        und limitiert auf den Wertebereich 0-40 LP. Der maximale Heilwert
        ist bei 1280 g Gewicht erreicht und skaliert in Schritten von 32 g
        linear. Pro LP, der geheilt wird, muss der Spieler Platz fuer 3
        Food haben (ueber eat_food() realisiert). Die Heilung wird ueber
        buffer_hp() mit einer Rate von 5 LP pro Heartbeat.

    Raum (OPTIONAL):
    ****************
    Legt die Fischdichte des Gewaessers fest. Der eingestellte Wert wirkt 
    sich auf die Wartezeit aus, die der Angler verbringen muss, bis ein 
    Fisch anbeisst. Berechnung im Detail (alle Zahlenwerte in Sekunden):
    - Basis-Wartezeit: delay = 80
    - Die Werte von P_FISH von Raum und Koeder werden addiert:
      summe = raum->QueryProp(P_FISH) + koeder->QueryProp(P_FISH)
      -> positive Werte (Bonus) werden auf 60 begrenzt und mit Zufalls-
         komponente von <delay> abgezogen:
         delay -= random(summe)
      -> negative Werte (Malus) werden auf -300 begrenzt und mit Zufalls-
         komponente auf <delay> aufgeschlagen:
         delay += random(-summe)

    Zusaetzlich wird ein weiterer Malus auf die Wartezeit faellig, falls 
    P_WATER in der Angel und/oder P_WATER im Koeder nicht zum aktuellen 
    Gewaesser passen. Der Malus betraegt jeweils 60+random(60) Sekunden.

    Der Standardwert fuer P_FISH im Raum ist 0 und bedeutet 100 % Bestand.
    Positive Werte erhoehen die Dichte, negative senken sie. Positive Werte 
    sollten nicht >100 sein.

    Sofern sich die Werte fuer P_FISH in den empfohlenen Grenzen bewegen,
    koennen Abzuege fuer falsches Gewaesser ueblicherweise kaum durch
    Boni auf Angel oder Koeder kompensiert werden. Ausserdem ist zu
    bedenken, dass es keine Boni fuer richtige Gewaesserwahl gibt.

    Koeder (OPTIONAL):
    ******************
    Ein Koeder kann mittels P_FISH die Fischdichte modifizieren, um hierueber
    ein besseres Beissen der Fische zu simulieren (reduziert die Wartezeit
    beim Angeln, siehe oben unter "Raum"). Wenn also der Raum einen Wert
    von -100 gesetzt hat und der Koeder +100, entspricht die Fischdichte im 
    Gewaesser wieder dem Normalwert.


BEISPIEL
--------
::

    protected void create() {
      [...]
      SetProp(P_FISH, F_REPLACE);
      [...]
    }
 
    // ReplaceFish() wird per call_out() mit Funktionsnamen (d.h. als String)
    // gerufen, daher muss die Sichtbarkeit public oder static sein.
    static void ReplaceFish() {
      tell_object(this_player(), break_string("Der Katzenhai glitscht Dir 
        "aus der Hand und streift seine Haut ab. Vor Dir steht eine "
        "niedliche, schwarze Katze!", 78));
      tell_room(environment(this_player()), break_string(
        this_player()->Name(WEM)+" glitscht ein Katzenhai aus der Hand "
        "und streift seine Haut ab. Ueberraschend steht eine niedliche, "
        "schwarze Katze vor Dir!", 78));
      object monster = clone_object("/d/irgendwo/irgendwer/mon/haikatze");
      monster->move(environment(TP), M_GO|M_SILENT);
      remove();
    }

SIEHE AUCH
----------
::

    Properties:   P_WATER, P_FOOD
    Basisobjekte: /std/items/fishing/fish.c, /std/items/fishing/haken.c
                  /std/items/fishing/koeder.c /std/items/fishing/angel.c
    Methoden:     GetAquarium(L), eat_food(), buffer_hp()


Zuletzt geaendert: 2020-Mar-09, Arathorn

