/*
 * Mudlibseite des iplookup systems, mit dem aus numerischen IP Adressen
 * der Hostname und der Ort, an dem sich der Rechner befinden berechnet wird.
 * Ergebnisse werden gecachet.
 *
 * (c)2010 rumata @ morgengrauen
 */

#pragma strict_types,save_types
#pragma no_clone,no_shadow

// Format der ipmap:
// Key: numerische ip als string (ohne fuehrende nullen)
// Data:
//  [ip,0] Hostname, der vom externen Server geliefert wurde.
//  [ip,1] Locationinformation, die vom externen Server kommt.
//    dieser sollte den Ort, oder falls es nicht besser geht, das Land
//    im Klartext liefern.
//  [ip,2] Zeitstempel, bei dessen erreichen der Wert geloescht werden
//    soll.
//  [ip,3] Status der Anfrage

private mapping ipmap;
#define IPMAP_HOST   0
#define IPMAP_CITY   1
#define IPMAP_EXPIRE 2
#define IPMAP_STATUS 3

// externer server, der der lokationdaten berechnet
#define IPLOOKUP_HOST "127.0.0.1"
#define IPLOOKUP_PORT 8711
#define IPLOOKUP_SAVE "/p/daemon/save/iplookup"

// moegliche statuswerte (platz zum zaehlen der anfragen gelassen)
#define STATUS_QUERYING      1
#define STATUS_AUTHORITATIVE 128

// so lange werden die daten im cache gehalten
#define CACHE_TIME    86400  /* 1 day */

// Ab dieser Groesse betrache ich den cache als "gross genug",
// um einen cache update in etappen zu rechtfertigen.
#define LARGE_CACHE_LIMIT 2000

// so lange sind wir bereit auf die antwort einer anfrage beim server zu warten
#define QUERY_TIME    60     /* 1 min */

#include <config.h>
#if MUDNAME == "MorgenGrauen"
#define LOG(x) log_file("rumata/iplookup.log",strftime("%Y-%m-%d %H:%M:%S: ")+(x)+"\n")
#else
#define LOG(x)
#endif

// PROTOTYPES

public void create();
public void reset();
static int active_entry( string key, mixed* data );
static void expire_cache_small();
static void expire_cache_large( string* keys );
public void update( string udp_reply );

// IMPLEMENTATION

public void create() {
	seteuid(getuid());
	restore_object( IPLOOKUP_SAVE );

	if( !mappingp(ipmap) || widthof(ipmap)!=4 ) {
		ipmap = m_allocate( 0, 4 );
		LOG("restore failed, creating new ipmap");
	} else {
	  LOG(sprintf("created (%d entries)",sizeof(ipmap)));
	}
}

public void reset() {
	if( sizeof(ipmap) < LARGE_CACHE_LIMIT ) {
		expire_cache_small();
	} else {
		LOG(sprintf("reset %d ->",sizeof(ipmap)));
		expire_cache_large( m_indices(ipmap) );
	}
  set_next_reset(10800);
}

static int active_entry( string key, mixed* data ) {
	return time() < data[IPMAP_EXPIRE];
}

// Den cache auf einfache weise aufraeumen.
static  void expire_cache_small() {
	int s = sizeof(ipmap);
	ipmap = filter( ipmap, #'active_entry );
	save_object( IPLOOKUP_SAVE );
	LOG(sprintf("reset (%d -> %d)",s,sizeof(ipmap)));
}

// Kompliziertere routine, die den cache etappenweise abarbeitet.
static	void expire_cache_large( string* keys ) {
	if( !pointerp(keys) ) return;
	int next=0;
	foreach( string key: keys ) {
		if( get_eval_cost() < 100000 ) break;
		if( ipmap[key,IPMAP_EXPIRE] < time() ) {
			m_delete( ipmap, key );
		}
		next++;
	}
	LOG(sprintf("checking %d of %d",next,sizeof(keys)));
	if( next<sizeof(keys) ) {
		call_out( #'expire_cache_large, 6, keys[next..] );
	} else {
		save_object( IPLOOKUP_SAVE );
		LOG(sprintf("reset -> %d (done)",sizeof(ipmap)));
	}
}

#define SEARCHING "<auf der Suche...>"

/* Erzeugt einen temporaeren Eintrag, der fuer eine Suche steht.
 * Wenn die Antwort kommt, wird der Eintrag mit den entgueltigen
 * Daten ueberschrieben.
 */
static void make_request( string ipnum ) {
	send_udp( IPLOOKUP_HOST, IPLOOKUP_PORT, ipnum );
	ipmap += ([ ipnum : ipnum ; SEARCHING ;
		time()+QUERY_TIME ; STATUS_QUERYING
	]);
}

/* Liefert zu einer gegebenen ipnum den Ort.
 * diese Funktion wird von simul_efun aufgerufen.
 * @param ipnum eine numerische ip-adresse oder ein interactive
 * @return den Ort (oder das Land) in dem sich die ip-adresse
 * laut externem Server befindet.
 */
public string country( mixed ipnum ) {
	string host,city;
	int expire,state;

	if( objectp(ipnum) ) {
		ipnum = query_ip_number(ipnum);
	}
	if( !stringp(ipnum) ) {
		return "<undefined>";
	}
	if( !m_contains( &host, &city, &expire, &state, ipmap, ipnum ) ) {
		make_request( ipnum );
		return SEARCHING;
	}

	return city;
}

/* Liefert zu einer gegebenen ipnum den Hostnamen.
 * diese Funktion wird von simul_efun aufgerufen.
 * @param ipnum eine numerische ip-adresse oder ein interactive
 * @return den Hostnamen der zu der angegebenen ip-adresse gehoert.
 * wenn der hostname nicht bekannt ist, wird die ipadresse zurueckgegeben.
 */
public string host( mixed ipnum ) {
	string host,city;
	int expire,state;

	if( objectp(ipnum) ) {
		ipnum = query_ip_number(ipnum);
	}
	if( !stringp(ipnum) ) {
		return "<undefined>";
	}
	if( !m_contains( &host, &city, &expire, &state, ipmap, ipnum ) ) {
		make_request( ipnum );
		return ipnum;
	}

	return host;
}

/* wird vom master aufgerufen, wenn eine antwort vom externen
 * iplookup dienst eintrudelt.
 */
public void update( string udp_reply ) {
	string* reply;
	if( previous_object()!=master() ) return;
	reply = explode(udp_reply,"\n");
	if( sizeof(reply)<4 ) return;

	if( reply[3] == "<unknown>" ) reply[3] = reply[1];
	if( reply[2] == "<unknown>" ) reply[2] = "irgendwoher";
	ipmap += ([ reply[1] : reply[3] ; reply[2] ; time()+CACHE_TIME ;
		STATUS_AUTHORITATIVE ]);
	//save_object( IPLOOKUP_SAVE );
}

int remove(int silent) {
  save_object( IPLOOKUP_SAVE );
  destruct(this_object());
  return 1;
}

// DEBUGGING CODE

#if 0
public void dump( string key, mixed* data ) {
	printf( "%s: %s (%s) s=%d bis %d\n", key, data[0], data[1],
		data[3], data[2] );
}

public void load(string s) {
	restore_object(s);
	if( !mappingp(ipmap) || widthof(ipmap)!=4 ) {
		ipmap = m_allocate( 0, 4 );
		LOG("restore failed, creating new ipmap");
	} else {
	  LOG(sprintf("created (%d entries)",sizeof(ipmap)));
	}
}

public void debug() {
	map( ipmap, #'dump );
	printf( "= %d Eintraege\n", sizeof(ipmap) );
}
#endif
