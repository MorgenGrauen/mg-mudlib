// MorgenGrauen MUDlib
//
// $Id: sperrer.c 8747 2014-04-26 13:08:47Z Zesstra $

#include <properties.h>
#include <moving.h>
#include <defines.h>
#include <config.h>
#include <wizlevels.h>

#define PO previous_object()
#define TI this_interactive()

create()
{
  if (clonep(ME))
  {
    write("Autodest\n");
    destruct(this_object());
  }
  upd();
}

upd()
{
  move_object("/room/netztot");
}

debug(what,arg)
{
  string log;
  
  if (!environment())
    return;
  log=sprintf("%s %s %O PO=%O PPO=%O TO=%O TI=%O verb=%O\n",
      dtime(time()), what, arg, 
      previous_object(), previous_object(1), 
      this_object(), this_interactive(), 
      (this_interactive()||this_player()||this_object())->_unparsed_args());
  log_file("NDEAD",log);
  
  if (find_player("jof"))
    tell_object(find_player("jof"),log);
  if (previous_object() && 
      (!interactive(PO)||IS_LEARNER(PO)) && 
      getuid(PO)!=ROOTID && PO!=this_object())
  {
    log_file("NDEAD",sprintf("DELETED %O\n",PO));
    catch(PO->remove());
    if (PO && !(PO==this_object()) && object_name(PO)!="/secure/simul_efun")
      destruct(PO);
    if (TI && IS_LEARNER(TI))
    {
      tell_object(TI,"MESSING WITH NETDEAD ROOM - STOP\n");
      TI->remove();
      if (TI)
        destruct(TI);
      log_file("NDEAD",sprintf("STOPPED %O\n",TI));
    }
    raise_error("DONT MESS WITH NETDEAD\n");
  }
  else if ( previous_object() && interactive(previous_object()) ){
      tell_object( previous_object(), "Wie bist Du denn hierhin gekommen?\n"
                   "Ich setz Dich besser mal in der Abenteurergilde ab.\n" );
      previous_object()->move( "/gilden/abenteurer", M_GO|M_NOCHECK );
      log_file( "NDEAD", sprintf( "MOVED %O\n", previous_object() ) );
  }
}

reset()
{
  upd();
}

id(str)
{
  debug("id",str);
}

Query(what)
{
  debug("query",what);
}

remove()
{
  debug("remove",0);
}

long()
{
  debug("long",0);
}

short()
{
  debug("short",0);
}
