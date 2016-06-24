// MorgenGrauen MUDlib
//
// master/ftpimp.c -- FTP Avatar
//
// $Id: ftpimp.c 6440 2007-08-18 23:00:49Z Zesstra $
//
// siehe /secure/ftpd

#pragma strong_types

#include "/secure/master.h"
#define FTPD "/secure/ftpd"

//private inherit "/std/shells/filesys/filesys";
private inherit "/secure/misc/filesys/filesys";

static string user; // Name des zugehoerigen Users

nomask private int secure_impcall() {
  return
    getuid(previous_object()) == ROOTID &&
    geteuid(previous_object()) == ROOTID;

}

// Nach dem Aufruf von SetUser wird der Filename geaednert, sodass
// das normale clonep in get_dir nicht mehr geht. Hier ein ersatz.
private nomask int my_is_clone() {
  return member(object_name(this_object()),':')>=0;
}

nomask void create() {
  if( !clonep(this_object()) ) {
    set_next_reset( -1 ); // blueprint soll nicht sterben
    return;
  }
  //if( secure_impcall() ) ::create();
}

nomask int SetUser( string name ) {
  if( !secure_impcall() || !clonep(this_object()) ) return -2;
  if( stringp(user) && user != "" ) {
    //write( "User="+user+"\n" );
    return -1; // schon ein name da!
  }
  user = lower_case(name);
  seteuid( user );
  set_next_reset( 1800 );
}

// Gibt einen negativen Wert zurück, wenn die Datei nicht gelesen werden
// kann oder einen string, der die gesamten Daten des Verzeichnisses
// enthält.
nomask mixed GetDir(string dir)
{
  mixed ret; ret = "";
  if( !secure_impcall() || !my_is_clone()) return -2;
  if( !stringp(user) || user=="" ) return -1;
  _ls_output( dir, &ret, user, 0x83 );
  set_next_reset( 1800 );
  return ret;
}

nomask void reset() {
  destruct(this_object());
}

varargs mixed move() {
  return -1;
}

// Fuer die Meldung auf FTP
nomask string name() {
  return capitalize(user);
}
