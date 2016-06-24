// MorgenGrauen MUDlib
//
// awmaster.c -- Armours- and Weapons-Master
//
#pragma strict_types
#pragma no_clone
#pragma no_shadow
#pragma no_inherit
#pragma verbose_errors
#pragma combine_strings
#pragma pedantic
#pragma range_check
#pragma warn_deprecated

#include <combat.h>
#include <language.h>
#include <properties.h>
#include <wizlevels.h>

#define SAVEFILE  "/secure/ARCH/awmaster"
#define ADUMPFILE "/log/ARCH/RUESTUNGEN"
#define DDUMPFILE "/log/ARCH/DAMAGERS"
#define WDUMPFILE "/log/ARCH/WAFFEN"

#define AWF_PUTON  1
#define AWF_PUTOFF 2
#define AWF_BOOST  4
#define AWF_ZAUBER 8
#define AWF_RESIST 16

#define AWM_TYPE      0
#define AWM_CLASS     1
#define AWM_EFF_CLASS 2
#define AWM_FLAGS     3
#define AWM_WEIGHT    4
#define AWM_VALUE     5
#define AWM_HANDS     6
#define AWM_D_TYPE    7
#define AWM_X_CLASS   8
#define AWM_TIME      9

mapping armours,weapons,damagers;
private int save_me_soon;

private int allowed()
{
  if (previous_object() && geteuid(previous_object())==ROOTID)
    return 1;
  if (!process_call() && previous_object() && this_interactive() && ARCH_SECURITY)
    return 1;
  return 0;
}

void create()
{
    seteuid(getuid(this_object()));

    if (!restore_object(SAVEFILE))
    {
        armours = ([]);
        weapons = ([]);
        damagers = ([]);
    }
    if (widthof(damagers) == 1) {
      mapping tmp = damagers;
      damagers = m_allocate(sizeof(tmp),2);
      foreach(string r, int flag: tmp) {
        damagers[r,0]=flag;
      }
    }
}

void save_me(int now)
{
  if (now)
    save_object(SAVEFILE);
  else
    save_me_soon=1;
}

void reset() {
  if (save_me_soon)
  {
    save_me_soon=0;
    save_me(1);
  }
}

public varargs int remove(int silent) {
  save_me(1);
  destruct(this_object());
  return 1;
}

int xflags(object ob){
    int re;
    mapping m;
    if (!ob || !objectp(ob))
        return 0;
    re=0;
    if (((object)ob->QueryProp(P_WEAR_FUNC))==ob  ||
        ((object)ob->QueryProp(P_WIELD_FUNC))==ob )
        re += AWF_PUTON;
    if (((object)ob->QueryProp(P_REMOVE_FUNC))==ob  ||
        ((object)ob->QueryProp(P_UNWIELD_FUNC))==ob )
        re += AWF_PUTOFF;
    if (((object)ob->QueryProp(P_DEFEND_FUNC))==ob ||
        ((object)ob->QueryProp(P_HIT_FUNC))==ob    )
        re += AWF_BOOST;
    // ists nen Mapping und nicht leer?
    if (mappingp(m=(mapping)ob->QueryProp(P_RESISTANCE_STRENGTHS))
        && sizeof(m))
        re += AWF_RESIST;
    return re;
}

void RegisterArmour()
{   object  ob;
    string  id;
    int     flag,h;

    if (!objectp(ob=previous_object()) || 
      member(inherit_list(ob),"/std/armour.c")==-1)
      return;
    id = explode(object_name(ob),"#")[0];
    if (member(armours,id))
    {
        armours[id][AWM_TIME]=time();
        flag=0;
        if ((h=(int)ob->QueryProp(P_AC)) > armours[id][AWM_CLASS])
        {
            armours[id][AWM_CLASS]=h;
            flag=1;
        }
        if ((h=(int)ob->QueryProp(P_EFFECTIVE_AC)) > armours[id][AWM_EFF_CLASS])
        {
            armours[id][AWM_EFF_CLASS]=h;
            flag=1;
        }
        if ((h=(int)ob->QueryProp(P_NR_HANDS)) < armours[id][AWM_HANDS])
        {
            armours[id][AWM_HANDS]=h;
            flag=1;
        }
        if ((h=xflags(ob)) != armours[id][AWM_FLAGS])
        {
            armours[id][AWM_FLAGS]=h;
            flag=1;
        }
    }
    else
    {
      armours += ([ id : 
        ([
            AWM_TYPE      : ob->QueryProp(P_ARMOUR_TYPE) ,
            AWM_CLASS     : ob->QueryProp(P_AC) ,
            AWM_EFF_CLASS : ob->QueryProp(P_EFFECTIVE_AC) ,
            AWM_FLAGS     : xflags(ob),
            AWM_WEIGHT    : ob->QueryProp(P_WEIGHT) ,
            AWM_VALUE     : ob->QueryProp(P_VALUE) ,
            AWM_HANDS     : ob->QueryProp(P_NR_HANDS) , // Fuer Schilde
            AWM_D_TYPE    : ob->QueryProp(P_DAM_TYPE) ,
            AWM_X_CLASS   : ob->QueryProp(P_EFFECTIVE_WC) ||
                            ob->QueryProp(P_WC),
            AWM_TIME      : time()
        ]) ]);
    }
    save_me(0);
}

void RegisterWeapon()
{   object  ob;
    string  id;
    int     flag,h;

    if (!objectp(ob=previous_object()) ||
      member(inherit_list(ob),"/std/weapon.c")==-1)
      return;
    id = explode(object_name(ob),"#")[0];
    if (member(weapons,id))
    {
        weapons[id][AWM_TIME] = time();
        flag=0;
        if ((h=(int)ob->QueryProp(P_WC)) > weapons[id][AWM_CLASS])
        {
            weapons[id][AWM_CLASS]=h;
            flag=1;
        }
        if ((h=(int)ob->QueryProp(P_EFFECTIVE_WC)) > weapons[id][AWM_EFF_CLASS])
        {
            weapons[id][AWM_EFF_CLASS]=h;
            flag=1;
        }
        if ((h=(int)ob->QueryProp(P_NR_HANDS)) < weapons[id][AWM_HANDS])
        {
            weapons[id][AWM_HANDS]=h;
            flag=1;
        }
        if ((h=xflags(ob)) != weapons[id][AWM_FLAGS])
        {
            weapons[id][AWM_FLAGS]=h;
            flag=1;
        }
    }
    else
    {
      weapons += ([ id :
        ([
            AWM_TYPE      : ob->QueryProp(P_WEAPON_TYPE) ,
            AWM_CLASS     : ob->QueryProp(P_WC) ,
            AWM_EFF_CLASS : ob->QueryProp(P_EFFECTIVE_WC) ,
            AWM_FLAGS     : xflags(ob),
            AWM_WEIGHT    : ob->QueryProp(P_WEIGHT) ,
            AWM_VALUE     : ob->QueryProp(P_VALUE) ,
            AWM_HANDS     : ob->QueryProp(P_NR_HANDS) ,
            AWM_D_TYPE    : ob->QueryProp(P_DAM_TYPE) ,
            AWM_X_CLASS   : ob->QueryProp(P_EFFECTIVE_AC) || 
                            ob->QueryProp(P_AC),
            AWM_TIME      : time()
        ]) ]);
    }
    save_me(0);
}

void RegisterDamager(object dam_ob,int old_dam, int new_dam)
{   object ob;
    int flag;
    string fn;

    if (!objectp(ob=previous_object()) ||
      (member(inherit_list(ob),"/std/weapon.c")==-1 &&
       member(inherit_list(ob),"/std/armour.c")==-1 ))
      return;
    if (old_dam>new_dam) // Repair
        flag=2;
    else if (new_dam>old_dam) // Damage
        flag=1;
    else
        return;
    if (!objectp(dam_ob))
        return;
    if (!(fn=old_explode(object_name(dam_ob),"#")[0]) || !stringp(fn))
        return;
    damagers[fn,0]=damagers[fn,0]|flag;
    damagers[fn,1]=time(); 
    save_me(0);
}

string dtdump(mixed arg)
{   string re;
    int i,w;

    if (stringp(arg))
        return capitalize(arg);
    if (!pointerp(arg) || !stringp(arg[0]))
        return "<NONE>";
    if ((i=sizeof(arg))==1)
        return capitalize(arg[0]);
    w = (31-i)/i;
    if (w--<1)
        return "<MANY>";
    for (re="",--i;i>=0;i--)
    {
        if (!stringp(arg[i]))
            re += "-";
        else
            re += capitalize(arg[i][0..w]);
        if (i)
            re += "|";
    }
    return re;
}

int Dump(mixed what, int sortidx)
{   string  file,*ind;
    mapping dump;

    if (!allowed())
        return -1;

    if (!what)
    {
        write("Nimm doch mal einen richtigen Parameter!\n");
        return 0;
    }
    if (stringp(what) && sizeof(what)>0)
    {
        what==what[0..0];
        if (what=="a")
            what=1;
        else if (what=="w")
            what=2;
        else if (what=="d")
            what=3;
        else
        {
            write("Nimm doch mal einen richtigen Parameter!\n");
            return 0;
        }
    }
    if (!intp(what) || what<1 || what>3)
    {
        write("Nimm doch mal einen richtigen Parameter!\n");
        return 0;
    }
    if (what==3) // Die 'damagers' haben ein anderes Ausgabeformat
    {
        printf("AWM: Dumping 'damagers' to '%s'\n",DDUMPFILE);
        if (sizeof(damagers) < 1) {
            write("AWM: Dump aborted, mapping empty.\n");
            return 1;
        }
        if (file_size(DDUMPFILE)>1)
            rm(DDUMPFILE);
        
        // nach letzter Aktualisierung sortieren.
        mixed sorted = sort_array(m_indices(damagers),
            function int (string a, string b) {
                return damagers[a,1] < damagers[b,1];
            } );

        string ausgabe = sprintf(
            "--- Damagers-Dump --- %s --- %s ---\n\n"+
            "%:15s D R [Filename]\n",
            dtime(time()),capitalize(getuid(this_interactive())),
            "Datum/Zeit");
        foreach(string rue : sorted) {
            ausgabe += sprintf("%:15s %1s %1s %s\n",
                strftime("%y%m%d-%H:%M:%S",damagers[rue,1]),
                (damagers[rue,0]&1?"+":"-"),
                (damagers[rue,0]&2?"+":"-"),
                rue);
        }
        
        write_file(DDUMPFILE, ausgabe);
        return 1;
    }
    if (what==2)
        what=0;
    file=(what?ADUMPFILE:WDUMPFILE);

    printf("AWM: Dumping '%s' to '%s'\n",
        (what?"armours":"weapons"),file);
    
    dump=(what?armours:weapons);
    
    if (sortidx) {
      ind = sort_array(m_indices(dump),
          function int (string a, string b)
          {return dump[a][sortidx] < dump[b][sortidx];} );
    }
    else
      ind = sort_array(m_indices(dump),#'>);
    
    if (sizeof(ind) < 1)
    {
        write("AWM: Dump aborted, mapping empty.\n");
        return 1;
    }

    if (file_size(file)>1)
        rm(file);
    
    string ausgabe = sprintf(
        "--- %s-Dump --- %s --- %s ---\n\n"+
        "[Filename], Datum/Zeit\n"+
        "    ____Typ___ CLS ECL XCL NFBR WGHT. VALUE H %30.30'_'|s\n",
        (what?"Ruestungs":"Waffen"),dtime(time()),
        capitalize(getuid(this_interactive())),"DamType(s)");

    foreach(string index : ind)
    {
        ausgabe += sprintf(
            "[%s] %s\n    %10s %3d %3d %3d %1s%1s%1s%1s %5d %5d %1d %-30.30s\n",
            index, strftime("%y%m%d-%H:%M:%S",dump[index][AWM_TIME]),
            dump[index][AWM_TYPE],
            dump[index][AWM_CLASS],
            dump[index][AWM_EFF_CLASS],
            dump[index][AWM_X_CLASS],
           (dump[index][AWM_FLAGS]&AWF_PUTON?"+":"-"),
           (dump[index][AWM_FLAGS]&AWF_PUTOFF?"+":"-"),
           (dump[index][AWM_FLAGS]&AWF_BOOST?"+":"-"),
           (dump[index][AWM_FLAGS]&AWF_RESIST?"+":"-"),
            dump[index][AWM_WEIGHT],
            dump[index][AWM_VALUE],
            dump[index][AWM_HANDS],
            dtdump(dump[index][AWM_D_TYPE]) );
    }
    write_file(file,ausgabe);
    write("AWM: Done.\n");
    return 1;
}

int Unregister(string what)
{
    if (!allowed())
        return -1;
    if (!what)
    {
        write("Du solltest schon einen Filenamen angeben!\n");
        return 0;
    } 
    if (member(armours,what))
    {
        m_delete(armours,what);
        write("Unregistered "+what+" from 'armours'.\n");
        return 1;
    }
    if (member(weapons,what))
    {
        m_delete(weapons,what);
        write("Unregistered "+what+" from 'weapons'.\n");
        return 1;
    }
    if (member(damagers,what))
    {
        m_delete(damagers,what);
        write("Unregistered "+what+" from 'damagers'.\n");
        return 1;
    }
    save_me(0);
    return 0;
}

int ResetDamagers()
{
    if (!allowed())
        return -1;
    damagers = m_allocate(0,2);
    save_me(1);
    return 1;
}

