// MorgenGrauen MUDlib
//
// more.c -- more files
//
// $Id: more.c 9142 2015-02-04 22:17:29Z Zesstra $

#pragma strict_types
#pragma save_types
#pragma no_clone
#pragma pedantic
#pragma range_check

inherit "/std/util/pager";

#include <pager.h>

#ifdef GERMAN
# define MSG_WRAPPED	" VOM ANFANG"
# define MSG_NOT_FOUND	" NICHTS GEFUNDEN"
# define MSG_NO_REGX	" FEHLENDER REG. AUSDRUCK"
# define MSG_ILLEGAL	" ILLEGALE ZEILENZAHL"
# define MSG_HELP       "\
Hilfe fuer more:\n\
\n\
b,B        -- Eine Seite zurueck.\n\
u,U        -- Eine halbe Seite zurueck.\n\
f,F        -- Eine Seite vorwaerts.\n\
d,D        -- Eine halbe Seite vorwaerts.\n\
<Zeile>    -- Springe zu Zeile <Zeile>\n\
q,x        -- Verlassen von more.\n\
/<RegExp>  -- Nach dem regulaeren Ausdruck <RegExp> suchen.\n"
#else
# define MSG_WRAPPED	" WRAPPED"
# define MSG_NOT_FOUND	" NOTHING FOUND"
# define MSG_NO_REGX	" NO PREV REGULAR EXPR"
# define MSG_ILLEGAL	" ILLEGAL LINE NUMBER"
# define MSG_HELP       "\
Help for more:\n\
\n\
b,B        -- One page back.\n\
u,U        -- Half a page back.\n\
f,F        -- One page forward.\n\
d,D        -- Half a page forward.\n\
<line>     -- Jump to line number <line>\n\
q,x        -- Quit or eXit more.\n\
/<regexp>  -- Search for the regular expression <regexp>.\n"
#endif

private nosave string cprompt = "";

string prompt(mixed pinfo, string add)
{
  int line,max; max = 1;
  if(pointerp(pinfo))
  {
     if(pinfo[CURL] + pinfo[PAGE] >= pinfo[MAXL]) line = pinfo[MAXL];
     else line = pinfo[CURL] + pinfo[PAGE];
     max = pinfo[MAXL];
  }
  if (pinfo[FLAG] & E_ABS)
    return sprintf("%s (%d/%d)%s %s",
		   ::prompt(pinfo, ""),
		   line-1, pinfo[MAXL],
		   cprompt, stringp(add)?add:"");
  else
    return sprintf("%s(%d%%)%s %s",
		   ::prompt(pinfo, ""),
		   line*100/max, 
		   cprompt, stringp(add)?add:"");
}

int search(mixed pinfo)
{
  int l, lines;
  mixed tmp;
  l = pinfo[CURL];
  while((tmp = fread(pinfo, l, pinfo[PAGE])) &&
        !sizeof(regexp(old_explode(tmp, "\n"), pinfo[REGX])))
    l += pinfo[PAGE];
  if(!tmp)
  {
    cprompt += MSG_WRAPPED;
    l = 1;
    while(l < pinfo[CURL] &&
          (tmp = fread(pinfo, l, pinfo[PAGE])) &&
          !sizeof(regexp(old_explode(tmp, "\n"), pinfo[REGX])))
      l += pinfo[PAGE];
    if(l >= pinfo[CURL]) return 0;
  }
  return l;
}

varargs int eval_command(mixed in, mixed pinfo)
{
  cprompt = "";
  if(stringp(in))
    switch(in)
    {
    case "?":
      write(MSG_HELP+"\n");
      return 0; 
    case "b":
    case "B":
      pinfo[CURL] -= pinfo[PAGE];
      break;
    case "u":
    case "U":
      pinfo[CURL] -= pinfo[PAGE] / 2;
      break;
    case "f":
    case "F":
      pinfo[CURL] += pinfo[PAGE];
      break;
    case "d":
    case "D":
      pinfo[CURL] += pinfo[PAGE] / 2;
      break;
    default:
    {
      int l;
      if(l = to_int(in))
      {
        if(l > pinfo[MAXL] || l < 1) return (cprompt = MSG_ILLEGAL, 0);
        pinfo[CURL] = l;
        break;
      }
      if(sizeof(in) && in[0] == '/')
      {
	if(sizeof(in) == 1)
	{
	  if(!pinfo[REGX]) return (cprompt = MSG_NO_REGX, 0);
	}
	else pinfo[REGX] = in[1..];
	if(l = search(pinfo)) pinfo[CURL] = l;
	else return (cprompt = MSG_NOT_FOUND, 0);
      }
      else return ::eval_command(in, pinfo);    
      break;
    }
    }
  else return ::eval_command(in, pinfo);
  return ::eval_command(-1, pinfo);
}   
