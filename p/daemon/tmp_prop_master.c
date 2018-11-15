#pragma strong_types,save_types
#pragma no_clone,no_shadow

#include <thing/properties.h>

#define SAVEFILE "/p/daemon/save/tmp_prop_master"
#define PROPMASTER "/p/daemon/tmp_prop_master"
#define INDEX(a,b) (object_name(a)+"##"+b)


#ifdef DEBUG
#   undef DEBUG
#endif

#ifdef DEBUG
#   define DBG(x) if ( find_player("rikus") ) \
                     tell_object( find_player("rikus"), x );
#else
#   define DBG(x)
#endif


mixed *reset_times;
mapping in_progress;


varargs public int remove( int silent );
public void reset();
public void print_debug_info();
static void call_next_update();
static void do_update();
public int IsTmpProp( object ob, string prop );
public void RemoveTmpProp( string prop );
varargs public void SetTmpProp( object ob, string prop, mixed newval,
                                int tm, int sp );


public void create()
{

    seteuid(getuid(this_object()));
    
    if ( !restore_object(SAVEFILE) ){
        in_progress = ([]);
        reset_times = ({});
    }

    call_next_update();
}


varargs public int remove( int silent )
{
    save_object(SAVEFILE);
    destruct(this_object());
    
    return 1;
}


public void reset()
{
    save_object(SAVEFILE);
}


public void print_debug_info()
{
    printf( "in_progress: %O\nreset_times: %O\n", in_progress, reset_times );
}


static void call_next_update()
{
    int dt;
    
    while ( remove_call_out("do_update") != -1 )
        ;
    
    if ( sizeof(reset_times) ){
        dt = reset_times[0][0] - time();

        if ( dt <= 0 )
            dt = 5;
        
        call_out( "do_update", dt );
    }
}


static void do_update()
{
    int i, max;
    string index;
    mixed *val;

    DBG( "do_update() aufgerufen!\n" );

    while ( remove_call_out("do_update") != -1 )
        ;
    
    call_out( "do_update", 30 ); // Zur Sicherheit...

    if ( !(max = sizeof(reset_times)) )
        return;
    
    for ( i = 0; i < max && reset_times[i][0] <= time(); i++ ){
        index = reset_times[i][1];

        if ( !pointerp(val = in_progress[index]) || !objectp(val[0]) )
	{
            m_delete( in_progress ,index );
            continue;
        }

        DBG( sprintf("Entferne Eintrag %O!\n", index) );

        if ( val[4] ){
            if ( val[0]->Query(val[1], F_VALUE) == val[3] ){
                DBG( "Alte Property per SetProp() restauriert!\n" );
                val[0]->SetProp( val[1], val[2] );
            }
        }
        else {
            if ( val[0]->Query( val[1], F_QUERY_METHOD ) == val[2] ){
                val[0]->Set( val[1], 0, F_QUERY_METHOD );
                DBG( "F_QUERY_METHOD entfernt!\n" );
            }
            
            if ( val[0]->Query( val[1], F_SET_METHOD ) == val[3] ){
                val[0]->Set( val[1], 0, F_SET_METHOD );
                DBG( "F_SET_METHOD entfernt!\n" );
            }
        }

        m_delete( in_progress ,index );
    }
    
    reset_times = reset_times[i..];
    call_next_update();
}


// Zur Abfrage, ob eine temporaere Property schon gesetzt ist
public int IsTmpProp( object ob, string prop )
{
    mixed oldval;

    if ( !objectp(ob) || !prop )
        return 0;
    
    if ( (oldval = in_progress[INDEX(ob, prop)])
         && ob == oldval[0] && prop == oldval[1] )
        return 1;
    
    return 0;
}


// Entfernen einer temporaeren Property.
// Ist nicht zum Entfernen per Hand gedacht, sondern kann nur vom Objekt selber
// aufgerufen werden. Geschieht, wenn bei gesetzter temporaerer F_SET_METHOD
// ein fremdes SetProp() auf die Property kommt.
public void RemoveTmpProp( string prop )
{
    string index;
    mixed *val;
    int i;
    
    DBG( "RemoveTmpProp!\n");
    if ( !previous_object() || !stringp(prop) || !sizeof(prop) )
        return;

    index = INDEX(previous_object(), prop);
    DBG( sprintf("RemoveTmpProp(%O)!\n", index) );

    if ( !(val = in_progress[index]) ){
        DBG( "Kein Eintrag vorhanden!\n" );
        return;
    }

    if ( objectp(val[0]) ){
        DBG( "Restauriere alten Zustand ...\n" );

        if ( val[4] ){
            if ( val[0]->Query(val[1], F_VALUE) == val[3] ){
                DBG( "Alte Property per SetProp() restauriert!\n" );
                val[0]->SetProp( val[1], val[2] );
            }
        }
        else {
            if ( val[0]->Query( val[1], F_QUERY_METHOD ) == val[2] ){
                val[0]->Set( val[1], 0, F_QUERY_METHOD );
                DBG( "F_QUERY_METHOD entfernt!\n" );
            }
            
            if ( val[0]->Query( val[1], F_SET_METHOD ) == val[3] ){
                val[0]->Set( val[1], 0, F_SET_METHOD );
                DBG( "F_SET_METHOD entfernt!\n" );
            }
        }
    }

    for ( i = sizeof(reset_times); i--; )
        if ( reset_times[i][1] == index ){
            reset_times[i..i] = ({});
            break;
        }
    
    DBG( "Eintrag entfernt!\n" );
    m_delete( in_progress, index );
}


// Property 'prop' im Objekt 'ob' fuer 'tm' Sekunden auf Wert 'newval' setzen.
// Wenn der Schalter 'sp' gesetzt ist, wird dafuer SetProp() verwendet,
// anstatt eine F_QUERY_METHOD zu benutzen.
//
// ACHTUNG:
//
// Das Setzen von 'sp' ist hoechst fehleranfaellig und sollte nur in wenigen
// Spezialfaellen geschehen, in denen eine SET-Methode aufgerufen werden muss
// (wie bei P_LIGHT z.B.). Ansonsten bitte den Default-Aufruf ohne 'sp'
// verwenden!
// 
// Es besteht sonst die Gefahr, dass die Property nicht korrekt zurueckgesetzt
// wird (wenn dieser Master oder 'ob' zerstoert wird - oder beides wie bei einem
// Crash) oder dass durch Wechselwirkungen mit anderen Query- oder Set-Methoden
// ein falscher Wert "restauriert" wird.

varargs public void SetTmpProp( object ob, string prop, mixed newval,
                                int tm, int sp )
{
    string index;
    mixed oldval, tmp;
    int max, pos, l, r;

    if ( !objectp(ob) || !stringp(prop) || !sizeof(prop) || tm <= 0 )
        return;

    index = INDEX(ob, prop);
    DBG( sprintf("SetTmpProp( %O, %O, %O, %O, %O )!\n", ob, prop, newval,
                 tm, sp) );

    // wenn bereits ein temporaerer Prozess laeuft, diesen nun beenden!
    if ( (oldval = in_progress[index])
         && ob == oldval[0] && prop == oldval[1] ){
        DBG( "Es laeuft bereits ein Vorgang fuer diese Property!\n" );
        
        // Wurde die alte temporaere Property per SetProp() gesetzt, muss der
        // urspruengliche Wert erst restauriert werden. 
        if ( oldval[4] ){
            if ( oldval[0]->Query(oldval[1], F_VALUE) == oldval[3] ){
            DBG( "Alte Property per SetProp() restauriert!\n" );
            oldval[0]->SetProp( oldval[1], oldval[2] );
            }
        }
        // Ansonsten einfach die F_QUERY- und F_SET_METHOD loeschen.
        else {
            if ( oldval[0]->Query( oldval[1], F_QUERY_METHOD ) == oldval[2] ){
                oldval[0]->Set( oldval[1], 0, F_QUERY_METHOD );
                DBG( "F_QUERY_METHOD entfernt!\n" );
            }

            if ( oldval[0]->Query( oldval[1], F_SET_METHOD ) == oldval[3] ){
                oldval[0]->Set( oldval[1], 0, F_SET_METHOD );
                DBG( "F_SET_METHOD entfernt!\n" );
            }
        }
        
        for ( l = sizeof(reset_times); l--; )
            if ( reset_times[l][1] == index ){
                reset_times[l..l] = ({});
                DBG( "Timer-Eintrag entfernt!\n" );
                break;
            }
    }

    if ( tm < 1000000 )
        tm += time();

    if ( tm < time() )
        return;

    if ( !sp ){
        // Default: F_QUERY_METHOD setzen
        oldval = ob->Set( prop, newval,  F_QUERY_METHOD );

        // F_SET_METHOD, damit die temporaere Property keinen zwischenzeitlich
        // gesetzten anderen Wert wieder "restauriert".

        tmp = ob->Set(prop, function mixed (mixed value, string propname) {
                RemoveTmpProp(propname);
                return ob->SetProp(propname, value);
              }, F_SET_METHOD);
    }
    else{
        // Fuer Spezialfaelle (P_LIGHT z.B.) wird die Property direkt mit
        // SetProp() gesetzt und spaeter mit dem alten Wert restauriert.
        // Achtung - extrem fehleranfaellig und wirklich nur fuer Spezialfaelle!

        // Alten Wert mit Query() sichern anstatt mit QueryProp(), um nicht
        // irgendwelchen F_QUERY_METHODs auf den Leim zu gehen.
        oldval = ob->Query( prop, F_VALUE );

        // Rueckgabewert des SetProp() speichern, um spaeter erkennen zu
        // koennen, ob die Property zwischenzeitlich veraendert wurde.
        tmp = ob->SetProp( prop, newval );
    }

    in_progress[index] = ({ ob, prop, oldval, tmp, sp });
    
    if ( !(max = sizeof(reset_times)) )
        pos = 0;
    else{
        l = 0;
        r = max - 1;
        
        while ( l < r ){
            pos = (l + r) / 2;
            
            if ( tm < reset_times[pos][0] )
                r = --pos;
            else
                l = ++pos;
        }
        
        while ( pos >= 0 && pos < max && tm < reset_times[pos][0] )
            pos--;
    }
    
    reset_times = reset_times[0..pos] + ({ ({tm, index}) })
        + reset_times[pos+1..];

    DBG( sprintf("Property an Stelle %O eingefuegt!\n", pos+1) );

    call_next_update();
}
