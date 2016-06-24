#pragma strong_types,rtt_checks

#include <items/kraeuter/kraeuter.h>

inherit STDPLANT;

void create()
{
  ::create();
  //printf("create(): %O\n",load_name());
  //replace_program();
  // Wenn es das Standardfile geclont wird, wird customizeMe(0) gerufen, was
  // den Kraeuter-VC dazu bringt, dieses mit den Daten des Krautes zu
  // konfigurieren, was der VC gerade erzeugt hat.
  // load_name() bleibt fuer alle VC-erzeugten Kraeuter-Blueprints das
  // PLANTITEM.
  // hier darf _nicht_ previous_object()->CustomizeObject() verwandt werden,
  // da nur die Blueprint wirklich vom VC erzeugt wird. Fuer die Clones
  // ruft der Driver den VC nicht jedesmal erneut auf.
  if (load_name() == PLANTITEM)
    customizeMe(0);
}

string GetOwner() {
  return "Padreic";
}
