#pragma strict_types, save_types, rtt_checks, pedantic
#pragma no_clone, no_shadow, no_inherit

inherit __DIR__"i_service";

#include <intermud.h>
#include <regexp.h>

string last_finger;

public string service_name()
{
  return "finger";
}

string QueryLastFinger()
{
  return last_finger;
}

// we received an intermud channel message
protected void recv_request(mapping data)
{
  last_finger=capitalize(to_string(data[SENDER]))+"@"+data[NAME];
  INETD->send(data[NAME], ([
        REQUEST: REPLY,
        RECIPIENT: data[SENDER],
        ID: data[ID],
        DATA: "/p/daemon/finger"->finger_single(data[DATA])
    ]) );
}

public int finger_remote(string name, string mud)
{
  int ret = request(mud, name);
  if (ret > 0)
    write("Anfrage abgeschickt.\n");
  else
  {
    switch(ret)
    {
      default:
        write("Es trat ein Fehler beim Versenden der Anfrage auf.\n");
        break;
    }
  }
  return 1;
}

