// Log-Daemon, um Geldbugs zu finden.
//
// Noch experimentell, ich feile noch fleissig an den Werten&Co.
// Wer mir ohne triftigen Grund daran herumpfuscht, der wird standesrechtlich
// mit faulen Eiern beworfen. ]:->
//
// Tiamak

#pragma strong_types,save_types
#pragma no_clone,no_shadow

#include <wizlevels.h>
#include <money.h>

#define LOGNAME "ADDMONEY"
#define SAVEFILE "/p/daemon/save/moneylog"

#define THRESHOLD_HOURLY 10000 // wieviel Geld darf man an einer Stelle pro
#define THRESHOLD_DAILY  50000 // Stunde/Tag bekommen, ohne dass geloggt wird?


private mapping mon, grenzen;
private int check, next_reset;


public void create()
{    
    // wir muessen Savefile schreiben duerfen
    seteuid(getuid());
    
    if ( !restore_object(SAVEFILE) ){
        mon = m_allocate( 0, 2 );
        grenzen = m_allocate( 0, 2 );
        check = 0;
        next_reset = 3600;
    }

    if ( next_reset > 3600 )
        next_reset = 3600;

    if ( next_reset < 60 )
        next_reset = 60;

    // Auswertung jede Stunde
    set_next_reset( next_reset );
    next_reset += time();
}


// Sortierfunktionen

static int _sort_hourly( string fn1, string fn2 )
{
    return mon[fn1, 0] > mon[fn2, 0];
}


static int _sort_daily( string fn1, string fn2 )
{
    return mon[fn1, 1] > mon[fn2, 1];
}


// im reset() wird die Auswertung vorgenommen
public void reset()
{
    int i;
    string *files, txt;

    // keinen Aufruf per Hand bitte
    if ( time() < next_reset )
        return;

    // Auswertung jede Stunde
    set_next_reset(3600);
    next_reset = time() + 3600;
    check++;
    txt = "";
    
    files = sort_array( m_indices(mon), #'_sort_hourly/*'*/ );

    for ( i = sizeof(files); i--; ){
        if ( !(grenzen[files[i], 0] < 0) &&
             (grenzen[files[i], 0] && mon[files[i], 0] > grenzen[files[i], 0])
             || (!grenzen[files[i], 0] && mon[files[i], 0] > THRESHOLD_HOURLY) )
            txt += sprintf( "%12d --- %s\n", mon[files[i], 0], files[i] );
    
        mon[files[i], 0] = 0;
    }

    // nur loggen, wenn es auch etwas zu loggen gibt
    if ( txt != "" )
        log_file( LOGNAME, sprintf( "%s: =================================="
                                    "==============(stuendlich)===\n",
                                    ctime(time())[4..15] ) + txt );

    // der "grosse" Check kommt nur einmal pro Tag
    if ( check < 24 ){
        save_object(SAVEFILE);
        return;
    }

    check = 0;
    txt = "";
    files = sort_array( m_indices(mon), #'_sort_daily/*'*/ );
    
    for ( i = sizeof(files); i--; )
        if ( !(grenzen[files[i], 1] < 0) &&
             (grenzen[files[i], 1] && mon[files[i], 1] > grenzen[files[i], 1])
             || (!grenzen[files[i], 1] && mon[files[i], 1] > THRESHOLD_DAILY) )
            txt += sprintf( "%12d --- %s\n", mon[files[i], 1], files[i] );

    if ( txt != "" )
        log_file( LOGNAME, sprintf( "%s: =================================="
                                    "==============(taeglich)=====\n",
                                    ctime(time())[4..15] ) + txt );

    mon = m_allocate( 1, 2 );
    save_object(SAVEFILE);
}


// die eigentliche Logg-Funktion
public void AddMoney( object ob, int amount )
{
    string fn;

    // keine Manipulationen per Hand bitte
    if ( !objectp(ob) || amount < 0 || !previous_object() ||
         (!query_once_interactive(previous_object()) &&
          load_name(previous_object()) != GELD)
         || IS_LEARNER(previous_object()) )
        return;

    fn = explode( object_name(ob), "#" )[0];

    mon[fn, 0] += amount;
    mon[fn, 1] += amount;
}


// Savefile noch schnell abspeichern - wir wollen ja keine Daten verlieren ;-)
public int remove()
{
    next_reset = query_next_reset(this_object()) - time();
    save_object(SAVEFILE);

    destruct(this_object());
    return 1;
}


// fuer bestimmte Files kann man die Grenzwerte einzeln setzen
public varargs int modify_threshold( string fn, int amount1, int amount2 )
{
    if ( !this_interactive() || this_interactive() != this_player() ||
         !IS_ARCH(this_interactive()) || process_call() )
        return -1;

    if ( !stringp(fn) || !intp(amount1) || !intp(amount2) )
        return -2;

    if ( !amount1 && !amount2 ){
        m_delete( grenzen, fn );
        write( "Eintrag " + fn + " geloescht.\n" );
        return 1;
    }

    grenzen[fn, 0] = amount1;
    grenzen[fn, 1] = amount2;
    
    write( break_string( "Die Grenzen fuer " + fn + " betragen jetzt " +
                         ((amount1 < 0) ? "unendlich viele" :
                          to_string(amount1)) + " Muenzen fuer den "
                         "stuendlichen und " +
                         ((amount2 < 0) ? "unendlich viele" :
                          to_string(amount2)) + " Muenzen fuer den taeglichen "
                         "Check.", 78 ) );

    save_object(SAVEFILE);
    return 1;
}


// einzeln gesetzte Grenzwerte abfragen
public varargs mixed query_thresholds( string str )
{
    int i;
    string *files, txt;
    
    if ( !this_player() )
        return deep_copy(grenzen);

    if ( stringp(str) && member( grenzen, str ) ){
        write( break_string( "Die Grenzen fuer " + str + " betragen " +
                             ((grenzen[str, 0] < 0) ? "unendlich viele" :
                              to_string(grenzen[str, 0])) + " Muenzen fuer den "
                             "stuendlichen und " +
                             ((grenzen[str, 1] < 0) ? "unendlich viele" :
                              to_string(grenzen[str, 1])) +
                             " Muenzen fuer den taeglichen Check.", 78 ) );
        return 0;
    }

    if ( stringp(str) && str != "" ){
        write( "Es sind keine Grenzen fuer " + str + " eingetragen!\n" );
        return 0;
    }

    files = sort_array( m_indices(grenzen), #'</*'*/ );
    txt = "Eingetragene Grenzwerte:\n================================="
        "=============================================\n";

    for ( i = sizeof(files); i--; )
        txt += sprintf( "%'.'-48s : %8d (h) %10d (d)\n",
                        files[i], grenzen[files[i], 0], grenzen[files[i], 1] );

    this_player()->More( txt );
    return 1;
}


// bisher geloggte Daten abfragen
public mixed query_status()
{
    int i;
    string *files, txt;
    
    if ( !this_player() )
        return deep_copy(mon);

    files = sort_array( m_indices(mon), #'</*'*/ );
    txt = "Geldquellen:\n================================="
        "=============================================\n";

    for ( i = sizeof(files); i--; )
        txt += sprintf( "%'.'-48s : %8d (h) %10d (d)\n",
                        files[i], mon[files[i], 0], mon[files[i], 1] );

    this_player()->More( txt );
    return 1;
}


// keine Shadows bitte
public varargs int query_prevent_shadow( object ob ) { return 1; }
