// MorgenGrauen MUDlib
//
// room/items.c -- creating extra items in room
//
// $Id: items.c 9538 2016-03-20 23:46:41Z Zesstra $

#pragma strong_types
#pragma save_types
#pragma pedantic
#pragma range_check
#pragma no_clone

inherit "std/container/items";

protected void create_super() {
  set_next_reset(-1);
}

void reset() 
{
  ::reset();

  object *inh = all_inventory(this_object());
  if ( !pointerp(inh) || sizeof(inh) < 10 )
    return;
  // nur wenn keine Spieler anwesend sind.
  if ( !sizeof(inh & users()) )
    remove_multiple(3);
}

// Per Default nur an alle Items im Inventar weiterleiten.
public varargs int ReceiveMsg(string msg, int msg_type, string msg_action,
                              string msg_prefix, object origin)
{
  int *res = all_inventory()->ReceiveMsg(msg, msg_type, msg_action,
                                         msg_prefix,
                                         origin || previous_object());
  if (sizeof(res))
    return min(res);
  return 0;
}

