#pragma strict_types, save_types, rtt_checks, pedantic
#pragma no_clone, no_shadow, no_inherit

// inherit the basic inetd
inherit __DIR__"inetd_base.c";

#include <living/comm.h>
#include <driver_info.h>

// and implement mud-individual stuff that should be implemented as needed. If
// the default is fine for your mud, keep it.
protected void set_user()
{
  seteuid(getuid());
}

protected void save_me()
{
//  save_object(__DIR__"ARCH/inetd");
}

protected int restore_me()
{
//  return restore_object(__DIR__"ARCH/inetd");
}

protected void export_peer_list(mapping list)
{
}

protected void debug_msg(string msg, int severity)
{
  object z=find_player("zesstra");
  if (z)
    z->ReceiveMsg(msg, MT_DEBUG, 0, "IM: ", this_object());
}

public void receive(string host, string msg, int hostport)
{
  if (previous_object() == master())
    process_fragment(host, msg, hostport);
}

protected void create()
{
  configure_host((<host_s> name: "MG-Test-"+__HOST_NAME__,
                  ip: __HOST_IP_NUMBER__,
                  port: driver_info(DI_UDP_PORT),
                  mtu: 1024,
        ));
  ::create();
}

