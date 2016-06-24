// block.c -- Hier sammeln sich die Raten fuer das Seherhaus
//
// (c) 1994 Wargon@MorgenGrauen
//
// $Id: block.c,v 1.2 2003/11/15 13:56:01 mud Exp $
//
#include <properties.h>
#include <language.h>
#include <moving.h>
#include <combat.h>
#include "haus.h"

inherit "std/clothing";

private nosave int shadowing;

create()
{
  if (!clonep(this_object())) return;

  ::create();

  SetProp( P_SHORT, "Der MG MASTER-BLOCK" );
  SetProp( P_NAME, "MASTERBLOCK" );
  SetProp( P_WEIGHT, 100 );
  SetProp( P_GENDER, MALE );
  SetProp( P_NODROP, 1 );
  SetProp( P_NEVERDROP, 1 );
  // wenn es keinen TP gibt, muss das Ding per SetProp(P_AUTOLOADOBJ)
  // konfiguriert werden, nicht hier. Zesstra, 20.10.2007
  if (objectp(this_player()))
    Set( P_AUTOLOADOBJ, ({ 0, this_player()->QueryProp(P_AGE) + MAX_TIME, 
	                   MAX_TIME/2, B_EP }) );
  Set( P_AUTOLOADOBJ, SECURED, F_MODE );
  SetProp( P_NOBUY, 1 );
  SetProp(P_MATERIAL, MAT_MARBLE);

  SetProp(P_WEAR_FUNC, this_object());
  SetProp(P_REMOVE_FUNC, this_object());

  AddId( ({ "block", "masterblock", "master-block", "MG MASTER-BLOCK", "mg master-block", "\n block" }) );

  AddCmd( "", "remindMe", 1);
}

int query_prevent_shadow(object ob)
{
  HLOG("SHADOW", sprintf("%s, von %O im Block.\n",dtime(time())[5..], ob));
  return 1;
}

int
WearFunc(object me)
{
  if (!shadowing && (Query(P_AUTOLOADOBJ)[V_FLAGS] & B_ACTIVE) ) {
    clone_object("/std/player/shadows/block_shadow");
    shadowing = 1;
  }
  return 1;
}

int
RemoveFunc(object me)
{
  // manchmal gibts keinen TP und machmal ist TP nicht der Spieler, der den
  // Block traegt.
  if (this_player() && this_player() == environment()) {
    shadowing = 0;
    this_player()->SeherHatGenug();
    return 1;
  }
  return 0;
}

static int
_query_value()
{
  return( Query(P_AUTOLOADOBJ)[0] );
}

static int
_query_cursed()
{
  return 0;
}

static int
_set_schwer(int schwer)
{
  int *al;

  al = Query(P_AUTOLOADOBJ);
  if (schwer && !(al[V_FLAGS] & B_FAST)) {
    al[V_DLINE] -= MAX_TIME/2;
    al[V_WTIME] /= 2;
    al[V_FLAGS] |= B_FAST;
  }
  else if (!schwer && (al[V_FLAGS] & B_FAST)) {
    al[V_DLINE] += MAX_TIME/2;
    al[V_WTIME] *= 2;
    al[V_FLAGS] &= ~B_FAST;
  }
  Set( P_AUTOLOADOBJ, al[0..]);
  return al[V_FLAGS] & B_FAST;
}

static int
_query_schwer()
{
  return (Query(P_AUTOLOADOBJ)[V_FLAGS] & B_FAST);
}

static mixed
_set_autoloadobj(mixed al)
{
  object ti;

  if ((ti = this_interactive()) && old_explode(object_name(ti),"#")[0] != "/secure/login")
    HLOG("SET_AL",sprintf("Block: TI = %O, ENV = %O\n",this_interactive(), environment()));

  /* Kompatibilitaet... */
  if (pointerp(al) && !(al[V_FLAGS] & B_EP)) {
    al[V_MONEY] *= 4;
    al[V_FLAGS] |= B_EP;
  }
  return Set(P_AUTOLOADOBJ, al);
}

static string
_query_long()
{
  int *al;
  int laufzeit, einzahlung;
  int ratenhoehe;
  string ret;

  al = Query(P_AUTOLOADOBJ);
  ret = "Dies ist der MG MASTER-BLOCK. Er besteht aus massivem Marmor, um die Sicher-\n"
       +"heit der eingezahlten Rate zu gewaehrleisten. Mit einem Goldkettchen versehen,\n"
       +"kann man ihn tragen wie ein Amulett. Dies ist auch noetig, wenn auf den Block\n"
       +"eingezahlt werden soll.\n";

  ratenhoehe = RATENHOEHE;
  if (al[V_FLAGS] & B_EXTEND)
    ratenhoehe += (RATENHOEHE*6)/10;
  laufzeit = 2*(al[V_DLINE]-this_player()->QueryProp(P_AGE));
  einzahlung = al[V_MONEY];

  if (laufzeit < 0)
    ret += "Leider hast Du die Zahlung dieser Rate verpasst!\n";
  else {
    if (einzahlung >= ratenhoehe)
      ret += "Er ist vollstaendig mit der Rate gefuellt.\n";
    else
      ret += sprintf( "Du hast bis jetzt %d (von %d) Punkten eingezahlt.\n", einzahlung, ratenhoehe );

    ret += time2string("Diese Rate muss bis in %h %H, %m %M und %s %S abbezahlt sein!\n", laufzeit);
  }
  return ret;
}

static int
remindMe()
{
  int noch, next, *al, ratenhoehe;
  object vertrag;

  al = Query(P_AUTOLOADOBJ);
  next = al[V_WTIME];
  ratenhoehe = RATENHOEHE;
  if (al[V_FLAGS] & B_EXTEND)
    ratenhoehe += (RATENHOEHE*6)/10;

  if (al[V_MONEY] < ratenhoehe) {
    noch = al[V_DLINE]-this_player()->QueryProp(P_AGE);
    if (noch >= 0) {
      if (next > noch) {
	call_out("doRemind", 0, this_player(), time2string( "Die naechste Rate wird in %h %H, %m %M und %s %S faellig!\n", 2*noch));
	do next -= WARN_TIME; while (next>0 && next>noch);
	if (next < 0)
	  next = 0;
	al[V_WTIME] = next;
	Set(P_AUTOLOADOBJ, al[0..]);
      }
    }
    else {
      if (al[V_FLAGS] & B_EXTEND) {
	string s;

	write( "Du hast die Strafrate nicht bezahlt!\n" );
	if (vertrag = present( "sehe\rvertrag", this_player())) {
	  write( "Damit verfaellt auch Dein Bausparvertrag! Und das im wahrsten Sinne des\nWortes...\n" );
	  vertrag->remove();
	}
	s = "Block: "+getuid(this_player())+" hat ueberzogen... ;)\n";
	HLOG(getuid(this_player()), s);
	HLOG("BANK.LOG", s);
	remove(1);
      }
      else {
	write( "Du hast die letzte Ratenzahlung verpasst!\n"
	      +"Die Ratenhoehe erhoeht sich um die Strafgebuehr.\n" );
	al[V_FLAGS] |= B_EXTEND;
	if (al[V_FLAGS] & B_FAST) {
	  al[V_DLINE] += MAX_TIME/4;
	  al[V_WTIME] = MAX_TIME/8;
	}
	else {
	  al[V_DLINE] += MAX_TIME/2;
	  al[V_WTIME] = MAX_TIME/4;
	}
	Set( P_AUTOLOADOBJ, al[0..]);
      }
    }
  }

  return 0;
}

int
Gutschreiben(int money)
{
  int back, ratenhoehe;
  mixed al;

  if (!QueryProp(P_WORN))  // Zum Gutschreiben muss der Block getragen werden!
    return money;

  al = Query(P_AUTOLOADOBJ);
  back = 0;

  ratenhoehe = RATENHOEHE;
  if (al[V_FLAGS] & B_EXTEND)
    ratenhoehe += (RATENHOEHE*6)/10;

  if (old_explode(object_name(previous_object()),"#")[0] != "/std/player/shadows/block_shadow") {
    mixed env;

    env = environment();
    if (env)
      env = getuid(env);
    else
      env = "BANK.LOG";
    HLOG(env, sprintf("Block: Gutschrift von aussen (%d EP), TI = %O)!\n", money, this_interactive()));
  }
  al[V_MONEY] += money;
  if (al[V_MONEY] >= ratenhoehe) {  // genug eingezahlt...
    back = al[V_MONEY]-ratenhoehe;
    al[V_MONEY] = ratenhoehe;
    al[V_FLAGS] &= ~B_ACTIVE;
    money -= back;
    call_out("blockVoll",0,environment());
  }

  Set(P_AUTOLOADOBJ, al[0..]);

  return back;
}

void
blockVoll(object pl)
{
  tell_object(pl, "Der Block fuer diese Rate ist jetzt voll!\n");
  pl->SeherHatGenug();	// shadow entfernen
}

void
doRemind(object pl, string str)
{
  tell_object(pl, str);
}

protected int PreventMove(object dest, object oldenv, int method) {
  if (previous_object() && previous_object()->QueryBuyFact()) // Im Laden...
    return ME_CANT_BE_DROPPED;
  return ::PreventMove(dest, oldenv, method);
}

protected void NotifyMove(object dest, object oldenv, int method) {

  if (oldenv || (object_name(previous_object()) != PATH+"sb_antrag" &&
		  object_name(previous_object()) != PATH+"sb_einzahlung" &&
		  object_name(previous_object()) != object_name(dest)))
    HLOG("MOVING", sprintf("Block: von %O -> %O mit %O (%O)\n",
			   environment(), dest,
			   previous_object(), getuid(previous_object())));

  return ::NotifyMove(dest, oldenv, method);
}

int
remove(int silent)
{
  if (clonep(this_object()) && environment())
    environment()->SeherHatGenug();

  return ::remove(silent);
}

// $Log: block.c,v $
// Revision 1.2  2003/11/15 13:56:01  mud
// MAT_MARBLE
//
// Revision 1.1.1.1  2000/08/20 20:22:42  mud
// Ins CVS eingecheckt
//
// Revision 1.4  1994/12/17  15:56:58  Wargon
// Etwas mehr Sicherheit gegen Zugriffe von aussen.
//
// Revision 1.3  1994/10/21  09:50:20  Wargon
// long()->_query_long()
// Shadow wird nur aktiv, wenn der Block gezueckt ist.
//
// Revision 1.2  1994/10/09  21:04:27  Wargon
// Man kann jetzt einstellen, ob der Block zu einem leichten, aber
// teureren oder zu einem schweren, aber billigeren Vertrag gehoert.
// Falls man eine Rate verpasst hat, verfaellt damit auch der Ver-
// trag.
// Gutgeschrieben wird nur, wenn der Block gezueckt ist.
// Gutschreiben() liefert ueberzaehliges Geld zurueck.
//
// Revision 1.1  1994/10/07  14:35:34  Wargon
// Initial revision
//
