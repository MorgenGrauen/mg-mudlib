#pragma strict_types
#pragma no_clone
#pragma no_shadow
#pragma no_inherit
//#pragma pedantic
//#pragma range_check
#pragma warn_deprecated

#include <wizlevels.h>
#include <daemon.h>
#include <events.h>
#include <strings.h>
#include <files.h>

#define FTPSAVE "/secure/ARCH/ftpd"

#define MAXLOGSIZE 2000000
#define SMALLLOGSIZE 200000
#define LOGTIME 120
#define MAXBUFFSIZE 2000

#define DEBUG(x)  if (find_player("zesstra"))\
            tell_object(find_player("zesstra"),\
                                      "EDBG: "+x+"\n")

// fuer FTP.
private mapping monitored;


#define D_LOGTIME    0  // Logendezeit, <int>
#define D_FLAGS      1  // Flags
#define D_ERSTIE     2  // Name des Ersties.
#define D_IPSTRING   3
#define D_INDEX      4  // Index fuer D_LOG, naechster freier index.
#define D_LOG        5  // Logdaten, <mixed> (Array)
/* Datenstruktur von D_LOG:
   ({ ({<zeit>, <kommando>, <environment>}), .... })
   */
#define DL_TIME  0  // int
#define DL_CMD   1  // string
#define DL_ENV   2  // string
private nosave mapping ldata = m_allocate(0,D_LOG);

// Flags:
#define FL_PERMANENT    1 // 'permanent', d.h. nicht nur kurz nach Einloggen
#define FL_SYNC         2 // nicht puffern, synchron auf Platte schreiben.

// Ja. Macht das bloss nicht nach.
#define P_SECOND "second"

public int query_bb();
public void writebb( string msg );
public varargs void BBWrite(string msg, int catmode);

public int add( string user, int timeout );
public int sub( string user );
public void ftpbb( string user, string msg );

private void scan_bb_opfer();
private void DumpData(string uid, string erstie, string ip, int flags, mixed logdata);
private void ProcessBuffer(string uid);
private void RemoveTemporaryPlayer(string uid);

public void create()
{
    seteuid( getuid(this_object()) );
    restore_object( FTPSAVE );
    scan_bb_opfer();
    EVENTD->RegisterEvent(EVT_LIB_LOGIN, "Eventhandler", this_object());
    EVENTD->RegisterEvent(EVT_LIB_LOGOUT, "Eventhandler", this_object());
    log_file("ARCH/bbmaster.log", strftime("%c: bbmaster wurde geladen.\n"));
}

// Auf den asynchronen Logout-event zu warten ermoeglicht es theoretisch, 1-2s
// das Log zu umgehen. Andererseits geht das ohnehin nur dann, wenn die
// Logzeit eh abgelaufen ist.
public void Eventhandler(string eid, object trigob, mixed data) {
  if (previous_object() == find_object(EVENTD)) {
    string uid;
    if (objectp(trigob)
        && strstr(load_name(trigob),"/std/shells/") == 0
        && !trigob->QueryGuest()) {
      // Bei Login und Logout den BBMode einschalten (weil der Loginevent ja
      // erst 1-2s nach Einloggen abgearbeitet wird.
      trigob->__set_bb(1);
      uid=getuid(trigob);
    }
    else {
      // kein Objekt mehr da. Vermutlich hat ein Spieler 'ende' gemacht, aber
      // es koennte auch sein, dass jemand mit nem Selbstzerstoerer den Event
      // gefakt hat. Aber selbst wenn, viel kann man damit nicht erreichen.
      uid = data[E_PLNAME];
      if (!stringp(uid)) return;
      // Pruefung auf nicht-Anwesenheit von uid waere noch moeglich, hat aber
      // Probleme, wenn ein Spieler sehr schnell wieder einloggt.
    }

    if (eid == EVT_LIB_LOGOUT && member(ldata,uid)) {
      // Wenn Logout und es gibt Daten im Puffer, koennte man die evtl.
      // wegschreiben oder loeschen.
      ProcessBuffer(uid);
      // auf jeden Fall temporaere Spieler entfernen. (Wichtig!)
      RemoveTemporaryPlayer(uid);
    }
  }
}

// schreibt alle Puffer synchron, ohne Callout... Kann laggen.
public int ProcessAllBuffers() {
  
    if (extern_call() && !ARCH_SECURITY)
    return -1;

  foreach(string uid, int logtime, int flags, string erstie, string ip,
          int index, mixed data: ldata) {
    if (index) {
      DumpData(uid, erstie, ip, flags, data);
      ldata[uid,D_LOG]=({});
      ldata[uid,D_INDEX]=0;
    }
  }
  return 1;
}

private void ProcessBuffer(string uid) {
    
  if (time() <= ldata[uid,D_LOGTIME]
      && ldata[uid,D_INDEX]) {
    // Daten wegschreiben, wenn Logzeit nicht abgelaufen. Sonst nicht.
    call_out(#'DumpData, 2, uid, ldata[uid,D_ERSTIE], ldata[uid,D_IPSTRING],
	                    ldata[uid,D_FLAGS], ldata[uid,D_LOG]);
  }
  ldata[uid,D_LOG] = ({});
  ldata[uid,D_INDEX] = 0;
}

private void DumpData(string uid, string erstie, string ip, int flags, mixed logdata) {
  string res = sprintf("\n%s%s, IP: %s\n", capitalize(uid),
                      (stringp(erstie) ? " ("+capitalize(erstie)+")" : ""),
		      (stringp(ip) ? ip : "Unbekannt"));
  logdata-=({0});
  foreach(mixed arr : logdata) {
    res+=sprintf("%O: %O [%s]\n", 
        strftime("%y%m%d-%H%M%S",arr[DL_TIME]),
        arr[DL_CMD], arr[DL_ENV] || "<unbekannt>");
  }

  //DEBUG("DumpData: "+res);
  if (flags & FL_PERMANENT)
    catch(log_file("ARCH/bb."+uid, res, MAXLOGSIZE));
  else if (file_size(LIBLOGDIR"/ARCH/bbmaster") == FSIZE_DIR)
    catch(log_file("ARCH/bbmaster/"+uid, res, SMALLLOGSIZE));
  // kein else, in anderen Faellen werden die Daten verworfen.
}

private void AddTemporaryPlayer(string uid) {
    // natuerlich nur, wenn noch nix eingetragen.
    if (!member(ldata, uid)) {
      object ob = find_player(uid) || find_netdead(uid);
      
      mixed erstie;
      if (ob)
        erstie = (string)ob->QueryProp(P_SECOND);

      ldata += ([uid: time() + LOGTIME + random(LOGTIME/2); 
	              0;
		      (stringp(erstie) ? erstie : 0);
		      query_ip_number(ob);
		      0; ({})
	        ]);
    }
}

private void RemoveTemporaryPlayer(string uid) { 
  if (!(ldata[uid,D_FLAGS] & FL_PERMANENT)) {
    m_delete(ldata, uid);
  }
}


// Vom Spielererobjekt bei Erschaffung in InitPlayer() gerufen.
public int query_bb()
{
    
    if (load_name(previous_object())[0..11] != "/std/shells/")
        return 0;

    // in jedem Fall wird nun (temporaer) der BB-Modus aktiviert.
    if (!previous_object()->QueryGuest())
      previous_object()->__set_bb(1);

    // nur fuer 'permanente' auch 1 zurueckgeben.
    return ldata[getuid(previous_object()),D_FLAGS] & FL_PERMANENT;
}



// neue Funktion. Kriegt nur Kommandosstring uebergegen, werden ggf. gepuffert
// und dann weggeschrieben.
public varargs void BBWrite(string msg, int catmode) {
  
  if ( !this_interactive() ||
      (extern_call() && 
       strstr(load_name(previous_object()), "/std/shells/") != 0 ) )
    return;

  string uid = getuid(this_interactive());

  if (!member(ldata, uid))
    AddTemporaryPlayer(uid);  
  else if (ldata[uid,D_LOGTIME] < time()) {
    // Logzeit abgelaufen. -> ENDE.
    if (ldata[uid,D_INDEX]) {
      this_interactive()->__set_bb(0);
      // es kann vorkommen, dass hier nen ProcessBuffer mit anderer uid
      // drinhaengt. Ist aber egal, dann wird der Puffer halt naechstesmal
      // geschrieben.
      if (find_call_out(#'ProcessBuffer) == -1)
          call_out(#'ProcessBuffer, 2, uid);
    }
    return;
  }

  // im synchronen Modus direkt auf Platte schreiben.
  //DEBUG("BBWrite: Flags von +"+uid+": "+to_string(ldata[uid,D_FLAGS])
  //    +"\n");
  if (ldata[uid,D_FLAGS] & FL_SYNC) {
    //DEBUG("BBWrite: Syncmodus\n");
    if (!catmode) {
      msg = sprintf("%s: %s [%O]\n", strftime("%y%m%d-%H%M%S"),
	  msg, environment(this_interactive()));
    }
    else
      msg = msg + "\n";
    log_file( "ARCH/bb."+uid, msg, MAXLOGSIZE );
    return;
  }

  // alle anderen werden erstmal gepuffert. 

  // wenn catmode und nen Index > 0 wird der Kram an den vorherigen Eintragen
  // angehaengt.
  int index = ldata[uid,D_INDEX];
  if (catmode && index > 0) {
    --index;
    ldata[uid,D_LOG][index][DL_CMD] += msg;
  }
  else {
    // Puffer vergroessern?
    if (index >= sizeof(ldata[uid,D_LOG]))
      ldata[uid,D_LOG]+=allocate(100);
    ldata[uid,D_LOG][index] = ({ time(), msg, 
	                         object_name(environment(this_interactive())) 
                               });
    ldata[uid,D_INDEX]++;
    // es kann vorkommen, dass hier nen ProcessBuffer mit anderer uid
    // drinhaengt. Ist aber egal, dann wird der Puffer halt naechstesmal
    // geschrieben.
    if (index > MAXBUFFSIZE 
        && find_call_out(#'ProcessBuffer) == -1)
      call_out(#'ProcessBuffer, 2, uid);
  }
}

// Alte Funktion, kriegt Strings, teilweise mit Datum, teilweise ohne,
// schrieb frueher nur weg. msg faengt entweder mit einem Datum/Zeit-String
// oder mit einem "->" an.
public void writebb( string msg )
{
  int catmode; 
 
  if ( !this_interactive() ||
      (extern_call() && 
       strstr(load_name(previous_object()), "/std/shells/") != 0 ) )
    return;
 
  // erstmal String bereinigen.
  msg = trim(msg,TRIM_RIGHT,"\n");
  if (strstr(msg,"->") == 0) {
    catmode=1;
    msg= " -> " + msg[2..];
  }
  else {
    // faengt mit Datumsstring an, erstes Leerzeichen ab dem zehnten Zeichen
    // suchen und von dort abschneiden.
    msg = msg[strstr(msg," ",10)+1 ..];
  }

  // Dann weitergeben
  BBWrite(msg, catmode);
}

private void scan_bb_opfer()
{
    string* lines;
    object pl;
    string uid;

    // diese user werden 'permanent' ueberwacht, nicht nur direkt nach dem
    // Einloggen.
    lines = explode( lower_case( read_file("/secure/ARCH/BB_OPFER.dump")
                                       || "" ), "\n" )[2..];
    
    foreach(string line : lines) {
        if( sizeof(line) && line[0] != '#' ) {
	    uid=line[0 .. member(line,' ')-1];
	    AddTemporaryPlayer(uid);
	    ldata[uid,D_LOGTIME] = __INT_MAX__;
	    ldata[uid,D_FLAGS] = FL_PERMANENT;
	    pl = find_player(uid) || find_netdead(uid);
	    if (pl)
	      pl->__set_bb(1);
	}
    }
}

// Neuladen ist kein grosses Problem, weil der bbmaster ja automatisch
// neugeladen wird. Nebeneffekt ist lediglich, dass fuer alle laufenden Logs
// die Logzeit wieder von vorne anfaengt. Da das Schreiben der Puffer aber Lag
// verursachen kann, duerfen es nur EM+.
public varargs int remove(int silent) {
 
  if (!ARCH_SECURITY)
    return 0;

  log_file("ARCH/bbmaster.log", strftime("%c: remove() called.\n"));

  // alle Puffer leeren...
  // ProcessAllBuffers() wird hierbei _ohne_ Limits aufgerufen! Kann fieses
  // Lag erzeugen, aber sonst wurde der Kram evtl. nicht ordentlich
  // geschrieben.
  limited(#'ProcessAllBuffers);
  destruct(this_object());
  return 1;
}


// Alles ab hier nur zum Ueberwachen von FTP-Aktivitaeten.
private int player_exists( string user )
{
    if ( !stringp( user ) || sizeof( user ) < 2 )
        return 0;
    
  return file_size( "/save/" + user[0..0] + "/" + user + ".o" ) > 0;
}

public int add( string user, int timeout )
{
    if ( !ARCH_SECURITY || process_call() )
        return -1;
    
    if( !stringp(user) || !player_exists(lower_case(user)) || !intp(timeout) ||
        !timeout )
        return -2;
    
    monitored[lower_case(user)] = timeout;
    save_object( FTPSAVE );
    
    return 1;
}


public int sub( string user )
{
    if ( !ARCH_SECURITY || process_call() )
        return -1;
    
    if( !stringp(user) || !member( monitored, lower_case(user) ) )
        return -2;
    
    m_delete( monitored, lower_case(user) );
    save_object( FTPSAVE );
    
    return 1;
}


public void ftpbb( string user, string msg )
{
    if( getuid(previous_object()) != ROOTID )
        return;

    if ( ldata[user,D_FLAGS] & FL_PERMANENT )
        log_file( "ARCH/bb."+user, msg, 2000000 );

    if ( monitored[user] ){
        if ( monitored[user] > 0 && monitored[user] < time() )
            sub( user );
        else
            CHMASTER->send( "FTP", capitalize(user), msg );
    }
}

