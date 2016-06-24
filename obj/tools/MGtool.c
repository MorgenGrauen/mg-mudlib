/*
 *  MGtool-1.3
 *  File: MGtool.c
 *  Maintainer: Kirk@MorgenGrauen
 */

/*------------------------------------------*/
/* the original Xtool is copyrighted by Hyp */
/*------------------------------------------*/

#pragma strict_types

inherit "/std/thing/properties";

#include <properties.h>
#if !defined(QUERYCACHED)
#define QUERYCACHED 4096
#endif
#if !defined (SETCACHED)
#define SETCACHED 4096
#endif
#include <moving.h>
#include "/secure/wizlevels.h"
#include "/secure/config.h"
#include <userinfo.h>
#include <defines.h>

#include "MGtool/tool.h"

#include "MGtool/toollib.h"

#define XDBG 1

nosave object  cloner;
nosave object  msgto=NULL;
nosave string *manpath=({TOOL_PATH+"/man.d/",
			 "/doc/efun/",
			 "/doc/lfun/",
			 "/doc/w/",
			 "/doc/helpdir/",
			 "/doc/LPC/",
			 "/doc/std/",
			 "/doc/concepts/",
			 ""});
nosave string  morefile=NULL;
nosave string *scriptline=NULL;
nosave string *history=allocate(MAX_HISTORY);
nosave int     moreflag=FALSE;
nosave int     moreoffset=1;
nosave int     term=NULL;
nosave int     scriptsize=NULL;
nosave int     nostore=FALSE;
nosave int     xlight=0;
nosave int     pipe_in=FALSE;
nosave int     pipe_out=FALSE;
nosave int     pipe_ovr=TRUE;
nosave string  pipe_if=NULL;
nosave string  pipe_of=NULL;
nosave int     xtk=FALSE;
nosave mapping variable=([]);
nosave string *cmds;
private nosave mapping line_buffer=([]);
private nosave string line_buffer_name="";
private nosave string more_searchexpr="";
int    morelines=MORE_LINES;
int    modi=(MODE_FIRST|MODE_PROTECT|MODE_SHORT);

#include "MGtool/toollib.c"
#include "MGtool/toolcmd.c"

#define SafeReturn(x) \
{ \
  cmds=({}); \
  pipe_in=FALSE; \
  pipe_of=NULL; \
  pipe_ovr=TRUE; \
  pipe_out=FALSE; \
  pipe_of=NULL; \
  return (x); \
}

/*----------------------------------------------------------------------
 * check some security aspects
 */

static int security()
{
  object prev;
 
  if( process_call() || getuid()!=getuid(cloner) ) return FALSE; // Rumata

  TK("UID: "+getuid(ME)+" cloner:"+getuid(cloner));
//  TK("prev: "+object_name(PREV)+" me:"+object_name(ME));
  if(prev=PREV)
  {
    if(!cloner)
      return TRUE;
    if(getuid(prev)==ROOTID||IS_ARCH(prev))
      return TRUE;
    if(prev==ME)
      return TRUE;
    return secure_level()>=query_wiz_level(cloner); // Rumata
    //return getuid(prev)==getuid()&&geteuid(prev)==geteuid()&&cloner==RTP;
  }
  else
    return cloner==NULL;
}

/*----------------------------------------------------------------------
 * own write function
 */

static int Write(string str)
{
  if(!stringp(str) || str=="")
    return FALSE;
  if(!cloner && objectp(this_player()))
    write(str);
  else
    tell_object(cloner, str);
  return TRUE;
}

/*----------------------------------------------------------------------
 * own command function
 */

static int Command(string str)
{
  int i;
  TK("Command: str: "+(str?str:"(NULL)"));
  nostore++;
  if(MODE(MODE_ECHO))
    WLN("Doing: "+str);
  i=(int)cloner->command_me(str);
  nostore--;
  return i;
}

/*----------------------------------------------------------------------
 * object searching
 */

static varargs object XFindObj(string str, int silent)
{
  object obj, env;
  string *strs;
  int i, s, cnt;
  
  if(!str)
    return NULL;
  TK("XFindObj: str: "+(str?str:"(NULL)"));
  env=ENV(cloner);
  str=string_replace(str, "\\.","°01");
  str=string_replace(str, "\\^", "°02");
  str=string_replace(str, "\\$", "°03");
  str=string_replace(str, "\\\\", "\\");
  if (find_object(str)) return find_object(str);
  if (file_size(str)>1) {
     call_other(str, "???");
     return find_object(str);
  }
  s=sizeof(strs=strip_explode(str, "."));
  while(s--)
  {
    if(strs[i]=="m"||strs[i]=="me")
      strs[i]=RNAME(cloner);
    else if(strs[i]=="h"||strs[i]=="here")
      strs[i]=object_name(ENV(cloner));
    if(obj=FindObj(strs[i++],env,(silent?1:0)))
      env=obj;
    else
      break;
  }
  return obj;
}

static varargs object FindObj(string str, object env, int silent)
{
  object obj, *inv;
  string tmp;
  int num, e;
  
  if (!stringp(str) || !sizeof(str) || !objectp(env))
    return NULL;
  str=string_replace(str, "°01", ".");
  while(str[e++]=='^')
    ;
  str=str[--e..<1];
  str=string_replace(str, "°02", "^");
  if(obj=VarToObj(str))
    ;
  else if(str[0]=='')
    str=string_replace(str, "°03", "$");
  else if(sscanf(str, "%d", num)&&(inv=all_inventory(env)))
  {
    if(num>0&&num<=sizeof(inv))
      obj=inv[num-1];
    else
    {
      WDLN("Specified object number out of range [1-"+sizeof(inv)+"]");
      return NULL;
    }
  }
  if(obj||(obj=present(str, env))||
     (obj=find_player(LOWER(str)))||
     (obj=find_living(str))||
     (obj=find_object(long_path(str))))
  {
    while(e--)
    {
      if(!(obj=ENV(obj)))
      {
	W("Specified object has no environment [");
	while(e--)
	  W("^");
	WDLN(str+"]");
	return NULL;
      }
    }
  }
  else
    if(!silent)
      WDLN("Specified object does not exist ["+str+"]");
  return obj;
}

/*----------------------------------------------------------------------
 * object variable handling
 */

static object VarToObj(string str)
{
  if (!stringp(str) || !sizeof(str) || str[0]!='$')
    return NULL;
  switch(str)
  {
    case "$m":
    case "$me":
    return cloner;
    case "$h":
    case "$here":
    return ENV(cloner);
    default:
    return variable[str[1..<1]];
  }
  return(NULL); //never reached
}

static string VarToFile(string str)
{
  return source_file_name(VarToObj(str));
}

static string VarToPureFile(string str)
{
  return pure_file_name(VarToObj(str));
}

/*----------------------------------------------------------------------
 * object description printing
 */

static void PrintObj(object obj, string file)
{
  object item, tmp;
  string str;
  int i;

  SECURE1();
  if(!obj)
    return;
  PrintShort("Short: ", obj, file);
  if(!file||file=="")
    WLN("Long :");
  else
    write_file(file,"Long :\n");
  if(query_once_interactive(obj))
    str=capitalize(getuid(obj));
  else
  {
    if(object_name(obj)[0..26]=="/d/unterwelt/objekte/teddy#" &&
       IS_ARCH(this_interactive()))
      str="Ein Teddy (stumm)";
    else
    {
      if(str=(string)obj->QueryProp(P_INT_LONG))
	;
      else if(str=(string)obj->QueryProp(P_LONG))
	;
      else
	str="- no long description -\n";
    }
  }
  if(!file||file=="")
    W(str);
  else
    write_file(file,str);
  FORALL(item, obj)
  {
    if(!i)
      if(!file||file=="")
	WLN("Content:");
      else
	write_file(file,"Content:\n");
    PrintShort(ARIGHT(++i+". ", 4, " "), item, file);
  }
}

static string ObjFile(object obj)
{
  return "["+short_path(object_name(obj))+"]";
}

static varargs void PrintShort(string pre, object obj, string file)
{
  string str;

  SECURE1();
  if(!obj)
    return;
  if(MODE(MODE_SHORT))
  {
    if (query_once_interactive(obj))
      str=capitalize(getuid(obj));
    else
    {
      if(!((str=(string)obj->QueryProp(P_INT_SHORT))||
	   (str=(string)obj->QueryProp(P_SHORT))))
	if(is_player(obj))
	  str=CRNAME(obj)+" (invisible)";
	else
	  str="- no short description -";
    }
    str=ALEFT(sprintf("%O ",str), 34, ".")+" ";
  }
  else
    str="";
  if(interactive(obj))
    str+="i";
  else if(living(obj))
    str+="l";
  else
    str+=".";
  str+=(shadow(obj, 0) ? "s" : ".");
  if(!file||file=="")
    WLN((pre+CAP(str)+" "+ObjFile(obj))[0..79]);
  else
    write_file(file,(pre+CAP(str)+" "+ObjFile(obj))[0..79]+"\n");
}

static varargs void DeepPrintShort(object env, int indent, string pre, string file)
{
  int i;
  object item;
  string str;
  
  SECURE1();
  if(!env)
    return;
  for(i=indent,str=""; i--; str+="  ");
  if(pre)
    str+=pre;
  if(!file||file=="")
    W(str);
  else
    write_file(file,str);
  i++;
  PrintShort("",env,file);
  FORALL(item, env)
    DeepPrintShort(item,indent+1,ARIGHT((++i)+". ",4," "),file);
}

static string break_string_hard(string str, int len, string pre)
{
  int s,p,t;
  string tmp;

  if(!str||!(s=sizeof(str)))
    return "";
  t=len-(p=sizeof(pre))-1;

  tmp="";
  while(p+s>len)
  {
    tmp+=pre+str[0..t]+"\n";
	str=str[t+1..];
	s-=t;
  }
  if(sizeof(str))
    tmp+=pre+str[0..]+"\n";
  return tmp;
}
  
static void dprop(string key, mixed data, object obj)
{
  if(pipe_out&&pipe_of)
    write_file(pipe_of,break_string_hard(mixed_to_string(obj->QueryProp(key),MAX_RECURSION),78,ALEFT(key+" ",18,".")+" = "));
  else
    W(break_string_hard(mixed_to_string(obj->QueryProp(key),MAX_RECURSION),78,ALEFT(key+" ",18,".")+" = "));
}

static string propflags(string key, object ob)
{
  int tmp;
  string *flags;
  tmp=(int)ob->Query(key,1);
  flags=({});
  tmp&SAVE ?	       flags+=({"SAV"}) : flags+=({"   "});
  tmp&PROTECTED ?      flags+=({"PRO"}) : flags+=({"   "});
  tmp&SECURED ?        flags+=({"SEC"}) : flags+=({"   "});
  tmp&NOSETMETHOD ?    flags+=({"NSM"}) : flags+=({"   "});
  tmp&SETMNOTFOUND ?   flags+=({"SMN"}) : flags+=({"   "});
  tmp&QUERYMNOTFOUND ? flags+=({"QMN"}) : flags+=({"   "});
  tmp&QUERYCACHED ?    flags+=({"QCA"}) : flags+=({"   "});
  tmp&SETCACHED ?      flags+=({"SCA"}) : flags+=({"   "});
  return ""+implode(flags,"|");
}

static void dprop2(string key, mixed data, object ob)
{
  if(pipe_out&&pipe_of)
    write_file(pipe_of,break_string_hard(mixed_to_string(propflags(key,ob),MAX_RECURSION),78,ALEFT(key+" ",18,".")+" = "));
  else
    W(break_string_hard(mixed_to_string(propflags(key,ob),MAX_RECURSION),78,ALEFT(key+" ",18,".")+" = "));
}

static mixed propmethods(string key, object ob)
{
  return (mixed)ob->Query(key,2);
}

static void dprop3(string key, mixed data, object ob)
{
  if(pipe_out&&pipe_of)
    write_file(pipe_of,break_string_hard(mixed_to_string(propmethods(key,ob),MAX_RECURSION),78,ALEFT(key+" ",18,".")+" = "));
  else
    W(break_string_hard(mixed_to_string(propmethods(key,ob),MAX_RECURSION),78,ALEFT(key+" ",18,".")+" = "));
}

static void DumpProperties(object obj, int flag)
{
  SECURE1();
  if(!obj)
    return;
  PIPE_DELETE(pipe_of);
  switch (flag) {
    case 0:
      walk_mapping(((mapping *)(obj->__query_properties()))[0],#'dprop,obj); //')
      break;
    case 1:
      walk_mapping(((mapping *)(obj->__query_properties()))[0],#'dprop2,obj); //')
      break;
    case 2:
      walk_mapping(((mapping *)(obj->__query_properties()))[0],#'dprop3,obj); //')
      break;
  }
}

/*----------------------------------------------------------------------
 * moving objects
 */

static int MoveObj(object obj1, object obj2, int silent)
{
  int err;
  object oldenv;

  SECURE2(FALSE);
  if(!(obj1&&obj2))
    return FALSE;
  oldenv=ENV(obj1);
  err=(int)obj1->move(obj2, M_SILENT|M_NOCHECK);
  if(!silent)
    switch(err)
    {
      case ME_PLAYER:
      WDLN("Object returned ME_PLAYER");
      if(oldenv && oldenv != ENV(obj1))
	WDLN("Object has been moved");
      return FALSE;
      case ME_TOO_HEAVY:
      WDLN("Object returned ME_TOO_HEAVY");
      if(oldenv && oldenv != ENV(obj1))
	WDLN("Object has been moved");
      return FALSE;
      case ME_CANT_TPORT_IN:
      WDLN("Object returned ME_CANT_TPORT_IN");
      if(oldenv && oldenv != ENV(obj1))
	WDLN("Object has been moved");
      return FALSE;
      case ME_CANT_TPORT_OUT:
      WDLN("Object returned ME_CANT_TPORT_OUT");
      if(oldenv && oldenv != ENV(obj1))
	WDLN("Object has been moved");
      return FALSE;
      default:
      WDLN("Object returned unknown return value");
      return FALSE;
    }
  return TRUE;
}

/*----------------------------------------------------------------------
 * save destructing of objects
 */

static void Destruct(object obj)
{
  if(!obj || !this_object())
    return;
  catch(obj->remove());
  if(objectp(obj) && !query_once_interactive(obj))
    destruct(obj);
}

static void DeepClean(object obj)
{
  if(!obj)
    return;
  filter(filter(deep_inventory(obj), "is_not_player", ME),
	       "Destruct", ME);
  if(is_not_player(obj))
    Destruct(obj);
}

/*----------------------------------------------------------------------
 * Show the inheritance tree of objects
 */

static object *SubNodes(object obj)
{
  int s;
  object *objs;
  string *inlist;
  
  if(!obj)
    return NULL;
  inlist=inherit_list(obj);
  s=sizeof(inlist);
  objs=({});
  while(s-->1)
    objs=({find_object(inlist[s])})+objs;
  return objs;
}

static void Inheritance(object obj, string func, string pre)
{
  int i, s;
  object *ln;
  string str;
  
  if(!obj)
    return;
  str=pre+" "+ObjFile(obj);
  if(func)
  {
    str=ALEFT(str+" ", 50, ".");
    if(function_exists(func, obj)==object_name(obj))
      str+=ARIGHT(" "+func, 19, ".");
    else
      str+=ARIGHT("", 19, ".");
  }
  if(pipe_out&&pipe_of)
    write_file(pipe_of,str+"\n");
  else
    WLN(str);
  ln=SubNodes(obj);
  for(i=0; i<sizeof(ln); i++)
    ln=ln-SubNodes(ln[i]);
  s=sizeof(ln);
  for(i=0; i<s; i++)
    Inheritance(ln[i], func, pre+".....");
}

/*----------------------------------------------------------------------
 * file name handling
 */

static string XFile(string file)
{
  TK("XFile: file: "+(file?file:"(NULL)"));
  if(file)
    switch(file[0])
    {
      case '@':
	return source_file_name(XFindObj(file[1..<1]));
      case '$':
	return source_file_name(XFindObj(file));
      default:
	return old_explode(long_path(file),"#")[0];
    }
  return NULL;
}

static string XFindFile(string file)
{
  TK("XFindFile: file: "+(file?file:"(NULL)"));
  if(file=XFile(file))
  {
    if(file_size(file)>=0)
      return file;
    if(file[<3..<1]!=".c" && file_size(file+".c")>0)
      return file+".c";
  }
  WDLN("File not found or not readable ["+short_path(file)+"]");
  return NULL;
}

/*----------------------------------------------------------------------
 * file printing, searching and executing
 */

static void XMoreFile(string file, int flag)
{
  int s,size;
  
  SECURE1();
  if(!file)
    return;
  
  // term=(string)cloner->QueryProp(P_TTY)!="dumb";
  if((size=(file_size(morefile=long_path(file))))>0)
  {
    if(size>100000)
      WDLN("Warning: large file");
    MoreFile(NULL);
  }
  else if(flag)
    WDLN("Cannot read file");
}

/*
static string read_buffer(string filename, int start, int number)
{
  mixed tmp;
  string result;
  int current;

  if(!filename||filename=="")
    return "";
  if(number<=1)
    number=1;
  number--;
  result="";
  if(filename!=line_buffer_name)
  {
    // TK("read_buffer: initializing");
    line_buffer=([]);
    line_buffer_name=filename;
    current=0;
  }
  else
    current=start;

  while(current<=start+number && stringp((tmp=load_buffer(current++))))
    if(current>=start)
      result+=tmp;
  return result;
}

static string load_buffer(int line)
{
  int i, current, start, length, *idx;

  if(intp(line_buffer[line]))
  {
    // TK("load_buffer: Notfound: "+line);
    current=line;
    while(current>0&&intp(line_buffer[--current]));
    for(i=current;i<=line;i++)
    {
      // TK("load_buffer: trying: "+i);
      if(i>0)
	idx=line_buffer[i-1];
      else
	idx=({0,0});
      length=read_line((start=idx[0]+idx[1]));
      // TK("load_buffer: "+start+"/"+length);
      line_buffer[i]=({start,length});
    }
  }
  idx=line_buffer[line];
  return
    read_bytes(line_buffer_name,idx[0],idx[1]);
}

static int read_line(int offset)
{
  mixed loc_buf;
  int length, pos;

  // TK("read_line: "+offset);
  length=0;
  while(!intp((loc_buf=read_bytes(line_buffer_name,offset+length,1024))))
  {
    if((pos=member(loc_buf,'\n'))>=0)
      return length+pos+1;
    length+=1024;
  }
  return length;
}
*/

static void MoreFile(string str)
{
  int i, off;
  string f, l, r;
  
  SECURE1();
  
  if (str /*&& sizeof(str)*/)
  {
		if( !sizeof(str) ) str="\0";
    if(term)
      W("M[1M");
    switch(str[0])
    {
      case 'q':
      case 'x':
      moreflag=FALSE;
      moreoffset=1;
      if(morefile==TMP_FILE||morefile==PIPE_FILE)
	rm(morefile);
      return NULL;
      break;
      case 'P':
      case 'U':
      moreflag=FALSE;
      moreoffset=moreoffset-morelines;
      case 'p':
      case 'u':
      moreoffset=moreoffset-2*morelines;
      break;
      case 'D':
      case 'N':
      moreoffset+=morelines;
      case 0:	/* RETURN */
      case 'd':
      if(moreflag)
      {
	moreflag=FALSE;
	moreoffset=1;
	if(morefile==TMP_FILE)
	  rm(morefile);
	return;
      }
      break;
      case '/':
      moreoffset--;
      more_searchexpr=str[1..<1];
      case 'n':
      i=moreoffset-morelines+1;
      if(more_searchexpr=="")
      {
	WDLN("No previous regular expression");
	return;
      }
      if(!regexp(({"dummy"}), more_searchexpr))
	WDLN("Bad regular expression");
      else
	while((l=read_file(morefile, i++, 1))&&
	      !sizeof(regexp(({l}), more_searchexpr)))
	  ;
      if(l)
      {
	WLN("*** Skipping ...");
	moreoffset=i-1;
      }
      else
      {
	WLN("*** Pattern not found");
	moreoffset-=morelines;
      }
      break;
      case '0'..'9':
      sscanf(str, "%d", i);
      moreoffset=i;
      break;
    }
  }
  else
  {
    moreoffset=1;
    moreflag=FALSE;
  }
  if(moreoffset<1)
    moreoffset=1;
  if(CatFile())
    W("*** More: q,u,U,d,D,/<regexp>,<line> ["+(moreoffset-1)+"] *** ");
  else
  {
    W("*** More: q,u,U,d,D,/<regexp>,<line> ["+(moreoffset-1)+"=EOF] *** ");
    moreflag=TRUE;
  }
  input_to("MoreFile");
  return;
}

// Schade eigentlich das ich es neuschreiben musste, aber es ist
// schneller neu geschrieben als durch die undokumentieren alten Funktionen
// durchzusteigen... *seufz*
//					  Padreic

static string last_file, *last_file_buffer;
static int    last_file_date, last_file_size, last_file_complete;

static string sread_line(int num)
{
   if (!morefile) return "";
   if (last_file!=morefile || last_file_date!=file_time(morefile)) {
      if (!(last_file=read_bytes(morefile, 0, 50000))) return "";
      last_file_date=file_time(morefile);
      last_file_buffer=explode(last_file, "\n");
      last_file_size=sizeof(last_file_buffer);
      if (sizeof(last_file)==50000 && last_file[<1]!='\n') {
	 last_file_size--; // letzte Zeile nicht vollstaendig gelesen
	 last_file_complete=0;
      }
      else if (file_size(morefile)>50000)
	last_file_complete=0;
      else last_file_complete=1;
      last_file=morefile;
   }
   if (num==0) num=1;
   // bei zu grossen Files nicht mehr alles buffern...
   if (num>last_file_size) {
      if (last_file_complete) return "";
      return (read_file(morefile, num, 1) || "");
   }
   return last_file_buffer[num-1]+"\n";
}

static int CatFile()
{
  int end;
  string l;
  
  end=moreoffset+morelines;
  while(moreoffset<end)
//    if((l=read_file(morefile, moreoffset, 1))!="")
    if((l=sread_line(moreoffset))!="")
    {
      moreoffset++;
      W(l);
    }
    else
      return FALSE;
//  if(read_file(morefile, moreoffset+1, 1)!="")
  if(sread_line(moreoffset+1)!="")
    return TRUE;
  else
    return FALSE;
}

static int XGrepFile(string pat, string file, int mode)
{
  int i, j, f, s, fsize;
  string tfile, *tmp, *ts, t;
  
  SECURE2(FALSE);
  TK("XGrepFile: pat: "+pat+" file: "+file+" mode: "+mode);
  if(!(pat&&file))
    return FALSE;
  tfile=TMP_FILE;
  fsize=file_size(file);
  for(i=0,f=0; i<fsize && t=read_bytes(file, i, 50000); i+=50000)
    tmp=strip_explode(t,"\n");
    if (t && t[<1]!='\n' && sizeof(t)==50000) {
       i-=sizeof(tmp[<1]);
       tmp=tmp[0..<2];
    }
    if(s=sizeof(tmp))
    {
      for(j=0;j<s;j++)
      {
	if(sizeof(ts=regexp(({(mode&XGREP_ICASE?lower_case(tmp[j]):tmp[j])}),pat)))
	{
	  if(!(mode&XGREP_REVERT))
	   {
	     if(!f++)
	       write_file(tfile, "*** File: "+file+" ***\n");
	     write_file(tfile, tmp[j]+"\n");
	   }
	}
	else if(mode&XGREP_REVERT)
	{
	  if(!f++)
	    write_file(tfile, "*** File: "+file+" ***\n");
	  write_file(tfile, tmp[j]+"\n");
	}
      }
    }
  return TRUE;
}

static void XExecFile(int line)
{
  int i;
  
  if(!scriptline)
    return;
  for(i=line; i<scriptsize&&i<line+EXEC_LINES; i++)
  {
    if(!scriptline[i])
      continue;
    if(!Command(scriptline[i]))
    {
      scriptline=NULL;
      return;
    }
  }
  if(i<scriptsize)
    call_out("XExecFile", EXEC_TIME, i);
  else
    scriptline=NULL;
}

static void XmtpScript(string dir, string file, string opt)
{
  int s, t;
  string *files;
  
  s=sizeof(files=get_dir(dir+"/*"));
  while(s--)
  {
    t=sizeof(files[s])-1;
    if(files[s] == ".." || files[s] == "." || files[s][t] == '~' ||
       (files[s][0] == '#' && files[s][t] == '#'))
      continue;
    if(file_size(dir+"/"+files[s])==-2)
    {
      write_file(file, "mkdir "+files[s]+" ; cd "+files[s]+"\n");
      XmtpScript(dir+"/"+files[s], file, opt);
      write_file(file, "cd ..\n");
    }
    else
      write_file(file, "mtp -r "+opt+" "+dir+"/"+files[s]+"\n");
  }
}

/*----------------------------------------------------------------------
 *  player properties handling
 */

static string PlayerIdle(object obj)
{
  string str;
  int i, tmp;
  
  if(!obj)
    return NULL;
  if((i=query_idle(obj))>=60)
  {
    str=ARIGHT(""+(i/3600), 2, "0");
    i-=(i/3600)*3600;
    str+="'"+ARIGHT(""+(i/60), 2, "0");
  }
  else
    str=".....";
  return str;
}

static string PlayerAge(object obj)
{
  string str;
  int i, tmp;
  
  if(!obj)
    return NULL;
  i=(int)obj->QueryProp(P_AGE);
  str=" "+ARIGHT(""+(i/43200), 4, ".");
  i-=(i/43200)*43200;
  return str+":"+ARIGHT(""+(i/1800), 2, "0");
}

static string crname(object who)
{
  string uid, lname;

  if((uid=getuid(who))==ROOTID && 
     object_name(who)[0..7]=="/secure/" &&
     (lname=(string)who->loginname()))
    return CAP(lname);
  return CAP(uid);
}

static string PlayerWho(object obj)
{
  object tmp;
  string str, stmp;
  str=ARIGHT(""+LEVEL(obj)  ,  3, " ");
  str+=ALEFT(" "+crname(obj)+" ", 12, ".");
  str+=PlayerAge(obj);
  str+=((int)obj->QueryProp(P_GENDER)==1 ? " m " : " f ");
  str+=(obj->QueryProp(P_FROG))  ? "f" : ".";
  str+=(obj->QueryProp(P_GHOST)) ? "g" : ".";
  str+=(obj->QueryProp(P_INVIS)) ? "i" : ".";
  str+=(query_editing(obj)||query_input_pending(obj) ? "e" : ".");
  str+=(obj->QueryProp(P_AWAY))  ? "a" : ".";
  str+=" "+PlayerIdle(obj)+" ";
  str+=(tmp=ENV(obj)) ? ObjFile(tmp) : "- fabric of space -";
  return str;
}

static string PlayerMail(object obj, int flag)
{
  string pre;
  
  pre=(flag) ? ALEFT(crname(obj)+" ", 12, ".")+" " : "";
  return pre+"mail: "+obj->QueryProp(P_MAILADDR);
}

static string PlayerIP(object obj, int flag)
{
  string pre;
  
  pre=(flag) ? ALEFT(crname(obj)+" ", 12, ".")+" " : "";
  return pre+"host: "+query_ip_name(obj)+" ("+query_ip_number(obj)+")";
}

static string PlayerRace(object obj, int flag)
{
  string tmp, pre;
  
  pre=(flag) ? ALEFT(crname(obj)+" ", 12, ".")+" " : "";
  pre=pre+"race: "+ALEFT(obj->QueryProp(P_RACE)+" ", 10, ".")+" guild: ";
  tmp=(string)obj->QueryProp(P_GUILD);
  return tmp ? pre+tmp : pre+"- none -";
}

static string PlayerDomain(object obj, int flag)
{
  int i, s;
  mixed *uinfo;
  string *domains, pre;
  
  pre=(flag) ? ALEFT(crname(obj)+" ", 12, ".")+" " : "";
  pre+="domainlord of: ";
  uinfo=(mixed*)MASTER->get_userinfo(getuid(obj));
  if(uinfo&&(domains=uinfo[3])&&(s=sizeof(domains)))
  {
    for(i=0; i<s; i++)
    {
      pre += CAP(domains[i]);
      if(i<s-1)
	pre += ", ";
    }
  }
  return pre;
}

static string PlayerStats(object obj, int flag)
{
  string pre;
  
  pre=(flag) ? ALEFT(crname(obj)+" ", 12, ".")+" " : "";
  pre+="hp="+ARIGHT(obj->QueryProp(P_HP), 3, "0");
  pre+="/"+ARIGHT(obj->QueryProp(P_MAX_HP), 3, "0");
  pre+=" sp="+ARIGHT(obj->QueryProp(P_SP), 3, "0");
  pre+="/"+ARIGHT(obj->QueryProp(P_MAX_SP), 3, "0");
  pre+=" food="+ARIGHT(obj->QueryProp(P_FOOD), 3, "0");
  pre+="/"+ARIGHT(obj->QueryProp(P_MAX_FOOD), 3, "0");
  pre+=" drink="+ARIGHT(obj->QueryProp(P_DRINK), 3, "0");
  pre+="/"+ARIGHT(obj->QueryProp(P_MAX_DRINK), 3, "0");
  pre+=" exps="+obj->QueryProp(P_XP);
  return pre;
}

static string PlayerSnoop(object obj, int flag)
{
  string tmp, pre;
  object victim;
  
  pre=(flag) ? ALEFT(crname(obj)+" ", 12, ".")+" " : "";
  pre=pre+"is snooped by: ";
  if(victim=query_snoop(obj))
    pre+=ARIGHT(" "+crname(victim), 12, ".");
  else
    pre+="............";
  return pre;
}

static string PlayerCmdAvg(object obj, int flag)
{
  string pre;
  
  pre=(flag) ? ALEFT(crname(obj)+" ", 12, ".")+" " : "";
  return pre+"cmdavg: "+(int)obj->_query_command_average();
}


/*----------------------------------------------------------------------
 * msg input to objects
 */

static void XMsgSay(string str)
{
  if(str=="."||str=="**")
  {
    WLN("[End of message]");
    say("[End of message]\n");
  }
  else
  {
    say(str+"\n");
    input_to("XMsgSay");
  }
}

static void XMsgTell(string str)
{
  if(str=="."||str=="**")
  {
    WLN("[End of message]");
    tell_object(msgto, "[End of message]\n");
  }
  else
  {
    tell_object(msgto, str+"\n");
    input_to("XMsgTell");
  }
}

static void XMsgShout(string str)
{
  if(str=="."||str=="**")
  {
    WLN("[End of message]");
    shout("[End of message]\n");
  }
  else
  {
    shout(str+"\n");
    input_to("XMsgShout");
  }
}

/*----------------------------------------------------------------------
 * own object moving
 */

int move(mixed dest)
{
  move_object(ME, cloner?cloner:dest);
  return TRUE;
}

/*----------------------------------------------------------------------
 * object id
 */

int id(string str)
{
  if(!security()&&MODE(MODE_SCANCHK)&&RTP&&!IS_ARCH(RTP))
    WDLN(crname(RTP)+" scanned you (id) ["+query_verb()+"] "+
	 (PREV ? ObjFile(PREV) : "[destructed object]"));
  return LOWER(str)==LOWER(TOOL_NAME);
}

/*----------------------------------------------------------------------
 * short and long description
 */

string short()
{
  return _query_short()+".\n";
}

string _query_short()
{
  string sh; // added by Rumata
  if(cloner)
  {
    if((!security())&&MODE(MODE_SCANCHK)&&!IS_ARCH(RTP))
      WDLN(crname(RTP)+" scanned you (short) ["+query_verb()+"] "+
	   (PREV ? ObjFile(PREV) : "[destructed object]"));
	if( sh=Query(P_SHORT) ) return sh; // added by Rumata
    return cloner->name(WESSEN)+" "+TOOL_TITLE+" ["+
      ctime(time())[11..18]+"]";
  }
  return TOOL_TITLE;
}

string long()
{
  return _query_long();
}

string _query_long()
{
  if(cloner&&!security()&&MODE(MODE_SCANCHK)&&!IS_ARCH(RTP))
  {
    WDLN(crname(RTP)+" scanned you (long) ["+query_verb()+"] "+
	 (PREV ? ObjFile(PREV) : "[destructed object]"));
  }
  return 
    "This is "+TOOL_NAME+" version "+TOOL_VERSION+
    " (maintained by Kirk@MorgenGrauen)\n"+
    "Original copyright held by Hyp.\n"+
    "Gamedriver patchlevel: "+__VERSION__+" master object: "+__MASTER_OBJECT__+
    "\n\nDo 'xhelp' for more information.\n";
}

string name(mixed dummy1, mixed dummy2)
{
  return _query_short();
}

/*----------------------------------------------------------------------
 * light stuff
 */

int _query_light()
{
  return xlight;
}

int _set_light(int x)
{
  return xlight;
}

/*----------------------------------------------------------------------
 * Autoloading
 */

mixed *_query_autoloadobj()
{
  return AUTOLOAD_ARGS;
}

void _set_autoloadobj(mixed *args)
{
  WLN(TOOL_TITLE+" ...");
  if(!pointerp(args))
    ;
  else if(sizeof(args)!=3)
    ;
  else if(!stringp(args[0]))
    ;
  else if(!intp(args[1]))
    ;
  else if(!intp(args[2]))
    ;
  else
  {
    if((string)args[0]!=TOOL_INTERNAL)
    {
      WLN("*****************************");
      WLN("***	    NEW EDITION      ***");
      WLN("***	do 'xtool news' for  ***");
      WLN("***	 more information    ***");
      WLN("*****************************");
    }
    modi=(int)args[1];
    morelines=(int)args[2];
    return;
  }
  W("(bad autoload, using default)\n");
}

/*----------------------------------------------------------------------
 * creation, updating and initialization stuff
 */

void update_tool(mixed *args, object obj)
{
  SECURE1();
  if(!(obj&&args))
    return;
  Destruct(PREV);
  _set_autoloadobj(args);
  move(obj);
}

void create()
{
  object obj;
  
  if(member(object_name(),'#')<0)
    return;
  if(!cloner&&!((cloner=TP)||(cloner=ENV(ME)))&&!interactive(cloner))
    destruct(ME);
  if(!IS_LEARNER(cloner))
    destruct(ME);
  SetProp(P_NODROP,"Das waere zu gefaehrlich.\n");
  SetProp(P_NEVERDROP,1);
  SetProp(P_NOBUY,1);
  if(file_size(SAVE_FILE+".o")>0)
  {
    WDLN("Loading "+TOOL_TITLE+" settings");
    restore_object(SAVE_FILE);
  }
  if(MODE(MODE_FIRST))
    call_out("move",0,cloner);
  call_out("add_insert_hook",1,ME);
}

void TK(string str)
{
  if (!xtk)
    return;
  tell_object(cloner,"XTOOL: "+str+"\n");
}

int Xtk(string str)
{
  xtk=!xtk;
  WDLN("Xtool internal tracing "+(xtk?"enabled":"disabled"));
  return TRUE;
}

void add_insert_hook(object obj)
{
  if(objectp(obj))
    cloner->AddInsertHook(obj);
}

void init()
{
  object first, prev;
  
  if(member(object_name(),'#')<0) return;
  first=first_inventory(ENV(ME));
  if(MODE(MODE_PROTECT)&&is_player(first)&&!IS_ARCH(first))
  {
    WDLN("WARNING: "+crname(first)+" tried to move into your inventory");
    tell_object(first, "You cannot move yourself into "+
		crname(cloner)+"'s inventory.\n");
    call_out("DropObj",0,first);
    return;
  }
  else if(MODE(MODE_FIRST)&&first!=ME)
    move(cloner);
  else actions();
}

void DropObj(object obj)
{
  if(!obj||!objectp(obj))
    return;
  obj->move(ENV(cloner),M_NOCHECK|M_NO_SHOW);
}

#define ACTIONS\
([\
  "xcallouts" : "Xcallouts";0;1,\
  "xcall"     : "Xcall";0;1,\
  "xcat"      : "Xcat";1;1,\
  "xcd"       : "Xcd";0;0,\
  "xclean"    : "Xclean";0;0,\
  "xclone"    : "Xclone";0;0,\
  "xuclone"   : "Xuclone";0;0,\
  "xcmds"     : "Xcmds";0;1,\
  "xdbg"      : "Xdbg";0;0,\
  "xdclean"   : "Xdclean";0;0,\
  "xddes"     : "Xddes";0;0,\
  "xdes"      : "Xdes";0;0,\
  "xdest"     : "Xdes";0;0,\
  "xdlook"    : "Xdlook";0;1,\
  "xdo"       : "Xdo";0;0,\
  "xdupdate"  : "Xdupdate";0;0,\
  "xecho"     : "Xecho";0;0,\
  "xeval"     : "Xeval";0;1,\
  "xforall"   : "Xforall";0;0,\
  "xgoto"     : "Xgoto";0;0,\
  "xhbeats"   : "Xhbeats";0;1,\
  "xgrep"     : "Xgrep";1;1,\
  "xhead"     : "Xhead";1;1,\
  "xhelp"     : "Xhelp";0;0,\
  "xinventory": "Xinventory";0;1,\
  "xids"      : "Xids";0;0,\
  "xinfo"     : "Xinfo";0;0,\
  "xinherit"  : "Xinherit";0;1,\
  "xlag"      : "Xlag";0;0,\
  "xlight"    : "Xlight";0;0,\
  "xload"     : "Xload";0;0,\
  "xlook"     : "Xlook";0;1,\
  "xlpc"      : "Xlpc";0;0,\
  "xman"      : "Xman";0;0,\
  "xmore"     : "Xmore";1;0,\
  "xmove"     : "Xmove";0;0,\
  "xmsg"      : "Xmsg";1;0,\
  "xmtp"      : "Xmtp";0;0,\
  "xproc"     : "Xproc";0;1,\
  "xprof"     : "Xprof";0;0,\
  "xprops"    : "Xprops";0;1,\
  "xquit"     : "Xquit";0;0,\
  "xscan"     : "Xscan";0;0,\
  "xset"      : "Xset";0;0,\
  "xsh"       : "Xsh";0;0,\
  "xsort"     : "Xsort";1;1,\
  "xtail"     : "Xtail";1;1,\
  "xtk"       : "Xtk";0;0,\
  "xtool"     : "Xtool";0;0,\
  "xtrace"    : "Xtrace";0;0,\
  "xtrans"    : "Xtrans";0;0,\
  "xupdate"   : "Xupdate";0;0,\
  "xwc"       : "Xwc";1;0,\
  "xwho"      : "Xwho";0;1,\
  ])

static string PrepareLine(string str)
{
  return str;
}

static string QuoteLine(string str)
{
  string *tmp,*tmp2;
  int i, i2, len, len2, qd, qs;

  str=string_replace(str,"\\\\","°BSHL");
  str=string_replace(str,"\\\"","°DBLQ");
  str=string_replace(str,"\\\'","°SGLQ");
  str=string_replace(str,"\\|","°PIPE");
  str=string_replace(str,"||","°OROR");
  str=string_replace(str,"->","°LARR");
  str=string_replace(str,"\\$","°DOLR");
  tmp=regexplode(str,"(\"|')");
  len=sizeof(tmp);
  qd=qs=0;
  for(i=0;i<len;i++)
  {
    if(i%2)
    {
      if(tmp[i]=="'")
	qd=(!qs?!qd:qd);
      else
	qs=(!qd?!qs:qs);
      if((tmp[i]=="\""&&!qd)||(tmp[i]=="'"&&!qs))
	tmp[i]="";
    }
    else
    {
      if(!qd)
      {
	len2=sizeof(tmp2=regexplode(tmp[i],"\\$[^ ][^ ]*"));
	for(i2=0;i2<len2;i2++)
	  if(i2%2)
	  {
	    TK("QuoteLine: "+tmp2[i2][1..]);
	    tmp2[i2]=(string)XFindObj((tmp2[i2])[1..]);
	  }
	tmp[i]=implode(tmp2,"");
      }
    }
  }
  if(qd||qs)
    return NULL;
  str=implode(tmp,"");
  TK("QuoteLine: str: "+str);
  return str;
}

static string UnquoteLine(string str)
{
  str=string_replace(str,"°BSHL","\\");
  str=string_replace(str,"°DBLQ","\"");
  str=string_replace(str,"°SGLQ","\'");
  str=string_replace(str,"°DQUO","");
  str=string_replace(str,"°SQUO","");
  str=string_replace(str,"°PIPE","|");
  str=string_replace(str,"°OROR","||");
  str=string_replace(str,"°LARR","->");
  str=string_replace(str,"°DOLR","$");
  TK("UnquoteLine: str: "+str);
  return str;
}

static string *ExplodeCmds(string str)
{
  string *tmp;
  
  tmp=regexplode(str,"\\||>|>>");
  while(tmp[<1]=="")
    tmp=tmp[0..<2];
  return tmp;
}

varargs int ParseLine(string str)
{
  string verb, arg;
  int ret;
  
  TK("ParseLine: str: "+(str?str:""));
  if(!sizeof(cmds))
  {
    // this is a single command or beginning of a command pipe
    verb=query_verb();

    // return on unknown commands
    if(!verb||!sizeof(verb)||!GetFunc(verb,TRUE))
      return FALSE;

    str=(string)this_player()->_unparsed_args();
    pipe_in=FALSE;
    pipe_of=NULL;
    pipe_ovr=TRUE;
    pipe_out=FALSE;
    pipe_of=NULL;
    // pass arguments to some special functions unparsed
    if(member(({"xlpc","xcall","xeval"}),verb)>=0)
    {
#ifdef XDBG
      TK("ParseLine: special func: "+verb);
#endif
      ret=CallFunc(verb,str);
      SafeReturn(ret);
    }
    // ok, here we go
    pipe_in=pipe_out=FALSE;
    pipe_if=pipe_of=NULL;
    pipe_ovr=TRUE;
    if(file_size(PIPE_FILE)>=0)
      rm(PIPE_FILE);
    if (str&&str!="")	  
    {
      if(!(str=QuoteLine(str)))
      {
	WDLN("Unterminated quotation");
	SafeReturn(TRUE);
      }
      cmds=ExplodeCmds(str);
    }
    else
      cmds=({""});
    arg=strip_string(cmds[0]);
  }
  else
  {
    cmds[0]=strip_string(cmds[0]);
    TK("ParseLine: cmds[0]: "+cmds[0]);
    if(sscanf(cmds[0],"%s %s",verb,arg)!=2)
    {
      verb=cmds[0];
      arg="";
    }
  }
  cmds=cmds[1..];
  TK("ParseLine: verb: "+verb);
  if (!verb||!sizeof(verb)||!GetFunc(verb,TRUE))
    SafeReturn(FALSE);
  TK("ParseLine(1): arg: "+arg+" cmds: "+sprintf("%O",cmds));
  switch(sizeof(cmds))
  {
    case 0:
    ret=CallFunc(verb,strip_string(UnquoteLine(arg)));
    SafeReturn(ret);
    break;
    
    case 1:
    WDLN("Missing rhs of command pipe");
    SafeReturn(TRUE);
    break;
    
    default:
    pipe_out=TRUE;
    switch(cmds[0])
    {
      case "|":
      pipe_of=PIPE_FILE;
      pipe_ovr=TRUE;
      cmds=cmds[1..];
      break;
      
      case ">":
      pipe_ovr=TRUE;
      if(sizeof(cmds)!=2)
      {
	WDLN("Illegal IO redirection");
	SafeReturn(TRUE);
      }
      pipe_of=cmds[1];
      cmds=({});
      break;
      
      case ">>":
      pipe_ovr=FALSE;
      if(sizeof(cmds)!=2)
      {
	WDLN("Illegal IO redirection");
	SafeReturn(TRUE);
      }
      pipe_of=cmds[1];
      cmds=({});
      break;
    }
  }
  TK("ParseLine(2): arg: "+arg+" cmds: "+sprintf("%O",cmds));
  if(!CallFunc(verb,strip_string(arg)))
    SafeReturn(FALSE);
  pipe_in=pipe_out;
  pipe_if=pipe_of;
  pipe_ovr=FALSE;
  pipe_out=FALSE;
  pipe_of=NULL;
  if(sizeof(cmds))
    call_out("ParseLine",0);
  else
    SafeReturn(TRUE);
  return TRUE;
}

static int CallFunc(string verb, string str)
{
  string fun;

  fun=GetFunc(verb,FALSE);
#ifdef XDBG
  TK("CallFunc: verb: "+verb+" str: "+str);
  TK("CallFunc: resolved function: "+(fun?fun:"(unresolved)"));
#endif
  if(str=="")
    str=NULL;
  return fun?(int)call_other(ME,fun,str):FALSE;
}

static string GetFunc(string verb, int test)
{
  string fun,*keys,key;
  int i,len;

  TK("GetFunc: verb: "+verb);

  if(verb[0..0]!="x") // Assume all commands start with "x"
    return 0;

  if (!(fun=(string)ACTIONS[verb,0])) { // Try exact hit first
    key="";
    len=sizeof(verb);
    for (i=sizeof(keys=m_indices(ACTIONS))-1;i>=0;i--) {
      TK("  trying: "+keys[i]);
      if(sizeof(keys[i])>=len&&keys[i][0..len-1]==verb) {
	if(sizeof(key)) {
	  WLN("Das ist nicht eindeutig ...");
	  return 0;
	}
	fun=ACTIONS[keys[i],0];
	key=keys[i];
	//break;
      }
    }
  } else
    key=verb;

  if(test)
    return fun;
  
  if (key) {
#ifdef XDBG
    TK("GetFunc: fun: "+fun+" (key: "+key+")\n"+
       "pipe_in: "+(pipe_in?"TRUE  ":"FALSE ")+(pipe_if?pipe_if:"(NULL)")+"\n"+
       "pipe_out: "+(pipe_out?"TRUE  ":"FALSE ")+(pipe_of?pipe_of:"(NULL)")+"\n"+
       "pipe_ovr: "+(pipe_ovr?"TRUE":"FALSE"));
#endif
    if (pipe_in&&!ACTIONS[key,PIPE_IN])
    {
      // this command does not read pipes
#ifdef XDBG
      TK("Illegal rhs of command pipe \""+fun+"\"\n");
#endif
      notify_fail("Illegal rhs of command pipe \""+fun+"\"\n");
      return 0;
    }
    else if (pipe_out&&!ACTIONS[key,PIPE_OUT])
    {
      // this command does not feed pipes
#ifdef XDBG
      TK("Illegal lhs of command pipe \""+fun+"\"\n");
#endif
      notify_fail("Illegal lhs of command pipe \""+fun+"\"\n");
      return 0;
    }
  }
  return fun;
}

void actions()
{
  if (!cloner||!RTP||cloner==RTP||query_wiz_level(cloner)<=query_wiz_level(RTP))
    add_action("ParseLine","",1);
  add_action("CommandScan", "", 1);
}

/*----------------------------------------------------------------------
 *  the checking stuff
 */

void InsertNotify(object obj)
{
  if(!cloner)
    return;
  if(MODE(MODE_FIRST) && find_call_out("move")==-1)
    call_out("move",0,cloner);
  if(MODE(MODE_INVCHECK))
    write_newinvobj(obj);
}

static void VarCheck(int show)
{
  int i, s;
  foreach(string k, mixed v : variable)
  {
    if (v) continue;
    if(show) WDLN("*** Variable $"+k+" has been destructed");
    m_delete(variable, k);
  }
}


int write_newinvobj(object obj)
{
  if(obj) WDLN("*** New object in inventory "+ObjFile(obj));
  return(1);
}

/*----------------------------------------------------------------------
 *  catch all commands, absorb forces and check history
 */

int CommandScan(string arg)
{
  string verb, cmd;
  object rtp;
  rtp=RTP;

  if(!cloner&&!(cloner=rtp)) destruct(ME);

  if((!MODE(MODE_PROTECT))||security()||
     query_wiz_level(cloner)<query_wiz_level(rtp))
  {
    verb=query_verb();
    if(verb&&DoHistory(verb+(arg ? " "+arg : "")))
      return TRUE;
    nostore=FALSE;
    return FALSE;
  }
  else
  {
    if(rtp)
    {
      WDLN("Your "+TOOL_TITLE+" protects you from a force by "+crname(rtp)+
	   " ["+query_verb()+(arg ? " "+arg+"]" : "]"));
      tell_object(rtp, crname(cloner)+"'s "+TOOL_TITLE+
		  " absorbes your force.\n");
    }
    else
    {
      WDLN("Your "+TOOL_TITLE+" protects you from a force ["+
	   query_verb()+(arg ? " "+arg+"]" : "]"));
    }
    return TRUE;
  }
}

int DoHistory(string line)
{
  int i;
  string cmd, *strs;
  
  SECURE2(FALSE);
  if(!stringp(line) || !sizeof(line))
    return TRUE;
  else if(line=="%!")
  {
    WLN("Current command history:");
    for(i=MAX_HISTORY; i; --i)
      if(history[i-1])
      {
	W(" "+ARIGHT(""+i, 2, " ")+": ");
	if(sizeof(history[i-1])>70)
	  WLN(ALEFT(history[i-1], 70, " "));
	else
	  WLN(history[i-1]);
      }
    return TRUE;
  }
  else if(line[0..1]=="%%" && (cmd=history[0]+line[2..<1]))
  {
    Command(cmd);
    return TRUE;
  }
  else if(line[0]=='^'&&(strs=strip_explode(line, "^")))
  {
    if(sizeof(strs)&&strs[0]&&(cmd=history[0]))
    {
      if(sizeof(strs)==2)
	cmd=string_replace(cmd, strs[0], strs[1]);
      else
	cmd=string_replace(cmd, strs[0], "");
      nostore--;
      Command(cmd);
      nostore++;
      return TRUE;
    }
  }
  else if(line[0]=='%' && (sscanf(line[1..<1], "%d", i)))
  {
    i= i>0 ? i : 1;
    i= i<=MAX_HISTORY ? i : MAX_HISTORY;
    if(cmd=history[i-1])
      Command(cmd);
    return TRUE;
  }
  else if(line[0]=='%')
  {
    for(i=0; i<MAX_HISTORY; i++)
    {
      if(history[i]&&
	 history[i][0..sizeof(line)-2]==line[1.. <1])
      {
	Command(history[i]);
	return TRUE;
      }
    }
  }
  else if(nostore<1)
    history=({line})+history[0..MAX_HISTORY-2];
  return FALSE;
}

