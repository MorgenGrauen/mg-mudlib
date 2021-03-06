is_class_member()
=================

FUNKTION
--------
::

     int is_class_member(string|string* class);

DEFINIERT IN
------------
::

     /std/thing/description.c

ARGUMENTE
---------
::

     string/string* class	- String oder Stringarray der Klasse(n)

BESCHREIBUNG
------------
::

     Es wird getestet, ob das Objekt in eine der in class angegebenen
     Klassen faellt. In diesen Test werden die folgenden Eigenschaften des
     Objektes einbezogen:

       1. Die Rasse des Objektes (bei Lebewesen),
       2. die IDs des Objektes und
       3. die explizit angegebenen Klassen des Objektes.
       4. einigen impliziten Klassen, die sich aus den Klassen in 3 ergeben.

     Die moeglichen Klassen sind in /sys/class.h definiert. Momentan stehen
     folgende Klassen zur Verfuegung:

     CL_AMMUNITION
          Das Objekt eignet sich als Munition.
     CL_ANIMAL
          Das Objekt ist ein Tier.
     CL_ARACHNID
          Das Objekt in ein Spinnenwesen.
     CL_BIGBANG
          Dieses Objekt kann mehreren Lebewesen auf einmal Schaden zufuegen.
     CL_BIRD
          Ein Vogel.
     CL_CRAWLING
          Dieses Wesen bewegt sich kriechend.
     CL_CURSE
          Das Objekt ist ein Fluch (zB. ein Sprachfluch).
     CL_DEMON
          Bei dem Objekt handelt es sich um einen Daemon.
     CL_DISEASE
          Eine Krankheit.
     CL_DRAGON
          Ein Drache.
     CL_DWARF
          Fuer unsere kleinen Gaeste...
     CL_ELF
          Elfen aller Art.
     CL_ELEMENTAL
          Ein Elementar irgendeiner Art. Material setzen waere angebracht.
     CL_EXPLOSIVE
          Bei dem Objekt handelt es sich um einen Sprengstoff.
     CL_FELINE
          Felinen und andere katzenartigen Lebewesen.
     CL_FISH
          Fische - keine Meeressaeuger!
     CL_FLYING
          Dieses Wesen bewegt sich fliegend.
     CL_FROG
          Froesche - auch gefroschte Spieler.
     CL_GHOST
          Geister und geisterhafte Wesen.
     CL_GHOUL
          Ein Ghoul. Er faellt automatisch in die Klasse CL_UNDEAD.
     CL_GIANT
          Ein riesiges Lebewesen.
     CL_GNOME
          Ein Gnom.
     CL_GOBLIN
          Ein Goblin.
     CL_HOBBIT
          Ein Hobbit.
     CL_HOBGOBLIN
          Ein Hobgoblin. Er faellt automatisch auch in die Klasse CL_GOBLIN.
     CL_HUMAN
          Ein Mensch.
     CL_INORGANIC
          Anorganische Lebewesen wie Metallmonster
     CL_INSECT
          Insekten (Nicht mit Spinnen verwechseln)
     CL_LIVING
          Lebewesen im allgemeinen.
     CL_MAMMAL
          Saeugetiere.
     CL_MAMMAL_LAND
          Landsaeugetiere
     CL_MAMMAL_WATER
          Meeressaeuger.
     CL_ORC
          Ein Ork.
     CL_PLANT
          Pflanzen und pflanzenartige Monster.
     CL_POISON
          Das Objekt ist selbst ein Gift
     CL_POISONOUS
          Das Objekt kann einen Spieler/NPC vergiften.
     CL_REPTILE
          Reptilien.
     CL_SHADOW
          Schattenwesen.
     CL_SKELETON
          Ein Skelett. Es faellt automatisch in die Klasse CL_UNDEAD.
     CL_SLIME
          Fuer Einzeller und aehnliches Schleimgetier
     CL_SNAKE
          Schlangen.
     CL_SWIMMING
          Dieses Wesen bewegt sich schwimmend.
     CL_TROLL
          Ein Troll.
     CL_UNDEAD
          Ein untotes Lebewesen.
     CL_WALKING
          Dieses Wesen bewegt sich gehend.
     CL_VAMPIRE
          Ein Vampir. Er faellt automatisch in die Klasse CL_UNDEAD.
     CL_ZOMBIE
          Ein Zombie. Er faellt automatisch in die Klasse CL_UNDEAD.

     Implizite Klassen:
     Bei einigen Klassen wird im AddClass() automatisch eine oder mehrere
     weiterer Klassen hinzugefuegt und im RemoveClass() die entsprechenden
     impliziten Klassen auch automatisch entfernt.
     Wuenscht man diese impliziten Klassen nicht, muss man nach dem AddClass()
     diese mittels eines entsprechenden RemoveClass() entfernen.
     Die impliziten Klassen einer Klasse lassen sich mit Hilfe der Funktion
     QueryImplicitClasses() in CLASSDB herausfinden:
       CLASSDB->QueryImplicitClasses(...)
     Momentan sind dies:
     CL_ZOMBIE:       CL_UNDEAD
     CL_SKELETON:     CL_UNDEAD
     CL_GHOUL:        CL_UNDEAD
     CL_VAMPIRE:      CL_UNDEAD
     CL_HOBGOBLIN:    CL_GOBLIN
     CL_MAMMAL_LAND:  CL_MAMMAL, CL_ANIMAL
     CL_MAMMAL_WATER: CL_MAMMAL, CL_ANIMAL
     CL_SNAKE:        CL_REPTILE
     CL_ARACHNID:     CL_ANIMAL
     CL_BIRD:         CL_ANIMAL
     CL_FISH:         CL_ANIMAL
     CL_FROG:         CL_ANIMAL
     CL_INSECT:       CL_ANIMAL
     CL_MAMMAL:       CL_ANIMAL
     CL_REPTILE:      CL_ANIMAL
     CL_SNAKE:        CL_ANIMAL

RUeCKGABEWERT
-------------
::

     1, wenn das Objekt in eine der angegebenen Klassen faellt, ansonsten 0.

SIEHE AUCH
----------
::

     AddClass(), RemoveClass(), /std/thing/description.c
     P_CLASS


20.01.2015, Zesstra

