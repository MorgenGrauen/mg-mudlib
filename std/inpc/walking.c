// MorgenGrauen MUDlib
//
// inpc/walking.c -- Intelligent herumlaufen
//
// $Id: walking.c 8755 2014-04-26 13:13:40Z Zesstra $
#pragma strong_types
#pragma save_types
#pragma range_check
#pragma no_clone
#pragma pedantic

#define NEED_PROTOTYPES
#include <thing/properties.h>
#include <thing.h>
#undef NEED_PROTOTYPES

#include <inpc.h>
#include <properties.h>
#include <moving.h>

#define ME this_object()
#define ENV environment()

int _set_inpc_walk_delay(int del) {
  if (del && del<5) del=5;
  return Set(P_INPC_WALK_DELAYS,del);
}

mixed _query_inpc_home() {
  mixed res;

  res=Query(P_INPC_HOME);
  if (!res) return "/room/void";
  return res;
}

int area_check(string fn) {
  mixed area;
  string *words;
  int i;

  if (!(area=QueryProp(P_INPC_WALK_AREA)))
    return 1; // Keine Beschraenkung im Gebiet?
  if (mappingp(area)) {
    if (area[fn])
      return 1; // Explizit erlaubter Raum
    words=old_explode(fn,"/");
    for (i=sizeof(words)-2;i>=0;i--)
      if (area[implode(words[0..i],"/")])
	return 1; // Erlaubtes Gebiet
    return 0; // Nicht erlaubtes Gebiet
  }
  if (pointerp(area)) {
    for (i=sizeof(area)-1;i>=0;i--)
      if (fn[0..(sizeof(area[i])-1)]==area[i])
	return 1; // Erlaubtes Gebiet
    return 0; // Nicht erlaubtes Gebiet
  }
  return 1;
}

int may_enter_room(mixed room) {
  int flags;
  string fn;
  object ob;
  
  if (objectp(room)) {
    fn=object_name(room);
    ob=room;
  } else if (stringp(room)) {
    fn=room;
    ob=find_object(room);
  } else
    return 1; // Dann sollte move schon nen Fehler machen
  if (fn=="/room/void") // Void darf IMMER betreten werden
    return 1;
  flags=QueryProp(P_INPC_WALK_FLAGS);
  if (!(flags & WF_MAY_LOAD)
      && !objectp(ob))
    return 0; // Darf nicht in nicht geladene Raeume folgen.
  if (!(flags & WF_MAY_WALK_BACK)
      && ob==QueryProp(P_INPC_LAST_ENVIRONMENT))
    return 0; // Darf nicht in den vorherigen Raum
  return area_check(fn);
}

int walk_random() {
  string *ex,*ex2;
  object env;
  int i,r,flags;
  
  if (!objectp(env=ENV))
    return 0;
  ex=m_indices(ENV->QueryProp(P_EXITS));
  flags=QueryProp(P_CAN_FLAGS);
  if (flags & WF_MAY_USE_SPECIAL)
    ex+=m_indices(ENV->QueryProp(P_SPECIAL_EXITS));
  ex2=ex[0..];
  while (i=sizeof(ex)) {
    r=random(i);
    command(ex[r]);
    if (ENV!=env)
      return 1;
    ex-=({ex[r]});
  }
  if (!(flags & WF_MAY_WALK_BACK)) {
    SetProp(P_INPC_LAST_ENVIRONMENT,0);//Dirty Hack, um Sackgassen zu verlassen
    ex=ex2;
    while (i=sizeof(ex)) {
      r=random(i);
      command(ex[r]);
      if (ENV!=env)
	return 1;
      ex-=({ex[r]});
    }
  }
  return move(QueryProp(P_INPC_HOME),M_TPORT);
}

int walk_route() {
}

int walk_to() {
}

int walk_follow() {
}

int walk_flee() {
}
