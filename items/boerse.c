#pragma strong_types,rtt_checks

inherit "/std/container";

#define U_REQ      "u_req"

#include <properties.h>
#include <language.h>
#include <defines.h>
#include <moving.h>
#include <money.h>

// zum debuggen und extra public
public string debugmode;
public string __set_debug(string recv) {return debugmode=recv;}
#include <living/comm.h>
#define ZDEBUG(x) if (stringp(debugmode) && find_player(debugmode)) \
  find_player(debugmode)->ReceiveMsg(x,MT_DEBUG,0,object_name()+": ",ME)
//#define ZDEBUG(x)

int dontacceptmoney;

protected void create()
{
  ::create();
  SetProp(P_NAME, "Geldboerse");
  SetProp(P_SHORT, "Eine Geldboerse");
  SetProp(P_LONG,  "Eine schoene aus Leder gefertigte Geldboerse.\n");
  SetProp(P_MATERIAL, ([MAT_LEATHER:100]));
  SetProp(P_GENDER, FEMALE);
  SetProp(P_VALUE,  80);
  SetProp(P_WEIGHT, 300);
  SetProp(P_WEIGHT_PERCENT, 50);
  SetProp(P_MAX_WEIGHT, 250000); // 1 mio. Muenzen.
  AddId(({"geldboerse", "boerse", BOERSEID}));
  SetProp(P_NOINSERT_MSG,
      "Du kannst immer nur eine Geldboerse gleichzeitig benutzen.\n");
}

static string _query_keep_on_sell()
{
  if (first_inventory() && living(environment()))
     return getuid(environment());
  return Query(P_KEEP_ON_SELL);
}

static int _query_amount()
{
  object ob = first_inventory();
  if (load_name(ob) == GELD)
    return ob->QueryProp(P_AMOUNT);
  return 0;
}

void AddAmount(int am)
{
  object ob = first_inventory();
  if (load_name(ob) == GELD)
    ob->AddAmount(am);
}

string short()
{
  int i;
  switch (i=QueryProp(P_AMOUNT)) {
    case 0:  return "Eine leere Geldboerse.\n";
    case 1:  return "Eine Geldboerse mit einer Muenze.\n";
    default: return "Eine Geldboerse mit "+i+" Muenzen.\n";
  }
  return 0;
}

// Geld darf nur rein, wenn diese Boerse schon Geld enthaelt
// ODER es auch keine andere im lebenden (!) Inventar gibt, die Geld
// enthaelt.
private int accept_money()
{
  // wenn wir gerade zerstoert werden, ist das hier gesetzt, dann akzeptieren
  // wir ein Geld. Sonst wurde Geld, was gerade in prepare_destruct()
  // rausbewegt wurde, evtl. wieder reinbewegt...
  if (dontacceptmoney)
    return 0;
  // Ausserhalb von Livings geht reinstecken von Geld immer.
  if (!living(environment()))
    return 1;
  // wenn in uns Geld ist, auch.
  object geld = first_inventory();
  if (geld && load_name(geld) == GELD)
    return 1; // erlaubt
  // wir haben kein Geld... Wenn es eine mit Geld im gleichen inv gibt,
  // nehmen wir keins.
  object andereboerse = present(GELDBOERSE_MIT_GELD, environment());
  if (objectp(andereboerse))
    return 0;

  return 1;
}

varargs int PreventInsert(object ob)
{
  if (ob && load_name(ob)==GELD
      && accept_money())
  {
    return ::PreventInsert(ob);
  }
  return 1; // nur geld erlaubt
}

public void MergeMoney(object geld)
{
  if (geld && previous_object() == geld
      && load_name(geld) == GELD
      && accept_money())
  {
    int fremdamount = geld->QueryProp(P_AMOUNT);
    // Da wir aus einen NotifyMove (d.h. move()) gerufen werden, darf hier
    // keinesfalls ein move() gemacht werden.
    // Wenn in uns Geld ist, prima, einfach P_AMOUNT veraendern.
    // Wenn nicht, muessen wir ein neues Geldobjekt clonen, falls fremdamount
    // > 0 ist.
    object meingeld = first_inventory();
    if (meingeld && load_name(meingeld) == GELD)
    {
      int meinamount = meingeld->QueryProp(P_AMOUNT);
      ZDEBUG(sprintf("MergeMoney: meinamount: %d, fremdamount: %d\n",
            meinamount,fremdamount));
      // wenn fremdamount positiv ist, vereinigen wir uns natuerlich, auch
      // wenn mein Geld negativ ist. Aber max. 1 Mio. Muenzen aufnehmen.
      if (fremdamount > 0)
      {
        int zubuchen = min(1000000-meinamount, fremdamount);
        ZDEBUG(sprintf("MergeMoney: zubuchen: %d\n", zubuchen));
        meingeld->SetProp(P_AMOUNT, meinamount + zubuchen);
        geld->SetProp(P_AMOUNT, fremdamount - zubuchen);
        // environment verstaendigen ueber Inventaraenderung... Und uns. Wir
        // machen nur die beiden, weil alle hoehren Envs nicht so wichtig
        // sind, dass es sofort gemacht werden muss.
        environment()->_set_last_content_change();
        _set_last_content_change();
      }
      // ansonsten vereinigen wir uns, wenn meinamount > 0 und fremdamount < 0
      // ist, aber nur soviel, dass hinterher in mir nix mehr verbleibt.
      else if (meinamount > 0 && fremdamount < 0)
      {
        int amount_to_merge = min(meinamount, abs(fremdamount));
        ZDEBUG(sprintf("MergeMoney: zubuchen: %d\n", amount_to_merge));
        meingeld->SetProp(P_AMOUNT, meinamount - amount_to_merge);
        geld->SetProp(P_AMOUNT, fremdamount + amount_to_merge);
        // environment verstaendigen ueber Inventaraenderung... Und uns. Wir
        // machen nur die beiden, weil alle hoehren Envs nicht so wichtig
        // sind, dass es sofort gemacht werden muss.
        environment()->_set_last_content_change();
        _set_last_content_change();
      }
      // in anderen Faellen vereinigen wir einfach nicht (beide negativ)
    }
    // Ansonsten nehmen wir nur positives Geld und nur, soweit es noch
    // reinpasst.
    else if (fremdamount > 0)
    {
      ZDEBUG(sprintf("MergeMoney: leere Boerse, fremdamount: %d\n",
             fremdamount));
      meingeld = clone_object(GELD);
      // nocheck, weil wir eigentlich <geld> bewegen wollen und schon
      // festgestellt haben, dass diese Boerse Geld akzeptiert. Tragen kann
      // der Spieler es auch (sogar ohne Gewichtsreduktion). Die Boerse soll
      // max. 1Mio Muenzen aufnehmen.
      int zubuchen = min(fremdamount,1000000);
      ZDEBUG(sprintf("MergeMoney: zubuchen: %d\n", zubuchen));
      meingeld->SetProp(P_AMOUNT, zubuchen);
      geld->SetProp(P_AMOUNT, fremdamount-zubuchen);
      meingeld->move(this_object(), M_NOCHECK);
      // environment verstaendigen ueber Inventaraenderung durch geld->SetProp
      environment()->_set_last_content_change();
    }
    ZDEBUG(sprintf("MergeMoney: Final: meinamount: %d, fremdamount: %d\n",
          meingeld->QueryProp(P_AMOUNT),geld->QueryProp(P_AMOUNT)));
  }
}

varargs int remove(int silent)
{
  dontacceptmoney=1;
  return ::remove(silent);
}

varargs int id(string str, int lvl)
{
  if (str==GELDBOERSE_MIT_GELD
      && load_name(first_inventory()) == GELD)
      return 1;
  return ::id(str, lvl);
}

