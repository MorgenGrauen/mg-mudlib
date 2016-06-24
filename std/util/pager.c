// MorgenGrauen MUDlib
//
// util/pager.c -- generic pager
//
// $Id: pager.c 8755 2014-04-26 13:13:40Z Zesstra $

#pragma strong_types
#pragma save_types
#pragma pedantic
#pragma range_check
#pragma no_clone

inherit "/std/util/input";

#include <pager.h>
#include <sys_debug.h>
#include <wizlevels.h>

string prompt(mixed pinfo, string add)
{
  return sprintf(MSG_PROMPT+"%s", stringp(add)?add:"");
}

string fread(mixed pinfo, int begin, int c)
{
  if(begin > pinfo[MAXL] || begin < 1) return 0;
  if(pinfo[FILE]) return read_file(pinfo[TEXT], begin, c);
  else
  {
    int start, end, l, x;

    if(member(pinfo[JUNK], begin)) 
    {
      start = pinfo[JUNK][begin];
      if(!(end = pinfo[JUNK][begin+c]))
        end = sizeof(pinfo[TEXT]);
      else end--;
      return pinfo[TEXT][start..end];
    }
    return 0;
  }
}

mixed call(closure ctrl, mixed ctrlargs)
{
  if (!ctrl||!closurep(ctrl)) return 0;
  if(!pointerp(ctrlargs)) return funcall(ctrl);
  return apply(ctrl, ctrlargs);
}

 
varargs int eval_command(mixed in, mixed pinfo)
{
  string cprompt, tmp;
  cprompt = "";
  if(in == "q" || in == "x")
  {
    if(closurep(pinfo[CTRL])) apply(pinfo[CTRL], pinfo[CARG]);
    return 1;
  }
  else
    if(!in || (stringp(in) && !sizeof(in))) pinfo[CURL] += pinfo[PAGE];
    else if(in != -1) return 0;

  if(pinfo[CURL] >= pinfo[MAXL]) 
    return (call(pinfo[CTRL], pinfo[CARG]), 1); 
  if(pinfo[CURL] <= 0) pinfo[CURL] = 1;
  if(pinfo[CURL] == 1) cprompt = MSG_TOP;
  if(pinfo[CURL] + pinfo[PAGE] >= pinfo[MAXL]) cprompt = MSG_BOTTOM;
  if(!tmp = fread(pinfo, pinfo[CURL], pinfo[PAGE]))
    return 0; /* (write(MSG_OOPS+"\n"), 0); */
  if (query_once_interactive(this_object()))
     tell_object(this_object(), tmp);
  else write(tmp);
  if(!(pinfo[FLAG] & E_PROMPT) &&
     (pinfo[CURL] + pinfo[PAGE] >= pinfo[MAXL] || !pinfo[PAGE]))
    return (call(pinfo[CTRL], pinfo[CARG]), 1);
  if(!(pinfo[FLAG] & E_CAT) &&
     (pinfo[MAXL] > pinfo[PAGE] || (pinfo[FLAG] & E_PROMPT)))
    input(#'prompt, ({ pinfo, cprompt }), #'eval_command, ({ pinfo }));
  return 1;
}

varargs public void More(string txt, int file,
			 mixed ctrl, mixed ctrlargs,
			 int flags)
{
  int j;
  mixed tmp, pinfo;
  if(!txt) return call(ctrl, ctrlargs);
  //         TEXT, FILE, CURL, MAXL, REGX, PAGE, CTRL, CARG
  pinfo = ({ txt, file, 1, 1, 20, 0, ctrl, ctrlargs, flags });

  if (pinfo[FILE])
  { 
     if (file_size(pinfo[TEXT])<50000)
     {
       pinfo[TEXT]=read_file(pinfo[TEXT]);
       if (!pinfo[TEXT])
       { 
         call(ctrl, ctrlargs);
         return;
       }  
       pinfo[FILE]=0;
     }
     else if (file_size(pinfo[TEXT])>1048576)
     {
	tell_object(this_player()||this_object(),break_string(sprintf(
               "Die Datei '%s' kann nicht angezeigt werden, da " 
               "sie groesser als 1 MB ist. %s",pinfo[TEXT],
               IS_LEARNER(this_player()||this_object())?"":"Bitte verstaendige "
               "diesbezueglich umgehend einen Magier."),78));
	call(ctrl,ctrlargs);
        return;
     }
  }

  if(!pinfo[FILE] && pinfo[TEXT][<1..<1] != "\n") pinfo[TEXT] += "\n";
  pinfo += ({ ([1:0]) });
  if(!pinfo[FILE]) 
    while((j = strstr(pinfo[TEXT], "\n", j+1)) != -1)
  	pinfo[JUNK][++pinfo[MAXL]] = j+1;
  else
    while(tmp = read_file(pinfo[TEXT], pinfo[MAXL], MAX_LINE_READ))
      pinfo[MAXL] += sizeof(explode(tmp, "\n"))+1;
  pinfo[PAGE] = PAGELENGTH;
  if(!pinfo[PAGE]) pinfo[FLAG] |= E_CAT;
  if ((this_interactive() && (j=(int)this_interactive()->QueryProp(P_MORE_FLAGS))) ||
      (this_player() && interactive(this_player()) &&
       (j=(int)this_player()->QueryProp(P_MORE_FLAGS))))
    pinfo[FLAG] |= j;
  
  pinfo[CURL] = 1;
  eval_command(-1, pinfo);
  return;
}






