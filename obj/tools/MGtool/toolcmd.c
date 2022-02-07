/*
 * MGtool-1.0
 * File: toolcmds.c
 * Maintainer: Kirk@MorgenGrauen
 */

/*------------------------------------------*/
/* the original Xtool is copyrighted by Hyp */
/*------------------------------------------*/

#include "tool.h"

/*----------------------------------------------------------------------
 * command functions
 */

int Xcall(string str)
{
  object obj, callobj;
  string file, callstr, callfun, callexpr, error, errlog;
  int *ru1, *ru2, xtime;
  mixed res;
  
  SECURE2(TRUE);
  USAGE2(str, "xcall <object>-><function>(<arguments>)");
  TK("Xcall: str: "+(str?str:"(NULL)"));
  if(sscanf(str, "%s->%s(%s", callstr, callfun, callexpr)!=3)
    return FALSE;
  if(!(callobj=XFindObj(callstr)))
    return TRUE;
  else
  {
#if 0
    write_file(TOOL_LOG,
	       sprintf("%s (%s) xcall %s (env: %s)\n",ctime(time()),
		       getuid(cloner),str,object_name(environment(cloner))));
#endif
    file=LPC_FILE+".c";
    if(file_size(file)>0)
      rm(file);
    if(obj=find_object(LPC_FILE))
      Destruct(obj);
    obj=0;
    write_file(file,
	       "#include <properties.h>\n"+
	       "#include <thing/properties.h>\n"+
	       "#include <defines.h>\n"+
	       "#include <wizlist.h>\n"+
	       "#include <moving.h>\n"+
	       "#include \"/secure/wizlevels.h\"\n"+
	       (file_size(PRIVATE_HEADER)>=0?"#include \""+PRIVATE_HEADER+"\"\n":"")+
	       "mixed get(string str){return previous_object()->XFindObj(str);}\n"+
	       "mixed eval(object obj,mixed me,mixed here){return obj->"+callfun+"("+callexpr+";}\n");
    errlog = ERR_FILE;
    if(file_size(errlog)>0)
      rm(errlog);
    if(error=catch((LPC_FILE)->__nixgibts__()))
      W("Error: "+error[1..]);
    else
    {
      obj=find_object(LPC_FILE);
      ru1=rusage();
      error=catch(res=({mixed})obj->eval(callobj, cloner, ENV(cloner)));
      ru2=rusage();
      if(error)
	W("Error: "+error[1..]);
      else
      {
	xtime=ru2[0]-ru1[0]+ru2[1]-ru1[1];
	WDLN("Evaluation time: "+(xtime<0 ? 0 : xtime)+" ms");
	PIPE_DELETE(pipe_of);
	if(pipe_out&&pipe_of)
	  write_file(pipe_of,mixed_to_string(res, MAX_RECURSION)+"\n");
	else
	WLN("Result: "+mixed_to_string(res, MAX_RECURSION));
	if(objectp(res))
    m_add(variable, "result", res);
      }
    }
    rm(file);
  }
  if(obj) Destruct(obj);
  return TRUE;
}

int Xcallouts(string str)
{
  mixed callouts;
  string tmp, file;
  int i, s;
  
  SECURE2(TRUE);
  TK("Xcallouts: str: "+(str?str:"(NULL)"));
  if(!pipe_out)
  {
    USAGE1(str, "xcallo(uts) [search pattern]");
    file=TMP_FILE;
    if(file_size(file)>0)
      rm(file);
    if(!str)
      str="^\\[~/";
    else if(!regexp(({"dummy"}), str))
    {
      WDLN("Bad regular expression");
      return TRUE;
    } 
  }
  else
  {
    USAGE1(str, "xcallo(uts)");
    if(str&&str!="")
    {
      WDLN("More arguments than expected");
      return TRUE;
    }
  }
  callouts=call_out_info();
  s=sizeof(callouts);
  PIPE_DELETE(pipe_of);
  for(i=0; i<s; i++)
  {
    if(callouts[i]&&pointerp(callouts[i]))
    {
      tmp=sprintf("%O %Os %O (%s)",callouts[i][0],callouts[i][2],
		  callouts[i][1],(sizeof(callouts[i])>3?
				  mixed_to_string(callouts[i][3],
						  MAX_RECURSION):"0"));
      if(pipe_out&&pipe_of)
	write_file(pipe_of,tmp+"\n");
      else
	if(sizeof(regexp(({tmp}), str)))
	  WLN(tmp);
    }
  }
  return TRUE;
}

int Xcat(string str)
{
  string *tmp,file;

  SECURE2(TRUE);
  TK("Xcat: str: "+str);
  if(!pipe_in)
  {
    USAGE2(str, "xcat <file>");
    if(!(file=XFindFile(str)))
    {
      WDLN("Can't find file");
      return TRUE;
    }
  }
  else
  {
    if(str&&str!="-")
      USAGE3("xcat -");

    if (file_size(file=pipe_if)<0)
    {
      WDLN("Missing input to xcat");
      return TRUE;
    }
  }
  tmp=old_explode(read_file(file,0,PIPE_MAX),"\n");
  if(pipe_in&&pipe_if==PIPE_FILE)
    rm(PIPE_FILE);
  if (pipe_out&&pipe_of)
    write_file(pipe_of,implode(tmp,"\n")+"\n");
  else
    WLN(implode(tmp,"\n"));
  return TRUE;
}

int Xcd(string str)
{
  object dest;
  string path;
  
  SECURE2(TRUE);
  USAGE1(str, "xcd [object]");
  TK("Xcd: str: "+(str?str:"(NULL)"));
  if(!str)
  {
    if(!(path=({string})cloner->QueryProp("start_home")))
      path="/";
  }
  else if((dest=XFindObj(str,1)))
    path="/"+implode(old_explode(object_name(dest),"/")[0..<2],"/");
  else
    path="";

  TK("Xcd: path: "+(path?path:"(NULL)"));
  if(!sizeof(path))
    path=str;
  PL->_cd(path);

  return TRUE;
}

int Xclean(string str)
{
  object env;
  
  SECURE2(TRUE);
  USAGE1(str, "xcle(an) [object]");
  TK("Xclean: str: "+(str?str:"(NULL)"));
  if(!str)
    env=ENV(cloner);
  if(env||(env=XFindObj(str)))
  {
    PrintShort("Cleaning: ", env);
    filter(filter(all_inventory(env), #'is_not_player),//'
		 "Destruct", ME);
  }
  return TRUE;
}

int Xclone(string str)
{
  object obj;
  string file, errlog, error;
  
  SECURE2(TRUE);
  USAGE2(str, "xclo(ne) <filename>");
  if(!(file=XFindFile(str))) return TRUE;
  errlog=ERR_FILE;
  if(file_size(errlog)>0) rm(errlog);
  WLN("Clone: "+short_path(file));
  if(!(error=catch(obj=clone_object(file))))
  { 
    m_add(variable, "clone", obj);
    if(!MoveObj(obj, ENV(cloner), TRUE))
      WDLN("Cannot move object into this room");
    else if(!({mixed})obj->QueryNoGet())
    {
      if(!MoveObj(obj, cloner, TRUE))
	WDLN("Cannot move object into your inventory");
    }
  }
  else
    W("Error: "+error[1..]);
  return TRUE;
}

int Xuclone(string str)
{
  object obj;
  string file, errlog, error;
  
  SECURE2(TRUE);
  USAGE2(str, "xuclo(ne) <filename>");
  if(!(file=XFindFile(str))) return TRUE;
  errlog=ERR_FILE;
  if(file_size(errlog)>0) rm(errlog);
  if(obj=find_object(file)) {
    Destruct(obj);
    WLN("Update and clone: "+short_path(file));
  }
  else
    WLN("Clone: "+short_path(file));
  if(!(error=catch(obj=clone_object(file))))
  {
    variable["clone"] = obj;
    if(!MoveObj(obj, ENV(cloner), TRUE))
      WDLN("Cannot move object into this room");
    else if(!({mixed})obj->QueryNoGet())
    {
      if(!MoveObj(obj, cloner, TRUE))
	WDLN("Cannot move object into your inventory");
    }
  }
  else
    W("Error: "+error[1..]);
  return TRUE;
}

int Xcmds(string str)
{
  object obj;
  mixed *cmds;
  int i, s;

  SECURE2(TRUE);
  USAGE1(str, "xcm(ds) [object]");
  TK("Xcmds: str: "+(str?str:"(NULL)"));
  if(!str)
    obj=ENV(cloner);
  else if(!(obj=XFindObj(str)))
  {
    WDLN("Can't find object");
    return TRUE;
  }
  s=sizeof(cmds=query_actions(cloner,1|2|4|8|16));
  PIPE_DELETE(pipe_of);
  for(i=0; i<s; i+=5)
    if(cmds[i+3]==obj)
      if(pipe_out&&pipe_of)
	write_file(pipe_of,ALEFT(cmds[i]+" ", 15, ".")+
		   (cmds[i+2] ? " * " : " . ")+cmds[i+4]+"()\n");
      else
	WLN(ALEFT(cmds[i]+" ", 15, ".")+
	    (cmds[i+2] ? " * " : " . ")+cmds[i+4]+"()");
  return TRUE;
}
  
int Xdbg(string str)
{
  object obj;
  
  SECURE2(TRUE);
  USAGE2(str, "xdb(g) <object>");
  TK("Xdbg: str: "+(str?str:"(NULL)"));
  if((obj=XFindObj(str)))
  {
    debug_info(1, obj);
    debug_info(0, obj);
  }
  return TRUE;
}

int Xdclean(string str)
{
  object env;
  
  SECURE2(TRUE);
  USAGE1(str, "xdc(lean) [object]");
  TK("Xdclean: str: "+(str?str:"(NULL)"));
  if(!str)
    env=ENV(cloner);
  if(env||(env=XFindObj(str)))
  {
    PrintShort("Deep cleaning: ", env);
    filter(filter(all_inventory(env), #'is_not_player, ME),//'
		 "DeepClean", ME);
  }
  return TRUE;
}

int Xddes(string str)
{
  object obj;
  
  SECURE2(TRUE);
  USAGE2(str, "xdd(es) <object>");
  TK("Xddes: str: "+(str?str:"(NULL)"));
  if((obj=XFindObj(str)))
  {
    PrintShort("Deep destruct: ", obj);
    filter(deep_inventory(obj), "Destruct", ME);
    Destruct(obj);
  }
  return TRUE;
}

int Xdes(string str)
{
  object obj;
  
  SECURE2(TRUE);
  USAGE2(str, "xde(s) <object>");
  TK("Xdes: str: "+(str?str:"(NULL)"));
  if((obj=XFindObj(str)))
  {
    PrintShort("Destruct: ",obj);
    Destruct(obj);
  }
  return TRUE;
}

int Xdlook(string str)
{
  object obj;
  
  SECURE2(TRUE);
  USAGE1(str, "xdl(ook) [object]");
  TK("Xdlook: str: "+(str?str:"(NULL)"));
  if(!str)
    obj=cloner;
  if(obj||(obj=XFindObj(str)))
  {
    PIPE_DELETE(pipe_of);
    DeepPrintShort(obj,NULL,NULL,(pipe_out&&pipe_of)?pipe_of:"");
  }
  return TRUE;
}

int Xdo(string str)
{
  int i, n, s;
  string *strs, cmd;
  
  SECURE2(TRUE);
  USAGE2(str, "xdo [<number1>#]<command1>[;[<number2>#]<command2>] ...");
  if(!str||str==""||!(s=sizeof(strs=strip_explode(str, ";"))))
    return FALSE;
  for(i=0; i<s; i++)
  {
    if(strs[i])
    {
      switch(sscanf(strs[i], "%d#%s", n, cmd))
      {
	case 0:
	if(!Command(strs[i])) return TRUE;
	break;
	case 1:
	if(cmd&&(!Command(cmd))) return TRUE;
	break;
	case 2:
	n= n<1 ? 1 : n;
	if(cmd)
	{
	  while(n--)
	    if(!Command(cmd)) return TRUE;
	}
	break;
      }
    }
  }
  return TRUE;
}

int Xdupdate(string str)
{
  int i, s;
  object obj;
  string file, *list;
  
  SECURE2(TRUE);
  USAGE2(str, "xdu(pdate) <filename>");
  if(!(file=XFindFile(str)))
    return TRUE;
  if(file[<2..<1]==".c")
    file=file[0..<3];
  if(obj=XFindObj(file))
  {
    PrintShort("Deep updating: ", obj);
    list=inherit_list(obj);
    for(s=sizeof(list); i<s; i++)
    {
      if(obj=find_object(list[i]))
	Destruct(obj);
    }
  }
  return TRUE;
}

int Xecho(string str)
{
  TK("Xecho: str: "+(str?str:"(NULL)"));
  WLN(str);
  return TRUE;
}

int Xeval(string str)
{
  object obj;
  string file, error;
  int *ru1, *ru2, xtime;
  mixed res;
  
  SECURE2(TRUE);
  USAGE2(str, "xev(al) <expression>");
  file=LPC_FILE+".c";
  if(file_size(file)>0)
    rm(file);
  if(obj=find_object(LPC_FILE))
    Destruct(obj);
#if 0
  write_file(TOOL_LOG,
	     sprintf("%s (%s) xeval %s\n",
		     ctime(time()),getuid(cloner),str));
#endif
  write_file(file,
	     "#include <properties.h>\n"+
	     "#include <thing/properties.h>\n"+
	     "#include <defines.h>\n"+
             "#include <wizlist.h>\n"+
	     "#include <moving.h>\n"+
	     "#include \"/secure/wizlevels.h\"\n"+
	     (file_size(PRIVATE_HEADER)>=0?"#include \""+PRIVATE_HEADER+"\"\n":"")+
	     "mixed get(string str){return previous_object()->XFindObj(str);}\n"+
	     "mixed eval(mixed me,mixed here){return "+str+";}");
  if(error=catch(obj=clone_object(file)))
    W("Error: "+error[1..]);
  else
  {
    ru1=rusage();
    error=catch(res=({mixed})obj->eval(cloner, ENV(cloner)));
    ru2=rusage();
    if(error)
      W("Error: "+error[1..]);
    else
    {
      xtime=ru2[0]-ru1[0]+ru2[1]-ru1[1];
      WDLN("Evaluation time: "+(xtime<0 ? 0 : xtime)+" ms");
      PIPE_DELETE(pipe_of);
      if(pipe_out&&pipe_of)
        write_file(pipe_of,mixed_to_string(res,MAX_RECURSION)+"\n");
      else
        WLN("Result: "+mixed_to_string(res, MAX_RECURSION));
      if(objectp(res))
        variable["result"] = res;
    }
  }
  rm(file);
  if(obj)
    Destruct(obj);
  return TRUE;
}

int Xforall(string str)
{
  int i, s, t, u;
  string pat, cmd, arg, *strs, *files, fh, fr, fe, ft, ff;
  
  SECURE2(TRUE);
  USAGE2(str, "xfo(rall) <filepattern> <command>");
  if(sscanf(str, "%s %s", pat, arg)!=2)
    return FALSE;
  files=long_get_dir(pat, FALSE);
  if(!(s=sizeof(files)))
  {
    WDLN("No matching files found");
    return TRUE;
  }
  strs=old_explode(files[0], "/");
  fh="/";
  if(t=sizeof(strs)-1)
    fh+=implode(strs[0..t-1], "/");
  for(i=0; i<s; i++)
  {
    ft=old_explode(files[i], "/")[t];
    if((u=sizeof(strs=old_explode(ft, ".")))&&--u)
    {
      ff=implode(strs[0..u-1], ".");
      fr=fh+"/"+ff;
      fe=strs[u];
    }
    else
    {
      fe="";
      ff=ft;
      fr=files[i];
    }
    cmd=string_replace(arg, "!!", files[i]);
    cmd=string_replace(cmd, "!e", fe);
    cmd=string_replace(cmd, "!f", ff);
    cmd=string_replace(cmd, "!h", fh);
    cmd=string_replace(cmd, "!r", fr);
    cmd=string_replace(cmd, "!t", ft);
    if(!(Command(cmd)))
      break;
  }
  return TRUE;
}

int Xgoto(string str)
{
  object obj, tmp;
  
  SECURE2(TRUE);
  USAGE1(str, "xgo(to) [object]");
  if(!str) str="~/workroom";
  if(!(obj=XFindObj(str)))
  {
    if(!(str=XFindFile(str)))
      return TRUE;
    if(catch(call_other(str, "???")))
      return TRUE;
    obj=find_object(str);
  }
  tmp=obj;
  while(obj&&living(obj))
    obj=ENV(obj);
  cloner->move(obj ? obj : tmp, M_TPORT);
  return TRUE;
}

int Xgrep(string str)
{
  int i, s, t, mode;
  string *files, *ts;
  
  SECURE2(TRUE);
  TK("Xgrep: str: "+(str?str:"(NULL)"));
  mode=0;
  if(!pipe_in)
  {
    USAGE2(str, "xgr(ep) [-i] [-v] <regexp> <filepattern>");
    if(!(ts=old_explode(str, " "))||!(s=sizeof(ts)))
      return FALSE;
    while(ts[0][0]=='-')
    {
      if(s<3)
      {
	WDLN("Too few arguments to xgrep");
	return FALSE;
      }
      switch(ts[0])
      {
	case "-v":
	  mode|=XGREP_REVERT;
	  ts=ts[1..];
	  s--;
	  break;
	case "-i":
	  mode|=XGREP_ICASE;
	  ts=ts[1..];
	  s--;
	  break;
	case "-vi":
	case "-iv":
	  mode|=XGREP_REVERT;
	  mode|=XGREP_ICASE;
	  ts=ts[1..];
	  s--;
	  break;
	default:
	  WDLN("Unknown option "+ts[0]+" given to xgrep");
	  return FALSE;
      }
    }
    str=implode(ts[0..s-2], " ");
  }
  else
  {
    if(!((ts=old_explode(str, " "))&&(s=sizeof(ts))))
      USAGE3("xgr(ep) [-i] [-v] <regexp>");
    while(ts[0][0]=='-')
    {
      if(s<2)
      {
	WDLN("Too few arguments to xgrep");
	return FALSE;
      }
      switch(ts[0])
      {
	case "-v":
	  mode|=XGREP_REVERT;
	  ts=ts[1..];
	  s--;
	  break;
	case "-i":
	  mode|=XGREP_ICASE;
	  ts=ts[1..];
	  s--;
	  break;
	case "-iv":
	case "-vi":
	  mode|=XGREP_REVERT;
	  mode|=XGREP_ICASE;
	  ts=ts[1..];
	  s--;
	  break;
	default:
	  WDLN("Unknown option "+ts[0]+" given to xgrep");
	  return FALSE;
      }
    }
    str=implode(ts[0..s-1], " ");
  }
  
  if(mode&XGREP_ICASE)
    str=lower_case(str);
  if(!(regexp(({"dummy"}), str)))
  {
    WDLN("Bad regular expression");
    return TRUE;
  }
  if(!pipe_in)
  {
    if(file_size(TMP_FILE)>0)
      rm(TMP_FILE);
    if((t=sizeof(files=long_get_dir(XFile(ts[s-1]), FALSE)))&&
       (file_size(files[0])>=0))
    {
      for(i=0; i<t; i++)
	XGrepFile(str, files[i], mode);
      if(pipe_out&&pipe_of)
      {
	PIPE_DELETE(pipe_of);
	if(!pipe_ovr)
	{
	  write_file(pipe_of,read_file(TMP_FILE,0,PIPE_MAX));
	  rm(TMP_FILE);
	}
	else
	  rename(TMP_FILE,pipe_of);
      }
      else
      {
	W(read_file(TMP_FILE,0,PIPE_MAX));
	rm(TMP_FILE);
      }
    }
    else
      WDLN("Cannot read file(s)");
  }
  else
  {
    string *tmp;
    if(file_size(pipe_if)<0)
    {
      WDLN("Missing input to xgrep");
      return TRUE;
    }
    TK("Xgrep: str: "+str+" mode: "+mode);
    s=sizeof(tmp=old_explode(read_file(pipe_if,0,PIPE_MAX),"\n"));
    PIPE_DELETE(pipe_of);
    for(i=0;i<s;i++)
    {
      // TK(tmp[i]+"<->"+str);
      if(sizeof(regexp(({(mode&XGREP_ICASE?lower_case(tmp[i]):tmp[i])}),str)))
      {
	// TK("Xgrep: matched!");
	if(!(mode&XGREP_REVERT))
	  if(pipe_out&&pipe_of)
	    write_file(pipe_of,tmp[i]+"\n");
	  else
	    WLN(tmp[i]);
      }
      else
	if(mode&XGREP_REVERT)
	  if(pipe_out&&pipe_of)
	    write_file(pipe_of,tmp[i]+"\n");
	  else
	    WLN(tmp[i]);
    }
  }
  return TRUE;
}

int Xhbeats(string str)
{
  object *hbeatinfo;
  string tmp;
  int i, s;
  
  SECURE2(TRUE);
  TK("Xhbeats: str: "+(str?str:"(NULL)"));
  if(!pipe_out)
  {
    USAGE1(str, "xhb(eats) [search pattern]");
    if(!str)
      str=RNAME(cloner);
    else if(!regexp(({"dummy"}), str))
    {
      WDLN("Bad regular expression");
      return TRUE;
    }
  }
  else
  {
    USAGE1(str,"xhb(eats)");
    if(str&&str!="")
    {
      WDLN("More arguments than expected");
      return TRUE;
    }
  }
  s=sizeof(hbeatinfo=heart_beat_info());
  PIPE_DELETE(pipe_of);
  for(i=0; i<s; i++)
    if(hbeatinfo[i]&&objectp(hbeatinfo[i]))
    {
      tmp=ObjFile(hbeatinfo[i]);
      if(sizeof(regexp(({tmp}), str)))
	if(pipe_out&&pipe_of)
	  write_file(pipe_of, tmp+"\n");
	else
	  WLN(tmp);
    }
  return TRUE;
}

int Xhead(string str)
{
  int lines;
  string *tmp, file;

  SECURE2(TRUE);
  TK("Xhead: str: "+(str?str:"(NULL)"));
  if(!pipe_in)
  {
    USAGE2(str, "xhead <-#> <file>");
    if(sscanf(str,"-%d %s",lines,file)!=2)
	return FALSE;
    if(!(file=XFindFile(file)))
      return FALSE;
  }
  else
  {
    USAGE2(str, "xhead <-#>");
    if(sscanf(str,"-%d",lines)!=1)
      return FALSE;
    if (file_size(file=pipe_if)<0)
    {
      WDLN("Missing input to xhead");
      return TRUE;
    }
  }
  tmp=old_explode(read_file(file,0,PIPE_MAX),"\n")[0..lines-1];
  if(pipe_in&&pipe_if==PIPE_FILE)
    rm(PIPE_FILE);
  if (pipe_out&&pipe_of)
  {
    PIPE_DELETE(pipe_of);
    write_file(pipe_of,implode(tmp,"\n")+"\n");
  }
  else
    WDLN(implode(tmp,"\n"));
  return TRUE;
}

int Xhelp(string str)
{
  SECURE2(TRUE);
  USAGE1(str, "xhe(lp)");
  XMoreFile(TOOL_MANPAGE, FALSE);
  return TRUE;
}

int Xids(string str)
{
  object obj;
  
  SECURE2(TRUE);
  USAGE2(str, "xid(s) <object>");
  TK("Xids: str: "+(str?str:"(NULL)"));
  if((obj=XFindObj(str)))
    WLN("UID=\""+getuid(obj)+"\" / EUID=\""+geteuid(obj)+"\"");
  return TRUE;
}

int Xinfo(string str)
{
  object obj;
  
  SECURE2(TRUE);
  USAGE2(str, "xinf(o) <object>");
  TK("Xinfo: str: "+(str?str:"(NULL)"));
  if((obj=XFindObj(str)))
  {
    if(is_player(obj))
    {
      WLN(PlayerWho(obj));
      WLN(PlayerMail(obj, FALSE));
      WLN(PlayerIP(obj, FALSE));
      WLN(PlayerRace(obj, FALSE));
      WLN(PlayerDomain(obj, FALSE));
      WLN(PlayerStats(obj, FALSE));
      WLN(PlayerSnoop(obj, FALSE));
    }
    else
      WDLN("Sorry, this is not a player");
  }
  return TRUE;
}

int Xinherit(string str)
{
  int s;
  object obj;
  string *strs, *inlist;
  
  SECURE2(TRUE);
  USAGE2(str, "xinh(erit) <object> [function]");
  TK("Xinherit: str: "+str);
  if(!(strs=strip_explode(str, " ")))
    return FALSE;
  if(obj=XFindObj(strs[0]))
  {
    inlist=inherit_list(obj);
    s=sizeof(inlist);
    while(s--)
    {
      if(catch(call_other(inlist[s], "???")))
      {
	WDLN("Failed to load all inheritance objects");
	return TRUE;
      }
    }
    obj=find_object(inlist[0]);
    PIPE_DELETE(pipe_of);
    if(sizeof(strs)==1)
      Inheritance(obj ,0 ,"");
    else
      Inheritance(obj, strs[1], "");
  }
  return TRUE;
}

int Xinventory(string str)
{
  int i, short;
  object item;
  mixed who;
  
  SECURE2(TRUE);
  USAGE1(str, "xi [-s] [player]");
  TK("Xinventory: str: "+str);
  short=0;
  who=cloner;
  if(str&&str!="")
  {
    if(str=="-s")
    {
      short=1;
      who=cloner;
    }
    else if(sscanf(str,"-s %s",who))
    {
      short=1;
      who=XFindObj(who);
    }
    else if(sscanf(str,"%s",who))
    {
      short=0;
      who=XFindObj(who);
    }
    else
      who=cloner;
  }
  if(!who)
    return FALSE;
  PIPE_DELETE(pipe_of);
  if(!(pipe_out&&pipe_of))
    WLN(({string})who->name(WESSEN)+" Inventory:"+(short?" (short)":""));
  if(!short)
    if(pipe_out&&pipe_of)
      FORALL(item, who) PrintShort(ARIGHT(++i+". ", 4, " "), item, pipe_of);
    else
      FORALL(item, who) PrintShort(ARIGHT(++i+". ", 4, " "), item);
  else
    if(pipe_out&&pipe_of)
      FORALL(item, who) write_file(pipe_of,++i+". ["+object_name(item)+"]\n");
    else
      FORALL(item, who) WLN(++i+". ["+object_name(item)+"]");
  return TRUE;
}

int Xlag(string str)
{
  float *lag;
  object daemon;
  string lags;

  if(!(daemon=load_object(LAG_O_DAEMON)))
    lag=({-1.0,-1.0,-1.0});
  else
    lag=({float*})daemon->read_lag_data();

  lags =sprintf("Letzte 60 min: %.1f%%\n",lag[2]);
  lags+=sprintf("Letzte 15 min: %.1f%%\n",lag[1]);
  lags+=sprintf("Letzte Minute: %.1f%%\n",lag[0]);

  WLN(lags);
  return TRUE;
}

int Xlight(string str)
{
  int addlight;

  SECURE2(TRUE);
  USAGE1(str, "xli(ght) [light]");
  if(str)
  {
    if(!sscanf(str, "%d", addlight))
      return FALSE;
    xlight+=addlight;
    cloner->AddIntLight(addlight);
  }
  WDLN("Current light levels: "+TOOL_NAME+"="+xlight+", room="+
       ({int})ENV(cloner)->QueryIntLight());
  return TRUE;
}

int Xload(string str)
{
  int i;
  object obj, *inv, vroom;
  string file, errlog, error;
  
  SECURE2(TRUE);
  USAGE2(str, "xloa(d) <filename>");
  if(!(file=XFindFile(str)))
    return TRUE;
  errlog=ERR_FILE;
  if(file_size(errlog)>0)
    rm(errlog);
  if(obj=find_object(file))
  {
    if(catch(call_other(VOID, "???")))
    {
      WDLN("Error: cannot find "+VOID+" to rescue players");
      return TRUE;
    }
    else
      vroom = find_object(VOID);
    if(inv=filter(all_inventory(obj), #'is_player, ME))//'
      for(i=0; i<sizeof(inv); i++)
	MoveObj(inv[i], vroom, TRUE);
    Destruct(obj);
    WLN("Update and load: "+file);
  }
  else
    WLN("Load: "+file);
  if(error=catch(call_other(file, "???")))
  {
    W("Error: "+error[1..]);
    if(vroom)
      tell_object(vroom, "*** Failed to load room. You are in the void!\n");
  }
  else if(inv)
  {
    obj=find_object(file);
    for(i=0; i<sizeof(inv); i++)
      if(inv[i])
	MoveObj(inv[i], obj, TRUE);
  }
  return TRUE;
}

int Xlook(string str)
{
  object obj;
  string file;
  
  SECURE2(TRUE);
  USAGE1(str, "xloo(k) [object]");
  TK("Xlook: str: "+str);
  PIPE_DELETE(pipe_of);
  file = pipe_out&&pipe_of ? pipe_of : "";
  if(str&&str!="")
  {
    if((obj=XFindObj(str)))
      PrintObj(obj,file);
  }
  else
  {
    obj=ENV(cloner);
    PrintObj(obj,file);
  }
  return TRUE;
}

int Xlpc(string str)
{
  object obj;
  string file, error;
  int *ru1, *ru2, time;
  mixed res;
  
  SECURE2(TRUE);
  USAGE2(str, "xlp(c) <lpc code>");
  file=LPC_FILE+".c";
  if(file_size(file)>0)
    rm(file);
  if(obj=find_object(LPC_FILE))
    Destruct(obj);
  write_file(file,
      "#pragma no_warn_missing_return,strong_types,warn_deprecated,rtt_checks\n"
      "#include <properties.h>\n"
      "#include <thing/properties.h>\n"
      "#include <defines.h>\n"
      "#include <wizlist.h>\n"
      "#include <moving.h>\n"
      "#include \"/secure/wizlevels.h\"\n"
      +(file_size(PRIVATE_HEADER)>=0?"#include \""+PRIVATE_HEADER+"\"\n":"")+
      "object get(string str){return previous_object()->XFindObj(str);}\n"+
      "mixed eval(mixed me, mixed here){"+str+"}");
  if(error=catch(obj=clone_object(file)))
    W("Error: "+error[1..0]);
  else
  {
    ru1=rusage();
    error=catch(res=({mixed})obj->eval(cloner, ENV(cloner)));
    ru2=rusage();
    if(error)
      W("Error: "+error[1..]);
    else
    {
      time=ru2[0]-ru1[0]+ru2[1]-ru1[1];
      WDLN("Evaluation time: "+(time<0 ? 0 : time)+" ms");
      WLN("Result: "+mixed_to_string(res, MAX_RECURSION));
      if(objectp(res))
        variable["result"] = res;
    }
  }
  rm(file);
  if(obj)
    Destruct(obj);
  return TRUE;
}

int Xman(string str)
{
  string manpage;
  int i, found;
  
  SECURE2(TRUE);
  USAGE2(str, "xma(n) <manpage>");
  W("Man: ");
  for(i=0, found=0; i<sizeof(manpath); i++)
  {
    manpage=manpath[i]+str;
    if(file_size(manpage)>=0)
    {
      WLN(manpage);
      XMoreFile(manpage, FALSE);
      found=1;
      break;
    }
  }
  if(!found)
    WLN("- no help available -");
  return TRUE;
}

int Xmore(string str)
{
  string *args, file;
  int line;
  
  SECURE2(TRUE);
  TK("Xmore: str: "+str);
  if (!pipe_in)
  {
    USAGE2(str, "xmor(e) <filename> [start]");
    switch(sizeof(args=strip_explode(str, " ")))
    {
      case 1:
      moreoffset=1;
      break;
      case 2:
      sscanf(args[1], "%d", line);
      moreoffset= line>0 ? line : 1;
      break;
      default:
      return FALSE;
    }
    if(file=XFindFile(args[0]))
      XMoreFile(file, TRUE);
  }
  else
  {
    if(file_size(pipe_if)<0)
    {
      WDLN("Missing input to xmore");
      return TRUE;
    }
    if (!str||str=="")
      line=1;
    else if (sscanf(str, "%d", line)!=1)
      USAGE3("xmor(e) [start]");
    moreoffset= line>0 ? line : 1;
    XMoreFile(pipe_if, TRUE);
  }
  return TRUE;
}

int Xmove(string str)
{
  object obj1, obj2;
  string what, into;
  
  SECURE2(TRUE);
  USAGE2(str, "xmov(e) <object> into <object>");
  if((sscanf(str, "%s into %s", what, into)==2)&&
     (obj1=XFindObj(what))&&(obj2=XFindObj(into)))
    MoveObj(obj1, obj2, FALSE);
  return TRUE;
}

int Xmsg(string str)
{
  string tmp;
  
  SECURE2(TRUE);
  TK("Xmsg: str: "+str);
  USAGE1(str, "xms(g) [to <object>|all]");
  if(!str||str=="")
  {
    WDLN("Send message into room");
    say("Message from "+CRNAME(cloner)+":\n");
    if(pipe_in&&pipe_if)
      say(read_file(pipe_if,0,PIPE_MAX));
    else
    {
      WDLN("End message with \".\" or \"**\"");
      input_to("XMsgSay");
    }
    return TRUE;
  }
  else if(sscanf(str, "to %s", tmp))
  {
    if(msgto=XFindObj(tmp))
    {
      PrintShort("Send message to: ", msgto);
      tell_object(msgto, "Message from "+CRNAME(cloner)+" to you:\n");
      if(pipe_in&&pipe_if)
	tell_object(msgto,read_file(pipe_if,0,PIPE_MAX));
      else
      {
	WDLN("End message with \".\" or \"**\"");
	input_to("XMsgTell");
      }
    }
    return TRUE;
  }
  else if(str=="all")
  {
    WDLN("Send message to all players");
    shout("Message from "+CRNAME(cloner)+" to all:\n");
    if(pipe_in&&pipe_if)
      shout(read_file(pipe_if,0,PIPE_MAX));
    else
    {
      WDLN("End message with \".\" or \"**\"");
      input_to("XMsgShout");
    }
    return TRUE;
  }
  return FALSE;
}

int Xmtp(string str)
{
  int s;
  string *strs, opt, dir, file;

  SECURE2(TRUE);
  USAGE2(str, "xmt(p) [options] <directory> <filename>");
  s=sizeof(strs=old_explode(str, " "));
  if(s<2)
    return FALSE;
  else if(s==2)
    opt="";
  else
    opt=implode(strs[0..s-3], " ");
  if(!(dir="/"+({int})MASTER->valid_read(strs[s-2], geteuid(),
					  "get_dir", ME))) {
    WDLN("No permission to open directory for reading");
    return TRUE;
  }
  if(!(file="/"+({int})MASTER->valid_write(strs[s-1], geteuid(),
					    "write_file", ME))) {
    WDLN("No permission to open script file for writing");
    return TRUE;
  }
  if(file_size(dir)!=-2 || file_size(file)==-2)
    return FALSE;
  XmtpScript(dir, file, opt);
  WDLN("Done");
  return TRUE;
}

int Xproc(string str)
{
  string *strs;

  SECURE2(TRUE);
  USAGE1(str, "xproc [-c] [-l] [-m] [-u] [-v]");

  if(file_size(TOOL_PATH+"/proc")!=-2)
  {
    WLN("Sorry, no /proc information available!");
    return TRUE;
  }
  
  if(!str||str==""||!(sizeof(strs=old_explode(str, " "))))
  {
    WLN("Load averages:");
    cat(TOOL_PATH+"/proc/loadavg");
    return TRUE;
  }
  
  while(sizeof(strs))
  {
    switch(strs[0])
    {
      case "-c":
      WLN("CPU info:");
      cat(TOOL_PATH+"/proc/cpuinfo");
      break;
      case "-l":
      WLN("Load averages:");
      cat(TOOL_PATH+"/proc/loadavg");
      break;
      case "-m":
      WLN("Memory usage:");
      cat(TOOL_PATH+"/proc/meminfo");
      break;
      case "-u":
      WLN("Uptime:");
      cat(TOOL_PATH+"/proc/uptime");
      break;
      case "-v":
      WLN("Version:");
      cat(TOOL_PATH+"/proc/version");
      break;
      default:
      WLN("Unknown option: "+strs[0]);
    }
    strs=strs[1..];
  }
  return TRUE;
}

int Xprof(string str)
{
  string *funcs, inh, tmp;
  mixed *data, *d;
  mapping xpr;
  object obj;
  int i, rn;

  SECURE2(TRUE);
  USAGE2(str, "xprof <<-c>|<-C> <file>>|<object>");
  if(str[0..2]=="-c "||str[0..2]=="-C ")
  {
    rn=(str[1]=='C');
    if(str=XFindFile(str[3..]))
    {
      inh=str=str[0..<3];
      if(obj=find_object(inh))
	Destruct(obj);
      if(catch(call_other(inh,"???")))
	return TRUE;
      obj=find_object(inh);
      if(rn)
      {
	inh+=".xprof.c";
	rm(inh);
	str+=".c";
	rename(str, inh);
      }
      else
      {
	str=XPROF_FILE;
	rm(str);
      }
      tmp="inherit \""+inh+"\";\n#include \""+XPROF_MACRO+"\"\n";
      funcs=m_indices(mkmapping(functionlist(obj, 0x08000001)))-({"__INIT"});
      for(i=sizeof(funcs); i--;)
	tmp+="F("+funcs[i]+",\""+funcs[i]+"\","+
	  "(A0,A1,A2,A3,A4,A5,A6,A7,A8,A9))\n";
      write_file(str, tmp);
      WDLN("Done");
    }
  }
  else if(obj=XFindObj(str))
  {
    if(xpr=({mixed})obj->__query_xprof_data__())
    {
      funcs=m_indices(xpr);
      data=m_values(xpr);
      rm(TMP_FILE);
      str="Evaluation cost per function:\nNAME			   "+
	" CALLS |  TOT.EXCL.  TOT.INCL. | REL.EXCL. REL.INCL.\n";
      for(i=sizeof(funcs); i--;)
      {
	if(d=data[i]) {
	  funcs[i]=ALEFT(""+funcs[i]+" ",25,".")+ARIGHT(" "+d[0], 6,".")+" | "+
	    ARIGHT(" "+d[1],10,".")	+" "+ARIGHT(" "+d[3],10,".")+" | "+
	    ARIGHT(" "+d[1]/d[0],9,".") +" "+ARIGHT(" "+d[3]/d[0],9,".");
	}
      }
      str+=implode(sort_array(funcs, "string_compare", ME),"\n")+"\n";
      write_file(TMP_FILE,str);
      str="\nElapsed time per function:\nNAME			  "+
	" CALLS |  TOT.EXCL.  TOT.INCL. | REL.EXCL. REL.INCL.\n";
      funcs=m_indices(xpr);
      for(i=sizeof(funcs); i--;)
      {
	if(d=data[i])
	{
	  funcs[i]=ALEFT(""+funcs[i]+" ",25,".")+ARIGHT(" "+d[0], 6,".")+" | "+
	    ARIGHT(" "+(d[2]+5)/10+"ms",10,".")+" "+
	    ARIGHT(" "+(d[4]+5)/10+"ms",10,".")+" | "+
	    ARIGHT(" "+d[2]/d[0]+"us",9,".")+" "+
	    ARIGHT(" "+d[4]/d[0]+"us",9,".");
	}
      }
      str+=implode(sort_array(funcs, "string_compare", ME),"\n")+"\n";
      write_file(TMP_FILE,str);
      XMoreFile(TMP_FILE, FALSE);
    }
    else
      WDLN("No profiling information available");
  }
  return TRUE;
}

int Xprops(string str)
{
  int flag;
  object obj;
  string *tmp;
  
  SECURE2(TRUE);
  USAGE2(str, "xprop(s) [-f|-m] <object>");
  TK("Xprops: str: "+str);
  tmp=old_explode(str," ");
  switch(tmp[0])
  {
    case "-f":
    {
      flag = 1;
      tmp=tmp[1..];
      break;
    }
    case "-m":
    {
      flag = 2;
      tmp=tmp[1..];
      break;
    }
  }
  str=implode(tmp," ");
  if((obj=XFindObj(str)))
    DumpProperties(obj,flag);
  return TRUE;
}

int Xquit(string str)
{
  SECURE2(TRUE);
  USAGE1(str, "xqu(it)");
  str=object_name(ENV(cloner));
  if(member(str, '#')<0)
    cloner->set_home(str);
  cloner->quit();
  return TRUE;
}

int Xscan(string str)
{  
  SECURE2(TRUE);
  USAGE1(str, "xsc(an)");
  W("Destructed variable(s): ");
  string* oldvars=m_indices(variable);
  VarCheck(FALSE);
  string* desvars=oldvars-m_indices(variable);
  desvars = map(desvars, function string (string k)
      {return "$"+k;} );
  W(CountUp(desvars));
  WLN("");
  return TRUE;
}

int Xset(string str)
{
  mixed obj;
  string name, tmp;
  
  SECURE2(TRUE);
  USAGE1(str, "xse(t) [$<name>=<object>]");
  if(str) {
    if(member(({"$me","$m","$here","$h"}),str)!=-1)
      WDLN("Sorry, this is a reserved variable ["+str+"]");
    else if(sscanf(str, "$%s=%s", name, tmp))
    {
      if(obj=XFindObj(tmp))
      {
        variable[name] = obj;
        WLN(" $"+name+"="+ObjFile(obj));
      }
    }
    else
      return FALSE;
  }
  else
  {
    VarCheck(FALSE);
    WLN("Current settings:");
    foreach(string key, mixed val : variable)
    {
      if (val)
        WLN(" $"+key+"="+ObjFile(val));
      else
        m_delete(variable, key);
    }
    WLN(" $me="+ObjFile(cloner));
    WLN(" $here="+ObjFile(ENV(cloner)));
  }
  return TRUE;
}

int Xsh(string str)
{
  SECURE2(TRUE);
  USAGE1(str, "xsh <filename>");
  if(scriptline)
  {
    WDLN("Cannot execute another script file until last execution has ended");
    return TRUE;
  }
  if(!(str=XFindFile(str)))
    return TRUE;
  str=read_file(str, 1, 1000);
  if(!(scriptline=old_explode(str, "\n")))
  {
    WDLN("Bad script file");
    return TRUE;
  }
  scriptsize=sizeof(scriptline);
  XExecFile(NULL);
  return TRUE;
}

int Xsort(string str)
{
  string *tmp,file;
  int s,reverse;

  SECURE2(TRUE);
  TK("Xsort: str: "+str);
  if(!pipe_in)
  {
    USAGE2(str, "xso(rt) [-r] [file]");
    if(!(tmp=old_explode(str, " "))||(s=sizeof(tmp))>2)
      return FALSE;
    if(tmp[0]=="-r")
      if(s==1)
	return FALSE;
      else
      {
	reverse=TRUE;
	tmp=tmp[1..];
      }
    else if(s>1)
      return FALSE;
    if(!(file=XFindFile(tmp[0])))
    {
      WDLN("Can't find file");
      return TRUE;
    }
  }
  else
  {
    if(str&&str!="")
      if(str=="-r")
	reverse=TRUE;
      else
	USAGE3("xso(rt) [-r]");
    if (file_size(file=pipe_if)<0)
    {
      WDLN("Missing input to xsort");
      return TRUE;
    }
  }
  tmp=old_explode(read_file(file,0,PIPE_MAX),"\n");
  if(pipe_in&&pipe_if==PIPE_FILE)
    rm(PIPE_FILE);
  tmp=sort_array(tmp,reverse?#'<:#'>);
    if (pipe_out&&pipe_of)
      write_file(pipe_of,implode(tmp,"\n")+"\n");
    else
      WDLN(implode(tmp,"\n"));
  return TRUE;
}

int Xtail(string str)
{
  string *tmp,file,sign;
  int lines;

  if (!str) {
    WDLN("Missing input to xtail");
    return TRUE;
  }

  sign="-";
  lines=10;
  SECURE2(TRUE);
  TK("Xtail: str: "+str);
  if(!pipe_in)
  {
    if(sscanf(str,"-%d %s",lines,file)==2)
      sign="-";
    else if(sscanf(str,"+%d %s",lines,file)==2)
      sign="+";
    else
      file=str;

    if(!(file=XFindFile(file)))
    {
      WDLN("Can't find file");
      return TRUE;
    }
  }
  else
  {
    if(sscanf(str,"-%d",lines)==1)
      sign="-";
    else if(sscanf(str,"+%d",lines)==1)
      sign="+";
    if (file_size(file=pipe_if)<0)
    {
      WDLN("Missing input to xtail");
      return TRUE;
    }
  }

  if(sign=="-")
  {
    if(!lines)
      return TRUE;
  }

  if(file_size(file)>50000)
  {
    WDLN("File too large");
    return TRUE;
  }

  if(sign=="+")
    tmp=old_explode(read_file(file,0,PIPE_MAX),"\n")[lines..];
  else
    tmp=old_explode(read_file(file,0,PIPE_MAX),"\n")[<lines..];
  if(pipe_in&&pipe_if==PIPE_FILE)
    rm(PIPE_FILE);
  if (pipe_out&&pipe_of)
    write_file(pipe_of,implode(tmp,"\n")+"\n");
  else
    WLN(implode(tmp,"\n"));
  return TRUE;
}

int Xtool(string str)
{
  int m;
  string tmp;
  object obj;
  
  SECURE2(TRUE);
  TK("Xtool: str: "+str);
  USAGE1(str, "xto(ol) [update|first=<on|off>|protect=<on|off>|"+
	 "invcheck=<on|off>|\n"+
	 "\t\tvarcheck=<on|off>|scanchk=<on|off>|short=<on|off>|\n"+
	 "\t\techo=<on|off>|more=<amount>|kill|news|save|load|reset]");
  if(str&&str!="")
  {
    if(sscanf(str, "more=%d", m))
    {
      if(m<5)
	WDLN("Sorry, amount of lines should be more then 5");
      else
      {
	WDLN("Setting amount of displayed lines to "+m);
	morelines=m;
      }
    }
    else
      switch(str)
      {
	case "update":
	if(obj=find_object(TOOL_PATH))
	  Destruct(obj);
	if(catch(obj=clone_object(TOOL_PATH)))
	  WLN("Updating "+TOOL_TITLE+" failed!");
	else
	  obj->update_tool(AUTOLOAD_ARGS, cloner);
	return TRUE;
	case "first=on":
	MODE_ON(MODE_FIRST);
	move(cloner);
	WDLN("Automatic moving into pole position of inventory turned on");
	break;
	case "first=off":
	MODE_OFF(MODE_FIRST);
	WDLN("Automatic moving into pole position of inventory turned off");
	break;
	case "protect=on":
	MODE_ON(MODE_PROTECT);
	WDLN("Protection from forces and illegal moves turned on");
	break;
	case "protect=off":
	MODE_OFF(MODE_PROTECT);
	WDLN("Protection from forces and illegal moves turned off");
	break;
	case "invcheck=on":
	MODE_ON(MODE_INVCHECK);
	WDLN("Automatic checking for new objects in inventory turned on");
	break;
	case "invcheck=off":
	MODE_OFF(MODE_INVCHECK);
	WDLN("Automatic checking for new objects in inventory turned off");
	break;
	case "varcheck=on":
	MODE_ON(MODE_VARCHECK);
	VarCheck(TRUE);
	WDLN("Automatic variable checking turned on");
	break;
	case "varcheck=off":
	MODE_OFF(MODE_VARCHECK);
	WDLN("Automatic variable checking turned off");
	break;
	case "scanchk=on":
	MODE_ON(MODE_SCANCHK);
	WDLN("Scan check turned on");
	break;
	case "scanchk=off":
	MODE_OFF(MODE_SCANCHK);
	WDLN("Scan check turned off");
	break;
	case "echo=on":
	MODE_ON(MODE_ECHO);
	WDLN("Echoing of multiple command execution turned on");
	break;
	case "echo=off":
	MODE_OFF(MODE_ECHO);
	WDLN("Echoing of multiple command execution turned off");
	break;
	case "short=on":
	MODE_ON(MODE_SHORT);
	WDLN("Use of short descriptions turned on");
	break;
	case "short=off":
	MODE_OFF(MODE_SHORT);
	WDLN("Use of short descriptions turned off");
	break;
	case "kill":
	WDLN(TOOL_NAME+" selfdestructs");
	destruct(ME);
	break;
	case "news":
	XMoreFile(TOOL_NEWS, FALSE);
	break;
	case "reset":
	WDLN("Resetting "+TOOL_TITLE);
	ME->__INIT();
	break;
	case "load":
	if(file_size(SAVE_FILE+".o")>0)
	{
	  WDLN("Loading "+TOOL_TITLE+" settings");
	  restore_object(SAVE_FILE);
	}
	else
	  WDLN("Sorry, cannot find file to load settings");
	break;
	case "save":
	WDLN("Saving "+TOOL_TITLE+" settings");
	save_object(SAVE_FILE);
	break;
	default:
	return FALSE;
      }
  }
  else
  {
    WLN(TOOL_TITLE+" settings:");
    tmp= (" first .... = "+(MODE(MODE_FIRST) ? "on\n" : "off\n"));
    tmp+=(" protect .. = "+(MODE(MODE_PROTECT) ? "on\n" : "off\n"));
    tmp+=(" invcheck . = "+(MODE(MODE_INVCHECK) ? "on\n" : "off\n"));
    tmp+=(" varcheck . = "+(MODE(MODE_VARCHECK) ? "on\n" : "off\n"));
    tmp+=(" scanchk .. = "+(MODE(MODE_SCANCHK) ? "on\n" : "off\n"));
    tmp+=(" echo ..... = "+(MODE(MODE_ECHO) ? "on\n" : "off\n"));
    tmp+=(" short .... = "+(MODE(MODE_SHORT) ? "on\n" : "off\n"));
    tmp+=(" more ..... = "+morelines);
    WLN(sprintf("%-80#s", tmp));
  }
  return TRUE;
}

int Xtrace(string str)
{
  string file;
  object obj;
  
  SECURE2(TRUE);
  USAGE1(str, "xtrac(e) <object>");
  if(!str||str=="")
  {
    trace(0);
    WDLN("Ending trace ["+short_path("/"+traceprefix(0))+"]");
  }
  else if(obj=XFindObj(str))
  {
    PrintShort("Tracing: ", obj);
    file=object_name(obj);
    file=file[1..<1];
    traceprefix(file);
    trace(TRACE_LEVEL);
  }
  return TRUE;
}

int Xtrans(string str)
{
  object obj;
  
  SECURE2(TRUE);
  USAGE2(str, "xtran(s) <object>");
  if((obj=XFindObj(str))&&ENV(obj))
  {
    tell_room(ENV(obj), CRNAME(obj)+" vanishes.\n");
    tell_room(ENV(cloner), CRNAME(obj)+
	      " is teleported into this room by "+CRNAME(cloner)+".\n");
    MoveObj(obj, ENV(cloner), TRUE);
    tell_object(obj, "You've been teleported to "+CRNAME(cloner)+".\n");
  }
  else
    WDLN("Failed to teleport object");
  return TRUE;
}

int Xupdate(string str)
{
  object obj;
  string file;
  
  SECURE2(TRUE);
  USAGE2(str, "xup(date) <filename>");
  if(!(file=XFindFile(str)))
    return TRUE;
  if(file[<2.. <1]==".c")
    file=file[0.. <3];
  if((obj=XFindObj(file))&&(obj=find_object(pure_file_name(obj))))
  {
    PrintShort("Updating: ", obj);
    Destruct(obj);
  }
  else
    WDLN("Object not found");
  return TRUE;
}

int Xuptime(string str)
{
  return TRUE;
}

int Xwc(string str)
{
  string file;
  string tmp, *tmp2;
  int i, chars, words, lines, nchars, nwords, nlines;
  
  SECURE2(TRUE);
  TK("Xwc: str: "+str);
  chars=words=lines=FALSE;
  if(!pipe_in)
  {
    USAGE2(str, "xwc [-cwl] <file>");
    if(str[0]=='-')
    {
      while((str=str[1..])[0]!=' '&&sizeof(str))
	switch(str[0])
	{
	  case 'c':
	  chars=TRUE;
	  break;
	  case 'w':
	  words=TRUE;
	  break;
	  case 'l':
	  lines=TRUE;
	  break;
	  default:
	  return FALSE;
	}
      str=str[1..];
    }
    if(!(file=XFindFile(str)))
    {
      WDLN("Can't find file");
      return TRUE;
    }
  }
  else
  {
    USAGE1(str,"xwc [-cwl]");
    if(str)
      if(str[0]=='-')
      {
	while((str=str[1..])[0]!=' '&&sizeof(str))
	  switch(str[0])
	  {
	    case 'c':
	    chars=TRUE;
	    break;
	    case 'w':
	    words=TRUE;
	    break;
	    case 'l':
	    lines=TRUE;
	    break;
	    default:
	    return FALSE;
	  }
      }
      else
	return FALSE;
    if(file_size(file=pipe_if)<0)
    {
      WDLN("Missing input to xwc");
      return TRUE;
    }
  }
  if(!(chars|words|lines))
    chars=words=lines=TRUE;
  nlines=nwords=nchars=0;
  tmp=read_file(file,0,PIPE_MAX);
  tmp2=explode(tmp,"\n");
  if(lines)
  {
    nlines=sizeof(tmp2);
    if(tmp2[<1]==""&&nlines>1)
      nlines--;
  }
  if(words)
    for(i=sizeof(tmp2)-1;i>=0;i--)
    {
      TK(sprintf("%O",tmp2[i]));
      if(tmp2[i]!="")
	nwords+=sizeof(regexplode(tmp2[i],"[	]")-({""," ","	"}));
      TK(sprintf("%O",regexplode(tmp2[i],"[	]")-({""," ","	"})));
      TK("nwords: "+nwords);
    }
  if(chars)
    for(i=sizeof(tmp2)-1;i>=0;i--)
      nchars+=sizeof(tmp2[i])+1;
  tmp2=0;
  tmp="";
  if(lines)
    tmp+=sprintf("%7d",nlines)+" ";
  if(words)
    tmp+=sprintf("%7d",nwords)+" ";
  if(chars)
    tmp+=sprintf("%7d",nchars)+" ";
  if(file!=pipe_if)
    tmp+=file;
  WLN(tmp);
  return TRUE;
}

int cmdavg_compare(string a, string b)
{
  int x,y;
  sscanf(a,"%~s cmdavg: %d",x);
  sscanf(b,"%~s cmdavg: %d",y);
  return x==y?0:(x>y?1:-1);
}

int Xwho(string opt)
{
  string *strs,func;
  
  SECURE2(TRUE);
  TK("Xwho: opt: \""+opt+"\"");
  USAGE1(opt, "xwh(o) [mail|ip|race|guild|domain|stats|snoop]");
  func="string_compare";
  if(!opt||opt=="")
    strs=map(users(), "PlayerWho", ME);
  else
    switch(opt)
    {
      case "mail":
      strs=map(users(), "PlayerMail", ME, TRUE);
      break;
      case "ip":
      strs=map(users(), "PlayerIP", ME, TRUE);
      break;
      case "race":
      case "guild":
      strs=map(users(), "PlayerRace", ME, TRUE);
      break;
      case "domain":
      strs=map(users(), "PlayerDomain", ME, TRUE);
      break;
      case "stat":
      case "stats":
      strs=map(users(), "PlayerStats", ME, TRUE);
      break;
      case "snoop":
      strs=map(users(), "PlayerSnoop", ME, TRUE);
      break;
      case "cmdavg":
      strs=map(users(), "PlayerCmdAvg", ME, TRUE);
      func="cmdavg_compare";
      break;
      default:
      return FALSE;
    }
  strs=sort_array(strs, func, ME);
  if(pipe_out&&pipe_of)
    write_file(pipe_of,implode(strs,"\n")+"\n");
  else
    WLN(implode(strs,"\n"));
  return TRUE;
}
