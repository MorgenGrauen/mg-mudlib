SearchPath()
============

FUNKTION
--------
::

     public int SearchPath(string from, string to, int para,
                           closure callback)

DEFINIERT IN
------------
::

     /p/daemon/pathd.c
     <sys/path.d>

ARGUMENTE
---------
::

     from     - Der Startpunkt
     to       - Der Endpunkt
     para     - Die Parawelt in der gesucht wird (Normalwelt: 0)
     callback - Closure, die am Ende der Pfadsuche gerufen wird

BESCHREIBUNG
------------
::

    Diese Funktion berechnet - sofern moeglich - den kuerzesten Pfad zwischen
    <from> und <to> in der (Para-)Welt <para>.

    

    Die Pfadsuche wird anhand von Daten ueber von Spielern gelaufene Wege
    durchgefuehrt. D.h. Gebiete, die von Spielern nicht (in letzter Zeit mal)
    betreten werden, sind auch dem Pfaddaemon nicht bekannt. Auch kann es
    Gebiete geben, wo zwar gebietsintern Pfade bekannt sind, aber keine Wege
    in den Rest vom MG.

    Da diese Suche im Allgemeinen SEHR aufwendig sein kann, wird sie meistens
    nicht sofort fertig, sondern dauert eine Weile. Wenn sie fertig ist, wird
    die Closure <callback> aufgerufen und ihr die Argumente <from>, <to>,
    <parawelt>, <kosten>, <path> und <cmds> uebergeben. Die Bedeutung
    dieser Argumente ist unten erklaert.

    Eine Suche nach einem Pfad in einer Parawelt fuehrt durch Raeume der
    gewuenschen Parawelt und durch Raeume der Normalwelt.

RUeCKGABEWERT
-------------
::

     1      - Suche wurde gestartet
     2      - Pfad gefunden (und callback gerufen)
    -1      - es laeuft schon ein Suchauftrage fuer die UID des anfragenden
              Objektes
    -2      - es laufen bereits zuviele Suchanfragen
    -3      - <from> und/oder <to> sind nicht bekannt

    

    An <callback> uebergebene Argumente am Ende der Pfadsuche:
        <from> - Startpunkt des Weges (string)
        <to>   - Zielpunkt des Weges (string)
        <para> - Parawelt des Weges (int)
        <costs>- Kosten des Wege. (int) Je hoeher, desto
                 laenger/unguenstiger. 0, wenn kein Pfad gefunden
        <path> - Array mit Raumnamen, die durchlaufen werden (string*)
                 0, wenn kein Pfad gefunden
        <cmds> - Array mit Kommandos zum Ablaufen des Weges (string*)
                 0, wenn kein Pfad gefunden

BEMERKUNGEN
-----------
::

   Es ist natuerlich nicht dazu gedacht, Spielern fertige Routen zwischen
   Orten zu geben - bzw. nur in Ausnahmefaellen.
   Pfadabfrgen werden geloggt.

   Die Angabe <costs> sagt grob etwas ueber die Laenge und vor allem ueber die
   "Qualitaet" des Pfades aus. Die steigt bei Paraweltwechseln, wenig
   abgelaufenen Verbindungen zwischen Raeumen oder wenn eine Verbindung kein
   normaler Ausgang ist.

   Die Closure <callback> sollte nicht zuviele Ticks verbrauchen.

BEISPIEL
--------
::

   #include <pathd.h>
   void suchergebnis(string from, string to, int para, int costs, string*
        path, string* cmds) {
        tell_object(find_player("zesstra"), sprintf(
            "Ergebnis Pfadsuche von %s nach %s in Para %d fuer %d:\n %O\n %O\n",
            from, to, para, costs, path, cmds));
   };

   ...
   mixed res=PATHD->SearchPath("/gilden/abenteurer",
                               "/d/ebene/dancer/lucky/room/pova_la3",
                               0, #'suchergebnis);


22.12.2015, Zesstra

