// MorgenGrauen MUDlib
//
// class.h -- Definitionen fuer Klassen von Objekten und Lebewesen.
//            Siehe AddClass()/is_class_member() in /std/thing/description.c
//
// $Id: class.h 9552 2016-04-24 09:53:13Z Arathorn $

#ifndef __CLASS_H__
#define __CLASS_H__

#define CLASSDB "/p/daemon/classdb"

/* -------------------- Klassen fuer "Lebewesen" -------------------- */

#define CL_ANIMAL "tier"
#define CL_ARACHNID "spinne"  // und Spinnenartige
#define CL_BIRD "vogel"
#define CL_DARKELF "dunkelelf"
#define CL_DEMON "daemon"
#define CL_DRAGON "drache"
#define CL_DWARF "zwerg"
#define CL_ELF "elf"
#define CL_ELEMENTAL "elementarwesen"
#define CL_FELINE "feline"
#define CL_FISH "fisch"
#define CL_FROG "frosch"
#define CL_FURIE "furie"
#define CL_HARPY "harpyie"
#define CL_GHOST "geist"
#define CL_GHOUL  "ghoul"
#define CL_GIANT "riese"
#define CL_GNOME "gnom"
#define CL_GOBLIN "goblin"
#define CL_HOBBIT "hobbit"
#define CL_HOBGOBLIN "hobgoblin"
#define CL_HUMAN "mensch"
#define CL_ILLUSION "illusion" // Bsp. beschworene Illusionen
#define CL_INSECT "insekt"
#define CL_LIVING "lebewesen"
#define CL_MAMMAL "saeugetier"
#define CL_MAMMAL_LAND "landsaeuger"
#define CL_MAMMAL_WATER "wassersaeuger" // Damit man Delphinen nicht das Haar wuschelt
#define CL_ORC "ork"
#define CL_PLANT "pflanze"
#define CL_REPTILE "reptil"
#define CL_SHADOW "schattenwesen"
#define CL_SKELETON "skelett"
#define CL_SLIME "schleimwesen" // Fuer Einzeller und aehnliches
#define CL_SNAKE "schlange"
#define CL_TROLL "troll"
#define CL_UNDEAD "untoter"
#define CL_VAMPIRE "_cl_vampir"
#define CL_ZOMBIE "zombie"
#define CL_SHAPECHANGER "gestaltwandler" // Bsp. Werwoelfe etc.
#define CL_GOLEM "golem"
#define CL_CONSTRUCT "konstrukt"

/* ------------------ Klassen nach Bewegungsmethoden ----------------- */

#define CL_WALKING  "gehend"
#define CL_CRAWLING "kriechend"
#define CL_SWIMMING "schwimmend"
#define CL_FLYING   "fliegend"

/* -------------------- Klassen fuer Gegenstaende -------------------- */

#define CL_EXPLOSIVE "sprengstoff"
#define CL_DISEASE "krankheit"
#define CL_CURSE "fluch"
#define CL_INORGANIC "anorganisch"
#define CL_POISON "gift"
#define CL_POISONOUS "giftiges"
#define CL_AMMUNITION "munition"
// Es kann auch fuer Gegenstaende CL_ILLUSION verwendet werden.

#define CL_ALL ({CL_ANIMAL, CL_ARACHNID, CL_BIRD, CL_DEMON, CL_DRAGON, \
                CL_DWARF, CL_ELF, CL_ELEMENTAL, CL_FELINE, CL_FISH, CL_FROG, \
                CL_FURIE, CL_HARPY, CL_VAMPIRE,\
                CL_GHOST, CL_GHOUL, CL_GIANT, CL_GNOME, CL_GOBLIN, CL_HOBBIT, \
                CL_HOBGOBLIN, CL_HUMAN, CL_INORGANIC, CL_INSECT, CL_LIVING, \
                CL_MAMMAL, CL_MAMMAL_LAND, CL_MAMMAL_WATER, CL_ORC, CL_PLANT, \
                CL_REPTILE, CL_SHADOW, CL_SKELETON, CL_SLIME, CL_SNAKE, \
                CL_TROLL, CL_UNDEAD, CL_ZOMBIE, CL_SHAPECHANGER, CL_WALKING, \
                CL_CRAWLING, CL_SWIMMING, CL_FLYING, CL_EXPLOSIVE, \
                CL_DISEASE, CL_CURSE, CL_POISON, CL_POISONOUS, \
                CL_AMMUNITION, CL_ILLUSION, CL_GOLEM, CL_CONSTRUCT })

#endif

