#pragma strong_types,save_types
#pragma no_clone,no_shadow

#include <wizlevels.h>

#define SAVEFILE  "/p/daemon/save/ruestungen"
#define DUMPFILE "/log/RUESTUNGEN"

mapping armours;

void create()
{
    seteuid(getuid(this_object()));

    if (!restore_object(SAVEFILE))
    {
        armours = ([]);
    }
}

void save_me()
{
    save_object(SAVEFILE);
}

void RegisterArmour()
{   object  ob;
    string  id;

    if (!objectp(ob=previous_object()) || 
      member(inherit_list(ob),"/std/armour.c")==-1)
        return;
    id = old_explode(object_name(ob),"#")[0];
    if (member(armours,id))
        return;
    armours += ([ id : 1]);
    save_me();
}

int Dump()
{   string  *ind;
    int     i;

    if (!this_interactive() || !IS_LORD(this_interactive()))
        return -1;
    if (file_size(DUMPFILE)>=0)
        rm(DUMPFILE);

    ind = sort_array(m_indices(armours),#'>);
    if ((i=sizeof(ind))<1)
        return 0;
    write_file(DUMPFILE,sprintf("Dumped: %s\n",dtime(time())));
    for (--i;i>=0;i--)
      write_file(DUMPFILE,ind[i]+"\n");
    return 1;
}
