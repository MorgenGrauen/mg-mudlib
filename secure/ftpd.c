/**************************************************************************
** ftpd.c
** Rumata@mg
** 13.6.1999
**
** Ermittlung der Zugriffsrechte aus den Userdaten.
**
** Dieses Objekt wird vom ftpimp aus aufgerufen, der wiederum vom master
** aufgerufen wird, um die Zugriffsrechte zu pruefen.
**
** Fuktionsweise:
** + Im Master wird vom UNIX-ftpd aus FtpAccess (/secure/master/network)
**   aufgerufen.
** + Dieses ruft dann, sobald ein Zugriff erkannt wird
**   QueryRead, QueryWrite oder QueryDir auf.
** + Diese fragen den Master, ob der Zugriff legal ist.
** + QueryDir liefert auch gleich das Ergebnis mit.
**
** Um ftp-zugriffe überwachen zu können, kann der ftpd eine liste der
** ueberwachten spieler liefern und deren Zugriffe auf dem FTP Kanal ausgeben.
**
** add( name, zeit ) - FTP Zugriffe des Spielers 'name' werden protokolliert.
**                     Nach 'zeit' wird dieser Zugriff automatisch beendet.
**                     Ist 'zeit' <= 0, wir auf ewig ueberwacht. 
** sub( name )       - FTP Snoop fuer Spieler 'name' direkt aufheben.
** list()            - Mapping der ueberwachten Spieler zurueckgeben.
**
** FILES:
**   /secure/master/network.c
**   /secure/ftpd.c
**   /std/shells/filesys/ftpimp.c
**   /secure/ARCH/ftpd.o
**   /p/daemon/channeld.init
**
***************************************************************************/

#include "/secure/wizlevels.h"
#include "/sys/daemon.h"

#define FTPDSAVE "/secure/ARCH/ftpd"
#define FTPD_CH "FTP"
#define BBMASTER "/secure/bbmaster"

mapping monitored;
// Zu jedem Spielernamen wird vermerkt, wie lange die Ueberwachung dauern soll.
// Zahlen <= 0 bedeuten fuer immer.

nomask void create() {
  if( clonep(this_object()) ) {
    destruct( this_object() );
    return;
  }
  seteuid( getuid() );
  if( !restore_object(FTPDSAVE) )
    monitored = ([]);
}

nomask int player_exists( string user ) {
  user = lower_case(user);
  if( !stringp( user ) || sizeof( user ) < 1 ) return 0;
  return file_size( "/save/"+user[0..0]+"/"+user+".o" ) > 0;
}

nomask varargs int add( string user, int timeout ) {
  if( !ARCH_SECURITY ) return -1;
  if( !player_exists(user) ) return -2;
  monitored[user] = timeout;
  save_object( FTPDSAVE );
  return 0;
}

nomask int sub( string user ) {
  if( !ARCH_SECURITY ) return -1;
  if( !player_exists(user) ) return -2;
  monitored -= ([ user ]);
  save_object( FTPDSAVE );
  return 0;
}

nomask mixed list() {
  if( !ARCH_SECURITY ) return -1;
  return deep_copy(monitored);
}

#define NEWIMP "yes"

private object findFtpImpFor( string user ) {
#ifdef NEWIMP
  return "/secure/impfetch"->impFor( lower_case(user) );
#else
  object imp;
  string fname;

  user = lower_case( user );
  fname = "/ftpimp:" + user;
  imp = find_object( fname );
  if( !objectp(imp) ) {
    imp = clone_object( "secure/ftpimp" );
    imp->SetUser(user);
    rename_object( imp, fname );
  }
  return imp;
#endif
}

private nomask void msg( string user, string m ) {
  int timeout;
  object r;

  if( !stringp(user) ) return;
  m += "\n";
  if( (r=find_player("rumata")) && user=="atamur" ) {
     timeout = CHMASTER->send(FTPD_CH,findFtpImpFor(user), m );
     tell_object( r, sprintf("%O\n", findFtpImpFor(user) ) );
  }
  BBMASTER->ftpbb(user,m);
  if( !member(monitored,user) ) return;
  timeout = monitored[user];
  if( timeout > 0 && timeout < time() ) {
    sub( user );
    return;
  }
  CHMASTER->send(FTPD_CH,findFtpImpFor(user), m );
}

nomask int secure() {
  return previous_object()==find_object(MASTER);
}

nomask mixed QueryRead( string user, string file ) {
  if( !secure() ) return -1;
  if( (IS_WIZARD(user) || file[0..4]=="/open")
      && file[0] == '/'
      && MASTER->valid_read(file, user, "read_file", 0)
      ) {
    msg( user, "read " + file );
    return "OK";
  } else {
    return "FAIL";
  }
}

nomask mixed QueryWrite( string user, string file ) {
  if( !secure() ) return -1;
  if( file[0] == '/'
      && MASTER->valid_write( file, user, "write_file", 0)
      ) {
    msg( user, "write " + file );
    return "OK";
  } else {
    return "FAIL";
  }
}

#define DBG(x) if(find_player("rumata")){tell_object(find_player("rumata"),"FTPD:"+x+"\n");}

//nomask mixed QueryDir( string user, string file ) {
//  string reply;
//  object imp;
//
//  if( !secure() ) return -1;
//  reply = QueryRead( user, file );
//  if( reply=="FAIL" ) return -1;
//
//  imp = findFtpImpFor( user );
//  if( !objectp(imp) ) return -1; // should never happen
//
//  return imp->GetDir( file );
//}

nomask mixed QueryDir( string user, string file ) {
  if( !secure() ) return -1;
  if( file[0] == '/'
      && MASTER->valid_read( file+"/*", user, "get_dir", 0))
      return "OK";
  else 
    return "FAIL";
}
