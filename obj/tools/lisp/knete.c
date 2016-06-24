// KNETE.C -- CommandLine Frontend to the Lisp2Closure compiler
// (c) 1994 by Hate@MorgenGrauen, TUBmud, NightFall
// --
// Copy, change and distribute this bit of software as much as you like,
// but keep the name of the original author in the header.

#include "tweak.h"

static inherit HOME("lisp");

#ifdef ALATIA
#  define TUBMUD
#endif
// TUBMUD is a COMPAT mode game! (well sort of, its a NATIVE/COMPAT mixture)
// the following defines should be the only ones to change
// define ENGLISH if you don't want german messages ;)
#if (defined(TUBMUD) || defined(TAPPMUD) || defined(UNItopia) || \
     defined(ADAMANT))
# ifdef ADAMANT
#   include <config.h>
    inherit "/lib/obj";
#   define WIZLOCAL "/wiz/"
#   define ID(x)        set_name(x)
#   define SHORT(x)     set_short(x)
#   define LONG(x)      set_long(x)
#   define ISWIZARD(pl) (LEVELMASTER->is_wizard(pl->query_real_name()))
# endif
# ifdef TUBMUD
#   include <kernel.h>
#   include <wizlevels.h>
    inherit ACCESS;
    inherit "/complex/item";
    inherit "/basic/autoload";
#   define WIZLOCAL "/players/"
#   define ID(x)        add_id(x)
#   define ISWIZARD(pl) (pl->query_level() >= WL_WIZARD)
# endif
# ifdef TAPPMUD
#   include <levels.h>
    inherit "/obj/thing";
#   define WIZLOCAL "/w/"
#   define ID(x)        set_alias(x)
#   define ISWIZARD(pl) (pl->query_level() >= LVL_WIZARD)
# endif
# define ENGLISH
# ifdef UNItopia
#   undef ENGLISH
#   ifdef FinalFrontier
#     include <level.h>
#   else
#     include <levels.h>
#   endif
    inherit "/i/item";
    inherit "/i/install";
#   define WIZLOCAL "/w/"
#   define ID(x)        add_id(x)
#   define ISWIZARD(pl) wizp(pl)
# endif
# define WIZHOME        (WIZLOCAL+this_player()->query_real_name())
# define LONG(x)        set_long(x)
# define SHORT(x)       set_short(x)
#else
  inherit "/std/thing";
# ifdef NIGHTFALL
#   define ENGLISH
# endif
# include <properties.h>
# include <wizlevels.h>
# define WIZHOME        ("/players/"+getuid(this_object()))
# define ID(x)          AddId(x); SetProp(P_NAME, x)
# define LONG(x)        SetProp(P_LONG, x)
# define SHORT(x)       SetProp(P_SHORT, x);
# define ISWIZARD(pl)   IS_WIZARD(pl)
#endif

nomask void create()
{
  if(sizeof(old_explode(object_name(this_object()), "#")) == 1) return;
  if(!interactive(this_player())) destruct(this_object());
#ifndef MORGENGRAUEN
  if((this_player() != find_player("hate") &&
      interactive(find_player("hate"))) &&
     old_explode(object_name(this_object()), "#")[0] == HOME("knete"))
  {
#ifdef TUBMUD
    if(check_privilege("hate")) set_privilege("hate");
    else set_privilege(0);
#else
    printf("*ERROR: do not clone this file!\n"
          +"        create a file with the following line in you home:\n"
          +"inherit \""+HOME("knete")+"\";\n");
    destruct(this_object());
    return;
#endif
  }
#endif
#if (defined(TUBMUD) || defined(UNItopia))
  item::create();
#elif (defined(ADAMANT))
  obj::create();
#else
  thing::create();
#endif
#ifdef ENGLISH
  ID("dough");
  ID("lisp");
  SHORT("A piece of dough\n");
  LONG("This piece of dough can be formed to your own convenience.\n");
#else
# ifdef UNItopia
    set_name("Knete");
# endif
  ID("knete");
  ID("lisp");
  SHORT("Ein Stueck Knete");
  LONG( 
  "Dieses Stueck Knete kann man sehr schoen an seinen persoenlichen Geschmack\n"
 +"anpassen.\n");
 this_object()->SetProp("autoloadobj",1);
 this_object()->SetProp("autoload",1);
#endif
  lisp::create();
}

varargs nomask void init(int x)
{
  lisp::init(x);
#if (defined(TUBMUD) || defined(UNItopia))
# ifndef TUBMUD
    item::init();
# endif
#elif (defined(ADAMANT))
  obj::init();
#else
  thing::init();
#endif
  add_action("cmdline", "", 1);
}

#ifdef TAPPMUD
query_auto_load() { return ({__FILE__}); }
drop() { return 1; }
#endif
#if ((defined(TUBMUD)) || (defined(ADAMANT)))
drop() { return 1; }
#endif

nomask string load(string file)
{
  if(previous_object() != this_object()) return "* Privilege violation\n";
  return lisp(read_file(file));
}

nomask int cmdline(string str)
{
  int cost;
  cost = get_eval_cost();
  if(this_player() != this_interactive()) return 0;

#ifndef MORGENGRAUEN
# ifndef TUBMUD
  if(this_player() && interactive(this_player()) &&
     ((this_player() != find_player("hate")) ||
      (this_player() != find_player("etah"))))
# endif
#else
  if(this_player() && interactive(this_player()))
#endif
  {
    mixed result;
    if(!ISWIZARD(this_player())) return 0;
#ifdef TAPPMUD
    notify_fail(lambda(({}), ({#'lisp/*'*/, 
				  query_verb()+(str?(" "+str):""), 1})));
#else
# ifdef MORGENGRAUEN
    str = this_player()->_unparsed_args();
# endif 
    result = lisp(query_verb()+(str?(" "+str):""), 1);
    if(result != -1 && cost >= 990000) {
      if(stringp(result))
        notify_fail(result);
      else
        if(result) notify_fail(sprintf("%O\n", result));
	else return 1;
      return 0;
    }
#endif
  }
}
