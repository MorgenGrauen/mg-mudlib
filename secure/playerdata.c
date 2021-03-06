// Dieses Ding kann ein Spieler-Savefile laden, um dort Daten via
// Query/QueryProp abzufragen. Sollte das Spielerobjekt existieren, wird
// stattdessen dort gefragt.

// Das Objekt hier bekommt aufgrund des Speicherortes (/secure/) die UID ROOT
// und darf folglich alles lesen.

// Spielerladen duerfen aber nur Objekte, welche die gleiche UID wie dieses
// Objekt (damit also in diesem Fall ROOT) haben.
// Daten abfragen darf nur das Objekt, welches den Spieler auch geladen hat.
// Damit ist sichergestellt, dass nur ROOT-Objekt ueber *dieses* Objekt (was
// ja ueberall lesen kann) Daten abfragen koennen.

#pragma no_shadow
#pragma strong_types,save_types,rtt_checks

#include <config.h>
#include <wizlevels.h>

inherit "/std/thing/properties";

object|string pl;
object user;

protected void create()
{
  ::create();
  seteuid(getuid());
  set_next_reset(300);
}

public int ReleasePlayer()
{
  pl = 0;
  user = 0;
  return 1;
}

public int LoadPlayer(string name)
{
  // Es duerfen nur Objekte mit der gleichen UID wie wir diesen Clone
  // benutzen! Ansonsten koennten z.B. nicht-ROOT-Objekte einen von einem
  // ROOT-Objekt erzeugten Clone hiervon nutzen und alles lesen...
  if (!previous_object()
      || getuid(previous_object()) != getuid(this_object()))
    return 0;
  if (query_wiz_level(getuid(previous_object())) < WIZARD_LVL)
    return 0;

  if (!master()->find_userinfo(name))
    return 0;

  // Wenn es das Spielerobjekt gibt, ist alles gut, dann fragen wir einfach
  // das.
  pl = find_player(name) || find_netdead(name);
  if (!pl)
  {
    // wenn nicht: Savefile einlesen.
    restore_object(SAVEPATH + name[0..0] + "/" + name);
    pl = name;
  }
  user = previous_object();
  set_next_reset(300);
  return 1;
}

public varargs mixed Query( string name, int Type )
{
  if (!user || user != previous_object())
    return 0;

  if (objectp(pl))
    return pl->Query(name,Type);
  else if (stringp(pl))
    return ::Query(name, Type);
  return 0;
}

public varargs mixed QueryProp( string name )
{
  if (!user || user != previous_object())
    return 0;

  if (objectp(pl))
    return pl->QueryProp(name);
  else if (stringp(pl))
    return ::QueryProp(name);
  return 0;
}


// kein Schreibzugriff
public varargs mixed Set( string name, mixed Value, int Type, int extern )
{
  return 0;
}

public void reset()
{
  if (clonep(this_object()))
    destruct(this_object());
}

public int clean_up(int ref)
{
  if (clonep(this_object())
      || ref <= 1 )
  {
    destruct(this_object());
  }
  return 1;
}

