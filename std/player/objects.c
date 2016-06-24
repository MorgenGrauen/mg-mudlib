// MorgenGrauen MUDlib
//
// player/objects.c -- object handling for player
//
// $Id: objects.c 8675 2014-02-18 20:39:54Z Zesstra $
#pragma strong_types
#pragma save_types
#pragma range_check
#pragma no_clone
#pragma pedantic

#define NEED_PROTOTYPES
#include "/sys/player/filesys.h"

#include <config.h>
#include <player.h>
#include <properties.h>
#include <language.h>
#include <moving.h>
#include <wizlevels.h>
#include <thing/moving.h>

static int update_object(string str) {
  object ob;
  string upd_file;
  if (!(str=_unparsed_args())) {
    notify_fail("Usage: Update <object_path>\n"); return 0;
  }
  upd_file = find_file(str,".c");
  if (!upd_file) upd_file=find_file(str);
  if (!upd_file) {
    notify_fail(str+": No such file.\n"); return 0;
  }
  ob = find_object(upd_file);
  if (!ob) {
    notify_fail(upd_file+": No such object.\n"); return 0;
  }
  destruct(ob);
  write(upd_file + ": will be reloaded at next reference.\n");
  return 1;
}

/*
 * "soft" means that the object is given the chance to self-destruct, thus
 * allowing it to do necessary cleanup like subtracting from the carried
 * weight of the environment(). We call remove() in the object to be
 * destructed.
 */
static int soft_update_object(string str) 
{
  object ob;
  string upd_file;
  if (!(str=_unparsed_args())) {
    notify_fail("Usage: update <object_path>\n"); return 0;
  }
  upd_file = find_file(str,".c");
  if (!upd_file) upd_file=find_file(str);
  if (!upd_file) {
    notify_fail(str+": No such file.\n"); return 0;
  }
  ob = find_object(upd_file);
  if (!ob) {
    notify_fail(upd_file+": No such object.\n"); return 0;
  }
  if (ob->remove() == 0) {
    notify_fail(upd_file+": doesn't want to be destructed!\n"); return 0;
  }
  write(upd_file + ": will be reloaded at next reference.\n");
  return 1;
}

int clone(string str) 
{
  object ob;
  string clone_file;

  if (!(str=_unparsed_args())){
    notify_fail("Usage: clone <object_path>\n"); return 0;
  }
  clone_file = find_file(str,".c");
  if (!clone_file) clone_file=find_file(str);
  if (!clone_file) {
    notify_fail(str+": No such file.\n"); return 0;
  }
  if (!(ob = clone_object(clone_file)))
    return notify_fail(str+": Failed to load.\n"), 0;

  /* Some objects destruct themselves rather fast */
  if (!objectp(ob))
    return notify_fail(str+": Destructed whilst created.\n"), 0;
  
  /* try to move the object to my environment */
  if ((ob->move(this_object(),M_GET)>0) || 
      (ob->move(environment(),M_NOCHECK)>0))
  {
    if (!objectp(ob))
      return notify_fail(str+": Destructed whilst created.\n"), 0;
    write("Cloned "+object_name(ob)+".\n");
    say(this_player()->name(WER,1) + " "
		+ this_player()->QueryProp(P_CLONE_MSG)+".\n");
    return 1;
  }
  say(this_player()->name(WER,1)+" malt wilde Zeichen in die Luft und "
      +"murmelt vor sich hin, aber nichts passiert...\n");
  destruct(ob);
  write(clone_file+": failed to move\n");
  return 1;
}

/*
 * "soft" means that the object is given the chance to self-destruct, thus
 * allowing it to do necessary cleanup like subtracting from the carried
 * weight of the environment(). We call remove() in the object to be
 * destructed.
 */
static int soft_destruct_object(string str)
{ 
  object ob;
  object *obs;
  string strWER,strWEN;

  if (!(str=_unparsed_args())){
    notify_fail("Usage: destruct <objectname>\n"); return 0;
  }
  strWER = lower_case(str);
  obs = this_player()->find_obs(strWER,PUT_GET_NONE);
  if (!obs || !sizeof(obs)) {
    notify_fail("Kein \"" + str + "\" gefunden.\n");
    return 0;
  }
  ob=obs[0];
  strWER=ob->name(WER);
  strWEN=ob->name(WEN);
  if (!strWER)
    strWER="jemand";
  if (!strWEN)
    strWEN="jemanden";

  if (ob->remove() == 0) {
    notify_fail(strWER+" will nicht 'destructed' werden!\n");
    say(this_player()->name(WER,1)+" versucht vergeblich, "+strWEN+
        " zu atomisieren.\n");
    return 0;
  }
  say(capitalize(strWER)+" "+this_player()->QueryProp(P_DESTRUCT_MSG)+".\n");
  write(capitalize(strWER)+" wird von dir zerstaeubt.\n");
  return 1;
}

static int destruct_object(string str)
{ 
  object ob;
  object *obs;
  string strWER,strWEN;

  if (!(str=_unparsed_args())) {
    notify_fail("Usage: Destruct <objectname>\n"); return 0;
  }
  strWER = lower_case(str);
  obs = this_player()->find_obs(strWER,PUT_GET_NONE);
  if (!obs || !sizeof(obs)) {
    notify_fail("Kein \"" + str + "\" gefunden.\n"); return 0;
  }
  ob=obs[0];
  strWER=ob->name(WER);
  strWEN=ob->name(WEN);

  say(capitalize(strWER)+" "+this_player()->QueryProp(P_DESTRUCT_MSG)+".\n");
  destruct( ob );
  write(capitalize(strWER)+" wird von dir zerstaeubt.\n");
  return 1;
}

static int load(string str)
{ 
  object env;
  string file;
  string err;

  if (!(str=_unparsed_args())) {
    notify_fail("Usage: load <object_path>\n"); return 0;
  }
  file = find_file(str,".c");
  if (!file) file=find_file(str);
  if (!file) {
    notify_fail(str+": No such file.\n"); return 0;
  }
  if ( err = catch(load_object(file);publish) )
    printf("Cannot load %O, err = %O\n",file,err);
  else write(file+"\n");
  return 1;
}

static int exec_playerob(string name)
{
  object ob, *inv;
  int i;

  if (!IS_LORD(this_object())) return 0;
  if (this_player() != this_interactive()) return 0;
  if (this_player() != this_object()) return 0;
  if (!(name=_unparsed_args())) return 0;
  name="secure/master"->_get_path(name,getuid(this_object()));
  if (catch(load_object(name);publish) ) 
  {
    write("BUG in "+name+"\n");
    return 1;
  }
  ob=clone_object(name);
  if (!ob) return 0;
  if (getuid(ob) != getuid(this_object()))
  {
    write("UID conflict.\n");
    destruct(ob);
    return 1;
  }
  log_file("EXEC", getuid(this_object())+" "+name+" "+dtime(time()));
  disable_commands();
  exec(ob,this_object());
  if (interactive(this_object()) || !interactive(ob))
  {
    enable_commands();
    write("Fehler in exec\n");
    return 1;
  }
  inv=all_inventory(this_object());
  ob->start_player(capitalize(getuid(this_object())));
  remove();
  return 1;
}

static mixed _query_localcmds()
{
  return ({
           ({"clone","clone",0,WIZARD_LVL}),
	   ({"destruct","soft_destruct_object",0,LEARNER_LVL}),
	   ({"Destruct","destruct_object",0,LEARNER_LVL}),
	   ({"load","load",0,WIZARD_LVL}),
	   ({"update","soft_update_object",0,LEARNER_LVL}),
	   ({"Update","update_object",0,LEARNER_LVL}),
	   ({"exec","exec_playerob",0,LEARNER_LVL})
	 });
}
