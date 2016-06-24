//  haustuer.c -- Beschreibung, Oeffnen und Schliessen
//
//  (c) 1995 Wargon@MorgenGrauen
//
// $Id: haustuer.c,v 1.1.1.1 2000/08/20 20:22:42 mud Exp $
//

#define NEED_PROTOTYPES
#include "../haus.h"
#include <properties.h>
#include <thing/properties.h>
#include <thing/description.h>
#include <thing/commands.h>

static int validHouse = 0;
static string owner;

void create()
{
  Set( H_DOOR, "Die Haustuer ist %s.\n" );
  Set( H_DOORSTAT, 0);
  Set( H_DOORLSTAT, ({ "geoeffnet", "geschlossen", 0, "abgeschlossen" }));

  Set( H_DOOR, SAVE, F_MODE);
  Set( H_DOORSTAT, SAVE, F_MODE);
  Set( H_DOORLSTAT, SAVE, F_MODE);

  AddCmd( ({"schliess", "schliesse"}), "schliesse");
  AddCmd("oeffne","oeffne");

  owner = getuid(this_object());
}

varargs string long(int mode)
{
  return sprintf(Query(H_DOOR), Query(H_DOORLSTAT)[Query(H_DOORSTAT)]);
}

static string SetTuerOwner(string o)
{
  validHouse=1;
  Set( H_DOORSTAT, D_CLOSED | D_LOCKED);
  return owner=o;
}

int oeffne(string str)
{
  int doorstat;

  notify_fail("Was willst Du oeffnen?\n");
  doorstat = Query(H_DOORSTAT);

  if (!id(str) && str != "tuer" && str != "haustuer")
    return 0;

  if (!(doorstat & D_CLOSED)) {
    notify_fail(capitalize(name(WER,1))+" ist gar nicht geschlossen!\n");
    return 0;
  }
  if (doorstat & D_LOCKED) {
    notify_fail(capitalize(name(WER,1))+" ist abgeschlossen.\n");
    return 0;
  }

  doorstat &= ~D_CLOSED;
  Set(H_DOORSTAT, doorstat & ~D_CLOSED);

  write("Du oeffnest "+name(WEN,1)+" von "+capitalize(owner)+".\n");
  tell_room(environment(this_object()),
	    sprintf("%s oeffnet %s von %s.\n",
		    this_player()->name(WER),
		    name(WEN,1),
		    capitalize(owner) ),
	    ({this_player()}));
  if (validHouse) {
    tell_room(find_object(RAUMNAME(owner,0))||this_object(),
	      this_player()->name(WER)+" oeffnet "+name(WEN)+".\n",
	      ({this_player()}));
    Save();
  }
  return 1;
}

private int close_door()
{
  if (Query(H_DOORSTAT) & D_CLOSED) {
    notify_fail(capitalize(name(WER,1))+" ist doch schon zu!\n");
    return 0;
  }
  Set(H_DOORSTAT, Query(H_DOORSTAT) | D_CLOSED);

  write("Du schliesst "+name(WEN,1)+" von "+capitalize(owner)+".\n");
  tell_room(environment(this_object()),
	    sprintf( "%s schliesst %s von %s.\n",
		     capitalize(this_player()->name(WER)),
		     name(WEN,1), capitalize(owner)),
	    ({this_player()}));
  if (validHouse) {
    tell_room(find_object(RAUMNAME(owner,0))||this_object(),
	      capitalize(this_player()->name(WER))+" schliesst "+name(WEN)+".\n",
	      ({this_player()}));
    Save();
  }
  return 1;
}

private int lock_door()
{
  object tp, home;
  string nam;
  int doorstat;

  tp = this_player();
  nam = capitalize(tp->name(WER));

  if (validHouse) {
    call_other(RAUMNAME(owner,0), "???");
    home = find_object(RAUMNAME(owner,0));

    if (!home->allowed_check(this_player())) {
      notify_fail("Du darfst "+name(WEN,1)+" von "+capitalize(owner)+" aber nicht abschliessen.\n");
      return 0;
    }
  }
  if ((doorstat = Query(H_DOORSTAT)) & D_LOCKED) {
    notify_fail("Die Tuer ist abgeschlossen.\n");
    return 0;
  }
  doorstat |= (D_LOCKED|D_CLOSED);
  write("Du schliesst "+name(WEN)+" ab.\n");
  Set(H_DOORSTAT, doorstat);
  tell_room(environment(this_object()),
	    sprintf( "%s schliesst %s ab.\n",
		    nam,
		    (getuid(tp)==owner ?
		     (tp->QueryPossPronoun(this_object(), WEN)+" "+name(RAW)) :
		     name(WEN,1) )),
	    ({tp}));
  if (validHouse) {
    tell_room(home, nam+" schliesst "+name(WEN)+" ab.\n",({tp}));
    Save();
  }
  return 1;
}

private int unlock_door()
{
  object tp, home;
  string nam;
  int doorstat;

  tp = this_player();
  nam = capitalize(tp->name(WER));

  if (validHouse) {
    call_other(RAUMNAME(owner,0), "???");
    home = find_object(RAUMNAME(owner,0));

    if (!home->allowed_check(this_player())) {
      notify_fail("Du darfst "+name(WEN,1)+" von "+capitalize(owner)+" aber nicht aufschliessen.\n");
      return 0;
    }
  }

  if (!((doorstat = Query(H_DOORSTAT)) & D_LOCKED)) {
    notify_fail("Die Tuer ist nicht abgeschlossen.\n");
    return 0;
  }

  Set(H_DOORSTAT, doorstat & ~D_LOCKED);
  write("Du schliesst "+name(WEN)+" auf.\n");
  tell_room(environment(this_object()),
	    sprintf("%s schliesst %s auf.\n",
		     nam,
		     (getuid(tp)==owner ?
		      (tp->QueryPossPronoun(this_object(), WEN)+" "+name(RAW)) :
		      name(WEN,1) )),
	    ({tp}));
  if(validHouse) {
    tell_room(home,nam+" schliesst "+name(WEN)+" auf.\n",({tp}));
    Save();
  }
  return 1;
}

int schliesse(string str)
{
  string was, wen, *s;

  notify_fail("Was willst Du schliessen ?\n");
  if (!str||str=="")
    return 0;
  s = old_explode(str, " ");
  if ((was = s[<1]) == "auf" || was == "ab")
    s = s[0..<2];
  else
    was = 0;

  wen=implode(s, " ");
  if (!id(wen) && wen != "tuer" && wen != "haustuer")
    return 0;
  if (!was || was=="")
    return close_door();
  switch (was)
  {
    case "auf": return unlock_door();
    case "ab":  return lock_door();
  }
  return 0;
}

// $Log: haustuer.c,v $
// Revision 1.1.1.1  2000/08/20 20:22:42  mud
// Ins CVS eingecheckt
//
