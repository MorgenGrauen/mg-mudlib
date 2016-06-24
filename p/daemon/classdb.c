// MorgenGrauen MUDlib
//
// /obj/classd.c -- master object for classes.
//
// Soll irgendwann mal Klassen aehnlich der Material-DB verwalten, momentan
// machts es erstmal einfach eine Ersetzung von Aliasen und ein Hinzufuegen
// von impliziten Klassen.
//
// $Id$
#pragma strict_types,save_types
#pragma no_clone, no_shadow, no_inherit
#pragma verbose_errors,combine_strings
#pragma pedantic, range_check, warn_deprecated

#include <class.h>

// Mapping von Aliasen fuer Klassen. Aliase duerfen nicht rekursiv sein.
private nosave mapping classaliases =
  ([
   ]);
// Mapping von impliziten Klassen fuer eine gegebene Klasse. Implizite Klassen
// sind als Array von Klassennamen gegeben. Implizite Klassen duerfen zur Zeit
// nicht rekursiv sein und keine Aliase enthalten.
private nosave mapping implicitclasses =
  ([
     CL_ZOMBIE:       ({CL_UNDEAD}),
     CL_SKELETON:     ({CL_UNDEAD}),
     CL_GHOUL:        ({CL_UNDEAD}),
     CL_VAMPIRE:      ({CL_UNDEAD}),
     CL_HOBGOBLIN:    ({CL_GOBLIN}),
     CL_MAMMAL_LAND:  ({CL_MAMMAL, CL_ANIMAL}),
     CL_MAMMAL_WATER: ({CL_MAMMAL, CL_ANIMAL}),
     CL_SNAKE:        ({CL_REPTILE}),
     CL_ARACHNID:     ({CL_ANIMAL}),
     CL_BIRD:         ({CL_ANIMAL}),
     CL_FISH:         ({CL_ANIMAL}),
     CL_FROG:         ({CL_ANIMAL}),
     CL_INSECT:       ({CL_ANIMAL}),
     CL_MAMMAL:       ({CL_ANIMAL}),
     CL_REPTILE:      ({CL_ANIMAL}),
     CL_SNAKE:        ({CL_ANIMAL}),
     CL_GOLEM:        ({CL_CONSTRUCT}),
   ]);

protected void create() {
}

/** Gibt den Alias der Klasse <cls> zurueck.
 * @param[in] cls string - Klassenname
 * @return string* - Alias von Klasse <cls>
 */
public string * QueryClassAlias(string cls) {
  if (!stringp(cls))
      raise_error(sprintf(
            "Got wrong argument, expected string, got %.50O\n",cls));
  return copy(classaliases[cls]) || cls;
}

/** Gibt die impliziten Klassen der Klasse <cls> zurueck.
 * @param[in] cls string - Klassenname
 * @return string* - Liste von impliziten Klassen von <cls>
 */
public string * QueryImplicitClasses(string cls) {
  if (!stringp(cls))
      raise_error(sprintf(
            "Got wrong argument, expected string, got %.50O\n", cls));
  return copy(implicitclasses[cls]) || ({});
}

/** Adds implicit classes to the classes given in <classes>.
 * Ausserdem werden Klassenaliase aufgeloest.
 * @param[in] classes string* - Klassenliste.
 * @return string* - neue Klassenliste
 * @attention Kann das uebergebene Array aendern. Das Ergebnisarray muss nicht
 * unique sein, sondern kann Dubletten enthalten.
 */
public string * AddImplicitClasses(string *classes) {
  if (!pointerp(classes))
      raise_error(sprintf(
          "Got wrong argument, expected array, got %.50O\n",classes));
  foreach(string cls : &classes) {
      // zuerst Aliase ersetzen
      if (member(classaliases, cls))
          cls = classaliases[cls];
      // dann implizite Klassen addieren.
      if (member(implicitclasses, cls))
          classes += implicitclasses[cls];
  }
  return classes;
}

