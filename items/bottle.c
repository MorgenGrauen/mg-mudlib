/****************************************/
/* BOTTLE.C -- a standard bottle object */
/*             or better a container for*/
/*             liquides                 */
/* --                                   */
/* (c) by Hate 1993                     */
/****************************************/

inherit "/std/thing";

/*****************************************************************************/
#include <properties.h>
#include <language.h>

#define PL		this_player()

#define GULP		20		/* millilitre is one gulp ;)         */

#define NO		({"hundert", "neunzig", "achtzig", "siebzig",\
                          "sechzig", "fuenfzig", "vierzig", "dreissig",\
                          "zwanzig", "zehn"})

#define FULL(what, amount)	(what*_MaxCapacity/amount)
#define PART(what, amount)	(what*amount/_MaxCapacity)
#define AVER(x,y, amount)	((FULL(x, amount)+y)/2)

private string _Fname, _Fdescr;
private int _Fprice, _Fheal, _Fstrength, _Fsoak;
private int _MaxCapacity, _Capacity, _NoDrinking;
private int _open, _NoClose;

/**** ALL THE PUBLIC FUNCTIONS ***********************************************/
public void LiquidContDescr(string Cname, string *Cids, string Cshort, string Clong,
                            int Cweight, int Capa, int NoClose, string Clabel)
/* give a decription for the liquid container          */
/* Cname   -- P_NAME of the container                  */
/* Cid     -- identity of the container                */
/* Cshort  -- P_SHORT of the container                 */
/* Clong   -- P_LONG of the container                  */
/* Cweight -- weight of the container                  */
/* Ccapa   -- capacity of the container                */
/* -- OPTIONAL --                                      */
/* NoClose -- means if 1 the container can't be closed */
/* Clabel  -- optional readable label (for bottles)    */
{
  SetProp(P_NAME, Cname);
  SetProp(P_SHORT, Cshort);
  SetProp(P_LONG, Clong);
  SetProp(P_READ_MSG, Clabel);
  SetProp(P_WEIGHT, Cweight);
  SetProp(P_IDS, Cids);
  _MaxCapacity = Capa;
  _Capacity = Capa;
  if(_NoClose = NoClose) _open = 1;
}

public int AddLiquid(int Famount, string Fname, string Fdescr, int Fprice,
                     int Fheal, int Fstrength, int Fsoak) 
/* add something in the container                                          */
/* Famount   -- amount of liquid to be added to container, if more ignored */
/*              or better the container will run over                      */
/* Fname     -- name of the liquid, if there is still something inside the */
/*              container, it it will be mixed                             */
/* Fdescr    -- if somebody wants to look at the liquid he sees it if the  */
/*              container is open                                          */
/* Fprice    -- the price of the liquid for the given amount               */
/* Fheal     -- the healing per for the given amount                       */
/* Fstrength -- the strength of the liquid for the given amount            */
/* Fsoak     -- the soak value for the given amount                        */
{
  if(_Capacity && Famount)
  {
    if(Famount<=_Capacity) _Capacity -= Famount;
    else { Famount = _Capacity; _Capacity = 0; } 
    if(!_Fname || _Fname == Fname) { _Fname = Fname; _Fdescr = Fdescr; }
    else { _Fdescr = "Eine Mixtur aus "+_Fname+" und "+Fname; 
           _Fname = _Fname+":"+Fname; }
    if(_Fprice) _Fprice = AVER(Fprice, _Fprice, Famount);
    else _Fprice = FULL(Fprice, Famount);
    if(_Fheal) _Fheal = AVER(Fheal, _Fheal, Famount);
    else _Fheal = FULL(Fheal, Famount);
    if(_Fstrength) _Fstrength = AVER(Fstrength, Fstrength, Famount);
    else _Fstrength = FULL(Fstrength, Famount);
    if(_Fsoak) _Fsoak = AVER(Fsoak, _Fsoak, Famount);
    else _Fsoak = FULL(Fsoak, Famount);
    SetProp(P_VALUE, PART(_Fprice, (_MaxCapacity-_Capacity)));
  }
  return Famount;
}   

/* to pour something out of the container                              */
/* Famount -- amount of liquid to be poured                            */
/* -- OPTIONAL --                                                      */
/* where   -- if set the amount will be added to the container 'where' */ 
public int PourLiquid(int Famount, object where)
{
  int pour, rest, oldc;

  oldc = _Capacity;
  if(objectp(where) && where->QueryContainer())
  {
    pour = Famount;
    while(pour &&
          (rest=where->AddLiquid(GULP, _Fname, _Fdescr, 
                                 PART(_Fprice, Famount),
                                 PART(_Fheal, Famount),
                                 PART(_Fstrength, Famount),
                                 PART(_Fsoak, Famount)))==GULP)
      pour -= GULP;
    Famount -= rest;
  }
  else
    if(objectp(where) && living(where))
    {
      if(Famount>_MaxCapacity-_Capacity) Famount = _MaxCapacity-_Capacity;
      if(PART(_Fsoak, Famount) && !where->drink_soft(PART(_Fsoak, Famount)))
        return 0; 
      if(!where->drink_alcohol(PART(_Fstrength, Famount)))
        return 0; 
      where->heal_self(PART(_Fheal, Famount));
    }
  _Capacity += Famount;
  SetProp(P_VALUE, PART(_Fprice, (_MaxCapacity-_Capacity)));
  return oldc != _Capacity;
}

/* sometimes there is something poinsonous inside the container           */
/* no -- must be 1 if it is not allowed to drink (standard yes)           */
/*       that means the liquid can be poured and filled, but nothing else */
public int AllowDrinking(int no) { return _NoDrinking = no; }

/* returns the amount and name of the contents as an array 0-contents 1-name */
public <int|string>* QueryContents() { return ({ _MaxCapacity-_Capacity, _Fname }); }

/* returns 1 if this is an liquid container */
public int QueryContainer() { return 1; }

/*****************************************************************************/
protected void create()
{
  if (!clonep(this_object())) return;
  ::create();

  SetProp(P_NAME, "Flasche");
  AddId("flasche");
  SetProp(P_SHORT, "Eine Flasche");
  SetProp(P_LONG, "Du siehst nichts besonderes an oder in der Flasche.\n");
  SetProp(P_GENDER, FEMALE);
  SetProp(P_WEIGHT,100);
}

void init()
{
  ::init();
  
  add_action("DoDrink", "trinke");
  add_action("DoDrink", "trink");
  add_action("DoPour", "schuette");
  add_action("DoPour", "schuett");
  add_action("DoPour", "fuelle");
  add_action("DoPour", "fuell");
  add_action("DoOpen", "oeffne");
  add_action("DoClose", "schliesse");
  add_action("DoLookInside", "schaue");
  add_action("DoLookInside", "schau");
}

int DoDrink(string arg)
{
  string howmuch, trash, str;
  int all, cnt;

  if(!_NoDrinking && arg && sscanf(arg, "%saus %s", str, trash)==2 && id(trash))
  {
    if(!_open) return write(capitalize(name(0,1))+" ist aber noch zu.\n") || 1;
    if(_Capacity==_MaxCapacity)
    {
      write("Du setzt "+name(WEN, 1)+" an und merkst, dass nichts mehr darin "
           +"ist.\n");
      say(PL->name()+" setzt "+name(WEN)+" an und merkt, dass nichts mehr darin"
         +" ist.\n");
      return 1;
    }
    if(sscanf(str, "alles%s", trash) == 1)
    {
      all = _Capacity;
      while(PourLiquid(GULP, PL));
      if(_Capacity==all) return write("Du schaffst es nicht zu trinken.\n"),0;
      if(_MaxCapacity>_Capacity)
        write("Du hast nur einen Teil zu trinken geschafft.\n");
      else
      {
        write("Du trinkst alles bis zum letzten Rest aus "+name(WEM, 1)+".\n");
        say(PL->name()+" trinkt alles bis zum letzten Rest aus "
           +name(WEM)+".\n");
      }
      return 1;
    }
    if(str==" " || (str=="schluck" || sscanf(str, "schluck%s", trash)) == 1) 
      if(PourLiquid(GULP, PL))
      {
        write("Du trinkst einen Schluck aus "+name(WEM, 1)+".\n");
        say(PL->name()+" trinkt einen Schluck aus "+name(WEM)+".\n");
        return 1;
      }
      else return write("Du schaffst es nicht zu trinken.\n") || 1;
    else
    {
      if(sscanf(str, "%s schluck%s", howmuch, trash)>=1) 
      switch(howmuch)
      {
        case "1":
        case "ein":
        case "einen": { cnt = 1; howmuch = "einen"; break; }
        case "2":
        case "zwei" : { cnt = 2; break; }
        case "3":
        case "drei" : { cnt = 3; break; }
        case "4":
        case "vier" : { cnt = 4; break; }
        case "5":
        case "fuenf": { cnt = 5; break; }
        case "6":
        case "sechs": { cnt = 6; break; }
        case "7":
        case "sieben":{ cnt = 7; break; }
        case "8":
        case "acht" : { cnt = 8; break; }
        case "9":
        case "neun" : { cnt = 9; break; }
        case "10":
        case "zehn" : { cnt = 10; break; }
        default: cnt = 0;
      }
      if(!cnt) { cnt = 1; howmuch = "einen"; }
      while(cnt-- && all=PourLiquid(GULP, PL));
      if(cnt>0 && all==_MaxCapacity) 
        write("Sooo viele Schluecke waren gar nicht in "+name(WEM, 1)+".\n");
      else write("Du trinkst ein paar Schluck aus "+name(WEM, 1)+".\n");
      say(PL->name()+" trinkt aus "+name(WEM)+".\n");
      return 1;
    }
  }
  return 0;
}

int DoPour(string arg)
{
  object dest;
  string where, str;

  if(arg && sscanf(arg, "%saus %s", str, where)==2 && id(where))
  {
    if(!_open) return write(capitalize(name(0,1))+" ist aber noch zu.\n") || 1;
    if(_Capacity==_MaxCapacity)
      return write("Es ist nichts in "+name(WEM, 1)+".\n") || 1;
    if(sscanf(str, "alles%s", where)==1) 
    {
      PourLiquid(_MaxCapacity, environment(this_player()));
      write("Du schuettest alles aus "+name(WEM, 1)+".\n");
      say(PL->name()+" schuettet alles aus "+name(WEM)+".\n");
      return _Capacity;
    }
    if(sscanf(str, "in %s", where)==1)
    {
      if(!dest = present(where)) dest = present(where, environment(PL));
      if(dest && !query_once_interactive(dest) &&
         PourLiquid(_MaxCapacity-_Capacity, dest))
      {
        write("Du fuellst "+dest->name(WEN, 1)+" mit "+_Fname+" ab.\n");
        say(PL->name()+" fuellt "+dest->name(WEN)+" mit "+_Fname+" ab.\n");
        return _Capacity;
      }
    }
  }
  return 0;
} 

int DoOpen(string str)
{
  if(id(str) && !_NoClose)
  {
    if(_open) return write(capitalize(name(0,1))+" ist schon offen.\n") || 1;
    write("Du oeffnest "+name(WEN, 1)+".\n");
    say(PL->name()+" oeffnet "+name(WEN)+".\n");
    _open = 1;
    return 1;
  }
  return 0;
}

int DoClose(string str)
{
  if(id(str) && !_NoClose)
  {
    if(!_open) return write(capitalize(name(0,1))+" ist schon zu.\n") || 1;
    write("Du machst "+name(WEN, 1)+" zu.\n");
    say(PL->name()+" schliesst "+name(WEN)+".\n");
    _open = 0;
    return 1;
  }
  return 0;
}

int DoLookInside(string str)
{
  string trash;

  if(str && sscanf(str, "in %s", trash)==1 && id(trash))
  {
    if(!_open) return write(capitalize(name(0,1))+" ist aber noch zu.\n") || 1;
    if(_Capacity==_MaxCapacity)
      return write("Es ist nichts in "+name(WEM, 1)+".\n") || 1;
    write(capitalize(name(0,1))
         +" ist zu "+NO[_Capacity*10/_MaxCapacity]+" Prozent voll mit "
         +_Fdescr+".\n");
    return 1;
  }
  return 0;
}

