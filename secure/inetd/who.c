#pragma strict_types, save_types, rtt_checks, pedantic
#pragma no_clone, no_shadow, no_inherit

inherit __DIR__"i_service";

#include <intermud.h>
#include <config.h>

int maxtoday, maxever;

public string service_name()
{
  return "who";
}

public void reset()
{
  string tmp1=read_file("/etc/maxusers.ever",0,1);
  string tmp2=read_file("/etc/maxusers",0,1);
  if (stringp(tmp1)&&sizeof(tmp1)) sscanf(tmp1,"%d %~s",maxever);
  if (stringp(tmp2)&&sizeof(tmp2)) sscanf(tmp2,"%d %~s",maxtoday);
}

protected void create()
{
  ::create();
  reset();
}

string adjust(string str,int wid) {
  return sprintf("%*|s",wid,str);
}

// we received an intermud who request
protected void recv_request(mapping data)
{
  string *lines="/obj/werliste"->QueryWhoListe(0, 1);
  string wholiste=implode(lines,"\n");

  lines=({
    "*------------------------------------------------------------------------*",
    "","","","",
    "*------------------------------------------------------------------------*"
  });
  string header=MUDNAME", LDMud "+__VERSION__;
  lines[1]="|"+adjust(header,sizeof(lines[0])-2)+"|";
  header="Adresse: MG.Mud.DE (87.79.24.60) 23 (alternativ 4711)";
  lines[2]="|"+adjust(header,sizeof(lines[0])-2)+"|";
  header="Uptime: "+uptime();
  lines[3]="|"+adjust(header,sizeof(lines[0])-2)+"|";
  header=_MUDLIB_NAME_"-Mudlib "_MUDLIB_VERSION_;
  lines[4]="|"+adjust(header,sizeof(lines[0])-2)+"|";
  header=implode(lines,"\n");

  wholiste=header+"\n"+wholiste+
    sprintf("\n*** Anwesende im "MUDNAME": Max. heute %d, Rekord %d\n",
        maxtoday,maxever);

  INETD->send(data[NAME],
              ([REQUEST: REPLY, ID: data[ID],
                RECIPIENT: data[SENDER],
                DATA: wholiste
                ]), 0);
}

//TODO: request() ?
//public int request(string mudname, string|int data)

