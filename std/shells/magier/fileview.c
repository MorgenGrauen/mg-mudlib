// MorgenGrauen MUDlib
//
// fileview.c
//
// $Id: fileview.c 9142 2015-02-04 22:17:29Z Zesstra $
#pragma strict_types
#pragma save_types
//#pragma range_check
#pragma no_clone
#pragma pedantic

#include <ansi.h>
#include <player/base.h>
#include <wizlevels.h>
#include <shells.h>
#include <daemon/mand.h>
#include <udp.h>
#define NEED_PROTOTYPES
#include <magier.h>
#include <thing/properties.h>
#include <player.h>

private nosave mapping colorstrings;
private nosave mapping oldman_result;

//                        ###################
//######################### INITIALISIERUNG #############################
//                        ###################

mixed _query_localcmds()
{
  return ({({"ls","_ls",0,LEARNER_LVL}),
           ({"more","_more",0,LEARNER_LVL}),
           ({"cat","_cat",0,LEARNER_LVL}),
           ({"head","_cat",0,LEARNER_LVL}),
           ({"tail","_cat",0,LEARNER_LVL}),
           ({"man","_man",0,LEARNER_LVL}),
           ({"rman","_rman",0,LEARNER_LVL}),
           ({"showprops","_showprops",0,WIZARD_LVL}),
           ({"grep","_grep",0,LEARNER_LVL})});
}


//                               ######
//################################ LS ###################################
//                               ######

//
// _ls: Der ls-Befehl: Verzeichnisse und Dateien anzeigen
// cmdline: Kommandozeile
//

//
// Funktionen zum Sortieren und fuer filter_ldfied/map_ldfied
//


private string _get_color(string color)
{
  return COLORS[lower_case(color)];
}


private void SetColorstrings()
{
  string tmp,term;
  mapping vars;
  vars=QueryProp(P_VARIABLES);
  colorstrings=m_allocate(0,2);
  switch(term=QueryProp(P_TTY))
  {
    case "vt100":
    case "ansi":
      if(stringp(vars["LS_DIR"]))
        tmp=implode(map(explode(vars["LS_DIR"],"+"),#'_get_color),
                    "");
      else
        tmp = (term == "ansi") ? ANSI_BLUE+ANSI_BOLD: ANSI_BOLD;
      colorstrings[DIR] = tmp+"%s"+(term == "vt100"?"/":"")+ANSI_NORMAL;
      if(term == "vt100") colorstrings[DIR, 1] = 1;
      if(stringp(vars["LS_OBJ"]))
        tmp=implode(map(explode(vars["LS_OBJ"],"+"),#'_get_color),
                    "");
      else
        tmp = (term == "ansi") ? ANSI_RED : ANSI_INVERS;
      colorstrings[OBJ] = tmp+"%s"+ANSI_NORMAL;
      if(stringp(vars["LS_VC"]))
        tmp=implode(map(explode(vars["LS_VC"],"+"),#'_get_color),
                    "");
      else
        tmp = (term == "ansi") ? ANSI_PURPLE : ANSI_INVERS;
      colorstrings[VC] = tmp+"%s"+ANSI_NORMAL;
      break;
    case "dumb":
      colorstrings[DIR] = "%s/"; colorstrings[DIR, 1] = 1;
      colorstrings[OBJ] = "%s*"; colorstrings[OBJ, 1] = 1;
      colorstrings[VC]  = "%s*"; colorstrings[VC , 1] = 1;
      break;
    default:
      colorstrings[DIR] = "%s";
      colorstrings[OBJ] = "%s";
      colorstrings[VC]  = "%s";
  }
  return;
}


private string _ls_output_short(mixed filedata,
                                int maxlen,int counter,int maxcount)
{
  string tmp;
  tmp=filedata[BASENAME];
  maxlen-=sizeof(tmp);
  switch(filedata[FILESIZE])
  {
    case -2: tmp=sprintf(colorstrings[DIR],tmp);
             maxlen-=colorstrings[DIR,1]; break;
    case -1: tmp=sprintf(colorstrings[VC],tmp);
             maxlen-=colorstrings[VC,1]; break;
    default: if (find_object(filedata[FULLNAME]))
             {
               maxlen-=colorstrings[OBJ,1];
               tmp=sprintf(colorstrings[OBJ],tmp); break;
             }
  }
  if (!maxcount) return tmp+"\n";
  return sprintf("%-*s%s",(maxlen+sizeof(tmp)),tmp,
	((counter++)==maxcount?(counter=0,"\n"):"  "));
}

private int _ls_maxlen(mixed filedata,int flags,int maxlen)
{
  string base;
  int size;
  base=filedata[BASENAME];
  if (!(flags&LS_A)&&(base[0]=='.')) return 0;
#if __VERSION__ < "3.2.9"
  if (sizeof(base)>maxlen) maxlen=sizeof(base);
#else
  maxlen=max(maxlen,sizeof(base));
#endif
  return 1;
}

private string _ls_output_long(mixed filedata, int flags,closure valid_read,
                               closure valid_write,closure creator_file)
{
  string *tmp,full,base,path,date,creator,group;
  int size,dir,ftime;
  object ob;
  
  base=filedata[BASENAME];
  if (!(sizeof(base))||((!(flags&LS_A))&&(base[0]=='.')))
    return 0;
  size=filedata[FILESIZE];
  path=filedata[PATHNAME];
  tmp=(string *)call_other(master(),"full_path_array",
                 filedata[FULLNAME],getuid());
  full=sprintf("/%s",implode(tmp,"/"));
  dir=(size==-2);
  ob=find_object(full);
  ftime=filedata[FILEDATE];
  date=dtime(ftime);
  date=sprintf("%s %s %s",date[9..11],date[5..6],
               ((time()-ftime)<31536000?date[19..23]:(" "+date[13..16])));
  creator="";
  group="";
  if (flags&LS_U)
  {
    creator=(string)call_other(master(),"creator_file",full);
    switch(creator)
    {
      case ROOTID: creator="root"; break;
      case BACKBONEID: creator="daemon"; break;
      default: if(!creator) creator="none"; break;
    }
  }
  if (flags&LS_G)
  {
    if (sizeof(tmp))
    {
      switch(tmp[0])
      {
      case WIZARDDIR: group="magier"; break;
      case "news": group="news"; break;
      case "mail": group="mail"; break;
      case "open": group="public"; break;
      case "p": group="project"; break;
      case DOMAINDIR: if (sizeof(tmp)>1) { group=tmp[1]; break; }
      default: group="mud"; break;
      }
    }
    else group="mud";
  }
  if (dir) base=sprintf(colorstrings[DIR],base);
  else
  {
    if (ob)
    {
      if (size==-1)
        base=sprintf(colorstrings[VC],base);
      else
        base=sprintf(colorstrings[OBJ],base);
    }
  }
  return sprintf(("%c%c%c%c %3d"+((flags&LS_U)?" %-24.24s":"%-0.1s")+
                 ((flags&LS_G)?" %-8.8s":"%0.1s")+" %8s %s %s\n"),
                 (dir?'d':'-'),
                 (!funcall(valid_read,full,getuid(),
                           "read_file",this_object())?'-':'r'),
                 (!funcall(valid_write,full,getuid(),
                           "write_file",this_object())?'-':'w'),
                 (ob?'x':'-'),
                 (dir?(sizeof((get_dir(full+"/*")||({}))-({".",".."}))):0),
                 creator,group,(dir?"-":size==-1?"<vc>":to_string(size)),
                                    date,base);
}


static int _ls(string cmdline)
{
  int flags,cmp,i,arg_size,size;
  int maxlen,counter,maxcount;
  string *args,output;
  mixed *tmp;
  closure output_fun,v_read,v_write,creator,sort_fun;
  
  cmdline=_unparsed_args()||"";
  args=parseargs(cmdline,&flags,LS_OPTS,1);
  if (flags==-1)
    return USAGE("ls [-" LS_OPTS "] [<Verzeichnis>] [<Verzeichnis2> ..]");
  SetColorstrings();
  output="";
  if (!sizeof(args)) args=({ QueryProp(P_CURRENTDIR) });
  if (!sizeof(args=file_list(args,MODE_LSA,0,"/")))
    return notify_fail("ls: Keine passenden Verzeichnisse gefunden.\n"), 0;
// Files sortieren die erste
  if (flags&LS_T) cmp=FILEDATE;
  else if (flags&LS_S) cmp=FILESIZE;
  else cmp=BASENAME; // =0 :-)
  sort_fun=lambda(({ 'a,'b }),({
    ((!cmp&&!(flags&LS_R))||(cmp&&(flags&LS_R))?#'>:#'<),
    ({#'[,'a,cmp}),({#'[,'b,cmp})}));
  args=sort_array(args,sort_fun);
// Ausgabeformat bestimmen
  if (flags&LS_L)
  {
    v_read=VALID_READ_CL;
    v_write=VALID_WRITE_CL;
    creator=CREATOR_CL;
  }
  arg_size=sizeof(args);
  if (arg_size>1||(arg_size&&args[0][FILESIZE]>=0))
  {
    if (flags&LS_L)
      tmp=map(args,#'_ls_output_long,flags,v_read,
                    v_write,creator)-({0});
    else
    {
      counter=0;maxlen=0;
      tmp=filter(args,#'_ls_maxlen,flags,&maxlen);
      if (maxlen>76) maxlen=76;
      maxcount=(78/(maxlen+2))-1;
      tmp=map(tmp,#'_ls_output_short,maxlen,&counter,maxcount);
    }
    output=sprintf("\n%d Dateien/Unterverzeichnisse:\n%s\n",
                   sizeof(tmp),implode(tmp,""));
  }
  for(i=0;i<arg_size;i++)
  {
    tmp=({});
    size=args[i][FILESIZE];
    if (size==-2)
    {
      tmp=file_list(({args[i][FULLNAME]+"/*"}),MODE_LSB,0,"/");
      tmp=sort_array(tmp,sort_fun);
      if (flags&LS_L)
        tmp=map(tmp,#'_ls_output_long,flags,v_read,
                      v_write,creator)-({0});
      else
      {
        counter=0;maxlen=0;
        tmp=filter(tmp,#'_ls_maxlen,flags,&maxlen);
        maxcount=(78/(maxlen+2));
        if (maxcount) maxcount--;
        tmp=map(tmp,#'_ls_output_short,maxlen,&counter,maxcount);
      }
      if (sizeof(tmp))
      {
        output+=sprintf("\n%s: Verzeichnis, %d Dateien/Unterverzeichnisse:\n",
                      args[i][FULLNAME],sizeof(tmp));
        output+=(implode(tmp,"")+((string)(tmp[<1][<1..<1])=="\n"?"":"\n"));
      }
      else
      {
        output+=sprintf("\n%s: Leeres Verzeichnis.\n",args[i][FULLNAME]);
      }
    }
  }
  More(output);
  return 1;
}

//                              ########
//############################### MORE ###################################
//                              ########
//
// _more_file: Mehrere Files hintereinander ausgeben
//

private void _more_file(string *arg)
{
  if (!sizeof(arg)) return;
  printf("more: Naechste Datei: %s\n",arg[0]);
  More(arg[0],1,#'_more_file,({ arg[1..]}));
  return;
}


private mixed _size_filter(mixed *arg)
{
  if (arg[FILESIZE]>0) return arg[FULLNAME];
  if (arg[FILESIZE]==0)
  {
    printf("%s: %s: Leere Datei.\n",query_verb()||"more",arg[FULLNAME]);
    return 0;
  }
  if (arg[FILESIZE]==-2)
    printf("%s: %s ist ein Verzeichnis.\n",query_verb()||"more",arg[FULLNAME]);
  else
    printf("%s: %s: Datei existiert nicht.\n", query_verb()||"more",
           arg[FULLNAME]);
  return 0;
}


//
// _more: Dateien anzeigen
// cmdline: Kommandozeile
//

static int _more(string cmdline)
{
  mixed *args;
  int flags;
  cmdline=_unparsed_args();
  args=parseargs(cmdline,&flags,"",1);
  if (flags==-1||!sizeof(args)) return USAGE("more <datei> [<datei2>..]");
  args=file_list(args,MODE_MORE,0,"/");
  if (!sizeof(args))
    return printf("more: %s: Keine passende Datei gefunden.\n",cmdline),1;
  args=map(args,#'_size_filter)-({0});
  if (sizeof(args)) _more_file(args);
  return 1;
}

//                         ###################
//########################## HEAD, TAIL, CAT #############################
//                         ###################

static int _cat(string cmdline)
{
  mixed *args;
  int flags;
  cmdline=_unparsed_args();
  args=parseargs(cmdline,&flags,"",1);
  if(flags==-1||!sizeof(args))
    return USAGE(query_verb()+" <dateiname> [<datei2>..]");
  args=file_list(args,MODE_CAT,0,"/");
  if (!sizeof(args))
    return printf("%s: %s: Keine passende Datei gefunden.\n",query_verb(),
                cmdline),1;
  args=map(args,#'_size_filter)-({0});
  if (!sizeof(args)) return 1;
  while(sizeof(args))
  {
    switch(query_verb())
    {
      case "cat":
        if (!cat(args[0]))
          printf("cat: %s konnte nicht angezeigt werden.\n",args[0]);
        break;
      case "head":
        if (!cat(args[0],0,10))
          printf("head: %s konnte nicht angezeigt werden.\n",args[0]);
        break;
      case "tail": tail(args[0]); break;
    }
    args=args[1..];
  }
  return 1;
}

//                              #######
//############################### MAN ###################################
//                              #######

static int _man(string cmdline)
{
  mixed *args;
  int i, flags;
  string *tmp, *tmp2;

  cmdline=_unparsed_args();
  args=parseargs(cmdline,&flags,MAN_OPTS,0);
  
  if (flags==-1 ||
      (sizeof(args)!=1 &&
       (sizeof(args)<2 || sizeof(args[1])>1 || !(i=to_int(args[1])))))
    return USAGE("man [-" MAN_OPTS "] <hilfeseite>");
  tmp=explode(args[0],"/");

  if (oldman_result && sizeof(tmp)==1 && sizeof(args)==1 &&
      sizeof(tmp[0])==1 && (i=to_int(tmp[0])) && member(oldman_result,i)) {
   tmp=({oldman_result[i,0],oldman_result[i,1]});
   i=0;
  }
  else if (!(flags&(MAN_M|MAN_R))&&sizeof(tmp)>1)
  {
    if (file_size(MAND_DOCDIR+args[0])>=0)
      tmp=({tmp[<1],args[0]});
    else
      tmp=({});
  }
  else
  {
    if (flags&MAN_R)
    {
      flags&=(~MAN_M);
      if (catch(regexp(({""}),args[0]))||
          !regexp(({""}),args[0]))
        return printf("man: Ungueltiger Ausdruck in Maske.\n"),1;
    }
    tmp=(string *)call_other(MAND,"locate",args[0],flags&(MAN_M|MAN_R));
  }

  oldman_result=(mapping)0;
  if(i && sizeof(tmp)>2 && sizeof(tmp)>=(i<<1))
    tmp=tmp[((i<<1)-2)..((i<<1)-1)];
  switch(sizeof(tmp))
  {
    case 0:
      printf("Keine Hilfeseite gefunden fuer '%s'.\n",args[0]);
      break;
    case 2:
       /*
         if (flags&MAN_I)
         {
         // BRANCH TO MANUALD
         return 1;
         }
       */
      printf("Folgende Hilfeseite wurde gefunden: %s\n",tmp[1]);
      More(MAND_DOCDIR+tmp[1],1);
      return 1;
    default:
      i=sizeof(tmp)>>1;
      tmp2=allocate(i);
      oldman_result=m_allocate(i,2);
      while(i)
      {
        tmp2[(i-1)]=sprintf("%d: ",i)+tmp[(i<<1)-2];
	oldman_result[i,0]=tmp[(i<<1)-2];
	oldman_result[i,1]=tmp[(i<<1)-1];
        i--;
      }
      printf("Es wurden folgende potentiell passenden Seiten gefunden:\n"
             "%'-'78.78s\n%s%'-'78.78s\n","",
             break_string(implode(tmp2," "),78),"");
      break;
  }
  return 1;
}

//                              ########
//############################### RMAN ##################################
//                              ########

static int _rman(string str)
{
  int flags;
  string *args;
  
  str=_unparsed_args();
  args=parseargs(str,&flags,"",0);
  if (flags==-1||sizeof(args)!=2)
    return USAGE("rman <hilfeseite> <mudname>");
  write("man: " +
        (string)call_other(UDP_CMD_DIR+"man","send_request",args[1],args[0]));
  return 1;
}


//                            #############
//############################# SHOWPROPS ###############################
//                            #############

//
// _showprops: Zeigt Properties zu einem Objekt an
//

static int _showprops(string str)
{
   int i;
   string *list, ausgabe;

   if (str) {
      if (str[0]!='/') str="/"+str;
      if (str[0..4]!="/std/")
      {
         printf("showprops: Es koennen nur Properties von Objekten "
                "aus /std/ angezeigt werden.\n");
         return 1;
      }
      if (str[<1]=='.') str+="c";
      if (str[<2..<1]!=".c") str+=".c";
      if (file_size(str)<0)
      {
        printf("showprops: %s: Es gibt kein Objekt diesen Namens.\n",str[0..<3]);
        return 1;
      }
      if (catch(call_other(str[0..<3], "???")))
      {
        printf("showprops: %s: Datei konnte nicht geladen werden.\n",str);
        return 1;
      }
   }
   if (!str || !find_object(str)) {
      notify_fail("Welche Properties moechtest Du sehen?"
                  " Bsp.: <showprops /std/npc>\n");
      return 0;
   }
   list=inherit_list(find_object(str));
#if __VERSION__ < "3.2.9"
   list=map(list,lambda(({'x}),({#'extract,'x,4,-2})));
   list+=map(list,lambda(({'x}),({#'[<,({#'old_explode,'x,"/"}),1})));
   list=map(m_indices(mkmapping(list)),lambda(({'x}),({#'+,({#'+,"/sys/",'x}),"h"})));
   list=filter(list,lambda(({'x}),({#'>,({#'file_size,'x}),0})) );
#else
   list=map(list,(: return $1[5..<2]+"h"; :));
   list+=map(list,(: return explode($1,"/")[<1]; :));
   list=map(m_indices(mkmapping(list)),(: return "/sys/"+$1; :));
   list=filter(list,(: return file_size($1)>0; :));
#endif
   list=sort_array(list, #'<);
   ausgabe="";
   for (i=sizeof(list);i--;)
   {
#if __VERSION__ < "3.2.9"
     str=implode(filter(old_explode(read_file(list[i]), "\n"),
        lambda( ({ 'x }), ({#'==, ({#'extract, 'x, 0, 9}), "#define P_"}) ))
                 , "\n");
#else
     str=implode(filter(explode(read_file(list[i]),"\n"),
                              (: return $1[0..9]=="#define P_";:)),"\n");
#endif
     if (str!="") ausgabe+=sprintf("%s\n%s\n\n", list[i], str);
   }
   if (ausgabe!="")
     More(ausgabe);
   else
     printf("Keine Property-Definitionen zu diesem Objekt gefunden!\n");
   return 1;
}

//                              ########
//############################### GREP ###################################
//                              ########

#if __VERSION__ < "3.2.9"

private int _grep_filter(string filename)
{
  return (call_other("valid_write",filename,getuid(this_object()),
                      "write_file",this_object())!=0);
}

#endif

//
// grep_file: Datei greppen
// rexpr: Regular Expression
// flags: Flags
//

private int grep_file(mixed filedata, string rexpr, int flags)
{
  string fullname,data,carry,*lines,*lines_orig,*tmp,*result;
  int ptr,count,i,nol,match,index;
  fullname=filedata[FULLNAME];
  if ((flags&GREP_F)&&fullname=="/players/"+getuid()+"/grep.out")
  {	 
	write_file("/players/"+getuid()+"/grep.out",
                   "Uebergehe grep.out ...\n");
	return RET_FAIL;
  }
  switch(filedata[FILESIZE])
  {
    case -2: return RET_FAIL;
    case -1: return ERROR(DOESNT_EXIST,fullname,RET_FAIL);
    case 0:  return RET_FAIL;
    default: break;
  }
  if (!MAY_READ(fullname)) return ERROR(NO_READ,fullname,RET_FAIL);
  carry=""; result=({});
  if (flags&GREP_I)
    rexpr=lower_case(rexpr);
  do
  {
    data=read_bytes(fullname,ptr,MAXLEN)||"";
    ptr+=MAXLEN;
    lines=explode(carry+data,"\n");
    switch(sizeof(lines))
    {
      case 0: continue;
      case 1:
        carry="";
        break;
      default:
        carry=lines[<1];
        lines=lines[0..<2];
        break;
    }
    lines_orig=lines;
    if (flags&GREP_I)
      lines=map(lines,#'lower_case);
    nol=sizeof(lines);
    for (i=0;i<nol;i++)
    {
      match=sizeof(regexp(lines[i..i],rexpr));
      if (flags&GREP_V) match=!match;
      if (match)
      {
        if (!(flags&GREP_C))
        {
          if (flags&GREP_N)
            result+=({ sprintf("%4d %s",index+i+1,lines_orig[i])});
          else
            result+=lines_orig[i..i];
        }
        count++;
      }
    }
    index+=nol;
  }
  while(sizeof(data)==MAXLEN);
  if (carry!="")
  {
    if (flags&GREP_I) carry=lower_case(carry);
    match=sizeof(regexp(({ carry }),rexpr));
    if (flags&GREP_V) match=!match;
    if (match)
    {
      if(!(flags&GREP_C))
      {
        if(flags&GREP_N)
           result+=({ sprintf("%4d %s",index+1,carry) });
        else
           result+=({carry});
      }
      count++;
    }
  }
  carry="";
  if (count)
  {
    if (flags&GREP_L) carry=sprintf("%s\n",fullname);
    else if (flags&GREP_H) data="";
    else data=sprintf(" %s:",fullname);
    if (flags&GREP_C) carry=sprintf("%s %d passende Zeile%s.\n",data,count,
                             (count==1?"":"n"));
    else
      carry=(data+"\n"+implode(result,"\n")+"\n");
  }
  if (flags&GREP_F)
    return write_file("/players/"+getuid()+"/grep.out",carry);
  write(carry);
  return RET_OK;
}

static int _grep(string cmdline)
{
  string rexpr,mask;
  mixed *args;
  int flags;
  cmdline=_unparsed_args();
  args=parseargs(cmdline,&flags,GREP_OPTS,0);
  if ((flags==-1)||!sizeof(args))
    return USAGE("grep [-" GREP_OPTS
          "] <regexp> <datei/verz> [<datei2> ... ] [<maske>]");
  rexpr=args[0];
  if (catch(regexp(({""}),rexpr))||!regexp(({""}),rexpr))
    return printf("grep: Ungueltiger Suchausdruck: %s\n",rexpr) ,1;
  args=args[1..];
  if (flags&GREP_M)
  {
    mask=args[<1];
    args=args[0..<2];
  }
  if (!sizeof(args))
      return USAGE("grep [-" GREP_OPTS
          "] <regexp> <datei/verz> [<datei2> ... ] [<maske>]");
  args=map(args,#'to_filename)-({0});
  /*
#if __VERSION__ < "3.2.9"
  args=filter(args,#'_grep_filter);
#else
  args=filter(args,(: return (valid_write($1,
            getuid(this_object()),"write_file",this_object())!=0):));
#endif
  */
  args=file_list(args,MODE_GREP,(flags&GREP_R?1:0),"/",mask);
  if (!sizeof(args))
    return printf("Keine passenden Dateien gefunden.\n"),1;
  if (flags&GREP_I) rexpr=lower_case(rexpr);
  if (flags&GREP_F)
  {
    if (file_size("/players/"+getuid()+"/grep.out")==-2||
	  !MAY_WRITE("/players/"+getuid()+"/grep.out"))
      return printf("grep: Datei /players/%s/grep.out kann nicht "
		      "geschrieben werden.\n",getuid()),1;
    else
      write_file("/players/"+getuid()+"/grep.out",
                 "Ausgabe von \"grep " + _unparsed_args() + "\":\n"); 
  }
  asynchron(args,#'grep_file,rexpr,flags,0);
  return 1;
}
