// MorgenGrauen MUDlib
//
// reply.c
//
// $Id: reply.c,v 1.2 2003/04/08 09:28:17 Rikus Exp $

#include <udp.h>

#ifndef DATE
#define DATE	ctime()[4..15]
#endif

void udp_reply(mapping data)
{
  string err,recpt,serv;
  object ob;
  
  
  if (pointerp(data[SYSTEM])&&member(data[SYSTEM],TIME_OUT)>-1)
  {
    if (data[SENDER])
    {
      if (stringp(data[SENDER])&&(ob=find_player(data[SENDER])))
      {
        switch(data[REQUEST])
        {
          case "tell": serv="teile mit: ";break;
          case "who":  serv="wer: ";break;
          default: serv=data[REQUEST]+": "; break;
        }
        tell_object(ob, break_string("Das Mud \'" + data[NAME] +
                                     "\' konnte nicht erreicht werden.\n",
                                     78,serv));
      }
      else
        if (objectp(ob = data[SENDER])||(ob = find_object(data[SENDER])))
          ob->udp_reply(data);
    }
    return;
  }
  if (data[RECIPIENT])
  {
    if (stringp(data[RECIPIENT])&&(ob = find_player(data[RECIPIENT])))
      tell_object(ob, "\n" + data[DATA]);
    else if (ob=find_object(data[RECIPIENT]))
      ob->udp_reply(data);
    return;
  }
  if (data[REQUEST]=="ping")return;
  if (data[DATA])
    log_file(INETD_LOG_FILE, DATE + ": Antwort von " + data[NAME] +
             "\n" + data[DATA] + "\n");
  return;
}
