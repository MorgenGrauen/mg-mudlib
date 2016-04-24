/*

   //TODO: implement retries
 */

#define protected public

#pragma strict_types, save_types, rtt_checks, pedantic
#pragma no_clone, no_shadow

#include <tls.h>
#include <config.h>
#include <driver_info.h>
#include <lpctypes.h>
#include <strings.h>

// data structures
struct host_s {
    string name;
    string key;
    string ip;
    int port;
    int last_contact;
    int first_contact;
    int reputation;
    string *services;
    int im_version;
    int ncttl;
    int mtu;
    mapping received;
};

struct fragment_s {
    string header;
    string buf;
};

struct packet_s {
    int id;
    int timestamp;
    int pversion;
    int pflags;
    struct fragment_s *fragments;
    string peername;
    mapping data;
    string buf;
};

struct service_s {
    string name;
    string obname;
    string fun;
    closure recv_cb;
};

struct request_s {
    int id;
    struct packet_s packet;
    closure callback;
    int timeout;
};

// Prototypes & Defines
protected void init();
void ping_many_muds(string *muds);
protected int calc_ncttl(struct host_s p);
protected void send_helo(string name);

// mud-compatibility layer - might implement efuns with lfuns if not available
// on the specific mud.
inherit __DIR__"inetd_compat";

// Mapping of known peers with their names as keys.
mapping peers = ([]);

// our own offered services
mapping services = ([]);

// our own host_s structure - it also contains our _private_ key.
struct host_s self = (<host_s>);

// reset counter for housekeeping
int reset_counter;

nosave int last_packet_id = 0;
nosave mapping requests = ([]);

// Minimum protocol version to accept (default 0)
int min_protocol_version = 0;

// *** lfuns in this section might be re-defined to suit the individual muds.
protected void debug_msg(string msg, int severity)
{
  write(msg);
}

protected void set_user()
{
  seteuid(getuid());
}

protected void save_me()
{
}

protected int restore_me()
{
}

protected void export_peer_list(mapping list)
{
}

// *** end of mud-specific lfuns ***

private string *sys_fields = ({HOST, NAME, PACKET, UDP_PORT, SYSTEM, VERSION,
                               FLAGS, SIGNATURE,
                               PACKET_LOSS, RESPONSE_TIME});

// *** public interface
public void register_service(string name, string fun)
{
  struct service_s serv;

  closure cl = symbol_function(fun, previous_object());
  if (!cl)
    raise_error("register_service(): function not public.\n");

  if (member(services, name))
    serv = services[name];
  else
    serv = (<service_s>);

  serv->name = name;
  serv->obname = object_name(previous_object());
  serv->fun = fun;
  serv->recv_cb = cl;
  services[name] = serv;
  self->services = m_indices(services);
  debug_msg(sprintf("Service registered: %O\n",serv),40);
}

public void unregister_service(string name)
{
  struct service_s serv = services[name];
  if (!structp(serv))
    return;
  // wenn von aussen gerufen, nur registriertes Objekt selber.
  if (extern_call() && serv->obname != object_name(previous_object()))
    raise_error("Service can only be unregistered by service object.\n");
  m_delete(services, name);
  self->services = m_indices(services);
  debug_msg(sprintf("Service unregistered: %s\n",name),40);
}

public string *QueryPeerList()
{
  return m_indices(peers);
}

// TODO: remove, for debug only
public struct host_s QueryPeer(string name)
{
  return peers[name];
}

public string PeerName()
{
  return self->name;
}

// **************** internal implementation ***********************


protected void configure_host(struct host_s new_self)
{
  new_self->first_contact = time();
  new_self->last_contact = time();
  new_self->reputation = __INT_MAX__;
  new_self->ncttl = __INT_MAX__;
  //if (!sizeof(new_self->services))
  //  raise_error("Illegal to configure peer without services!\n");
  if(new_self->key)
    new_self->im_version=2500;
  else
    new_self->im_version=2000;
  if (new_self->mtu < 1024)
    raise_error("The minimum UDP length (MTU) must >= 1024 bytes!\n");
  // ok, uebernehmen
  self = new_self;
  debug_msg(sprintf("localhost configured: %O\n",self),30);
}

// Updates (or adds a new) peer.
protected struct host_s add_new_peer(struct host_s p)
{
  // TODO: validate
  // This must not happen, the caller must check for existence to prevent
  // hijacking peer names...
  if (member(peers, p->name))
    raise_error("Attempt to add existing peer!\n");
  if (p->mtu < 1024) p->mtu = 1024;
  p->received = ([]);
  peers[p->name] = p;
  debug_msg(sprintf("new peer added: %O\n",p),50);
  return p;
}

// Updates (or adds a new) peer.
protected void update_peer(struct host_s p)
{
  //TODO validate
  struct host_s old = peers[p->name];
  if (!old)
    peers[p->name] = p;
  else
  {
    // we always keep some data...
    old->key = p->key;
    old->ip = p->ip;
    old->port = p->port;
    if (p->last_contact > old->last_contact)
        old->last_contact = p->last_contact;
    if (p->first_contact < old->first_contact)
        old->first_contact = p->first_contact;
    old->services=p->services;
    old->reputation = p->reputation;
    old->im_version = p->im_version;
    if (p->mtu >= 1024) old->mtu = p->mtu;
    old->ncttl = p->ncttl;
    // keep packet fragments, don't update received.
    debug_msg(sprintf("peer updated: %O\n",self),50);
  }
}

// remove a peer from the known peer list
protected void kill_peer(string name)
{
  m_delete(peers, name);
  debug_msg(sprintf("peer deleteted: %s\n",name),50);
}

// Update last_contact value to current time.
protected void touch_peer(string name)
{
  struct host_s p = peers[name];
  if (p)
    p->last_contact = time();
}

// imports old style host lists. Should only be done once.
public int import_host_list(string file) {

  int res;
  mixed data = (read_file(file));
  if (!data) {
    debug_msg("*Error in reading host file: " + file +"\n\n", 100);
    return 0;
  }

  data = old_explode(data, "\n");

  foreach(string line: data)
  {
    if (line == "" || line[0] == '#')
      continue;
    mixed fields = old_explode(line, HOSTFILE_DEL);
    if (sizeof(fields) < 5) {
      debug_msg(sprintf(
            "*Parse error in hosts file: %s, Line: %s\n", file, line), 100);
      continue;
    }

    struct host_s p = (<host_s>);
    p->name = lower_case(fields[HOSTLIST_NAME]);
    // skip already known peers
    if (member(peers, p->name))
      continue; // mud already in list

    p->services = old_explode(fields[HOSTLIST_LOCAL_COMMANDS],HOSTFILE_DEL2);
    if (member(p->services,"*") != -1)
        p->services = p->services - ({ "*" }) + DEFAULT_COMMANDS;
    p->ip = fields[HOSTLIST_IP];
    p->port = to_int(fields[HOSTLIST_UDP_PORT]);
    // we fake first and last contact, otherwise it would be expired if not online.
    p->first_contact = time();
    p->last_contact = time();
    p->mtu = 1024;
    p->ncttl = calc_ncttl(p);

    add_new_peer(p);
    ++res;
  }
  debug_msg(sprintf("old-stype host list imported: %s, %d new peers\n",
                    file,res),50);
  return res;
}


protected void init()
{
  if (!self)
    raise_error("Can't init/startup without configuration!\n");

  // Register the core services. Must be callother for previous_object being
  // ourself
  this_object()->register_service(PING, "recv_ping");
  this_object()->register_service(QUERY, "recv_query");
  this_object()->register_service(REPLY, "recv_reply");
  this_object()->register_service(HELO, "recv_helo");
  debug_msg(sprintf("init() performed, pinging known peers.\n"),30);
  ping_many_muds(m_indices(peers));
}

protected string encode(mixed v)
{
  switch(typeof(v))
  {
    case T_NUMBER:
      return to_string(v);
    case T_STRING:
      return "$"+v;
    default:
      // convert into a string representation - hopefully the receiver
      // knows what to do with it.
      return sprintf("$%Q",v);
  }
}

#define SIGLENGTH 32+4 // length of signature including "$S:|"

protected void sign_fragment(struct fragment_s f)
{
  if (self->key)
  {
    f->header=sprintf("%s$S:%s|", f->header,
                      hmac(TLS_HASH_SHA512, self->name, f->buf));
    debug_msg(sprintf("signed fragment: %s, %s.\n",f->header,f->buf),90);
  }
  else
    f->header="";
}


// creates a packet structure with fragments to send (can be only one, if the
// packet does not need to be split).
protected struct packet_s packetize(struct host_s dest, mapping data)
{
  // some field names in the packet are reserved and must not be used by
  // users, they are added by the inetd.
  if (sizeof(data & sys_fields))
    raise_error(sprintf(
          "Data must not contain reserved system fields: %O.\n",
          data & sys_fields));

  // create packet structure
  struct packet_s packet = (<packet_s>
                            id: ++last_packet_id, timestamp: time(),
                            pversion: self->im_version, pflags: 0,
                            fragments: 0, peername: 0, data: data, buf: 0);

  // add the system fields (NAME, V, F must be the first fields!)
  // the ID field is special, because maybe the packet is an answer to a
  // request. In this case, we must keep the original ID
  if (!member(data, ID))
    data[ID] = packet->id;
  else
  {
    if (data[REQUEST] != REPLY)
        raise_error(sprintf("ID only permitted in reply packets!"));
  }
  // NAME is added below
  data[UDP_PORT] = self->port;

  // create an array of fields (key:value pairs)
  string data_field;
  string *  fields = allocate(sizeof(data)+3);
  fields[0] = sprintf("NAME:%s", self->name);
  fields[1] = sprintf("V:%d", packet->pversion);
  fields[2] = sprintf("F:%d", packet->pflags);
  int cindex = 3;
  foreach(string key, mixed val : data)
  {
    // the DATA field must be added last, so remember its encoded string.
    // Also, it is not checked for reserved characters
    if (key == DATA)
      data_field = sprintf("%s:%s", encode(DATA), encode(val));
    else
    {
      if (!stringp(key) || !sizeof(key))
        raise_error(sprintf("send((): Illegal field name %.30O in data "
                            "payload.\n",key));
      string ekey = encode(key);
      string eval = encode(val);
      // check key and value strings for reserved characters
      if (regmatch(ekey,"[|:]") || regmatch(eval, "[|:]"))
        raise_error(sprintf("send(): Field name or value contains reserved "
                            "character: %.50s,%.50s\n", ekey, eval));
      fields[cindex] = sprintf("%s:%s",ekey,eval);
      ++cindex;
    }
  }
  // add the DATA field if existing
  if (data_field)
    fields[cindex] = data_field;

  // write the string buffer
  packet->buf = implode(fields, "|");

  // now split into fragments if packet too large for one.
  int mtu = min(self->mtu, dest->mtu);
  if ((mtu - SIGLENGTH - sizeof(packet->buf)) < 0)
  {
    // multiple fragments, each starting with
    // PKT:peername:packet-id:packet-number/total-packets|
    // fsize is the maximum payload length per fragment.
    string fheader = sprintf("PKT:%s:%d:", self->name, packet->id);
    int fsize = mtu - SIGLENGTH - sizeof(fheader)
                - 8; // sizeof("nnn/mmm|")
    string buffer = packet->buf;
    int fcount = sizeof(buffer) / fsize;
    // if there is a modulo left, we need one additional fragment
    if (sizeof(buffer) % fsize)
      ++fcount;
    if (fcount > 999)    // too many fragments?
      raise_error("packet too long to send.\n");
    // allocate the complete fragment array.
    packet->fragments = allocate(fcount);
    foreach(int i: fcount)
    {
      struct fragment_s f = (<fragment_s>
                     header:sprintf("%s%d/%d|",fheader,i+1,fcount),
                     buf: buffer[i*fsize .. min((i+1)*fsize, sizeof(buffer))-1]
          );
      sign_fragment(f);
      packet->fragments[i] = f;
      debug_msg(sprintf("Created fragment %d/%d\n", i,fcount),90);
    }
  }
  else
  {
    // one fragment, and the fragment header will be empty.
    struct fragment_s f = (<fragment_s> header: "",
                           buf: packet->buf);
    sign_fragment(f);
    packet->fragments = ({f});
    debug_msg(sprintf("Created fragment 1/1\n"),90);
  }
  return packet;
}

// encodes the mapping data into a packet and sends it to <dest>. The
// key-value pairs of <data> are converted into intermud fields. The keys must
// be strings and only the DATA field can contain : or |.
// Also, <data> must not contain any reserved field names.
// If the caller expects an answer, <cb> must contain a calleable closure. In
// this case, the inetd remembers the request. When the answer arrives, the
// callback <cb> is called with the request id (int) and a mapping (the answer
// packet) as arguments. The return value of send() is in this case the
// request id used for storage.
// <= 0 for error, > 0 for success.
public int send(string dest, mapping data, closure cb)
{
  if (!sizeof(data))
    raise_error("Illegal sending empty packets.");
  if (!sizeof(dest))
    raise_error("Can't send without destination: %O.\n");
  // find peer data. If not found, abort and tell the caller
  struct host_s peer = peers[dest];
  if (!structp(peer))
    return -1;

  if (member(peer->services, data[REQUEST]) == -1)
    raise_error("Remote host doesn't offer service "+
                data[REQUEST] + ".\n");

  // then packetize the data, i.e. create packet structure, encode into buffer
  // and split into fragments as needed and sign fragments.
  // There will be at least one fragment structure.
  struct packet_s packet = packetize(peer, data);

  // loop over all fragments and send them
  foreach(struct fragment_s f : packet->fragments)
  {
    debug_msg(sprintf("%O <- %.500O\n",peer->ip, f->header + f->buf), 100);
    // TODO: error handling
    send_udp(peer->ip, peer->port, f->header + f->buf);
  }
  // delete the buffer
  packet->buf = 0;
  // delete the fragments until we support re-transmission of fragments
  packet->fragments = 0;

  // we keep the original data for reference, if storing the request.

  // if an answer is expected, there is a callback closure. Then we store the
  // request with a timeout of 120s.
  if (cb)
  {
    struct request_s r = (<request_s> packet->id, packet, cb, time()+120);
    requests[r->id] = r;
    debug_msg(sprintf("Answer expected, request stored\n"),80);
  }

  return packet->id;
}

// like send(), but sends to all peers offering the service <service>. But
// limited to peers we had contact with in the last 24h.
public int broadcast(string service, mapping data, closure cb)
{
  if (!service)
    raise_error("No service given!\n");
  if (!sizeof(data))
    raise_error("Illegal sending empty packets.");
//TODO: check if we could send the same packet to all peers, instead of
//packetizing for each peer as well.
  int ret;
  int tlc_cutoff = time() - 86400;
  foreach(string dest, struct host_s peer : peers)
  {
    if (peer->last_contact >= tlc_cutoff
        && member(peer->service, service) != -1)
    {
      send(dest, data, cb);
      ++ret;
    }
  }
  return ret;
}

protected string|int decode(string arg)
{
    if (sizeof(arg) && arg[0] == '$')
        return arg[1..];
    if (to_string(to_int(arg)) == arg)
        return to_int(arg);

    return arg;
}

// must be validated before. Parses the packet buffer into the given packet
// structure and returns it. In case of errors (e.g. malformed packets), 0 is
// returned.
protected struct packet_s parse_packet(struct packet_s packet)
{
  string *fields = explode(packet->buf, "|");
  // The DATA field may contain any number of | which don't signify a field.
  // Therefore counting | over-estimates the number of fields. To prevent
  // excessive memory allocation, we limit the allocation to 32 fields. The
  // mapping may still grow if there are really more fields.
  packet->data = m_allocate(min(sizeof(fields),32));
  int cindex;
  while (cindex < sizeof(fields))
  {
    string header, info;
    /// DATA fields can be denoted by a preceeding blank field :-/
    if (!sizeof(fields[cindex]))
    {
      ++cindex;
      header = DATA;
      //Test for illegal packet length (no DATA)
      if (cindex >= sizeof(fields))
        return 0;
      // take the first "field" of DATA (the rest is added below)
      info = fields[cindex];
    }
    else if (sscanf(fields[cindex], "%s:%s", header, info) != 2)
      return 0;
    header = decode(header);
    if (header == DATA)
    {
      // add the rest of the packet and combine back into one string if
      // necessary
      if (cindex < sizeof(fields)-1)
        info = implode(({info}) + fields[cindex+1 ..], "|");
      // and we have finished after decoding info
      cindex = sizeof(fields);
    }
    packet->data[header] = decode(info);
    ++cindex;
  }
  // only allow printable characters for these fields.
  response[SENDER] =
             regreplace(response[SENDER],"[:^print:]|\n","",1);
  response[NAME] =
             regreplace(response[NAME],"[:^print:]|\n","",1);
  response[RECIPIENT] =
             regreplace(response[RECIPIENT],"[:^print:]|\n","",1);

  return packet;
}

protected void defragment_packet(struct packet_s p)
{
  if (sizeof(p->fragments))
  {
    p->buf = "";
    foreach(struct fragment_s f : p->fragments)
    {
      p->buf += f->buf;
    }
    p->fragments = 0;
  }
}

protected int validate_signature(string signature, string buf, struct host_s src)
{
  if (!src->key)
    return 0;
  // TODO: debug purpose
  return 1;
}

// returns a packet_s or 0 for invalid or incomplete (!) packet. In both
// cases, the caller should not do anything. In case of incomplete packets,
// the fragment will be stored, if it was valid.
protected struct packet_s validate_fragment(string buffer, struct host_s sender)
{
  string signature, pname;
  int packetid, fragno, totalfrags, version, flags;
  struct packet_s packet;
  struct fragment_s f;

  if (sizeof(buffer) > MAX_UDP_LENGTH)
    return 0;

  if (sscanf(buffer, "S:%s|NAME:%s|V:%d|$F:%d|%~s",
             signature, pname, version, flags) == 5)
  {
    // non-fragmented v2.5+ packet
    debug_msg(sprintf("Received v2.5+ packet.\n"),90);
    pname = lower_case(pname);
    packet = (<packet_s> timestamp: time(), pversion: version,
              pflags: (flags & ~FL_VALID_SIGNATURE),
              buf: buffer[strstr(buffer,"|")+1..]
             );
    debug_msg(sprintf("Received v2.5+ packet from %s.\n",pname),90);
    sender = peers[pname];
    // if we don't know the sender yet, we create a new peer with its name.
    // The ip and port will be updated during parsing.
    if (!sender)
    {
      sender = add_new_peer( (<host_s> name: pname,
                          first_contact: time(),
                          last_contact: time(),
                          mtu: 1024,
                          im_version : version,
                          received: m_allocate(5)
                         ));
      // but if we can't create one, we discard the packet
      if (!sender)
        return 0;
    }
    packet->peername = pname;
    // try to check the packet signature and record the result, if
    // successful.
    if (validate_signature(signature, packet->buf, sender))
    {
      packet->pflags |= FL_VALID_SIGNATURE;
    }
  }
  else if (sscanf(buffer, "PKT:%.1s:%d:%d/%d|%~s",
                  pname, packetid, fragno, totalfrags) == 5)
  {
    // this is a fragmented packet
    // we at least check if the info about fragmentation is sane.
    if (totalfrags > 999 || fragno > totalfrags
        || totalfrags < 1 || fragno < 1)
      return 0;

    pname=lower_case(pname);

    // is it even a IM 2.5+ packet? (want to avoid copying the buffer,
    // therefore this approach)
    if (sscanf(buffer, "PKT:%.1~s:%~d:%~d/%~d|$S:%.10s|%~s",
               signature) == 6)
    {
      // create fragment (we know, there are at least two |)
      int first_pipe = strstr(buffer,"|");
      int sig_end = strstr(buffer, "|", first_pipe+1);
      f = (<fragment_s> header: buffer[0..first_pipe],
                      buf: buffer[sig_end+1 ..]
          );
      debug_msg(sprintf("Received v2.5+ packet %d, fragment %d/%d from %s.\n"
                ,packetid, fragno, totalfrags, pname),90);
    }
    else
    {
      // create fragment (we know, there is at least one |)
      debug_msg(sprintf("Received v2 packet %d, fragment %d/%d from %s.\n"
                ,packetid, fragno, totalfrags, pname),90);
      int header_end = strstr(buffer, "|");
      f = (<fragment_s> header: buffer[0..header_end],
                      buf: buffer[header_end+1 ..]
          );
    }

    // get the sender
    sender = peers[pname];
    // if we don't know the sender yet, we create a new peer with its name.
    // The ip and port will be updated during parsing, when the packet is
    // complete.
    if (!sender)
    {
      // if we can't create one, we discard the fragment
      sender = add_new_peer( (<host_s> name: pname,
                          first_contact: time(),
                          last_contact: time(),
                          mtu: 1024,
                          received: m_allocate(5)
                         ));
      if (!sender)
        return 0;
    }
    // did we already received a fragment? - get its packet
    packet = sender->received[packetid];
    // if not create new packet structure and add it to the receive queue of
    // the peer
    if (!packet)
    {
      packet = (<packet_s> id : packetid, timestamp: time(),
                peername: pname,
                fragments : allocate(totalfrags, 0)
               );
      m_add(sender->received, packetid, packet);
    }

    // now we can also check the signature, if there is one.
    if (signature &&
        validate_signature(signature, f->header+f->buf, sender))
    {
         packet->pflags |= FL_VALID_SIGNATURE;
    }
    else
    {
      // if there is no valid signature, but we received a fragment of this
      // packet with valid signature, we discard the fragment.
      if (packet->pflags & FL_VALID_SIGNATURE)
      {
        debug_msg(sprintf("Received fragment %d with invalid signature in "
                          "signed packet %d from %s. Discarding fragment.\n"
                          ,fragno, packetid, pname),40);
        return 0;
      }
    }

    // add fragment to its slot - if we receive a fragment twice, the last one
    // arriving wins.
    packet->fragments[fragno] = f;
    // if not all fragments are received, we end processing here and wait for
    // more.
    if (member(packet->fragments, 0))
      return 0;
  }
  else
  {
    // we assume a non-fragmented legacy packet, no real validation
    // possible.
    packet = (<packet_s> timestamp: time(),
              buf: buffer
             );

    debug_msg(sprintf("Received v2 packet.\n"),90);
  }
  // at this point we have a complete packet - but we may have to defragment
  // it yet.
  if (packet->fragments)
  {
    defragment_packet(packet);
    // and remove it from the receive queue of the sender (note, if we have
    // fragments in the packet, we also have a valid sender at this point)
    m_delete(sender->received, packet->id);
    // and check for versions and flags - will be there for IM 2.5+ packets.
    // Note: if the sender claims a version < 2000 here, this is also fine.
    // If not there, we assume IM 2/Zebedee.
    if (sscanf(buffer, "NAME:%~s|V:%d|$F:%d|%~s",
               version, flags) == 4)
    {
      packet->pversion = version;
      // store flags, but do not set the FL_VALID_SIGNATURE flag ;-)
      packet->pflags |= (flags & ~FL_VALID_SIGNATURE);
    }
    else
      packet->pversion = 2000;
  }

  // if the packet has a version >= 2500, it must be signed. If not, we
  // discard it.
  if (packet->pversion >= 2500
      && !(packet->pflags & FL_VALID_SIGNATURE))
  {
    // But there is one exception: if we don't have a key for the
    // peer yet, we accept the packet anyway. This is needed to receive its
    // public key, which we are going to ask them (soon).
    if (sender->key)
    {
      debug_msg(sprintf("Received unsigned packet from v2.5+ peer with "
                        "key. Discarding."),40);
      return 0; // key, but no valid signature - discard it.
    }
  }

  // Maybe we accept only packets conforming to a specific protocol version
  // (or newer). This can be used to accept only signated packets.
  // also we don't accept protocol version older than we already saw for this
  // peer to prevent downgrade attacks
  if (packet->pversion < min_protocol_version)
    return 0;

  return packet;
}

/* Check wether a UDP request was valid. Is done after validating and parsing
 * the packet (and all its fragments).
 * Logs are made and "host" information is updated as appropriate.
 * Arguments: Decoded UDP packet (struct packet_s)
 *            sending peer (struct host_s)
 * Returns:   0 for valid packets, an erro  r string otherwise.
 */
string validate_request(struct packet_s p, struct host_s sender)
{
  // If we have no peername, it was a legacy packet. Try to find the NAME in
  // the packet payload.
  if (!p->peername)
  {
    if(!member(p->data, NAME))
      return "Name of sending peer not given.\n";
    p->peername = lower_case(p->data[NAME]);
  }

  if (p->peername == self->name)
    return "Someone tried to fake ourself!\n";

  // if needed, check the payload for the ID. The ID may be absent...
  if (!p->id)
    p->id = p->data[ID];
  // ... unless this is a reply to a request. And also, a reply can only be
  // valid, when know about the initial request sent by us.
  struct request_s req;
  if (p->data[REQUEST] == REPLY)
  {
    if (!p->id)
      return "Reply packet without packet ID.\n";
    // and we take note of the request while we are at it, in case we need it.
    req = requests[p->id];
    if (!structp(req))
      return "Reply packet without request.\n";
  }

  // if we still have no sender (legacy non-fragmented packets), we finally
  // create one, because we know the sender by name now.
  sender = peers[p->peername];
  if (!sender)
  {
    // but if we can't create one, we discard the request
    sender = add_new_peer( (<host_s> name: p->peername,
                        first_contact: time(),
                        last_contact: time(),
                        mtu: 1024,
                        received: m_allocate(3),
                       ));
    if (!sender)
      return "New peer, but peer list too full.\n";
  }

  // we don't accept protocol version older than we already saw for this
  // peer to prevent downgrade attacks.
  if (p->pversion < sender->im_version)
    return "Downgraded peer intermud version.";
  else
    sender->im_version = p->pversion;

  // if we have a packet with version >= 2500 and no key for the peer
  // yet, we ask them by sending a HELO packet (which also transmits our
  // public key).
  // Note: if we have a packet with version >= 2500, no valid signature, but
  // already a key for the peer, validate_fragment() would already have
  // discarded the packet/fragment and we would not be here.
  // Of course, don't HELO, if this is a helo.
  if (p->pversion >= 2500 && !sender->key
      && p->data[REQUEST] != HELO)
  {
    // also don't HELO, if this is a response to a HELO...
    if (!req || req->packet->data[REQUEST] != HELO)
      send_helo(p->peername);
  }

  // Note: HELO packets or HELO reponses in packets without valid signature
  // can't survive until here if we know a key for the peer. Therefore, we
  // don't have to validate that specifically.

  // TODO: e.g. some requests are allowed only in signed packets.

  return 0; // request ok!
}

// processes the received packet or fragment. Must be called from the
// function called by the mudlib master in this object.
protected void process_fragment(string host, string msg, int hostport)
{
  // Catch if someone tries to fake ourself.
  if (host == self->ip && hostport == self->port)
    return;
  debug_msg(sprintf("Received packet from %s:%d.\n",
                    host, hostport), 100);
  // First (try to) validate the received fragment (or packet).
  struct host_s sender;
  struct packet_s p = validate_fragment(msg, &sender);
  // If we got a packet structure, the packet is complete and all the
  // fragments were valid. If not, the packet is either invalid (and to be
  // discarded) or not complete yet. In both cases, processing ends here.
  if (!p)
    return;

  // first parse the packet into a mapping. And again, when not successful,
  // 0 will be returned.
  p = parse_packet(p);
  if (!p)
    return;

  // record sending host data, may be important for validating the request.
  p->data[HOST] = host;
  p->data[UDP_PORT] = hostport;

  debug_msg(sprintf("Packet valid and complete.\n"), 90);

  // Validate the request...
  string reason = validate_request(p, &sender);
  if (reason)
  {
    debug_msg("validate_request(): discarding request: "+reason+"\n", 30);
    return; //discard
  }
  sender->last_contact = time();
  // if we received the packet from a host/port combination that is different
  // from the one we know for this peers, we update the ip address and port.
  // Note: if the packet was signed, the origin is proved by now. If not...
  // well... We believe it now.
  if (sender->ip != host || sender->port != hostport)
  {
    debug_msg(sprintf("Updating address of peer %s from %s:%d to %s:%d.\n",
                      sender->name, sender->ip, sender->port,
                      host, hostport), 40);
    sender->ip = host;
    sender->port = hostport;
  }

  touch_peer(sender->name);

  // then execute/forward it.
  struct service_s srv = services[p->data[REQUEST]];
  if (srv)
  {
    if (!srv->recv_cb)
   {
      srv->recv_cb = symbol_function(srv->fun, find_object(srv->obname));
      if (!srv->recv_cb)
      {
        // If no callable, the service is unregistered.
        unregister_service(srv->name);
        return;
      }
    }
    funcall(srv->recv_cb, p->data);
  }
  else
    // unknown service. Discard for now. //TODO:: send reply
    return;

} // process_fragment()


// returns an increase in reputation (may be 0),
// may be overloaded in inheritees.
protected int auto_promote_peer(struct host_s p)
{
  // only if we heard from that peer in the last day.
  if (p->last_contact + 86400 < time())
    return 0;
  // TODO: decide if auto-promotion above n should be tied to having a key.
  if (p->reputation < 1
      && p->first_contact < time() - 7*86400)
    return 1;
  if (p->reputation < 2
      && p->first_contact < time() - 30*86400)
    return 1;
  if (p->reputation < 3
      && p->first_contact < time() - 12*30*86400)
    return 1;

  return 0;
}

// Calculate the not-connected-time-to-live depending on the reputation. May
// be overloaded in inheritees.
protected int calc_ncttl(struct host_s p)
{
  switch(p->reputation)
  {
    case 0:
      return 3*86400;
    case 1:
      return 14*86400;
    case 2:
      return 3*30*86400;
    case 3:
      return 6*30*86400;
    default:
      return 12*30*86400;
  }
}

// checks if a peer should be expired (forgotten) and deletes it from our
// known peer list if necessary.
protected int check_and_delete_peer(struct host_s peer)
{
  if (peer->last_contact + peer->ncttl < time())
  {
    m_delete(peers, peer->name);
  }
}

void reset()
{
  set_next_reset(600);
  // housekeeping of peer list every 60 resets (6h).
  reset_counter = ++reset_counter % 60;

  // expire stale requests
  foreach(int id, struct request_s r : requests)
  {
    if (r->timeout < time())
    {
      // in case of timeouts we call the callback, but with no data
      funcall(r->callback, id, r->packet->data, 0);
      m_delete(requests, id);
    }
  }

  // peer housekeeping: increse reputation, expire peers
  // expire fragments in all peers
  foreach(string name, struct host_s peer : peers)
  {
    // should we do housekeeping of peer list?
    if (!reset_counter)
    {
      // first check if peer should be expired, because we had no contact for an
      // extended time.
      if (check_and_delete_peer(peer))
        continue;
      // check if the reputation of a peer can be auto-promoted
      peer->reputation += auto_promote_peer(peer);
      peer->ncttl = calc_ncttl(peer);
    }

    if (!mappingp(peer->received) || !sizeof(peer->received))
      continue;

    // check all incomplete fragments
    foreach(int id, struct packet_s pack : peer->received)
    {
      if (pack->timestamp + 120 < time())
        m_delete(peer->received, id);
      //TODO: send info to sender?
    }
  }
}

// *** Core services every implementation must have.

// we received a reply to one of our pings
protected void recv_ping_reply(int id, mapping request, mapping response)
{
  //TODO: what should we do?
}

// we received a ping request
protected void recv_ping(mapping data)
{
  send(data[NAME], ([ID: data[ID], REQUEST:REPLY,
                     DATA: self->name +" is alive!\n"]), 0);
}

// sends a ping request to a peer
protected void send_ping(string mud)
{
  send(mud, ([ REQUEST: PING ]), #'recv_ping_reply);
}

// We received a reply for one of our QUERY requests.
protected void recv_query_reply(int id, mapping request, mapping response)
{
  //TODO: what do we do with the information?
}

// send a query request asking for <prop>
public void send_query(string name, string prop)
{
  if (!stringp(name) || !stringp(prop))
    return;
  send(name, ([REQUEST: QUERY, DATA: prop,
               SENDER: getuid(previous_object()) ]), #'recv_query_reply);
}

// we received a reply request.
protected void recv_query(mapping data)
{
  mapping ret;
  switch(data[DATA])
  {
    case "commands":
      ret = ([DATA: implode(self->services, ":") ]);
      break;
    case "email":
      ret = ([DATA: EMAIL]);
      break;
    case "hosts":
      string tmp="";
      foreach(struct host_s p : peers)
      {
        tmp += p->name + ":" + p->ip + ":" + p->port
               + ":" + implode(p->services, ",") + ":" +
               implode(p->services, ",") + "\n";
      }
      ret = ([DATA: trim(tmp, TRIM_RIGHT, "\n") ]);
      break;
    case "inetd":
    case "version":
      ret = ([DATA: self->im_version ]);
      break;
    case "list":
      ret = ([DATA: "commands,email,hosts,inetd,version,mud_port" ]);
      break;
    case "mud_port":
      ret = ([DATA: query_mud_port() ]);
      break;
    case "time":
      ret = ([DATA: time()]);
      break;
    default:
      return; // Just ignore request for the time being.
  }
  ret[REQUEST] = REPLY;
  ret[RECIPIENT] = data[SENDER];
  ret[ID] = data[ID];
  ret["QUERY"] = data[DATA];  //TODO: this is not right, right?

  send(data[NAME], ret, 0);
}

// Called, when we receive a reply of OUR HELO request.
protected void recv_helo_reply(int id, mapping request, mapping response)
{
  struct host_s peer = peers[response[NAME]];
  // if we already have a key for the peer, we accept data and answer only if
  // the packet was correctly signed. Although the check should be redundant,
  // because then the packet should have been discarded earlier. But for this,
  // better safe, than sorry.
  if (peer->key
      && !(response[FLAGS] & FL_VALID_SIGNATURE))
    raise_error("Unexpected unsigned packed from "
                + peer->name + ".\n");

  mapping theirinfo = json_parse(response[DATA]);
  // yes - this is a way to update a key... Sending a HELO packet signed
  // with the old key
  peer->key = response["pkey"];
  if (response["mtu"] >= 1024)
    peer->mtu = min(response["mtu"], MAX_UDP_LENGTH);
  if (pointerp(data["services"]))
    peer->services = data["services"];
}

// we received a HELO packet, send back our info.
protected void recv_helo(mapping data)
{
  struct host_s peer = peers[data[NAME]];
  // if we already have a key for the peer, we accept data and answer only if
  // the packet was correctly signed. Although the check should be redundant,
  // because then the packet should have been discarded earlier. But for this,
  // better safe, than sorry.
  if (peer->key
      && !(data[FLAGS] & FL_VALID_SIGNATURE))
    raise_error("Unexpected unsigned packed from "
                + peer->name + ".\n");

  mapping theirinfo = json_parse(data[DATA]);
  // yes - this is a way to update a key... Sending a HELO packet signed
  // with the old key
  peer->key = theirinfo["pkey"];
  if (theirinfo["mtu"] >= 1024)
    peer->mtu = min(theirinfo["mtu"], MAX_UDP_LENGTH);
  if (pointerp(theirinfo["services"]))
    peer->services = theirinfo["services"];

  mapping ourinfo = (["mtu": self->mtu, "pkey": self->key,
                      "services": self->services ]);
  send(data[NAME], ([ID: data[ID], REQUEST:REPLY,
                     DATA: json_serialize(ourinfo) ]), 0 );
}

// send a HELO packet.
protected void send_helo(string name)
{
  mapping ourinfo = (["mtu": self->mtu, "pkey": self->key,
                      "services": self->services]);
  send(name, ([REQUEST:HELO,
                     DATA: json_serialize(ourinfo) ]), #'recv_helo_reply );
}

// We received some reply for one of our requests - find our initial request
// and forward the data to the callback.
protected void recv_reply(mapping data)
{
  // validate_request ensured that we have the initial request.
  int pid = data[ID];
  debug_msg(sprintf("Received answer for request %d\n",pid),90);
  struct request_s req = requests[pid];
  m_delete(requests, pid);
  funcall(req->callback, pid, req->packet->data, data);
}

/*
 * Make a PING request to all muds in the "hosts" mapping to set
 * HOSTLIST_STATUS information.
 * But don't ping all muds at once, because that may overflow the callout
 * table during mud startup, when hundreds of objects make callouts.
 */
void ping_many_muds(string *muds)
{
  if (!pointerp(muds))
    muds=m_indices(peers);
  if (!sizeof(muds))
    return;
  string *part;
  if (sizeof(muds) > 9)
    part=muds[0..9];
  else
    part=muds;
  foreach(string mud: part)
    send_ping(mud);
  muds -= part;
  if (sizeof(muds))
    call_out(#'ping_many_muds, 4, muds);
}

protected void create()
{
  if (object_name(this_object()) == __FILE__[0..<3])
  {
    set_next_reset(-1);
    return;
  }
  set_user();
  restore_me();
  init();
}

protected void create_super()
{
  set_next_reset(-1);
}

int remove(int silent)
{
  reset();
  save_me();
  destruct(this_object());
  return 1;
}

