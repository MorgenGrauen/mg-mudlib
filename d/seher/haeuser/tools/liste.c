#include "../haus.h"
#include <wizlevels.h>

mapping megamap;

create() {
  seteuid(getuid(this_object()));
  restore_object(SAVEFILE);
  do_dump();
}

do_dump()
{
  string *ow, typ;
  int i, r, r2;
  ow = m_indices(megamap);
  ow = sort_array(ow,#'<);
  i = sizeof(ow)-1;
  for(r=0, r2=0; i>=0; i--) {
    r = megamap[ow[i],HP_ROOMS];
    if (IS_LEARNER(ow[i])) printf("%13s %d MAGIER(%d)\n", ow[i],r,query_wiz_level(ow[i]));
    else if (!IS_SEER(ow[i])) printf("%13s %d KEIN SEHER MEHR\n", ow[i], r);
  }
  destruct(this_object());
}
