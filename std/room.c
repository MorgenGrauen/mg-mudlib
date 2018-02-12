// MorgenGrauen MUDlib
//
// room.c -- room base object
//
// $Id: room.c 9475 2016-02-19 21:16:17Z Zesstra $
#pragma strong_types
#pragma save_types
#pragma range_check
#pragma no_clone
#pragma pedantic

inherit "/std/thing/properties";
inherit "/std/thing/language";
inherit "/std/hook_provider";
inherit "/std/room/light";
inherit "/std/container/inventory";
inherit "/std/room/moving";
inherit "/std/room/restrictions";
inherit "/std/room/description";
inherit "/std/room/exits";
inherit "/std/room/commands";
inherit "/std/room/items";
inherit "/std/room/doors";

#include <thing/properties.h>
#include <config.h>
#include <properties.h>
#include <rooms.h>
#include <language.h>
#include <wizlevels.h>
#include <moving.h>
#include <defines.h>
#include <doorroom.h>
#include <functionlist.h>
#include <hook.h>

void reset()
{
  items::reset();
  exits::reset();
  doors::reset();
}

static int
_query_noget()
{
  return 1;
}

void maybe_replace_program()
{
  string *list, first;
  object first_ob;

  if (object_name(this_object())=="/std/room" ||
      !(first_ob=find_object(first=(list=inherit_list(this_object()))[1])) ||
      (sizeof(list)!=1+sizeof(inherit_list(first_ob))) ||
      (1!=sizeof(list=functionlist(this_object(),
           RETURN_FUNCTION_NAME|NAME_INHERITED))) ||
      list[0]!="create")
    return;
  replace_program(first);
}

protected void create()
{
  maybe_replace_program();
  /* Set effective userid to userid */
  /* so that we may clone other things */
  seteuid(getuid(this_object()));
  properties::create();
  restrictions::create();
  commands::create();
  light::create();
  description::create();
  exits::create();
  items::create();
  doors::create();
  offerHook(H_HOOK_INIT, 1);

  SetProp(P_NAME,0);
  SetProp(P_NAME_ADJ,({}));
  Set(P_SHORT,0);
  Set(P_LONG,0);
  Set(P_TRANSPARENT,0);
  Set(P_ADJECTIVES,({}));
  Set(P_IDS,({}));
  Set(P_WEIGHT,PROTECTED,F_MODE);
  Set(P_TOTAL_WEIGHT,PROTECTED,F_MODE);
  Set(" clean counter ",2);
}

protected void create_super() {
  set_next_reset(-1);
}

private int
check_clean_count()
{
  int cc;

  cc=Query(" clean counter ");
  if (--cc<=0)
    return 1;
  Set(" clean counter ",cc);
  return 0;
}

int
clean_up(int arg)
{
  // Never try again when P_NEVER_CLEAN is set.
  if (Query(P_NEVER_CLEAN)) return 0;

  // do not cleanup, if this room is used as blueprint for clones or inherited
  // (arg>1) or if there are valid hook consumers waiting for something to
  // happen here. This is temporary and could change, so return 1;
  if (arg>1 || HHasConsumers())
      return 1;

  // if there are any items we have produced ourselfes check them
  mixed itema = QueryProp(P_ITEMS);
  if(pointerp(itema))
  {
    mixed names;
    int i;
    i = sizeof(names = all_inventory(this_object()));
    while(i--)
    {
      if (query_once_interactive(names[i]))
      {
        Set(" clean counter ",2);
        return 1;
      }
      if(objectp(names[i])) names[i] = explode(object_name(names[i]),"#")[0];
    }

    for(i = sizeof(itema)-1; i >= 0; i--)
    {
      // Semantik:
      // 1. Wenn RITEM_OBJECT ein array ist, kann evtl ge'clean'ed werden.
      // 2. Wenn es ein REFRESH_NONE Item ist und es entweder nicht mehr 
      //    existiert oder nicht in diesem Raum ist, kein clean_up (es wuerde
      //    beim neuladen des Raumes ja wieder erzeugt;
      //    falls es aber hier ist, wird es mitvernichtet, dann ists ok)
      // 3. Wenn es ein REFRESH_DESTRUCT ist und noch existiert, aber nicht
      //    hier ist, KEIN clean_up.
      if (!pointerp(itema[i][RITEM_OBJECT]) 
          && ((itema[i][RITEM_REFRESH] == REFRESH_NONE 
              && (!itema[i][RITEM_OBJECT] 
                || environment(itema[i][RITEM_OBJECT])!=this_object())) 
            || (itema[i][RITEM_REFRESH] == REFRESH_DESTRUCT 
              && itema[i][RITEM_OBJECT] 
              && environment(itema[i][RITEM_OBJECT]) != this_object())))
          return 1;
      names -= (pointerp(itema[i][RITEM_FILE]) ?
          itema[i][RITEM_FILE] : ({ itema[i][RITEM_FILE] }));
    }
    // if there are objects left in the room do not clean up but try again later
    if(sizeof(names) && !check_clean_count()) return 1;
  }
  else
    // is there any object lying around?
    if(first_inventory(this_object()) && !check_clean_count()) return 2;

  // do clean_up
  //log_file("clean_up_log",sprintf(
  //        "%s:%s: %O\n",ctime(time())[11..18],__HOST_NAME__,this_object()));

  remove();
  // wenn der Raum sich im remove() nicht zerstoert, hat er dafuer vermutlich
  // nen Grund. Evtl. klappts ja naechstes Mal.

  return(1);
}

/* Instead of printing the exits with the long description, we implement */
/* the command "exits" to show them. */
int
show_exits() {
  mixed ex;
  if( this_player()->CannotSee() ) return 1;
  if ((ex=QueryProp(P_HIDE_EXITS)) && intp(ex)) return 1;
  if (ex = GetExits(this_player())) write(ex);
  return 1;
}

int
toggle_exits(string str)
{
  int ex;

  /* Nur das aktuelle Environment des Spielers ist zustaendig fuer die
     Auflistung der Ausgaenge. Anderenfalls wird das Kommando von Raeumen
     im Raum (z.B. Transportern) abgefangen und es werden dessen Ausgaenge
     aufgelistet.
     Sprich: keine Auflistung von Aussen. */
  if (environment(this_player()) != this_object()) return 0;
  if (!str) return show_exits();
  if (str!="auto") return 0;
  ex = this_player()->QueryProp(P_SHOW_EXITS);
  this_player()->SetProp(P_SHOW_EXITS, !ex);
  if (ex) write("Ausgaenge werden nicht mehr automatisch angezeigt.\n");
  else write("Ausgaenge werden automatisch angezeigt.\n");
  return 1;
}

void
init()
{
  if (HookFlow(H_HOOK_INIT, 0)[H_RETCODE] == H_CANCELLED)
      return;
  Set(" clean counter ",2);
  exits::init();
  commands::init();
  description::init();
  doors::init();

  add_action("toggle_exits", "exits");
  add_action("toggle_exits", "ausgang");
  add_action("toggle_exits", "ausgaenge");
  add_action("toggle_exits", "aus");
}

int _query_para(){
  int re;
  if(re=Query(P_PARA))return re;
  if(sizeof(regexp(({object_name(this_object())}),".*\\^0$")))
      return -1;
  return to_int(
    regreplace(object_name(this_object()),".*\\^\([1-9][0-9]*\)$","\\1",1));
}

//dies ist ein Raum, war gewuenscht von mehreren Leuten.
status IsRoom() {return(1);}
