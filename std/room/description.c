// MorgenGrauen MUDlib
//
// room/description.c -- room description handling
//
// $Id: description.c 9468 2016-02-19 21:07:04Z Gloinson $

#pragma strong_types
#pragma save_types
#pragma pedantic
#pragma range_check
#pragma no_clone

inherit "/std/container/description";

#define NEED_PROTOTYPES

#include <properties.h>
#include <defines.h>
#include <wizlevels.h>
#include <language.h>
#include <doorroom.h>
#include <room/description.h>

protected void create()
{
  ::create();
  SetProp(P_NAME, "Raum");
  SetProp(P_INT_SHORT,"<namenloser Raum>");
  SetProp(P_INT_LONG,0);
  SetProp(P_ROOM_MSG, ({}) );
  SetProp(P_FUNC_MSG, 0);
  SetProp(P_MSG_PROB, 30);
  AddId(({"raum", "hier"}));
}

public varargs void init(object origin)
{
  // Wenn P_ROOM_MSG gesetzt oder P_FUNC_MSG und kein Callout laeuft,
  // Callout starten.
  mixed roommsg = QueryProp(P_ROOM_MSG);
  if( ( (roommsg && sizeof(roommsg)) ||
        QueryProp(P_FUNC_MSG) ) &&
      (find_call_out("WriteRoomMessage")==-1))
    call_out("WriteRoomMessage", random(QueryProp(P_MSG_PROB)));
}

varargs void AddRoomMessage(string *mesg, int prob, mixed func)
{
  if (mesg && !pointerp(mesg))
    raise_error(sprintf(
      "AddRoomMessage(): wrong argument type, expected Array or 0, "
      "got %.20O",mesg));

   SetProp(P_ROOM_MSG, mesg);

  if (prob>0)
    SetProp(P_MSG_PROB, prob);

  if (func)
    SetProp(P_FUNC_MSG, func);
}

static void WriteRoomMessage()
{
  int msgid;

  string *room_msg = QueryProp(P_ROOM_MSG);
  <string|string*> func_msg = QueryProp(P_FUNC_MSG);
  if ((!room_msg || !sizeof(room_msg)) && !func_msg)
    return;

  if (room_msg&&sizeof(room_msg))
  {
    msgid = random(sizeof(room_msg));
    // Defaultwerte sind fuer Altcode schwierig
    send_room(this_object(), room_msg[msgid],
              MT_LOOK|MT_LISTEN|MT_FEEL|MT_SMELL|
              MSG_DONT_STORE|MSG_DONT_BUFFER|MSG_DONT_WRAP);
  }

  if (func_msg)
  {
    string func;
    if (stringp(func_msg))
      func=(string)func_msg;
    else
      func=func_msg[random(sizeof(func_msg))];
    if (func && function_exists(func))
      call_other (this_object(), func, msgid);
  }

  while (remove_call_out("WriteRoomMessage")!=-1);
  if(this_object() && sizeof(filter(
       deep_inventory(this_object()), #'interactive))) //')))
    call_out("WriteRoomMessage", max(15, QueryProp(P_MSG_PROB)));
}

varargs string int_long(mixed viewer,mixed viewpoint,int flags)
{
  string descr, inv_descr;
  string|int transparency;

  flags &= 3;
  if( IS_LEARNER(viewer) && viewer->QueryProp( P_WANTS_TO_LEARN ) )
    descr = "[" + object_name(ME) + "]\n";
  else
    descr = "";

  descr += process_string(QueryProp(P_INT_LONG)||"");
  
  // ggf. Tueren hinzufuegen.
  if (QueryProp(P_DOOR_INFOS)) {
    string tmp=((string)call_other(DOOR_MASTER,"look_doors"));
    if (stringp(tmp) && sizeof(tmp))
        descr += tmp;
  }
  
  // ggf. Ausgaenge hinzufuegen.
  if ( viewer->QueryProp(P_SHOW_EXITS) && (!QueryProp(P_HIDE_EXITS) 
	|| pointerp(QueryProp(P_HIDE_EXITS))) )
    descr += GetExits(viewer) || "";

  // Viewpoint (Objekt oder Objektarray) sind nicht sichtbar
  inv_descr = (string) make_invlist(viewer, all_inventory(ME) 
		  - (pointerp(viewpoint)?viewpoint:({viewpoint})) ,flags);

  if ( inv_descr != "" )
    descr += inv_descr;

  if(environment() && (transparency=QueryProp(P_TRANSPARENT)))
  {
    if(stringp(transparency)) descr += transparency;
    else                   descr += "Ausserhalb siehst Du:\n";

    descr += environment()->int_short(viewer,ME);
  }

  return descr;
}

string int_short(mixed viewer,mixed viewpoint)
{
  string descr, inv_descr;

  descr = process_string( QueryProp(P_INT_SHORT)||"");

  // Ist das letzte Zeichen kein Satzzeichen einen Punkt anhaengen, sonst nur
  // den \n.
  int i;
  if (sizeof(descr))
    i = descr[<1];

  if(i!='.' && i!='!' && i!='?')
    descr+=".\n";
  else
    descr+="\n";

  // Aber ggf. den Pfad fuer Magier vor dem \n einfuegen.
  if(IS_LEARNING(viewer))
    descr[<1..<2]=" [" + object_name(ME) + "]";

  if ( ( viewer->QueryProp(P_SHOW_EXITS)
         || ( environment(viewer) == ME && !viewer->QueryProp(P_BRIEF) ) )
       && (!QueryProp(P_HIDE_EXITS) || pointerp(QueryProp(P_HIDE_EXITS))) )
    descr += GetExits(viewer) || "";

  // Viewpoint (Objekt oder Objektarray) sind nicht sichtbar
  inv_descr = (string) make_invlist( viewer, all_inventory(ME) 
		  - (pointerp(viewpoint)?viewpoint:({viewpoint})) );

  if ( inv_descr != "" )
    descr += inv_descr;

  return descr;
}

/** Roommessages abschalten, wenn keine Interactives mehr da sind.
  */
// TODO: Irgendwann das varargs loswerden, wenn in der restlichen Mudlib
// TODO::exit() 'richtig' ueberschrieben wird.
varargs void exit(object liv, object dest) {
  // fall erbende Objekte das liv nicht uebergeben. Pruefung nur auf
  // previous_object(). Wenn Magier da noch irgendwelche Spielchen mit
  // call_other() & Co treiben, haben wir Pech gehabt, macht aber nicht viel,
  // weil die Raummeldungen dann im naechsten callout abgeschaltet werden.
  if (!living(liv=previous_object())) return;

  object *interactives = filter(all_inventory(), #'interactive);
  // liv wurde noch nicht bewegt, ggf. beruecksichtigen.
  if ( !sizeof(interactives) ||
      (interactive(liv) && sizeof(interactives) < 2) )
    while (remove_call_out("WriteRoomMessage")!=-1);
}

static string _query_int_long() {return Query(P_INT_LONG, F_VALUE);}


// Querymethode fuer P_DOMAIN - gibt die Region an, in der Raum liegt.
// Automatisch ermittelt, sofern er unter /d/ liegt...
static string _query_lib_p_domain()
{
  // Manuell gesetztes P_DOMAIN hat Prioritaet.
  string domain = Query(P_DOMAIN, F_VALUE);
  if (sizeof(domain))
    return domain;

  string fn = object_name();
  if (strstr(fn, "/d/") == 0)
  {
    return capitalize(explode(fn, "/")[2]);
  }

  return "unbekannt";
}

<string|string*>* _set_harbour_name( <string|string*>* desc) 
{
  if ( sizeof(desc)!=2 )
  {
    raise_error(sprintf("Unacceptable data in P_HARBOUR, sizeof() was %d, "
      "expected 2.", sizeof(desc)));
  }
  else if ( !stringp(desc[0]) )
  {
    raise_error("Wrong data type in P_HARBOUR[0]: expected 'string'.");
  }
  else if ( pointerp(desc[1]) && sizeof(desc[1])<1 )
  {
    raise_error("Insufficient data in P_HARBOUR[1]: expected 'string*', "
      "got '({})'.");
  }
  else if ( stringp(desc[1]) )
  {
    desc[1] = ({desc[1]});
  }
  return Set(P_HARBOUR, desc, F_VALUE);
}

