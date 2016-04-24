#pragma strict_types, save_types, rtt_checks, pedantic
#pragma no_clone, no_shadow, no_inherit

inherit __DIR__"i_service";

#include <intermud.h>
#include <regexp.h>
#include <living/comm.h>
#include <defines.h>
#include <daemon/mand.h>

#define INETD_MANPAGE_FOUND "[ManD@%s] Folgende Seite wurde gefunden: %s\n"
//                              mud                                 file
#define INETD_NO_MANPAGE    "[ManD@%s] Keine Hilfeseite gefunden fuer '%s'.\n"
//                              mud                                 page
#define INETD_MANPAGES      "[ManD@%s] Die folgenden Seiten passen:\n" \
                             "%'-'78.78s\n%s%'-'78.78s\n"
//                                mud  "" pagelist ""
#define INETD_MAN_REQUESTED "%s@%s: Abfrage abgeschickt.\n"
//                             page  mud#ifndef LOCAL_NAME

#define LOCAL_NAME "MorgenGrauen"

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
          ob->ReceiveMsg(regreplace(response[DATA],"[:^print:]|\n","",1),
                         MT_NOTIFICATION, "hilfe", 0, ME);
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
                         MT_NOTIFICATION, "hilfe", "remote man: ", ME);
    }
  }
}

// we received an intermud manpage request
protected void recv_request(mapping data)
{
  string manpage = data[DATA];
  string *tmp = explode(manpage,"/");
  if (sizeof(tmp)>1)
  {
    if (file_size(MAND_DOCDIR+manpage)>=0)
      tmp=({tmp[<1], manpage});
    else
      tmp=({});
  }
  else
    tmp=(string *)call_other(MAND,"locate",data[DATA],0);
  // Gilden ausfiltern
  mapping pages = ([]);
  int index=sizeof(tmp);
  while(index--)
  {
    if (tmp[1][0..1]!="g.") pages[tmp[index]]=tmp[index-1];
    index--;
  }
  string ret;
  switch(sizeof(pages))
  {
    case 0:
      ret=sprintf(INETD_NO_MANPAGE,LOCAL_NAME,manpage);
      break;
    case 1:
      tmp=m_indices(pages)[0];
      ret=sprintf(INETD_MANPAGE_FOUND,LOCAL_NAME,pages[tmp]);
      index=0;
      while(manpage=read_file(MAND_DOCDIR+tmp,index))
      {
        ret+=manpage;
        index+=MAX_READ_FILE_LEN;
      }
      break;
    default:
      ret=sprintf(INETD_MANPAGES,LOCAL_NAME,"",
                  break_string(implode(m_values(pages)," "),78),"");
      break;
  }
  INETD->send(data[NAME],
              ([REQUEST: REPLY, RECIPIENT: data[SENDER], ID: data[ID],
                DATA: ret
                ]),
              0);
}

// Old interface. Request remote man page via intermud
public string send_request(string mudname, string pagename)
{
  if (request(mudname, pagename))
      return sprintf(INETD_MAN_REQUESTED,pagename,mudname);

  return "Fehler beim Anfordern.\n";
}


