// MorgenGrauen MUDlib
//
// $Id: netztot.c 8747 2014-04-26 13:08:47Z Zesstra $

#include <wizlevels.h>
#include <moving.h>

protected void create()
{
  "/obj/sperrer"->NixGibts();
}

private void weg(object ob)
{
  if (!objectp(ob))
    return;
  ob->remove();
  if (ob)
    destruct(ob);
}

private void wegraeumen(object ob)
{
  object *x;

  if (!objectp(ob))
    return;
  for (x=deep_inventory(ob);sizeof(x);x=x[1..])
    weg(x[0]);
  ob->move("/room/void",M_NOCHECK|M_SILENT);
  weg(ob);
}

void reset()
{
  set_next_reset(120);
  foreach(object ob: all_inventory(this_object()))
  {
    if (interactive(ob))
    {
      catch(this_player()->move("/room/void",M_GO));
      set_object_heart_beat(ob,1);
    }
    else if (!query_once_interactive(ob) && object_name(ob)!="/obj/sperrer")
      call_out(#'wegraeumen,1,ob);
  }
  "/obj/sperrer"->upd();
}

void init()
{
  if (!this_player())
    return;
  catch(this_player()->StopHuntingMode());
  if (interactive(this_player()))
    catch(this_player()->move("/room/void",M_GO));
  if (!query_once_interactive(this_player()))
  {
    this_player()->remove();
    if(this_player())
      destruct(this_player());
  }
  set_object_heart_beat(this_player(),0);
}

public string int_long()
{
  return "Dies ist der Netztotenraum. Es ist dunkel. Du siehst nichts.\n";
}

public string int_short()
{
  return "Nichts zu sehen.\n";
}

public mixed QueryProp(string str)
{
  switch (str) {
    case "int_long": return int_long();
    case "int_short": return int_short();
  }
  return 0;
}

// Nicht jeder Magier darf den Netztotenraum entsorgen.
public string NotifyDestruct(object caller) {
    if( (caller!=this_object() && !ARCH_SECURITY) || process_call() ) {
      return "Du darfst den Netztotenraum nicht zerstoeren!\n";
    }
    return 0;
}

