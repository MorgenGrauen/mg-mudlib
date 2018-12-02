#pragma strong_types,rtt_checks

inherit "/std/container";
#include <moving.h>

object cloner;

protected void create()
{
  cloner=previous_object();
  ::create();
  // Das Objekt raeumt sich selber im reset auf. (D.h. kein Env-Check)
  SetAutoObject(1);
}

// Dieses Objekt raeumt sich immer im ersten reset auf - es ist kurzlebig und
// transient und Aenderungen an seinen Daten bleiben nicht erhalten.
void reset()
{
  remove(1);
}

public varargs void init(object origin)
{
  // leer ueberschreiben. Das klingt abstrus, hat aber folgenden Hintergrund:
  // dieses Objekt ist nicht dazu gedacht, in irgendeine Umgebung zu gelangen.
  // Und selbst wenn das jemand mit M_NOCHECK tut, soll die Interaktion dieses
  // Objekts mit anderen minimiert werden. Also keine Anmeldung von actions
  // etc.
}

// Auch dies ist ungewoehnlich: Bewegung nie erlauben, daher eruebrigt sich
// der Aufruf des geerbten PreventMove(). Auch wird M_NOCHECK nicht
// beruecksichtigt... Don't do this anywhere else.
protected int PreventMove(object dest, object oldenv, int method)
{
  return ME_NOT_ALLOWED;
}

public object *AllVirtualEnvironments()
{
  if (cloner)
  {
    object *cloner_envs = all_environment(cloner)
                          || cloner->AllVirtualEnvironments();
    if (cloner_envs)
      return ({cloner}) + cloner_envs;
    return ({cloner});
  }
  return 0;
}
