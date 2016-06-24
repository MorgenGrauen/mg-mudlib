#pragma strong_types,save_types

#include <defines.h>
#include <moving.h>
#include <properties.h>
#include <language.h>

private nosave object pl;

void create()
{
  if( IS_BLUE(ME) ) return;
  shadow( PL, 1);
  pl = PL;
}

int
AddExp(int ep)
{
  object block;
  int diff, lim;

  block = present("\n block", pl);
  lim = 30 + random(10);

  if ( ep > lim &&				      // Mindestwert
       previous_object() &&
       ( previous_object() == pl ||		      // zB. GiveQuest()
	 ( living(previous_object()) && 	      // Oder NPCs
	  !query_once_interactive(previous_object())
	 )
       )
     )
  {
    diff = block->Gutschreiben(ep-lim);
    return pl->AddExp(lim+diff);
  }
  return pl->AddExp(ep);
}

void
SeherHatGenug()
{
  unshadow();
  destruct(this_object());
}

