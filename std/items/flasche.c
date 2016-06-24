/*OBJ*/
/* 2013-Mai-23, Arathorn
   Umfassend ueberarbeitet; setup(), init() und ReplaceWasser() entsorgt,
   P_WEIGHT und P_LONG dynamisch gebaut, so dass das staendige Umsetzen
   der Properties mittels setup() nach jedem Fuellen/Leeren entfaellt.
 */

/* Letzte Aenderung: 4.5.2004, Arathorn.
   Zeile 149: cont_obj->remove() eingefuegt, damit das geclonte Objekt
   nach dem Abfragen seines Namens wieder entfernt wird.
   Update 6.6.2004, Arathorn. Habe um ein destruct() ergaenzt, weil das
   trotz remove() hartnaeckig weitergebuggt hat. *fluch*
 */

/* Version 1.3 by Fraggle (17.1.1995) */

 // PreventInsert: verhindert Fuellen wie std/container
/* Version 1.2 by Fraggle (17.1.1995) */

 // Flasche kann nun mittels environment(TP)->GetLiquid()
 // etwas anderes als Wasser enthalten, sofern
 // environment(TP)->QueryProp(P_WATER)==W_OTHER
 // BUG: auch 1l Methan wiegt 1 Kg, aendere ich spaeter

/* Version 1.1 by Fraggle (17.1.1995) */
#pragma strong_types,rtt_checks
#pragma no_clone

inherit "/std/thing";
#include <language.h>
#include <properties.h>
#include <items/flasche.h>
#include <items/fishing/fishing.h>

#define DEFAULT_LIQ "Wasser"
#define TP this_player()

private string co_filename;
public string liquid_name=DEFAULT_LIQ;

string dynamic_long();

protected void create() {
  ::create();

  AddId(({"wasserbehaelter","behaelter","\nwater_source"}));
  SetProp(P_NAME, "Wasserbehaelter");
  SetProp(P_GENDER , MALE);
  SetProp(P_ARTICLE, 1);
  SetProp(P_SHORT, "Ein Standard-Wasserbehaelter");
  SetProp(P_LONG,"none");
  Set(P_LONG, #'dynamic_long, F_QUERY_METHOD);
  SetProp(P_LONG_EMPTY,""); // Beschreibung fuer leeren Zustand
  SetProp(P_LONG_FULL,"");  // Beschreibung fuer gefuellten Zusand
  SetProp(P_LIQUID,1000);   // Fuellmenge = 1 Liter
  SetProp(P_WATER,0);       // Flasche ist defaultmaessig leer!
  SetProp(P_VALUE,10);

  // P_WEIGHT auf Leergewicht der Flasche setzen, QueryMethode liefert
  // die Summe aus Leergewicht+Fuellung zurueck (Dichte = 1).
  SetProp(P_WEIGHT,20);
  Set(P_WEIGHT, function int () {
     return ( Query(P_WEIGHT,F_VALUE)+
              (QueryProp(P_WATER)?QueryProp(P_LIQUID):0) );
     }, F_QUERY_METHOD);

  AddCmd(({"fuell","fuelle"}),"cmd_fuelle");
  AddCmd(({"leere", "entleere"}),"cmd_leere");
}

protected void create_super() {
  set_next_reset(-1);
}

// Behaelter ist gefuellt, dann ist die Langbeschreibung entweder die 
// P_LONG_FULL, falls angegben, oder die P_LONG + Beschreibung der Fuellung.
// Genauso, wenn sie leer ist.
string dynamic_long() {
  string l=Query(P_LONG,F_VALUE);
  if ( QueryProp(P_WATER) ) {
    string lf = QueryProp(P_LONG_FULL);
    if ( stringp(lf) && sizeof(lf) )
      l=lf;
    else
      l+=capitalize(QueryPronoun(WER))+" enthaelt "+liquid_name+".\n";
    // Falls die Flasche mit etwas anderem als Wasser gefuellt wird, die
    // Langbeschreibung fuer "volle Flasche" (P_LONG_FULL) aber nur fuer
    // Wasser ausgelegt ist, wird "Wasser" durch den Inhalt von liquid_name
    // ersetzt.
    if ( liquid_name != DEFAULT_LIQ )
      l=regreplace(l, DEFAULT_LIQ, liquid_name, 1);
  } else {
    string le = QueryProp(P_LONG_EMPTY);
    if ( stringp(le) && sizeof(le) )
      l=le;
    else
      l+=capitalize(QueryPronoun(WER))+" ist leer.\n";
  }
  return l;
}

// Zum Ueberschreiben! PreventInsert(object ob){return 0;} z.B.
// macht die Flasche gasdicht.
// Oder man kann die Flasche verschliessbar machen.
int PreventInsert(object obj)
{
  if(obj->id("gas")) { //default: NICHT Gasdicht!
    write(obj->Name(WER,1)+" entweicht sofort wieder!\n");
    return 1;
  }
  return 0;
}

// Transferiert den Inhalt der Flasche an <dest>
protected int transfer_to(object dest)
{
  int water=QueryProp(P_WATER);
  if (!water)
  {
    write(Name(WER,1) + " ist schon leer!\n");
    return 0;   // War schon leer!
  }
  int contents=QueryProp(P_LIQUID);

  if ( water&W_OTHER )
  {
    dest->PutLiquid(co_filename);
  }
  else
  {
    dest->AddWater(contents);
  }
  SetProp(P_WATER,0);
  RemoveId(lower_case(liquid_name));
  liquid_name=DEFAULT_LIQ;
  return contents; //gib die ml an Umgebung ab :)
}

// Entleert die Flasche ins Environment der Flasche, allerdings nicht, wenn
// dies ein Lebewesen ist, dann wird das Environment von dem genommen.
// TODO: Eine Flasche in einem Paket leeren wurde dann in das paket entleeren.
// Das waere an sich sogar richtig... Nur: gewollt? Alternative koennen wir
// auch das aeusserste Environment nehmen, was nicht lebt.
public int empty()
{
  if (environment())
  {
    // Environment des Benutzers finden.
    object env = environment();
    while (living(env))
      env=environment(env);
    return transfer_to(env);
  }
  return 0;
}

// Fuellt die Flasche aus <src>
protected int fill_bottle(object src)
{
  int liquidtype = src->QueryProp(P_WATER);
  if(liquidtype)
  {
    if(QueryProp(P_WATER)) {
      write(Name(WER,1)+" ist bereits voll!\n");
      return 1;
    }
    // Wasser von Umgebung abziehen!
    // Man kann als Magier die Funktion AddWater(int n) dazu benuetzten,
    // beispielsweise eine Pfuetze zu leeren, ...
    src->AddWater(-QueryProp(P_LIQUID));
    object cont_obj;
    if(liquidtype&W_OTHER)
    {
      // Mittels GetLiquid() kann die Flasche mit was anderem als Wasser
      // gefuellt werden.
      co_filename=src->GetLiquid();
      if (co_filename)
      {
        cont_obj=clone_object(co_filename);
        if(PreventInsert(cont_obj))
        {
          // Hier passiert eigentlich das gleiche wie nach dem ifblock, aber
          // auch noch Funktion beenden.
          // TODO: Rueckgaenig machen von AddWater()?
          // TODO: Die Meldung aus dem PreventInsert() muesste eigentlich
          // _vorher_ noch mit einer Befuellmeldung begleitet werden.
          cont_obj->remove(1);
          if ( objectp(cont_obj) )
            cont_obj->move("/room/muellraum",M_PUT);
          cont_obj=0;
          return 0;
        }
        else
          liquid_name=cont_obj->name(WEN);
        // In jedem Fall wird das Objekt wieder zerstoert - es wurde nur fuer
        // das Ermitteln des liquid_name benutzt... Weia.
        if ( cont_obj ) cont_obj->remove();
        if ( cont_obj ) cont_obj->move("/room/muellraum",M_PUT);
      }
    }
    SetProp(P_WATER,liquidtype);
    AddId(lower_case(liquid_name));
    //wie praktisch, 1 ml == 1 g :) // Aber nur fuer Wasser, du VOGEL! :-|
    return 1;
  }
  else {
    write("Du findest hier nichts, was Du in "+name(WEN,1)+
      " fuellen koenntest!\n");
    return 0;
  }
  return 0;
}

static int cmd_leere(string str)
{
  object dest;
  notify_fail("Was willst Du denn (wo hinein) leeren?\n");
  if (!str)
    return 0;

  string strbottle,strobj;
  // leere flasche
  if (id(str))
  {
    //NOOP
  }
  // leere flasche in xxx
  else if (sscanf(str,"%s in %s",strbottle,strobj)==2)
  {
    if (!id(strbottle))
      return 0;
    dest = present(strobj, environment(this_player()))
           || present(strobj, this_player());
    if (!dest)
      return 0;
  }
  else
    return 0;
  // Syntaxpruefung fertig.

  if(!QueryProp(P_WATER))
  {
    write("Da ist kein "+liquid_name+" drin!\n");
    return 1;
  }

  if (dest)
  {
    write(break_string("Du leerst "+name(WEN,1)+ " in "
          + dest->name(WEN) + ".",78));
    say(break_string(TP->name()+" leert "+name(WEN,0)
          + " in " + dest->name(WEN) + ".",78),TP);
    transfer_to(dest);
    return 1;
  }
  write(break_string("Du leerst "+name(WEN,1)+".",78));
  say(break_string(TP->name()+" leert "+name(WEN,0)+".",78),TP);
  empty();
  return 1;
}

public int cmd_fuelle(string str)
{
  string strbottle,strobj;

  notify_fail("Was willst Du denn (womit) fuellen?\n");
  if(!str)
    return 0;

  // fuelle flasche
  if (id(str))
  {
    if (fill_bottle(environment(this_player())))
    {
      write(break_string("Du fuellst etwas "+liquid_name+" in "
            +name(WEN,1)+".",78));
      say(break_string(TP->Name(WER)+" fuellt etwas "
            +liquid_name+" in "+name(WEN)+".",78), TP);
    }
    return 1;
  }
  // fuelle flasche aus xxx
  // fuelle flasche mit xxx
  // fuelle xxx in flasche
  // fuelle flasche in xxx
  // fuelle xxx aus flasche
  // fuelle xxx mit flasche
  else if (sscanf(str,"%s in %s",strobj,strbottle)==2
           || sscanf(str,"%s mit %s",strbottle,strobj)==2
           || sscanf(str,"%s aus %s",strbottle,strobj)==2)
  {
    object obj;
    // Flasche befuellen?
    if (id(strbottle)
        && ( obj=present(strobj, environment(this_player())) 
                 || present(strobj, this_player()) )
       )
    {
      if (fill_bottle(obj))
      {
        write(break_string(
              "Du fuellst etwas "+liquid_name+" aus " + obj->name(WEM,1) 
              + " in "+name(WEN,1)+".",78));
        say(break_string(TP->Name(WER)+" fuellt etwas "+liquid_name+ " aus " 
              + obj->name(WEM,1) + " in "+name(WEN)+".",78), TP);
      }
      return 1;
    }
    // anderes Gefaess befuellen?
    else if (id(strobj)
        && ( obj=present(strbottle, environment(this_player())) 
                 || present(strbottle, this_player()) )
       )
    {
      if (transfer_to(obj))
      {
        write(break_string(
              "Du fuellst etwas "+liquid_name+" aus " + name(WEM,1)
              + " in "+obj->name(WEN,1)+".",78));
        say(break_string(TP->Name(WER)+" fuellt etwas "+liquid_name+ " aus " 
              + name(WEM,1) + " in "+obj->name(WEN)+".",78), TP);
      }
      return 1;
    }
  }
  // Syntax passt nicht.
  return 0;
}

int IsBottle() {
   return 1;
}

