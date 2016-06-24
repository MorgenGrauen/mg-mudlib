// MorgenGrauen MUDlib
//
// dnslookup.c - kleines DNS-Lookup-Objekt mit Cache.
//               Braucht externen DNS-Resolver, der per UDP ansprechbar ist.
//
// Autor: Zesstra

/* Im Mudlibmaster in receive_udp() braucht es noch etwas a la:
  if( message[0..9]=="DNSLOOKUP\n" ) {
    "/p/daemon/dnslookup"->update( message );
    return;
  }

  ext. Objekt erwartet ganz simpel <hostname>\n als Eingabe und sendet zurueck:
  DNSLOOKUP\n<hostname>\n<ip-adresse>|UNKNOWN\n
  (Das | steht fuer oder.)
*/

#pragma strong_types,rtt_checks

// externer server, der den DNS-Resolve macht.
#define IPLOOKUP_HOST "127.0.0.1"
#define IPLOOKUP_PORT 8712

mapping cache = ([]);

mapping _query_cache() {return cache;}

protected void create()
{
  seteuid(getuid());
  set_next_reset(3600);
}

// ja... total simpel... nicht mehr aufwand als noetig.
void reset()
{
  cache = ([]);
  set_next_reset(10800);
}

// eigentliche Anfrage an den externen Resolver senden.
protected void make_request(string hostname)
{
  send_udp( IPLOOKUP_HOST, IPLOOKUP_PORT, hostname+"\n" );
  if (sizeof(cache) > 1000)
    reset();
  cache[hostname]=0; // cached request, but unknown result (yet)
}

// Hostnamen asynchron aufloesen.
public string resolve( string hostname )
{
  if (member(cache, hostname))
    return cache[hostname];
  make_request( hostname );
  return 0;  // leider...
}

// Pruefung, ob von <ip> Verbindungen zu unserer IP nach <localport>
// weitergeleitet werden.
public int check_tor(string ip, int localport)
{
  string *arr=explode(ip,".");
  if (!sizeof(arr)==4)
    return 0;
  string req =
    sprintf("%s.%s.%s.%s.%d.60.24.79.87.ip-port.exitlist.torproject.org",
            arr[3],arr[2],arr[1],arr[0],localport);
  
  return resolve(req) == "127.0.0.2";
}

// Pruefung, ob <ip> auf einigen Blacklists steht.
public int check_dnsbl(string ip)
{
  string *arr=explode(ip,".");
  if (!sizeof(arr)==4)
    return 0;
  string req =
    sprintf("%s.%s.%s.%s.dnsbl.dronebl.org",
            arr[3],arr[2],arr[1],arr[0]);
  
  return resolve(req) != 0;
}

/* wird vom master aufgerufen, wenn eine antwort vom externen
 * iplookup dienst eintrudelt.
 */
public void update( string udp_reply )
{
  if( previous_object()!=master() ) return;
  string *reply = explode(udp_reply,"\n");
  if( sizeof(reply)<3 ) return;

  if (reply[2] == "UNKNOWN")
    cache[reply[1]] = 0;
  else
    cache[reply[1]] = reply[2];
}

