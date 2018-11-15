/*
 * queue.c -- Eine Schlange, an der man anstehen kann
 *
 * (c) 1994 Wargon@MorgenGrauen
 *
 */

#include <properties.h>
#include <defines.h>
#include <wizlevels.h>
#include <moving.h>
#include "queue.h"

#define TP this_player()

inherit "std/thing/moving";
inherit "std/room";

static mapping peopleInQueue;
static object lastMoved;

/* Spielerkommendos */
static int anstellen(string str);
static int draengeln(string str);

/* Spieler in Schlange einfuegen/aus Schlange entfernen */
static void enqueue(object ob);
private void leaveQueue(object ob, int silent);

private int queueScan();
private object adjust(int thresh);

/* werden bei "schlafe ein" bzw. "ende" aufgerufen */
void BecomesNetDead(object player);
void PlayerQuit(object player);

protected void create()
{
  if (IS_BLUE(this_object())) return;

  ::create();

  SetProp( P_INDOORS, 1 );
  SetProp( P_LIGHT, 1 );
  SetProp( P_GENDER, FEMALE );

  SetProp( P_SHORT, "Eine Schlange" );
  SetProp( P_LONG, "Du siehst eine Schlange.\n" );
  SetProp( P_INT_SHORT, "In der Schlange" );
  SetProp( P_INT_LONG, "Du stehst in einer Schlange.\n" );
  SetProp( P_NAME, "Schlange" );
  SetProp( P_TRANSPARENT, 0 );

// Netztote werden beim disconnecten eh aus der Schlange entfernt.
// Hiermit wird die Ausgabe des Hinweises unterdrueckt.
  SetProp( P_NETDEAD_INFO, 1 );

  AddId( ({ "queue", "schlange" }) );

  peopleInQueue = ([ ]);
  lastMoved = 0;

// Defaults setzen
  SetProp( Q_CYCLE_TIME, 15 );
  SetProp( Q_DEST, "/gilden/abenteurer" );
  SetProp( Q_LENGTH, 3 );
  SetProp( Q_SUSPEND, 0 );
  SetProp( Q_MSGS, ({ "@@wer@@ ist an der Reihe!", "Du bist jetzt an der Reihe!",
      "Der Naechste ist an der Reihe, und die anderen ruecken auf.",
      "Die Schlange rueckt weiter. Sogleich stellt sich jemand neu an." }) );

  AddSpecialExit( "raustreten", "raustreten" );

  AddCmd( ({ "anstellen", "stell", "stelle" }), "anstellen" );
  AddCmd( ({ "draengeln", "draengle", "draengel", "vordraengeln" }), "draengeln" );
}

int clean_up(int i) { return 0; }

public varargs int
remove(int silent)
{
  string *wer;
  mixed raus;
  int i;

  while( remove_call_out( "rotate" ) != -1 );
  while( remove_call_out( "funny" ) != -1 );

  if (clonep(this_object())) {
    if (!(raus = environment()))
      raus = "/gilden/abenteurer";

    if (i = sizeof(wer = m_indices(peopleInQueue))) {
      tell_room(this_object(), "Die Schlange loest sich auf.\n" );

      while ((--i)>0)
  if (peopleInQueue[wer[i],1])
    peopleInQueue[wer[i],1]->move(raus, M_GO | M_SILENT | M_NO_SHOW);
    }
  }
  return ::remove(silent);
}

string
_query_long()
{
  <string|object>* people = filter(all_inventory(ME), #'interactive);
  string ret = Query(P_LONG);
  string add = "In der Schlange stehen " + QueryProp(Q_LENGTH) + " Leute.";
  if (sizeof(people)) {
    people = people->Name(WER);
    add += sprintf(" Unter anderem steh? dort %s.", 
                   sizeof(people)==1?"t":"en", CountUp(people));
  }
  return ret + break_string(add, 76);
}

string
_query_int_long()
{
  string ret, add;
  int act, cur;

  ret = Query(P_INT_LONG);
  add = "";
  act = peopleInQueue[getuid(TP), 0];
  cur = QueryProp(Q_LENGTH);

  if (act == 0)
    add += ( "Du bist als naechster an der Reihe. Hinter Dir"
      +" stehen noch " + (cur - 1) +" Leute an." );
  else {
    if (act == 1)
      add += "Es ist noch jemand vor Dir dran.";
    else
      add += ( "Es sind noch " + act + " Leute vor Dir dran." );

    if ((cur = cur - act -1) > 0) {
      if (cur == 1)
  add += " Es steht noch jemand hinter Dir.";
      else
  add += ( " Hinter Dir stehen noch " + cur +" Leute an." );
    }
  }
  return ret + break_string(add, 76);
}

int _query_qLength()
{
  return Query(Q_LENGTH) + sizeof(filter(all_inventory(this_object()), #'interactive/*'*/));
}

static int
anstellen(string str)
{
  object tp;
  string *part;

  if (query_verb() != "anstellen") {
    notify_fail( "stelle was?\n" );
    if (!str || str == "")
      return 0;

    part = old_explode(str, " ");
    if (part[0] != "an" && part[<1] != "an") {
      notify_fail( "Syntax: stelle an <schlange> an\n" );
      return 0;
    }
    if (!id(implode(part[1..<2], " ")))
      return 0;
  }
  enqueue(TP);
  return 1;
}

static int
raustreten()
{
  if (environment(this_object())) {
    leaveQueue(TP, 0);
    return 1;
  }
  return 0;
}

static int
draengeln(string str)
{
  int prob;
  string *people;

  if (environment(TP) != this_object())
    return 0;

  prob = random(11);

  if (10 == prob) {
    say( TP->Name(WER) + " will sich vordraengeln. Zur Strafe wird "
      +TP->QueryPronoun(WER) +" von der auf-\n"
      +"gebrachten Menge aus der Schlange hinausgeworfen. Du grinst Dir eins.\n" );
    write( "Du draengelst Dich vor, wirst aber von der aufgebrachten "
      +"Menge aus der\nSchlange herausgedraengt.\n" );
    leaveQueue(TP, 0);
  }
  else if (prob < 8) {
    say( TP->Name(WER) + " will sich vordraengeln.\n" );
    write( "Du willst Dich vordraengeln.\n" );
    tell_room(this_object(), sprintf("Es kommen Rufe auf: %s\n",
       ({ "He! Was soll denn das? Unerhoert!",
    "Also wirklich! Unverschaemtheit sowas!",
    "...soll'n das? Mistbacke!",
    "Jau, soweit kommts noch! Ich glaub' es geht los!",
    "Wieder mal typisch "+TP->QueryProp(P_RACESTRING)[WER]+"!",
    "Wenn sich das jeder erlauben wuerde...",
    "Ne, ne, das geht aber nicht! Mal langsam, "+
       (TP->QueryProp(P_GENDER)==MALE ? "Buerschchen":"Frollein")+"!",
    "POLIZEI! HILFAEEEH!!! Also die Jugend von heute..." })[prob] ));
    say( capitalize(TP->QueryPronoun())+" ueberlegt es sich anders.\n" );
    write( "Du ueberlegst es Dir anders.\n" );
  }
  else {
    say( TP->Name(WER) + " draengelt sich vor.\n" );
    write( "Du draengelst Dich vor.\n" );
    prob = peopleInQueue[getuid(TP)];
    m_delete(peopleInQueue, getuid(TP));
    for(people = m_indices(peopleInQueue); sizeof(people); people = people[1..] ) {
      if(peopleInQueue[people[0],0] < prob)
      peopleInQueue[people[0],0]++;
    }
    peopleInQueue += ([ getuid(TP) : 0; TP ]);
  }
  return 1;
}

static void
enqueue(object ob)
{
  int i;

  if (environment(ob) == this_object()) {
    say(ob->Name(WER) + " stellt sich wieder hinten an.\n", ob );
    tell_object(ob, "Du stellst Dich wieder hinten an.\n" );

    i = peopleInQueue[getuid(ob),0];
    m_delete(peopleInQueue, getuid(ob));
    adjust(i);
    peopleInQueue += ([ getuid(ob) : QueryProp(Q_LENGTH)-1; ob ]);
  }
  else {
    peopleInQueue += ([ getuid(ob) : QueryProp(Q_LENGTH); ob ]);
    ob->move(this_object(), M_NOCHECK, 0, "stellt sich an "+name(WEM)+" an",
      "stellt sich hinten an");
  }
  if (find_call_out("rotate") == -1)
    call_out("rotate", QueryProp(Q_CYCLE_TIME));
}

private void
leaveQueue(object ob, int silent)
{
  int i;

  if (silent)
    ob->move(environment(this_object()), M_NOCHECK | M_SILENT);
  else
    ob->move(environment(this_object()), M_NOCHECK, 0, "verlaesst die Schlange",
    "verlaesst "+name(WEN)+" und steht jetzt neben Dir" );

  i = peopleInQueue[getuid(ob), 0];

  m_delete(peopleInQueue, getuid(ob));

  if (sizeof(peopleInQueue))
    adjust(i);
  else {
    while(remove_call_out("rotate") != -1);
    while(remove_call_out("funny") != -1);
  }
}

private int
queueScan()
{
  string *peop;
  object act;
  int i, a;

  peop = m_indices(peopleInQueue);

  for (i=sizeof(peop)-1; i>=0; i--) {
    if (!objectp(act = peopleInQueue[peop[i],1]) ||
  (environment(act) != this_object())) {
      a = peopleInQueue[peop[i], 0];
      m_delete(peopleInQueue, peop[i]);
      adjust(a);
    }
  }

  return sizeof(peopleInQueue);
}

private object
adjust(int thresh)
{
  string *peop;
  object ret;
  int cnt, i;

  ret = 0;
  peop = m_indices(peopleInQueue);

  for (i=sizeof(peop)-1; i>=0; i--) {
    cnt = peopleInQueue[peop[i],0];
    if (cnt > thresh)
      cnt--;
    if (cnt < 0)
      ret = peopleInQueue[peop[i],1];
    peopleInQueue[peop[i],0] = cnt;
  }
  return ret;
}

void rotate()
{
  object ich;
  string oth, *msgs;
  int rotating, inDest;

  rotating = 1;

  if (QueryProp(Q_SUSPEND)) {
    if (lastMoved) {
      rotating = 0;
      inDest = (object_name(environment(lastMoved)) == QueryProp(Q_DEST));
      if (!interactive(lastMoved) || !inDest )
  rotating = 1;
      if (interactive(lastMoved) && inDest && environment() && query_idle(lastMoved) > 180) {
  tell_object(lastMoved, "Du wirst rausgeworfen.\n");
  lastMoved->move(environment(), M_GO, 0, "idlet herein");
  rotating = 1;
      }
    }
  }
  if (rotating) {
    queueScan();
    msgs = QueryProp(Q_MSGS);
    if (ich = adjust(-1)) {
      oth = implode(explode(msgs[QMSG_OTHER], "@@wer@@"), ich->name(WER) );
      oth = implode(explode(oth, "@@wessen@@"), ich->name(WESSEN) );
      oth = implode(explode(oth, "@@wem@@"), ich->name(WEM) );
      oth = implode(explode(oth, "@@wen@@"), ich->name(WEN) );
      tell_room(this_object(), capitalize(break_string(oth, 78)), ({ ich }) );
      tell_object(ich, break_string(msgs[QMSG_ME], 78) );
      tell_room(environment(), break_string(msgs[QMSG_OUTSIDE], 78) );
      ich->move(QueryProp(Q_DEST), M_GO);
      lastMoved = ich;
      m_delete(peopleInQueue, getuid(ich));
      if (!sizeof(peopleInQueue))
  return;
    }
    else
      tell_room(this_object(), break_string(msgs[QMSG_DEFAULT], 78) );
  }

  if (random(100) < 20)
    call_out("funny", random(QueryProp(Q_CYCLE_TIME)) );

  call_out("rotate", QueryProp(Q_CYCLE_TIME));
}

void funny()
{
  string m;

  switch(random(7)) {
    case 0:
      m= "Jemand vor Dir laesst tierisch einen fahren. Seltsamerweise wirft man\n"
  +"aber Dir missbilligende Blicke zu.\n";
      break;
    case 1:
      m= "Von hinten draengelt wer! Wuetend drehst Du Dich um, schaust hoch, schaust\n"
  +"hoeher, schaust noch hoeher... und schon ist Deine Wut verflogen!\n";
      break;
    case 2:
      m= "Vor Langeweile popelt jemand vor Dir in der Nase. Das quietschende Ge-\n"
  +"raeusch laesst Dir das Blut in den Adern gefrieren.\n";
      break;
    case 3:
      m= "Ein fliegender Haendler landet neben der Schlange und bietet Kurzwaren\n"
  +"feil. Da aber niemand etwas kaufen will, hebt er kurz darauf wieder ab\n"
  +"und fliegt zu einer anderen Schlange, um dort sein Glueck zu versuchen.\n";
      break;
    case 4:
      m= "Vom hinteren Ende der Schlange sickert das Geruecht durch, dass der\n"
  +"Schalter heute geschlossen ist. Du hoffst instaendig, dass dies nicht\n"
  +"wahr ist.\n";
      break;
    case 5:
      m= "Fasziniert beobachtest Du, wie eine Spinne zwischen Deinem Vordermann und\n"
  +"der Wand ein kunstvolles Netz spinnt.\n";
      break;
    case 6:
      m= "Boah, iss dat langweilich...\n";
      break;
  }
  tell_room(this_object(), m);
}

void
BecomesNetDead(object player)
{
  if(present(player,this_object()))leaveQueue(player, 0);
}

void
PlayerQuit(object player)
{
  if(present(player,this_player()))leaveQueue(player, 1);
}

