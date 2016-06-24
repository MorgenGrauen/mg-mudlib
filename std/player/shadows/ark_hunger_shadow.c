/* Der Shadow fuer den Hungerfluch */
/* /d/ebene/ark/wolf/obj/hunger.c  */
/* Shadow: /std/player/shadows/ark_hunger_shadow.c */
/* Original: /d/ebene/ark/wolf/obj/hunger_obj.c */

#pragma strong_types,save_types

#include <defines.h>
#include <properties.h>

#define BS break_string

object spieler;

void Setzen(object sp)
{
 if (!objectp(sp) || !interactive(sp)) return destruct(this_object());
 spieler=sp;
 if (!shadow(sp,1)) destruct(this_object());
}

int _query_food() { return 0; }

int _query_max_food() { return 0; }

int eat_food(int strength, int testonly)
{
 if (strength==0) return 1;
 if (strength>0)
 {
  strength=0;
  if (spieler)
  {
    tell_object(spieler, BS("Du hast zwar einen tierischen Hunger, doch irgendwas sagt Dir, dass Du das, was "+
                            "Du gerade essen willst, nicht mehr in Deinen Magen bekommst. Das ist ziemlich "+
                            "gefaehrlich, hoffentlich verhungerst Du nicht !", 78));
    return 0;
  }
 }
 else
 {
  if (spieler)
  {
   tell_object(spieler, BS("Dein Hunger vertieft sich so nur, Du leidest Hoellenqualen.", 78));
   spieler->reduce_hit_points(strength);
  }
  return 0;
 }
}

void Loeschen() {
    unshadow();
    destruct(this_object());
}
