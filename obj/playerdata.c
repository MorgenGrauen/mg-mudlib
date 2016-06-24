// Dieses Ding kriegt die UID vom Clonenden.
// Der Master exportiert allerdings per export_uid() ggf. eine UID an dieses
// Objekt.
// Abfragen kann es nur das Objekt, welches den Spieler geladen hat.

#pragma no_inherit,no_shadow
#pragma strong_types,save_types,rtt_checks

#include <config.h>
#include <wizlevels.h>

inherit "/std/thing/properties";

object|string pl;
object user;

protected void create()
{
  ::create();
  seteuid(0);
  set_next_reset(300);
}

public int ReleasePlayer()
{
  pl = 0;
  user = 0;
  // Das Objekt darf keine EUID mehr haben, damit ROOT-Objekte ihm ggf. eine
  // neue UID exportieren koennen, damit es einen neuen Spieler laden kann.
  seteuid(0);
  return 1;
}

public int LoadPlayer(string name)
{
  if (query_wiz_level(getuid(previous_object())) < WIZARD_LVL)
    return 0;

  seteuid(getuid());

  mixed userinfo=MASTER->get_userinfo(name);
  if (!pointerp(userinfo))
    return 0;
  // Wenn es das Spielerobjekt gibt, ist alles gut, dann fragen wir einfach
  // das.
  pl = find_player(name) || find_netdead(name);
  if (!pl)
  {
    // wenn nicht: Savefile einlesen.
    restore_object("/"SAVEPATH + name[0..0] + "/" + name);
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

