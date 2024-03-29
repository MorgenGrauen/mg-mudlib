AddVItem()
==========

FUNKTION
--------

  public varargs void AddVItem(string key, int refresh, mapping shadowprops, string path, mapping props)


DEFINIERT IN
------------

  /std/container/vitems.c

ARGUMENTE
---------

  key
    Eindeutige Bezeichnung des vItems und mit dieser wieder loeschbar.
    Das vItem bekommt dies u.U. auch als ID (s.u.)
  refresh
    Refresheinstellungen des vItems, nachdem es mitgenommen wurde
  shadowprops
    Properties des vItems *im Raum* (optional, wenn <path> genutzt)
  path
    Vorlage/Templat des vItems und Blueprint fuer Clones (optional)
  props
    Properties, welche in Clones von der Vorlage gesetzt werden (optional)

BESCHREIBUNG
------------

  Dem Container/Raum wird ein virtuelles Item hinzugefuegt. Dieses ist
  kein im Container vorhandenes reales Objekt, kann aber (weitgehend) wie ein
  solches untersucht und behandelt werden.

  Falls das vItem ein reales Objekt als Templat hat (d.h. <path> angegeben
  wird), kann das vItem standardmaessig mitgenommen werden, es wird dafuer
  automatisch erzeugt. (Das Mitnehmen ist aber wie immer mit P_NOGET
  verhinderbar.)

  Das Mapping <shadowprops> enthaelt die Properties, welche das vItem
  im Container beschreiben. Hat das vItem ein Templat, werden alle
  Properties, welche *nicht* in <shadowprops> enthalten sind, aus dem
  Templat entnommen. Auf diese Weise kann man einem Objekt im Raum
  unterschiedliche Eigenschaften geben.

  Wird ein vItem mitgenommen, wird <path> geclont und die im Argument <props>
  angegebenen Properties in ihm gesetzt. Auf diese Weise kann man das
  genommene Objekt noch individuell konfigurieren.

  Werden fuer ein reines vItem ohne Vorlage keine P_IDS in <shadowprops>
  angegeben, so bekommt das vItem den string <key> als ID, damit es irgendwie
  im Raum ansprechbar ist.

  Zu beachten ist: in <shadowprops> enthaltene Properties *ersetzen* (nicht
  ergaenzen) im Regelfall diejenigen im Templat *und* in <props>. In <props>
  enthaltene Properties *ersetzen* wiederum diejenigen im Templat.
  Ausnahmen sind Properties, welche ein Mapping oder ein Array enthalten:
  Deren Inhalt wird *hinzugefuegt*. Dies betrifft insbesondere die Properties:
  P_DETAILS, P_READ_DETAILS, P_SMELLS, P_SOUNDS, P_TOUCH_DETAILS, P_IDS,
  P_CLASS, P_ADJECTIVES.
  Will man dies nicht, kann man den Propertynamen so angeben:
  VI_PROP_RESET(P_DETAILS), was dazu fuehrt, dass P_DETAILS vor dem
  Hinzufuegen geloescht wird.

  Nachdem ein vItem mitgenommen wurde, ist es im Raum nicht mehr ansprechbar.
  Es wird es je nach <refresh> wieder verfuegbar:

    - VI_REFRESH_NONE
      Es wird erst nach dem Neuladen/Neuerstellen des Containers wieder
      verfuegbar.
    - VI_REFRESH_ALWAYS
    - VI_REFRESH_REMOVE
      Es wird mit dem naechsten Reset wieder verfuegbar, wenn das Item
      genommen wurde (auch wenn es danach wieder um Raum abgelegt wurde).
      *Vorsicht*
    - VI_REFRESH_DESTRUCT
      Es wird mit dem naechsten Reset wieder verfuegbar, wenn das geclonte
      Objekt nicht mehr existiert.
    - VI_REFRESH_MOVE_HOME
      Das geclonte Objekt wird im naechsten Reset 'zurueckgeholt', d.h.
      zerstoert und das vItem ist wieder im Raum verfuegbar.
    - VI_REFRESH_INSTANT
      Es ist abweichend vom Normalverhalten sofort nach Mitnehmen wieder
      verfuegbar und mitnehmbar. **Vorsicht!**

  Die Refresh-Konstanten sind in /sys/container/vitems.h definiert und nur
  verfuegbar, wenn diese Datei inkludiert wird.


BEMERKUNGEN
-----------

  * vItems koennen geschachtelt werden, falls es sich um vItems mit Vorlagen
    handelt.
  * vItems sind verhaeltnismaessig aufwendig. Sie sollten nur verwendet
    werden, wenn sie einen Mehrwert ggue. einfachen Details haben. Es bringt
    nichts, jedes Detail gegen ein vItem zu ersetzen.
  * Es kann aber praktisch sein, komplexe Detailbaeume an einem vItem zu
    verankern, wenn man diesen Detailbaum ein- oder ausblenden will.

BEISPIELE
---------

.. code-block:: pike

  // Ein einfaches VItem ohne Templat, nicht nehmbar
  AddVItem("regal", VI_REFRESH_NONE,
           ([P_LONG: "Das Wandregal hat 3 schwarze Bretter.",
             P_GENDER: NEUTER,
             P_IDS: ({"wandregal"}), // "regal" hat es automatisch
             P_DETAILS: ([
               "bretter": "Die Regelbretter sind ganz zerkratzt.\n",
               "kratzer": "Da war wohl jemand achtlos.\n"
             ]),
             P_TOUCH_DETAILS: ([
               SENSE_DEFAULT: "Du erspuerste tiefe Kratzer.\n",
               "kratzer": Oha, die Kratzer sind ganz schoen tief!\n",
             ]),
             P_READ_DETAILS: ([SENSE_DEFAULT:
               "Auf dem Regal ist eingraviert: 'Dies ist ein vItem-Test.'\n"
             ]),
             P_SMELLS: ([SENSE_DEFAULT:
               "Das Regal riecht gar nicht. Wirklich nicht.\n"]),
           ]) );

  // Ein VItem mit Templat, nehmbar
  AddVItem("schaufel", VI_REFRESH_REMOVE, 0, "/items/schaufel");

  // Ein VItem mit Templat, nicht nehmbar
  AddVItem("schaufel", VI_REFRESH_NONE, ([P_NOGET:1]), "/items/schaufel");

  // Ein VItem, welches P_DETAIL komplett ueberschreibt und alle Details
  // loescht.
  AddVItem("schaufel", VI_REFRESH_REMOVE,
           ([VI_PROP_RESET(P_DETAILS): ([]) ]),
           "/items/schaufel");

  // Eine Zwiebel im Raum, eine andere Beschreibung hat als die im
  // Spielerinventar
  AddVItem("zwiebel", VI_REFRESH_REMOVE,
           ([P_LONG: "Die Zwiebel sieht etwas vertrocknet aus.\n",
             P_DETAILS: ([ ({"blatt","blaetter"}): "Sie haengen herab.\n"]),
           ]),
           "/items/kraeuter/zwiebel",
           // Die hier gesammelte Zwiebel ist besonders toll
           ([P_ZWIEBEL_TYP: EXTRA_STARK]) );

  // Eine modifizierte Eisenstange
  AddVItem("eisstange", VI_REFRESH_REMOVE,
           0,
           "/items/eisenstange",
           ([P_LONG:BS("Diese Eisstange eignet sich hervorragend dazu, "
                       "mit ihr auf jemanden einzuschlagen.\n"),
             P_SHORT:"Ein Eisstange.",
             P_NAME: "Eisstange",
             VI_PROP_RESET(P_IDS): ({"stange", "eisstange"}),
             VI_PROP_RESET(P_MATERIAL): ([MAT_ICE: 100]),
             VI_PROP_RESET(P_DAM_TYPE): ({DT_BLUDGEON, DT_COLD}),
             P_WEIGHT: 5000,
             P_VALUE: 500,
             VI_PROP_RESET(P_SMELLS): ([SENSE_DEFAULT:
               "Das Eis riecht nach Vanille!\n"]),
            ])
          );

  // Ein Ueberraschungs-Ei - verwandelt sich beim Nehmen in was anderes.
  // (Keine tolle Idee, soll nur zeigen, dass es geht.)
  AddVItem("ue-ei", VI_REFRESH_REMOVE,
         ([P_SHORT:"Ein Ue-Ei.",
           P_LONG: BS("Da liegt tatsaechlich ein Ue-Ei auf dem Boden. "
             "Wenn man es nimmt, verwandelt es sich in etwas anderes."),
           VI_PROP_RESET(P_IDS): ({"ue-ei","ei"}),
           ]),
         "/items/schaufel"
        );


SIEHE AUCH
----------

  :doc:`RemoveVItem`, :doc:`AddItem`, :doc:`RemoveItem`,
  /doc/std/vitems

Last modified: 03.04.2019, Zesstra

