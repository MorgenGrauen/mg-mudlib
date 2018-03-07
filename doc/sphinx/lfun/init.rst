init()
======

FUNKTION
--------

  public varargs void init(object origin);

DEFINIERT IN
------------

  /std/room/description.c

ARGUMENTE
---------

	keine

BESCHREIBUNG
------------

  init() wird immer dann aufgerufen, wenn ein lebendes Objekt in die
  Naehe anderer Objekte bewegt wird oder ein nicht lebendiges Objekt
  in die Naehe von Lebewesen gelangt. init() wird dabei in allen
  Objekten aufgerufen, bei denen es notwendig ist.

  Der Hauptzweck dieser Funktion besteht darin, den Objekten
  untereinander ihre jeweiligen Befehle zugaenglich zu machen.
  Waehrend dies in anderen MudLibs durch eine Reihe von
  add_action()-Aufrufen im Rumpf von init() geschah, geschieht dies in
  der MG-MudLib bei Objekten, die /std/thing/commands.c erben
  (das sind zB. alle Standardobjekte) quasi automatisch
  (dort werden die Befehle dann per AddCmd() im create() angemeldet,
  man spart sich die Implementierung von init(). Der andere Weg ist aber
  natuerlich immer noch moeglich.

  Der Ablauf der init()-Kette ist wie folgt:

    1. Ist das Objekt X, welches ins Zielobjekt D bewegt wurde, ein
       Lebewesen, so wird in D init() aufgerufen, wobei this_player() auf
       X gesetzt ist.
    2. Dann wird fuer jedes Objekt C in D folgendes ausgefuehrt:

      + Ist C ein Lebewesen, dann wird init() in X aufgerufen, wobei
        this_player() auf C gesetzt ist.
      + Ist X ein Lebewesen, dann wird init() in C aufgerufen, wobei
        this_player() auf X gesetzt ist.

    3. Schliesslich wird in dem Fall, dass D lebendig ist, in X init()
       aufgerufen, wobei this_player() auf D gesetzt ist.

RUeCKGABEWERT
-------------

  keiner

BEMERKUNGEN
-----------

  * Wenn man init() ueberschreibt und vergisst, in etwaigen geerbten Programmen
    das init() zu rufen, resultiert das in schweren Bugs

BEISPIELE
---------

  D sei ein Raum, in dem sich das Lebewesen L1 sowie die Gegenstaende
  N1 und N2 befinden.
  Betritt ein Spieler X diesen Raum, so werden folgende init()s
  aufgerufen:

    1. D->init();  // this_player() == X
    2. X->init();  // this_player() == L1
    3. L1->init(); // this_player() == X
    4. N1->init(); // this_player() == X
    5. N2->init(); // this_player() == X

  Gelangt dagegen ein nichtlebendiges Objekt nach D, so sieht das Ganze
  wie folgt aus:

    1. X->init();    // this_player() == L1

SIEHE AUCH
----------

  :doc:`exit`, :doc:`AddCmd`, :doc:`NotifyInsert`

  add_action(E)

Last modified: 07.03.2018, Zesstra

