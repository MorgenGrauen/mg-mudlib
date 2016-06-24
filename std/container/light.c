// MorgenGrauen MUDlib
//
// container/light.c -- Lichtsystem fuer Container
//
// $Id: description.c 6986 2008-08-22 21:32:15Z Zesstra $

inherit "/std/thing/light";

#pragma strict_types
#pragma save_types,rtt_checks
#pragma range_check
#pragma no_clone
#pragma pedantic

#define NEED_PROTOTYPES

#include <container.h>
#include <properties.h>

// BTW: _query_last_content_change() sollte bloss nie auch -1 fuer
// uninitialisiert zurueckliefern...
private nosave int last_light_calc = -1;

protected void create()
{
  ::create();
  // wieviel Licht schluckt der Container _muss_ > 0 sein!
  SetProp(P_LIGHT_TRANSPARENCY, 2);
}

protected void create_super() {set_next_reset(-1);}


static mixed _query_light_transparency()
{
   if (QueryProp(P_TRANSPARENT))
     return Query(P_LIGHT_TRANSPARENCY,F_VALUE);
   return 999;
}

public int add_light_sources(int * sources) {
  float light = 0.0;
  //printf("als(%O): %O\n",this_object(),sources);
  // Alle Lichtlevel werden als Exponent von e aufgefasst und die Summe dieser
  // Potenzen gebildet.
  foreach(int l : sources) {
     if (l > 0)
       light += exp(l);
     else if (l < 0)
       light -= exp(abs(l));
     // l==0 muss ignoriert werden
  }
  // anschliessend wird aus dieser Summe der natuerliche Logarithmus
  // berechnet.
  // auf diese Weise haben hoehere Lichtlevel ueberproportional viel Einfluss
  // auf die Helligkeit im Raum, kleine Lichtlevel fallen aber nicht komplett
  // raus.
  if (light > 0) {
      light = log(light);
  }
  else if (light < 0) {
      light = -log(abs(light));
  }

  //printf("als(): %O\n",light);
  // runden und als int zurueckgeben.
  if (light >= 0)
    return to_int(light+0.5);
  //else
  return to_int(light-0.5);
}

static int _query_total_light()
{
  int totallight;

  if ( _query_last_content_change() == last_light_calc )
    return Query(P_TOTAL_LIGHT);

  // eigenes P_LIGHT und P_TOTAL_LIGHT der enthaltenen Objekte verrechnen
  int intlight = add_light_sources(
      all_inventory()->QueryProp(P_TOTAL_LIGHT) + ({QueryProp(P_LIGHT)}));
  // P_INT_LIGHT (gerundet) abspeichern
  Set(P_INT_LIGHT, intlight, F_VALUE);

  // Licht nach aussen muss Containertransparenz beruecksichtigen.
  if (intlight > 0) {
    totallight = intlight - QueryProp(P_LIGHT_TRANSPARENCY);
    if (totallight < 0)
      totallight = 0;
  }
  else {
    totallight = intlight + QueryProp(P_LIGHT_TRANSPARENCY);
    if (totallight > 0)
      totallight = 0;
  }
  //printf("_query_total_light(%O): %O\n",this_object(),totallight);

  last_light_calc = _query_last_content_change();

  // Runden und Konversion nach int nicht vergessen...
  if (totallight > 0)
    return SetProp(P_TOTAL_LIGHT, to_int(totallight + 0.5));
  //else
  return SetProp(P_TOTAL_LIGHT, to_int(totallight - 0.5));
}

static int _query_int_light()
{
   int intlight, envlight;

   if ( _query_last_content_change() != last_light_calc )
       _query_total_light();

   // P_INT_LIGHT des environments kann sich natuerlich aendern _ohne_ das
   // etwas an einem container geaendert wird. Daher Auswertung jedes mal
   // neu aktualisieren.
   if (!environment()
       || !(envlight=(int)environment()->QueryProp(P_INT_LIGHT)))
      return Query(P_INT_LIGHT, F_VALUE);
   else {
      intlight = Query(P_INT_LIGHT, F_VALUE);
      int transparency = QueryProp(P_LIGHT_TRANSPARENCY);

      // bei einem transparentem Container kann natuerlich das Licht des
      // environments den Container auch ausleuchten....
      // (Anmerkung: in dem Licht von draussen ist unser Lichtanteil
      // natuerlich auch drin, gedaempft um die Transparenz (raus und rein).
      // Ich vernachlaessige das.)
      if (abs(envlight) > transparency) {
        if (envlight > 0)
            envlight -= transparency;
        else
            envlight += transparency;
        // jetzt Licht von aussen und innen verrechnen
        intlight = add_light_sources(({intlight, envlight}));
      }
      // else: nur P_INT_LIGHT, Licht des Environments keine Bedeutung
   }
   return intlight;
}

static int _set_light(int light)
{
  last_light_calc=-1;

  return ::_set_light(light);
}

