// MorgenGrauen MUDlib
//
// guestmaster.c -- Verwalter von Gast-Logins
//
// $Id: guestmaster.c 8909 2014-08-20 21:33:55Z Zesstra $
#pragma no_clone,no_inherit,no_shadow
#pragma strict_types,save_types,rtt_checks

#include <wizlevels.h>

nosave int max_guests = 10; /* Max no. of guests. -1 is 'unlimited' */
nosave object *guests = ({});
nosave int    *ltime = ({});

protected void create()
{
  seteuid(getuid());
}

nomask int new_guest () {
  int ix;

  if (!max_guests) return 0;
  if (load_name(previous_object()) != "/secure/login")
    return 0;
 
  if ((ix = member(guests,0)) == -1) {
    ix = sizeof(guests);
    if (max_guests < 0 || ix < max_guests) 
    {
      guests += ({ 0 }), ltime += ({ 0 });
    }
    else {
      int mintime, minix;
      mintime = time();
      for (ix = 0; ix < sizeof(guests); ix++) {
        if (guests[ix] && ltime[ix] < mintime) {
          mintime=ltime[ix];
          minix=ix;
        }
      }
      ix = minix;
    }
  }
  return ix+1;
}

nomask void set_guest (int ix, object pl) {
  if (load_name(previous_object()) != "/secure/login") return;
  guests[ix-1] = pl;
  ltime[ix-1] = time();
}

nomask int query_max_guests() { return max_guests; }
nomask int set_max_guests(int v) {
  if (!process_call() && LORD_SECURITY)
    max_guests = v;
  return max_guests;
}

