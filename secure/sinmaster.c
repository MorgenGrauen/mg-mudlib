// MorgenGrauen MUDlib
//
// secure/sinmaster.c -- Das Strafregister
//
// $Id: sinmaster.c 9142 2015-02-04 22:17:29Z Zesstra $

#define SIN_SAVE "/secure/ARCH/sins"
#define SIN_LOG  "ARCH/sins"
#define SIN_DUMP "/secure/ARCH/sins.dump"

#include <defines.h>
#include <properties.h>
#include <wizlevels.h>

#pragma strict_types

mapping sins;

public void create()
{
    seteuid(getuid(ME));
    if ( !restore_object(SIN_SAVE) )
        sins = ([]);
}

static void save_me()
{
    save_object(SIN_SAVE);
}

private varargs int is_allowed(int archonly)
{
  if (previous_object() && geteuid(previous_object())==ROOTID)
    return 1;
  if (!process_call() && previous_object() && this_interactive() && (ARCH_SECURITY || (!archonly && IS_DEPUTY(secure_euid())) ) )
    return 1;
  return 0;
}

public nomask int query_prevent_shadow()
{
    return 1;
}

public string ListSinners()
{   string *names;

    if ( !is_allowed() )
      return "ACCESS DENIED\n";

    if ( sizeof(names=m_indices(sins))<1 )
      return "Es sind keine Eintraege vorhanden.\n";

    names = sort_array( map( names, #'capitalize ), #'> );

    return sprintf("Liste der eingetragenen Suender:\n%#78.6s\n",
                   implode(names,"\n"));
}

public string ListSins(string who)
{   string re;
    int    i,j;

    if ( !is_allowed() )
      return "ACCESS DENIED\n";

    if ( !stringp(who) || (sizeof(who)<1) )
      return "SYNTAX ERROR.\n";

    if ( !member(sins,who) || !pointerp(sins[who]) || ((j=sizeof(sins[who]))<1) )
      return sprintf("Es liegen keine Eintraege fuer '%s' vor.\n",CAP(who));

    for ( i=1, re = ((string)sins[who][0]+"\n") ; i<j ; i++ )
      re += sprintf("%3d: %s\n",i,(string)sins[who][i]);

    return re;
}

static void _add_entry(string who, string entry, object pl)
{   string *add;

    if ( member(sins,who) && pointerp(sins[who]) && (sizeof(sins[who])>0) )
        add = (string*)sins[who];
    else
        add = ({ sprintf("Eintraege fuer '%s':",CAP(who)) });

    add += ({ entry });

    sins[who] = add;

    save_me();

    log_file(SIN_LOG,
        sprintf("%s Eintrag fuer %s von %s\n",
            dtime(time()),CAP(who),CAP(getuid(pl)) ) );
}

public string AddSin(string who, string text)
{   object pl;
    string ersti;

    if ( !is_allowed() )
      return "ACCESS DENIED\n";

    if ( !stringp(who) || (sizeof(who)<1)
        || !stringp(text) || (sizeof(text)<1) )
      return "SYNTAX ERROR.\n";

    if ( text[0..2]=="-f " )
      text=text[3..];
    else if ( file_size(sprintf("/save/%s/%s.o",who[0..0],who))<1)
      return sprintf("Es gibt keinen Spieler namens '%s'\n",who);

    text = dtime(time()) + " ("+CAP(getuid(RPL))+")\n" 
         + break_string(text,78,"     " );

    _add_entry( who, text, RPL );

    if ( objectp(pl=(find_player(who)||find_netdead(who)))
        && !IS_WIZARD(pl) // Magier haben manchmal komische Ersties ...
        && stringp(ersti=(string)pl->QueryProp(P_SECOND)) )
    {
        return ( sprintf("Ok.\nFuege Eintrag bei Ersti '%s' hinzu.\n",ersti)
                + AddSin( lower_case(ersti), ("-f siehe "+who) ) );
    }

    return "OK.\n";
}

public string RemoveSin(string who, int nr)
{   string *rem;

    if ( !is_allowed(1) )
      return "ACCESS DENIED\n";

    if ( !intp(nr) || (nr<1) || !stringp(who) || (sizeof(who)<1) )
      return "SYNTAX ERROR.\n";

    if ( !member(sins,who) || !pointerp(sins[who]) || (sizeof(sins[who])<1) )
      return sprintf("FEHLER: Keine Eintraege fuer '%s' vorhanden.\n",
                     CAP(who));

    rem = (string*)sins[who];

    if ( sizeof(rem)<=nr )
      return "FEHLER: Diesen Eintrag gibt es nicht.\n";

    rem[nr] = 0;

    rem -= ({ 0 });

    log_file(SIN_LOG,
        sprintf("%s Loeschung bei %s von %s\n",
            dtime(time()),CAP(who),CAP(getuid(RPL)) ) );


    if ( sizeof(rem)<2 )
    {
        m_delete(sins,who);
        save_me();
        return sprintf("Letzten Eintrag von '%s' geloescht.\n",CAP(who));
    }

    sins[who] = rem;
    save_me();

    return sprintf("Eintrag %d von '%s' geloescht.\n",nr,CAP(who));
}

public varargs string Dump(int flag)
{   string *snr,*sns,dump;
    int    i,j,k,s,t;

    if ( !is_allowed(0) )
      return "ACCESS DENIED\n";

    if ( !flag && file_size(SIN_DUMP)>1 )
      rm( SIN_DUMP );

    s=i=sizeof(snr=sort_array(m_indices(sins),#'<));

    if ( i<1 )
      return "Keine Suender da.\n";

    dump = sprintf("\n%|78s\n%'='78.78s\n\n",
               sprintf("Dump der Suenden-Eintraege (%s):",dtime(time())),
               "");

    for ( --i,t=0 ; i>=0 ; i-- )
    {
        j = sizeof(sns=sins[snr[i]]);
        t += (j-1);
        dump += (sns[0]+"\n\n");

        for ( k=1 ; k<j ; k++ )
            dump += sprintf("%3d: %s\n",k,sns[k]);

        dump += sprintf("%'='78.78s\n\n","");
    }

    dump += sprintf("Statistik:\n\n"+
                    "  Es sind %d Suender mit insges. %d Eintraegen vorhanden.\n"+
                    "  Das macht einen Schnitt von %.6f Eintraegen.\n\n",
                    s,t,( to_float(t)/to_float(s) ));

    if ( !flag )
    {
        write_file(SIN_DUMP,dump);
        return sprintf("Es wurden %d Suender gedumped.\n",s);
        
    }
    return dump;
}
