// MorgenGrauen MUDlib
//
// www.who.c
//
// $Id: www.who.c 8755 2014-04-26 13:13:40Z Zesstra $

#pragma strong_types
#pragma combine_strings

#include <config.h>
#include <properties.h>
#include <www.h>

string MakeLink(<object|string>* entry)
{
  string nm;
  int idx;

  entry[1] = regreplace(entry[1], "<", "\\&lt;", 1);
  entry[1] = regreplace(entry[1], ">", "\\&gt;", 1);
  nm = getuid(entry[0]);
  if(nm == " R O O T ") return "<TT>"+entry[1][0..2]+"</TT>"+entry[1][3..];
  idx = strstr(lower_case(entry[1]), nm);
  return "<TT>"+entry[1][0..2]+"</TT>"+entry[1][3..idx-1]
       + "<A HREF=\""+MUDWWW+"?"+REQ+"="+R_FINGER+"&"+USER+"="+nm+"\"><B>"
       + entry[1][idx..idx = idx+sizeof(nm)]
       + "</B></A>"
       + entry[1][idx+1..];
}

string Request(mapping cmds)
{
  if (!sizeof(cmds))
    return ERROR("Anfrage ung&uuml;ltig!");
  // Wer-Liste abfragen
  <object|string>** list = WHO;
  int size=sizeof(list);
  string *who = allocate(size);

  // Listenreihenfolge umkehren.
  foreach(int i : size) {
    who[i] = MakeLink(list[<i+1]);
  }

  return "<H2>Wer ist gerade in "MUDNAME"?</H2><HR>"
       + "<OL><LI>"+implode(who, "\n<LI>")+"</OL>";
}
