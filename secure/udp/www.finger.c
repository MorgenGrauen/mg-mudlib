// MorgenGrauen MUDlib
//
// www.finger.c
//
// $Id: www.finger.c 8755 2014-04-26 13:13:40Z Zesstra $

#pragma strong_types
#pragma combine_strings

#include <properties.h>
#include <www.h>
#include <regexp.h>

string Request(mapping cmds)
{
  if(!sizeof(cmds) || !stringp(cmds[USER]))
    return ERROR("Kein Nutzer angegeben!");
  /*
   * Kann ja sein, dass ein Spieler auf die Idee kommt, HTML-Tags
   * in seine Beschreibung einzubauen. Unsere Seite ist aber schon
   * interaktiv genug. (Anm: Nur <.*>-Vorkommnisse zu ersetzen nutzt
   * nix, da man auch mit einzelnen Zeichen Schaden machen kann.
   */
  string result = regreplace(FINGER("-a "+cmds[USER]), "<","\\&lt;",1); 
  result = regreplace(result, ">","\\&gt;",1);
  string *reslines = explode(result,"\n");
  /*
   * Grund des kommenden Codeblocks ist , dass manche Spieler ihre
   * Homepage mit "http://mg.mud.de" angeben, andere nur"mg.mud.de" 
   * schreiben. Damit aber der Browser den Link als absolut interpretiert, 
   * muss das http:// davor stehen, und zwar nur einmal. 
   */
  string *tmp = regexp(reslines,"^Homepage:");
  if (sizeof(tmp)&&stringp(tmp[0])&&sizeof(tmp[0])>16) {
	  string tmp2;
    string quoted = regreplace(tmp[0],"([[\\]+*?.\\\\])","\\\\\\1", 1);
    if (tmp[0][10..16]=="http://")
      tmp2=sprintf("Homepage: <A HREF=\"%s\">%s</A>",
		      tmp[0][10..],tmp[0][10..]);
    else
      tmp2=sprintf("Homepage: <A HREF=\"http://%s\">%s</A>",
		      tmp[0][10..],tmp[0][10..]);
    result = regreplace(result,quoted,tmp2,1);
  }
  tmp = regexp(reslines,"^Avatar-URI:");
  if (sizeof(tmp)) {
     result = regreplace(result,
             "Avatar-URI: ([^\n]*)",
             "Avatar-URI: <a href=\\1>\\1</a>",1); 
    if (sizeof(regexp(({tmp[0]}),"http[s]{0,1}://",RE_PCRE))) {
      string uri = regreplace(tmp[0], "Avatar-URI: ([^\n]*)", "\\1",1);
      result = "<img src=\""+uri+"\" height=150 alt=\"" + capitalize(cmds[USER])
               + "\" /><br>"
               + result;
    }
  }

  result = regreplace(result,
		      "E-Mail-Adresse: ([^\n]*)",
		      "E-Mail-Adresse: Bitte nachfragen...",1);

  result = regreplace(result,
		      "Messenger: ([^\n]*)",
		      "Messenger: Bitte nachfragen...", 1);

  result = regreplace(result,
		      "ICQ: ([^\n]*)",
		      "ICQ: Bitte nachfragen...", 1);

  return "<H2>Wer ist "+capitalize(cmds[USER])+"?</H2><HR>"
    +"<PRE>"+result+"</PRE>";
}
