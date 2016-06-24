/*
Letzte Aenderung: 29.01.01 von Tilly (Zusatz-Abfrage in heart_beat() )
                  25.08.01 von Tiamak (Tillys Abfragen debuggt ;^)
*/
#pragma strong_types,rtt_checks

inherit "/std/thing";
#include <properties.h>
#include <language.h>
#include <moving.h>

int    alter;
string furzerWER,furzerWESSEN;

void create()
{
  if(!clonep(this_object())) return;
  ::create();
  SetProp(P_SHORT, 0);
  SetProp(P_LONG, 0);
  SetProp(P_INVIS,1);
  SetProp(P_NAME, "Etwas");
  SetProp(P_GENDER, 1);
  SetProp(P_ARTICLE,0);
  SetProp(P_NOGET,"Haeh?\n");
  AddId("_furz_");
  if (this_player())
    AddId("Furz_"+getuid(this_player()));
  alter = 0;
  set_heart_beat(1);
}

void heart_beat()
{
  int i;
  if(!environment())
  {
    set_heart_beat(0);
    destruct(this_object());
    return;
  }
  if(living(environment()))
  {
    set_heart_beat(0);
    destruct(this_object());
    return;
  }
  if (!alter)
    tell_room(environment(), "Fuerchterlicher Gestank erfuellt den Raum.\n");
  alter++;
  if (!random(10))
  {
    i = random (5);
    if (i==0)
      tell_room(environment(), furzerWESSEN+" Furz stinkt wirklich grauenerregend.\n");
    if (i==1)
      tell_room(environment(), "Du haeltst Dir die Nase zu.\n");
    if (i==2)
      tell_room(environment(), "Widerliche Duenste erfuellen den Raum.\n");
    if (i==3)
      tell_room(environment(), "Der Furz riecht, als wuerde " + furzerWER +" innerlich verfaulen.\n");
    if (i==4)
      tell_room(environment(), "Du musst Dich beinah uebergeben, als " + furzerWESSEN + " Furz in Deine Nase kriecht.\n"); 
  }
  if (alter==45) {
      tell_room(environment(), furzerWESSEN + " widerlicher Furz hat sich endlich verzogen.\n");
      destruct(this_object());
    }
}


void set_alter(int a)
{ alter=a; }

void set_furzer (object wer)
{
  if (objectp(wer))
    {
      furzerWER=wer->name(WER);
      furzerWESSEN=wer->name(WESSEN);
      AddId("Furz_"+getuid(wer));
    }
}

varargs int move(mixed dest, int methods, string direction, string textout,string textin)
{
  int erg;
  object otherfurz;

  erg=::move(dest,methods,direction,textout,textin);
  
  // Vanion, 24.09.02: Kann sein, dass das move nich klappt,
  //                   dann sollte das Objekt weg.
  if (erg != MOVE_OK || !environment(this_object())) 
  {
    if (!remove()) destruct(this_object());
    return 0;
  }
  if (this_player())
    {
      if (otherfurz=present("Furz_"+getuid(this_player())+" 2",environment()))
	{
	  otherfurz->set_alter(0);
	  return remove();
	}
    }
  return erg;
}
