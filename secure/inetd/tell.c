#pragma strict_types, save_types, rtt_checks, pedantic
#pragma no_clone, no_shadow, no_inherit

inherit __DIR__"i_service";

#include <intermud.h>
#include <regexp.h>
#include <living/comm.h>
#include <defines.h>

// we received a reply to one of our messages.
// Note: if response is 0, the peer did not respond (timeout)...
protected void recv_tell_reply(int id, mapping request, mapping response)
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
                         MT_COMM|MT_FAR, MA_TELL, 0, ME);
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
                         MT_NOTIFICATION, MA_TELL, Name(0), ME);
    }
  }
}

// we received an intermud tell message
protected void recv_tell(mapping data)
{
  object ob;
  string msg;

  if (data[RECIPIENT] &&
       (ob = find_player(lower_case(data[RECIPIENT]))) &&
       interactive(ob))
  {
    if (!stringp(data[SENDER]) || !sizeof(data[SENDER]))
        data[SENDER]="<Unknown>";

    currentname = data[SENDER] + +"@"+data[NAME];

    if (!stringp(data[DATA]) || !sizeof(data[DATA]))
        msg="<Nichts>";
    else
        msg= regreplace(data[DATA],"[:^print:]|\n","",1);

    message_prefix=capitalize(data[SENDER])+"@"+data[NAME]+
                   " teilt Dir mit: ";

    int ret = ob->ReceiveMsg(msg, MT_COMM|MT_FAR,
                             MA_TELL, message_prefix, ME);
    currentname = 0;

    // Wenn Empfaenger unsichtbar ist, bekommt der Absender immer die gleiche
    // Meldung, egal was das Ergebnis von ReceiveMsg() ist.
    if (ob->QueryProp(P_INVIS))
    {
      msg ="\nRoot@"MUDNAME": Spieler "+
                        capitalize(data[RECIPIENT])+
                        " finde ich in "MUDNAME" nicht!\n";
    }
    // Sonst kriegt er eine sinnvolle Angabe.
    else
    {
      switch(ret)
      {
        case MSG_DELIVERED:
          msg = break_string(data[DATA], 78,
                             "Du teilst "+capitalize(data[RECIPIENT]) + "@"
                             LOCAL_NAME + " mit: ");
          if(ob->QueryProp(P_AWAY))
            msg=sprintf("%s%s@"MUDNAME" ist gerade nicht da: %s\n",
                       msg, ob->name(WER),ob->QueryProp(P_AWAY));
          else if ((i=query_idle(ob))>=600)
          { // 10 Mins
            string away;
            if (i<3600)
                away=time2string("%m %M",i);
            else
                away=time2string("%h %H und %m %M",i);
            msg = sprintf("%s%s@"MUDNAME" ist seit %s voellig untaetig.\n",
                          msg, ob->Name(WER), away);
          }
          break;
        case MSG_BUFFERED:
          msg= ob->Name(WER) + " moechte gerade nicht gestoert werden."
              "Die Mitteilung wurde von einem kleinen Kobold in Empfang "
              "genommen. Er wird sie spaeter weiterleiten!";
          break;
        case MSG_IGNORED:
        case MSG_VERB_IGN:
        case MSG_MUD_IGN:
          msg = ob->Name(WER) + " hoert gar nicht zu, was Du sagst.";
          break;
        case MSG_SENSE_BLOCK:
          msg = ob->Name(WER) + " kann Dich leider nicht wahrnehmen.";
              break;
        case MSG_BUFFER_FULL:
          msg = ob->Name(WER) + " moechte gerade nicht gestoert werden."
              "Die Mitteilung ging verloren, denn der Kobold kann sich "
              "nichts mehr merken!";
            break;
        default:
          msg = ob->Name(WER) + " hat Deine Nachricht leider nicht "
              "mitbekommen.";
          break;
      }
    }
  }
  // Spieler nicht gefunden.
  else
      msg ="\nRoot@"MUDNAME": Spieler "+
                        capitalize(data[RECIPIENT])+
                        " finde ich in "MUDNAME" nicht!\n";

  INETD->send(data[NAME],
              ([REQUEST: REPLY, RECIPIENT: data[SENDER],
                ID: data[ID],
                DATA: msg]), 0);

}

public int send(string name, string mud, string msg)
{
  return INETD->send(mud,
                     ([REQUEST: "tell", RECIPIENT: name,
                       SENDER: getuid(previous_object()),
                       DATA: msg
                       ]), #'recv_tell_reply);
}

protected void create()
{
  INETD->register_service("tell", #'recv_tell);
}

public varargs int remove(int silent)
{
  INETD->unregister_service("tell");
  destruct(this_object());
}


