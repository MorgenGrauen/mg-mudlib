#pragma strict_types, save_types, rtt_checks, pedantic
#pragma no_clone, no_shadow, no_inherit

#include <intermud.h>
#include <regexp.h>
#include <living/comm.h>
#include <defines.h>
#include <daemon/mand.h>

protected nosave string currentname = "INETD-service";

public string service_name()
{
  return "undefined";
}

public string name(int egal)
{
  return currentname || "INETD-"+service_name();
}

public string Name(int egal)
{
  return capitalize(name(egal));
}

// we received a reply to one of our requests.
// Note: if response is 0, the peer did not respond (timeout)...
protected void recv_reply(int id, mapping request, mapping response)
{
  if(response)
  {
    if (stringp(response[RECIPIENT]))
    {
      object ob = find_player(response[RECIPIENT])
                  || find_object(response[RECIPIENT]);
      if (ob)
      {
          currentname = capitalize(data[SENDER])+"@"+capitalize(data[NAME]);
          ob->ReceiveMsg(regreplace(response[DATA],"[:^print:]|\n","",1),
                         MT_NOTIFICATION, service_name(), 0, ME);
          currentname = 0;
    }
  }
  else
  {
    // no response, timeout...
    if (request[SENDER] && stringp(request[SENDER]))
    {
      object ob = find_player(request[SENDER])
                  || find_object(request[SENDER]);
      if (ob)
          ob->ReceiveMsg("Das Mud \'" + request[NAME] +
                         "\' konnte nicht erreicht werden.\n",
                         MT_NOTIFICATION, 0, service_name(), ME);
    }
  }
}

// we received an intermud request
protected void recv_request(mapping data)
{
  // implement it!
  raise_error("recv_request() has to be implemented!\n");
}

// send request via intermud
public int request(string mudname, string|int data)
{
  return INETD->send(mudname, ([REQUEST: servicename(),
                                DATA: data,
                                SENDER: getuid(previous_object())]),
                  #'recv_reply) > 0;
}

protected void create()
{
  INETD->register_service(service_name(), #'recv_request);
  currentname = "INETD-"+service_name();
}

public varargs int remove(int silent)
{
  INETD->unregister_service(service_name());
  destruct(this_object());
}


