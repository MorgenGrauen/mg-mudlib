
#pragma strict_types,save_types
#pragma no_clone,no_shadow

#include <daemon.h>
#include <logging.h>

static int d_start, d_end, e_start, e_end;

protected void create()
{    
    d_end = d_start = e_end = e_start = file_size(__DEBUG_LOG__);
}

int check( string ch, object pl, string cmd, string txt )
{
    mixed tmp;
    
    if ( ch != "Debug" && ch != "Entwicklung" && ch != "Warnungen" )
        return 0;
    
    if( objectp(pl) && query_once_interactive(pl) && query_wiz_level(pl) > 15 )
        switch(cmd){
        case C_FIND:
        case C_LIST:
        case C_JOIN:
        case C_LEAVE:
            return 1;
        case C_SEND:
            // Wer (noch) nicht auf dem Kanal ist, bekommt auch keine
            // Ausgabe - sonst gibt es selbige doppelt, da der CHMASTER
            // einen automatisch den Kanal betreten laesst ...
            if ( !objectp(pl) || !pointerp(tmp=(mixed)pl->QueryProp(P_CHANNELS)) ||
                 member( tmp, lower_case(ch) ) == -1 )
                return 1;

            switch( lower_case(txt) ){
            case "backtrace":
                {
                    string bt, log;
                    int start, end;
                    
                    if ( ch == "Debug" ) {
                        start = d_start;
                        end = d_end;
                    }
                    else if (ch == "Entwicklung") {
                        start = e_start;
                        end = e_end;
                    }
		    else if (ch == "Warnungen") {
			pl->Message( "[" + ch + ":] Backtrace fuer Warnungen "
			    "nicht verfuegbar!\n");
			return 0;
		    }
		    else return(1);
                    
                    if( start >= end ) 
                        bt = "[" + ch + ":] Kein Backtrace verfuegbar!\n";
                    else {
                        log = regreplace( read_bytes( __DEBUG_LOG__, start,
                                                      (end-start>10000)?10000:
						      end - start ) || "\nFehler beim Einlesen des Debuglogs: Kein Backtrace verfuegbar!\n",
                                          "(Misplaced|current_object|"
                                          "Connection|Host)[^\n]*\n",
                                          "", 1 );
                        bt = "[" + ch + ":] -- BACKTRACE BEGIN --\n"
                            + log
                            + "[" + ch + ":] -- BACKTRACE END --\n";
                    }
                    
                    call_out( symbol_function( "Message", pl ), 0, bt );
                }
                break;

            default:
		if ( ch != "Warnungen" ) {
		  pl->Message( "[" + ch + ":] Hilfe...\n"
                             "[" + ch + ":] Folgende Kommandos stehen zur "
                             "Verfuegung:\n"
                             "[" + ch + ":] 'backtrace' -- sendet den "
                             "Backtrace zum Fehler\n"
                             "[" + ch + ":] 'hilfe'     -- sendet diese "
                             "Hilfeseite\n");
		}
		else {
		  pl->Message( "[" + ch + ":] Keine Kommandos verfuegbar!\n");
		}
            }
	}
    
    return 0;
}

string name() { return "<Debugger>"; }
string Name() { return "<Debugger>"; }

void ChannelMessage( mixed a )
{
    int fs;

    fs = file_size(__DEBUG_LOG__);
    
    switch( a[0] ){
    case "Debug":
        if (  fs > d_end && fs > e_end ){
	    d_start = max((d_end > e_end ? d_end : e_end),0);
            d_end = fs;
        }
        else if (fs < d_start || fs <= d_end) {
	    // Debuglog wurde wohl neu geoeffnet.
	    d_start = d_end = e_start = e_end = fs;
	}
        break;
        
    case "Entwicklung":
        if (  fs > d_end && fs > e_end ){
	    e_start = max((e_end > d_end ? e_end : d_end),0);
            e_end = fs;
        }
        else if (fs < e_start || fs <= e_end) {
	    // Debuglog wurde wohl neu geoeffnet.
	    d_start = d_end = e_start = e_end = fs;
	}
        break;
    }
}
 
