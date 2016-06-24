// MorgenGrauen MUDlib
//
// www.c -- WWW Guest Client
//
// $Id: www.c 8755 2014-04-26 13:13:40Z Zesstra $

#pragma strong_types
#pragma combine_strings

#ifdef MORGENGRAUEN
# include <properties.h>
#endif

#include <udp.h>
#include <www.h>

#undef DEBUG 

private mixed pending; // pending udp requests

// HTMLunescape -- try to resolve %<hex> escape codes from HTML/WWW
private string HTMLunescape(string char)
{
  int len;
  if(!char || !(len = sizeof(char))) return "";
  if(char[0] == '%' && len = 3) {
    int val, ch;
    while(--len) {
      switch(char[len]) {
      case '0'..'9':
        val = (int)char[len..len];
        break;
      case 'A'..'F':
        val = char[len]-55;
        break;
      }
      if(len < 2) val <<= 4;
      ch += val;
    }
    return sprintf("%c", ch);
  }
  return char;
}

private string translate(string str)
{
  return implode(map(regexplode(str, "[%].."), #'HTMLunescape/*'*/), "");
}

// decode() -- decode the input cmds string
private mapping decode(string input)
{
  mixed tmp; int i;
  mapping cmds;
  cmds = ([]);
  i = sizeof(tmp = old_explode(translate(input), "&"));
  while(i--)
  {
    if(sizeof(tmp[i] = old_explode(tmp[i], "=")) == 2)
      cmds[tmp[i][0]] = tmp[i][1];
  }
  return cmds;
}

// put() -- put together a key and a value
private string put(string key, mapping val)
{
  return key+"="+val[key];
}
// encode() -- encode the input cmds string
private string encode(mapping input)
{ return implode(map(m_indices(input), #'put/*'*/, input), "&"); }

void Send(mapping data, string text, mixed back)
{
  if(strstr((string)previous_object(), UDPPATH+"/")) return;
  if(!data && !pending) return;
  else if(!data && pending)
  {
    data = pending[0]; back = pending[1]; pending = 0;
  }
  INETD->_send_udp(data[NAME], ([
      REQUEST: REPLY,
      RECIPIENT: data[SENDER],
      ID: data[ID],
      "URL":data[DATA],
      DATA: "\n\n"+text+"\n"
      ]));
}

private string exch(string str, mapping to)
{
  if(!to[str]) return str;
  return to[str];
}

private string xcode(string str, mapping to)
{
  return implode(map(regexplode(str, implode(m_indices(to), "|")),
                           #'exch/*'*/, to), "");
}

void udp_www(mapping data)
{
  string text, error;
  string back; int size;
  mapping cmds;
  error = catch(size = sizeof(cmds = decode(data[DATA])));
  if(cmds[BACK]) back = xcode(cmds[BACK], (["XampX":"&", "XeqX":"="]));
  cmds[BACK] = xcode(encode(cmds-([BACK])), (["&":"XampX", "=":"XeqX"]));
  if(error ||
     error=catch(text=("/"+object_name(this_object())+"."+cmds[REQ])->Request(cmds)))
  {
#ifdef DEBUG
    text = "<H1>Fehler: "+error+"</H1><HR><H3>Kontext:</H3>"
   + "<PRE>"+sprintf("%O", data)+"</PRE>";
#else
    text = "<H1>Fehler: Anfrage konnte nicht bearbeitet werden!</H1>";
#endif
    log_file(WWWLOG, "ERROR: "+error[0..<2]+", DATA FOLLOWS:\n");
  }
  log_file(WWWLOG, sprintf("[%s] %s\n", ctime(time())[4..15], data[DATA]));
  if(cmds[REQ] == R_INTERMUD && !text)
  {
    pending = ({data, back});
    return 0;
  }
  pending = 0;
  funcall(#'Send, data, text, back);
}
