// MorgenGrauen MUDlib
//
// OBJECTD.C -- object daemon
//
// $Date: 1995/04/03 14:47:02 $
// $Revision: 9510 $
/* $Log: objectd.c,v $
 * Revision 1.2  1995/04/03 14:47:02  Wargon
 * QueryObject() verwendet bei BluePrints jetzt auch AddItem.
 *
 * Revision 1.1  1995/03/31  13:30:33  Hate
 * Initial revision
 *
 */

#pragma strong_types,save_types,rtt_checks
#pragma no_clone, no_shadow

#include <rooms.h>
#include <properties.h>
#include <defines.h>
#include <daemon.h>

mapping objects = ([]);
private nosave int do_save;

#define CLASS   0
#define DATA    1

protected void create()
{
  seteuid(getuid(ME));
  restore_object(OBJECTD_SAVE);
}

protected void reset() {
  if (do_save)
  {
    save_object(OBJECTD_SAVE);
    do_save=0;
  }
}

string AddObject(object obj, string env)
{
  if(!obj || !env || !objectp(obj) || !stringp(env))
    return 0;

  // save information
  if(!member(objects, env))
    objects[env] = ({ ({  object_name(obj), obj->QueryProp(P_AUTOLOAD) }) });
  else
    objects[env] += ({ ({ object_name(obj), obj->QueryProp(P_AUTOLOAD) }) });

  do_save=1;
  return env;
}

int RemoveObject(object|string obj, string env)
{
  if(!obj || !env || !stringp(env))
    return 0;

  int ret;
  if(member(objects, env))
  {
    mixed oblist = objects[env];
    foreach(mixed arr: &oblist)
    {
      if (load_name(arr[CLASS]) == load_name(obj))
      {
        arr = 0;
        ++ret;
        // nur eins Austragen, nicht alle, falls mehr als einmal angemeldet
        break;
      }
    }
    objects[env] = oblist - ({0});
  }
  if(!sizeof(objects[env]))
    m_delete(objects, env);

  do_save=1;
  return ret;
}

// Fragt nicht wirklich ab, sondern erstellt die angemeldeten Objekt in env.
// Wenn kein env, wird previous_object() genommen.
varargs void QueryObject(string envname)
{
  object env;
  // take the caller as the questioning object
  if(!envname || !stringp(envname))
    env = previous_object();
  else
    env = find_object(envname);

  // target must be a blueprint
  if(IS_CLONE(env)) return 0;

  envname=object_name(env);
  if(member(objects, envname))
  {
    mixed oblist = objects[envname];
    foreach(mixed arr : &oblist)
    {
       // Wir muessen unterscheiden, ob ein Clone angemeldete wurde oder eine
       // Blueprint.
       object o;
       string *n = explode(arr[CLASS], "#");
       if (sizeof(n) > 1 && sizeof(n[1]) > 0)
         o = env->AddItem(n[0], REFRESH_DESTRUCT);
       else
         o = env->AddItem(n[0], REFRESH_DESTRUCT, 1);
       o->SetProp(P_AUTOLOAD, arr[DATA]);
       // Und das neue Objekt merken (ne Verwendung hat das zur Zeit aber
       // nicht)
       arr[CLASS] = object_name(o);
    }
  }
}

// Liefert eine Liste der in env angemeldeten Objekte. Wenn kein env, wird
// previous_object() genommen.
public varargs string QueryObjects(string envname)
{
  object env;
  // take the caller as the questioning object
  if(!envname || !stringp(envname))
    env = previous_object();
  else
    env = find_object(envname);

  // target must be a blueprint
  if(IS_CLONE(env)) return 0;

  mixed arrarr = objects[object_name(env)];
  if (arrarr)
  {
    return CountUp(map(arrarr, #'[, CLASS),", ",", ");
  }
  return 0;
}

public varargs int remove(int silent)
{
  save_object(OBJECTD_SAVE);
  destruct(ME);
  return 1;
}

/*
void clean(string *list)
{
  if (!list)
      list = m_indices(objects);
  while (sizeof(list) && get_eval_cost() > 50000)
  {
    tell_object(this_player(),"Cleaning: "+list[0]+"\n");
    mixed arrarr = objects[list[0]];
    foreach(mixed arr: &arrarr)
    {
      if (load_name(arr[CLASS]) == "/p/service/miril/advent/obj/tuerchen")
      {
        arr = 0;
      }
    }
    objects[list[0]] = arrarr - ({0});
    list = list[1..];
  }
  if (sizeof(list))
      call_out(#'clean, 4);
  else
  {
    save_object(OBJECTD_SAVE); 
    tell_object(this_player(), "Fertig!");
  }
}
*/
