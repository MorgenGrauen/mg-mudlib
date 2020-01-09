/* rumatas twitter client, sendet text
 * an einen twitterproxyprozess, der sie dann an twitter
 * weiterschickt.
 *
 * version 2
 * mit persistenter verbindung zum proxy und input-channel
 *
 * 25.5.2016 rumata@mg.mud.de
 */
#pragma no_clone

#include <wizlevels.h>
#include <daemon/channel.h>

#define HOST "127.0.0.1"
#define PORT 4911

#define DEBUG(x) tell_object(find_player("rumata"),x+"\n")

void create() {
	seteuid(getuid());
  CHMASTER->join("twitter",this_object());
}

object caller;
string msgbuf;

// Explizit erlaubte Personen/UIDs mit ihren Kuerzeln
mapping registered = ([
	"rumata": "ru",
	"arathorn": "ara",
	"ark": "ark",
	"zesstra": "ze",
	"zook": "zo",
	"humni": "hu",
	"miril": "mi",
	"gloinson": "gl",
  "amaryllis": "ama",
  "zaphob": "zap",
]);

// Erlaubt sind EM+ und ausgewaehlte weitere Personen
// geprueft wird anhand der UID des Interactives und weiterhin, dass alle
// Objekte in der Aufrufkette mindestens ein gleich grosses Level haben wie
// der Interactive.
int allowed() {
	return ARCH_SECURITY
         || ( this_interactive()
              && member(registered, getuid(this_interactive()))
              && secure_level() >= (query_wiz_level(geteuid(this_interactive())))
             ) ;
}
// Wer darf das Tool bekommen/nutzen?
// Alle oben eingetragenen UIDs.
public int tool_allowed(object pl)
{
  return IS_ARCH(pl) || member(registered, getuid(pl));
}

string sig( object pl ) {
	string uid = getuid(pl);
	if( !uid ) return "??";
	string abbr = registered[uid];
	if( !abbr ) return uid;
	return abbr;
}

void emit( string msg ) {
	CHMASTER->send("twitter",this_object(),msg,MSG_SAY);
	/*
	foreach( string uid: m_indices(registered) ) {
		object pl = find_player(uid);
		if( pl ) tell_object(pl,msg);
	}*/
}

/* Offizielle API funktion
 * xeval "/secure/misc/twitter"->twitter("@_zesstra_ welcome back")
 */
void twitter(string msg) {
	int err = 0;
	if( !allowed() ) {
		write( "Twitter ist ARCH+Berechtigte only.\n" );
		return;
	}
	msg = msg + "^" + sig(this_interactive()) + "\n";
	if(interactive(this_object())) {
		tell_object(this_object(),msg);
	} else {
		msgbuf = msg;
		caller = this_interactive();
		if( (err=net_connect(HOST,PORT))!=0 ) {
			write( "Konnte Tweet nicht senden. err="+err+"\n" );
		}
	}
}

// sonderfunktion fuer den fall, dass man die verbindung
// aufbauen will, ohne dass etwas auf twitter erscheint
void connect() {
	int err = 0;
	if( interactive(this_object()) ) return;
	msgbuf = "";
	caller = this_interactive();
	if( (err=net_connect(HOST,PORT))!=0 ) {
		write( "Konnte Tweet nicht connected. err="+err+"\n" );
	}
}

varargs void logon(int flag) {
	if( flag<0 ) {
		tell_object(caller,"Twitterproxy antwortet nicht?\n");
  } else {
		enable_commands();
		add_action("input","",1);
		write(msgbuf);
		if( msgbuf!="" ) emit( msgbuf );
		//"/secure/master"->disconnect(this_object());
	}
}

void NetDead() {
	disable_commands();
}

string query_real_name() {
	return "(Twitter)";
}

varargs string Name( int casus, int demon ) {
	return "Twitter";
}

// daten werden via tell_object an dieses objekt gesendet
// und dann nach aussen weitergeben
void catch_tell( string msg ) {
	if( allowed() && interactive(this_object()) ) {
		write( msg );
	}
}

// hier landen daten von aussen
int input( string s ) {
	string msg = query_verb();
	if( s ) msg = msg + " " + s;
	emit( msg ); //"Twitter teilt dir mit: "+msg+"\n" );
	return 1;
}

