#pragma strict_types,save_types,rtt_checks
#pragma pedantic,range_check
#pragma no_inherit,no_clone
#pragma no_shadow

#define _NEED_DROPMASTER_IMPLEMENTATION_
#include <dropmaster.h>
#include <defines.h>
#include <wizlevels.h>

struct random_event_s {
  string *names;
  int reduction;
  int *rnd_values;
  int *default_rnd_values;
  string creator;
};

// Mapping mit einem random_event_s als Wert pro Schluessel
// Schluessel: 0: globaler Eintrag, ansonsten BP-Namen oder Teile von Pfaden
// (/d/<eben>/<magier>/<gebiet>/)
private mapping randomEvents;

private struct random_event_s SanitizeRE(struct random_event_s re)
{
    // rnd_values, default_rnd_values und names muessen gleich lang sein -> auf
    // das kuerzeste kuerzen. Achtung: macht implizit eine Kopie - WICHTIG!
    int size = sizeof(re->rnd_values);
    if (pointerp(re->names))
        size = min(size, sizeof(re->names));
    if (pointerp(re->default_rnd_values)) {
      size = min(size, sizeof(re->default_rnd_values));
      re->default_rnd_values = re->default_rnd_values[0..size-1];
    }
    if (pointerp(re->names))
      re->names = re->names[0..size-1];

    re->rnd_values = re->rnd_values[0..size-1];

    return re;
}

// key==0 signifies the global key
public varargs void CreateRandomEvent(string key, int *rnd_val, 
                                      string *rnd_names, int red)
{
  struct random_event_s re = (<random_event_s>);
  
  if (!member(randomEvents,key)) {
    re->rnd_values = copy(rnd_val) || DFLT_RND_VALUES;
    // wenn rnd_val == 0 ist, ist das OK, dann wird spaeter immer
    // DFLT_RND_VALUES genommen.
    re->default_rnd_values = copy(rnd_val);
    // names darf auch 0 sein, wenn Defaultwerte genommen werden sollen.
    re->names = copy(rnd_names);
    re->reduction = red || RND_REDUCTION;
    re->creator = object_name(extern_call() ? previous_object() : this_object());

    randomEvents[key] = SanitizeRE(re);
  }
}

public varargs int ChangeRandomEvent(string key, int *rnd_val, int *rnd_val_dflt, 
                                      string *rnd_names, int red)
{
  if (process_call()) return -1;
  if (!member(randomEvents, key)) return -2;
  struct random_event_s re = randomEvents[key];
  // Schreibzugriff erlauben? Nur fuer Erschafferobjekte und EM.
  // Bemerkung: RM+ haben Zugriff, weil sie Schreibzugriff auf das
  // Erschafferobjekt haben.
  if (extern_call()
      && re->creator != object_name(previous_object())
      && !ARCH_SECURITY)
    return -1;

  if (pointerp(rnd_val))
    re->rnd_values = rnd_val;
  if (pointerp(rnd_val_dflt))
    re->default_rnd_values = rnd_val_dflt;
  if (pointerp(rnd_names))
    re->names = rnd_names;
  if (red)
    re->reduction = red;

  SanitizeRE(re);

  return 1;
}

public int DeleteRandomEvent(string key)
{
  if (process_call()) return -1;
  if (!member(randomEvents, key)) return -2;
  struct random_event_s re = randomEvents[key];
  // Schreibzugriff erlauben? Nur fuer Erschafferobjekte und EM.
  // Bemerkung: RMs haben Zugriff, weil sie Schreibzugriff auf das
  // Erschafferobjekt haben.
  if (extern_call()
      && re->creator != object_name(previous_object())
      && !ARCH_SECURITY)
    return -1;

  m_delete(randomEvents, key);
  return 1;
}

void create()
{
  seteuid(getuid(this_object()));
  restore_object(DROPSAVE);
  if(!randomEvents || !mappingp(randomEvents))
  {
    randomEvents = ([]);
  }
  if (!member(randomEvents, GLOBAL_KEY))
    CreateRandomEvent(GLOBAL_KEY, DFLT_RND_VALUES, DFLT_RND_NAMES, RND_REDUCTION);
  set_next_reset(3600 + random(10800));
}

void saveme(){
  save_object(DROPSAVE);
}

varargs int remove()
{
  saveme();
  destruct(ME);
  return 1;
}

public varargs int dropRare(int rarelevel, string key) {
  if (!member(randomEvents, key))
    return 0;
  
  struct random_event_s re = randomEvents[key];
  if (rarelevel < 0 || rarelevel >= sizeof(re->rnd_values))
    return 0;
  if (random(re->rnd_values[rarelevel]) == 0)
  {
    // Defaultwert wiederherstellen.
    if (pointerp(re->default_rnd_values))
      re->rnd_values[rarelevel] = re->default_rnd_values[rarelevel];
    else
      re->rnd_values[rarelevel] = DFLT_RND_VALUES[rarelevel];
    // und droppen.
    return 1;
  }
  else
  {
    // Wahrscheinlichkeit erhoehen, indem rnd_values reduziert wird.
    re->rnd_values[rarelevel] -= re->reduction;
    // negative rnd_values sind ok, random() ist dann immer 0.
  }
  // fall-through
  return 0;
}

// Droppt genau 1 oder 0 Items aus dem gegebenen Set. Benutzt hierbei das
// mittels <key> definierte Randomevent und den zur jeweiligen ID gegebenen
// <rarelevel>.
// <set> muss in der Form
// { {ID,RARELEVEL}, {ID,RARELEVEL}, ... }
// gegeben werden. <ID> muss dabei != 0 sein und wird im Erfolgsfall
// zurueckgeben.
mixed dropSetItem(mixed set, string key) {
  
  if (pointerp(set))
  {
    set=filter(set, function int (mixed el) {
      return pointerp(el) && sizeof(el)>=2 && intp(el[1]);
    } );
    set=sort_array(set, function int (mixed a, mixed b) {
      return a[1]>b[1];
    });
    
    foreach(mixed el: set) {
      if(dropRare(el[1], key)){
        return el[0];
      }
    }      
  }
  
  return 0;
}

void reset()
{
  set_next_reset(3600*72);
  // ein wenig aufraeumen

  // Alle Events rauswerfen, deren Erschafferobjekt nicht mehr exisiert
  // (Nachteil: nicht-geladene VC-Objekte werden nicht beruecksichtigt).
  foreach(string key, struct random_event_s re: randomEvents) {
    if (!find_object(re->creator)
        && file_size(re->creator + ".c") <= 0)
      m_delete(randomEvents, key);
  }

  saveme();
}

