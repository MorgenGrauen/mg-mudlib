// (c) by Padreic (Padreic@mg.mud.de)
/* Changelog:
   * 09.11.06 Zesstra
     object_info()[11] durch query_next_reset() ersetzt.
     */
#include "../files.h"

inherit "/std/thing";

static int leichen;

void create()
{
    leichen=0;
    if (!clonep(ME))
       set_next_reset(-1);
    else {
       ::create();
       SetProp(P_INVIS, 1);
       SetProp(P_NODROP, 1);
       SetProp(P_NEVERDROP, 1);
       AddId(WALDID("leichenmp"));
       set_next_reset(3600*3); // erster reset nach 3h
    }
}

int remove()
{
    closure clo;
    if (environment()) {
      clo=environment()->Query(P_MAX_SP, F_QUERY_METHOD);
      if (clo && to_object(clo)==ME)
          environment()->Set(P_MAX_SP, 0, F_QUERY_METHOD);
    }
    destruct(ME);
    return 1;
}

int calculate_mp()
{
    int mp;
    switch(leichen) {
       case 1:  mp = 5;  break;
       case 2:  mp = 9;  break;
       case 3:  mp = 12; break;
       case 4:  mp = 14; break;
       case 5:  mp = 15; break;
       default: mp = 0;
    }
    SetProp(P_X_HEALTH_MOD, ([P_SP:mp]) );
}

void new_corpse()
{
    closure clo;
    clo=PL->Query(P_MAX_SP, F_QUERY_METHOD);
    if (clo && to_object(clo)!=ME) {
       call_out("remove", 0);
       return;
    }
    if (leichen<5) {
       leichen++;
       set_next_reset(3600+3);
    }
    else set_next_reset( (query_next_reset(ME)-time())+3600 );
    calculate_mp();
}

void reset()
{
   leichen--;
   if (environment()) {
      tell_object(environment(), "Du spuerst wie Deine mentalen Kraefte schwaecher werden...\n");
      if (environment()->QueryProp(P_SP) > environment()->QueryProp(P_MAX_SP))
          environment()->SetProp(P_SP, environment()->QueryProp(P_MAX_SP));
   }
   calculate_mp();
   if (leichen<=0)
      remove();
   else set_next_reset(3600);
}
