// MorgenGrauen MUDlib
//
// Pubmaster.c -- Registrating the heal-values of pubs
//
// $Id: pubmaster.c 9373 2015-10-22 19:02:42Z Zesstra $

#include <pub.h>

#define SAVEFILE "/secure/ARCH/pub"
#define CLOGFILE "/log/ARCH/pubchange"
#define DUMPFILE "/log/ARCH/PUBS"
#define DUMPLIST "/log/ARCH/PUBLIST"

#define DUMPF(str) write_file(DUMPFILE,str)
#define DUMPL(str) write_file(DUMPLIST,str)

#pragma strong_types

#include <properties.h>
#include <wizlevels.h>
#include <health.h>

mapping pubs;
nosave string  *watchprops;
nosave int     watchsize;

void create()
{
    seteuid(getuid(this_object()));
    if (clonep(this_object()))
    {
        destruct(this_object());
        return;
    }
    if (!restore_object(SAVEFILE))
        pubs = ([]);

    watchprops = ({ P_HP, P_SP, P_ALCOHOL, P_DRINK, P_FOOD, P_VALUE,
                    "rate", "delay" });
    watchsize  = sizeof(watchprops);
}

void save_data()
{
    save_object(SAVEFILE);
}

int CalcMax(mapping info, string fn)
{   float heal,delay,factor;

    if (!info || !mappingp(info))
        return 0;
    if ( (info[P_ALCOHOL]+info[P_DRINK]+info[P_FOOD])<=0     ||
         (info[P_ALCOHOL] && !(info[P_DRINK]||info[P_FOOD])) )
        return 0;

    delay = to_float(info["delay"]);
    if (delay>PUB_MAXDELAY)
        delay=PUB_MAXDELAY;

    if (!stringp(fn) || !member(pubs,fn)) // External query?
        factor=1.0;
    else
        factor=pubs[fn,1];

    heal = to_float( info[P_ALCOHOL]*ALCOHOL_DELAY +
                     info[P_DRINK]  *DRINK_DELAY   +
                     info[P_FOOD]   *FOOD_DELAY    ) 
           * PUB_SOAKMULT
           / to_float( ALCOHOL_DELAY + DRINK_DELAY + FOOD_DELAY );
    heal = heal + 
           to_float(info[P_VALUE])/(PUB_VALUEDIV+to_float(info["rate"])) -
           exp(to_float(info["rate"])/PUB_RATEDIV1)/PUB_RATEDIV2;
    heal = heal * factor *
           (PUB_WAITOFFS + (exp(delay/PUB_WAITDIV1)/PUB_WAITDIV2));

    return to_int(heal);
}

int RegisterItem(string item, mapping info)
{   object pub;
    string fn;
    int    i,c,cmax,max,heal;

    if (!objectp(pub=previous_object()))
        return -1;
    if (member(inherit_list(pub), "/std/room/pub.c") == -1)
        return -1;
    if (!item || !stringp(item) || !info || !mappingp(info))
        return -2;

    if (info["rate"]<1)
        info["rate"]=1;
    if (info["delay"]<0)
        info["delay"]=0;

    // Loadname, weil VCs als ein Pub zaehlen sollen (das Tutorial hat ein
    // VC-Pub fuer jeden Spieler. Die sollen nicht alle einzeln hier erfasst
    // werden.
    fn = load_name(pub);
    c = 0;

    heal=info[P_HP]+info[P_SP];

    if (!member(pubs,fn))
        pubs += ([ fn : ([]); 1.0 ]);

    if (!member(pubs[fn],item))
    {
        max=CalcMax(info,fn);
        pubs[fn] += ([ 
            item : ([
                P_HP            : info[P_HP],
                P_SP            : info[P_SP],
                P_VALUE         : info[P_VALUE],
                P_DRINK         : info[P_DRINK],
                P_ALCOHOL       : info[P_ALCOHOL],
                P_FOOD          : info[P_FOOD],
                "rate"          : info["rate"],
                "delay"         : info["delay"],
                "maxheal"       : heal,
                "maxrate"       : 10
                ])
            ]);
        c=1;
    }
    else
    {
        for (i=watchsize-1;i>=0;i--)
            if (info[watchprops[i]]!=pubs[fn][item][watchprops[i]])
            {
                pubs[fn][item][watchprops[i]]=info[watchprops[i]];
                c=1;
            }
        if (heal>pubs[fn][item]["maxheal"])
        {
            pubs[fn][item]["maxheal"]=heal;
            c=1;
        }
        max=pubs[fn][item]["maxset"];
        cmax=CalcMax(info,fn);
        if (cmax>max)
            max=cmax;
    }

    if (c)
        save_data();

    if ( heal>max || info["rate"]>pubs[fn][item]["maxrate"] )
        return 0;

    return 1;
}

private int allowed()
{
  if (previous_object() && geteuid(previous_object())==ROOTID)
    return 1;
  if (!process_call() && previous_object() && ARCH_SECURITY)
    return 1;
  return 0;
}



int SetMaxHeal(string pub, string item, int new)
{   int old;

    if (!allowed())
        return -2;
    if (!pub || !stringp(pub) || !item || !stringp(item) || !new)
        return -1;
    if (!member(pubs,pub) || !member(pubs[pub],item))
        return 0;
    old=pubs[pub][item]["maxset"];
    pubs[pub][item]["maxset"]=new;
    write_file(CLOGFILE,
        sprintf("%s - %s\n%s:%s HEAL %d > %d\n",
            dtime(time()),
            (this_interactive() ? getuid(this_interactive()) : "???"),
            pub,item,old,new));
    save_data();
    return 1;
}

int SetMaxRate(string pub, string item, int new)
{   int old;

    if (!allowed())
        return -2;
    if (!pub || !stringp(pub) || !item || !stringp(item) || !new)
        return -1;
    if (!member(pubs,pub) || !member(pubs[pub],item))
        return 0;
    old=pubs[pub][item]["maxrate"];
    pubs[pub][item]["maxrate"]=new;
    write_file(CLOGFILE,
        sprintf("%s - %s\n%s:%s RATE %d > %d\n",
            dtime(time()),
            (this_interactive() ? getuid(this_interactive()) : "???"),
            pub,item,old,new));
    save_data();
    return 1;
}

int SetPubFactor(string pub, float new)
{   float old;

    if (!allowed())
        return -2;
    if (!pub || !stringp(pub) || !new || !floatp(new) || new<1.0 || new>1.5)
        return -1;
    if (!member(pubs,pub))
        return 0;
    old=pubs[pub,1];
    pubs[pub,1]=new;
    write_file(CLOGFILE,
        sprintf("%s - %s\n%s FACT %6.4f > %6.4f\n",
            dtime(time()),
            (this_interactive() ? getuid(this_interactive()) : "???"),
            pub,old,new));
    save_data();
    return 1;
}

varargs int ClearPub(string pub, string item)
{
    if (!allowed())
        return -2;
    if (!pub || !stringp(pub) || (item && !stringp(item)))
        return -1;
    if (!member(pubs,pub))
        return 0;
    if (!item)
        m_delete(pubs,pub);
    else if (!member(pubs[pub],item))
        return 0;
    else
      m_delete(pubs[pub],item);
    save_data();
    return 1;
}

int DumpPubs()
{   string *publist,*itemlist;
    int    pubi,itemi;

    if (!allowed())
        return -2;
    if (file_size(DUMPFILE)>1)
        rm(DUMPFILE);
    if (file_size(DUMPLIST)>1)
        rm(DUMPLIST);
    DUMPF(sprintf("Kneipen-Menu-Liste vom %s:\n\n",dtime(time())));
    DUMPL(sprintf("Kneipenliste vom %s:\n\n",dtime(time())));
    publist=sort_array(m_indices(pubs),#'</*'*/);
    if ((pubi=sizeof(publist))<1)
    {
        DUMPF("No pubs in List\n");
        DUMPL("No pubs in List\n");
        return -1;
    }
    for (--pubi;pubi>=0;pubi--)
    {
        DUMPL(sprintf("%-'.'70s %6.4f\n",
            publist[pubi],pubs[publist[pubi],1]));
        DUMPF(sprintf("PUB: %s\nFAC: %6.4f\n\n",
            publist[pubi],pubs[publist[pubi],1]));
        itemlist=sort_array(m_indices(pubs[publist[pubi]]),#'</*'*/);
        if ((itemi=sizeof(itemlist))<1)
        {
            DUMPF("  No items in list\n\n");
            continue;
        }
        DUMPF(sprintf(
            "  %|'_'30.30s %5s %3s %3s %3s %3s %3s %2s %2s %3s %3s\n",
            "ITEM","VALUE","HPH","SPH","DRI","ALC","FOO","RA","MR","MAX",
            "FND"));
        for (--itemi;itemi>=0;itemi--)
            DUMPF(sprintf(
                "  %-'.'30.30s %5d %3d %3d %3d %3d %3d %2d %2d %3d %3d\n",
                itemlist[itemi],
                (int)pubs[publist[pubi]][itemlist[itemi]][P_VALUE],
                (int)pubs[publist[pubi]][itemlist[itemi]][P_HP],
                (int)pubs[publist[pubi]][itemlist[itemi]][P_SP],
                (int)pubs[publist[pubi]][itemlist[itemi]][P_DRINK],
                (int)pubs[publist[pubi]][itemlist[itemi]][P_ALCOHOL],
                (int)pubs[publist[pubi]][itemlist[itemi]][P_FOOD],
                (int)pubs[publist[pubi]][itemlist[itemi]]["rate"],
                (int)pubs[publist[pubi]][itemlist[itemi]]["maxrate"],
                (int)pubs[publist[pubi]][itemlist[itemi]]["maxset"],
                (int)pubs[publist[pubi]][itemlist[itemi]]["maxheal"] ));
        DUMPF("\n");
    }
    return sizeof(publist);
}

// bereinigt die Kneipenliste von allen Kneipen, die auf der Platte nicht mehr
// existieren.
public void CleanPublist() {
  foreach(string pub: pubs) {
    if (file_size(pub+".c") <= 0)
      m_delete(pubs,pub);
  }
  save_data();
}

