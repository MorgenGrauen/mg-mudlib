// (c) 2003 by Padreic (Padreic@mg.mud.de)
// Das Objekt realisiert einen Tarnzauber, aehnlich
// dem Tarnhelm. Jedoch ohne Shadow und nur auf die
// Rasse beschraenkt.

#include <properties.h>
#include <language.h>
#include <moving.h>
#include <defines.h>

inherit "/std/thing";

static string race;

create()
{
  if (!clonep(this_object())) return;
  ::create();
  SetProp(P_SHORT, 0);
  SetProp(P_LONG, "Tarnzauber");
  SetProp(P_NAME, "Tarnzauber");
  SetProp(P_INVIS, 1);
  SetProp(P_GENDER, MALE);
  SetProp(P_VALUE,  0);
  SetProp(P_NOGET, 1);
  SetProp(P_WEIGHT, 0);
  Set(P_DEFEND_FUNC, ME);
  AddId("\ntarnzauber");
}

// die NPCs im Zauberwald durchschauen den Zauber... :o)
string _query_race()
{
   if (previous_object(1) && getuid(previous_object(1))==getuid())
      return previous_object()->_query_race()||race;
   return "Dunkelelf";
}

void Initialize(object pl)
{
   object *armours;
   pl->Set(P_RACE, #'_query_race, F_QUERY_METHOD);
   armours=(pl->QueryProp(P_ARMOURS));
   if (member(armours, ME)>=0) return 0;
   pl->SetProp(P_ARMOURS, armours+({ ME }));
   race=pl->Query(P_RACE);
}

int QueryDefend(string *dtyp, mixed spell, object enemy)
{
  mixed am;
  // noch ein paar paranoide Sicherheitsabfragen... :o)
  if (!previous_object() ||
      !pointerp(am=previous_object()->QueryProp(P_ARMOURS)) ||
      member(am, ME)==-1) return 0;
  tell_object(previous_object(), "Dein Tarnzauber loest sich auf.\n");
  remove();
}

int DefendFunc(string *dtyp, mixed spell, object enemy)
{
   return QueryDefend(dtyp, spell, enemy);
}

varargs int move(mixed dest, int method)
// beim Tod soll sich der Zauber entfernen und auch nirgends rumfliegen
{
   int res;
   res=(int)::move(dest, method);
   if (!living(environment())) remove();
   return res;
}

varargs int remove(int silent)
{
  if (living(environment())) environment()->Set(P_RACE,0,F_QUERY_METHOD);
  return (int)::remove(silent);
}
