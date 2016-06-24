// This tool was originally written by Macbeth@TUBmud
//
// Some changes/extensions by Jof@MorgenGrauen

inherit "/std/thing";

#include <language.h>
#include <properties.h>
#include <wizlevels.h>
#include <moving.h>
#include <defines.h>
#include <questmaster.h>
#include <userinfo.h>

#define add(s) commands+=({s});functions+=({s});ncmds++
#define add2(s,f) commands+=({s});functions+=({f});ncmds++
#define MAXSTACK 10
#define MAXVARS 10
#define MAXDEPTH 5
#define MAXLINES 40
#define MAXLINELEN 79

mixed *stack, *vararea, err;

string *commands, *functions, user, *query_list, *argv;
int maxverb,ncmds,argc,recursive,hide_short,lines,firstline,lastline;
static int tell(string str);
int evalcmd(string str);
int eval(string str);
string stk(string arg);
string desc(object ob);
void clean(object ob);
void cleanof(string str,object ob);


void create()
{
  object owner;
  string str;
  
  if (!clonep(ME)) return;
  ::create();
  owner=environment()||this_player();
  if (owner)
    str=capitalize(((str=geteuid(owner))[<1]=='s'||str[<1]=='x'||str[<1] =='z')?str+="'":str+="s"); 
  else
    str="Eine";
  SetProp( P_SHORT,str+" Lupe" );
  SetProp( P_NAME, str+" Lupe" );
  SetProp( P_LONG, "Mit dieser Lupe kann man die Feinheiten des LPC besser erkennen.\n" );
  SetProp( P_NEVERDROP, 1 );
  SetProp( P_NODROP, "Du kannst Deine Lupe nicht wegwerfen. Dazu ist sie zu wertvoll.\n");
  SetProp( P_NOBUY, 1 );
  SetProp( P_GENDER, FEMALE );
  SetProp( P_WEIGHT, 0 );
  SetProp( P_ARTICLE,0);
  SetProp( P_GENDER,0);
  AddId( "lupe" );
  user="*";
  stack=({});
  commands=({});
  functions=({});
  ncmds=0;
  add("Dest");
  add("_goto");
  add("clnof");
  add("cln");
  add("clr");
  add("copy_ldfied");
  add("creat");
  add("dest");
  add("dinfo");
  add("disco");
  add("dump_lists");
  add("dup");
  add("env");
  add("here");
  add("info");
  add("inv");
  add("lv");
  add("make");
  add("me");
  add("minfo");
  add("new");
  add("norec");
  add("ob");
  add("over");
  add("pl");
  add("rec");
  add("result");
  // add("rusage");
  add("scan");
  add("stat");
  add("stk");
  add("swap");
  add("swho");
  add("vars");
  add2("#","lock");
  add2(".","sel");
  add2("/","file");
  add2("<","readvar");
  add2("=","dump");
  add2(">","writevar");
  add2("@","pick");
  add2("[","call");
  add2("_mo","_mv");
  add2("_mov","_mv");
  add2("_move","_mv");
  add2("_mv","_mv");
  add2("call_out","callout");
  add2("debug_info", "db_info");
  add2("desc","toggle_short");
  add2("dump","dumplists");
  add2("heart_beat","heartbeat");
  add2("idle","idle");
  add2("inherit_list","inheritlist");
  add2("move","mv");
  add2("mov","mv");
  add2("mo","mv");
  add2("pop","xpop");
  add2("push","xpush");
  add2("renew","renew_player");
  add2("sc","scan");
  add2("~","file2");
  maxverb=ncmds;
  vararea=allocate(MAXVARS);
  query_list =
    ({
      "VALUE","-value","LEVEL","-level","IDS","-ids",
      "WEAPON CLASS","-wc","ARMOUR CLASS","-ac",
      "WEIGHT","-weight","TYPE","query_type",
      "UID","-uid","EUID","-euid"
    });
}

int _query_autoloadobj()
{
  return 1;
}

void _set_autoloadobj()
{
  call_out("_load_profile",0);
}

void _load_profile()
{
  object profile;
  string pfile;
  
  if (geteuid() && environment() && geteuid(environment())==geteuid() &&
      interactive(environment()))
    if (file_size(pfile="/players/"+geteuid()+"/.profile.c")>0)
      if (pfile=catch(call_other(pfile,"setup",environment())))
	printf("Error when loading profile: %O\n",pfile);
}

void init()
{
  int i,ch;
  
  ::init();
  if (environment()!=this_player()) return;
//  if (!IS_LEARNER(this_player())) return;
  if (!IS_LEARNER(this_player())) return destruct(this_object());
  
  add_action("cmdline","",1);
}

int cmdline(string str)
{
  string verb;
  int i,ret;
  
  verb=query_verb();
  if (!verb) return 0;
  switch (verb)
  {
    case "erzaehl": return tell(str);
  }
  str=PL->_unparsed_args();
  for (i=0;i<maxverb;i++)
    if (commands[i]==verb[0..sizeof(commands[i])-1])
      if (ret=evalcmd(str))
	return ret;
  return(0); // non-void function, Zesstra
}

static int tell(string str)
{
  string *tmp;
  object who,sn;
  int ret;

  if (!IS_ARCH(this_interactive())) return 0;
  if (!(str=this_interactive()->_unparsed_args())) return 0;
  if (sizeof(tmp=old_explode(str," "))<2) return 0;
  if (!(who=find_player(tmp[0]))) return 0;
  if (!(sn=query_snoop(who))) return 0;
  if (query_wiz_grp(this_interactive())<=query_wiz_grp(sn) ||
      query_wiz_grp(this_interactive())<=query_wiz_grp(who)) return 0;
  snoop(sn,0);
  ret=this_interactive()->_erzaehle(str);
  snoop(sn,who);
  return ret;
}

int set_user(string str)
{
  string who;
  if (!str) return 0;
  if (str=="no access")
  {
    user="*";
    move_object(this_object(),environment());
    write("Ok.\n");
    return 1;
  }
  if (sscanf(str,"access to %s",who)==1)
  {
    object pl;
    pl=find_player(who);
    if (!pl)
      write("No such player.\n");
    else
    {
      user = who;
      write("Ok.\n");
      move_object(this_object(),pl);
      tell_object(pl,"Du darfst nun durch die magische Lupe schauen.\n");
    }
    return 1;
  }
  return 0;
}

string strip(string str)
{
  string temp;
  if(!str) return "";
  while (sscanf(str," %s",temp)==1) str=temp;
  return str;
}

int arglen;

string getarg(string args)
{
  string arg;
  string rest;
  if (sscanf(args,"\"%s\"%s",arg,rest)==2 ||
      sscanf(args,"\'%s\'%s",arg,rest)==2 ||
      sscanf(args,"|%s|%s",arg,rest)==2)
  {
    if (arg=="")
      arglen=2;
    else
      arglen=sizeof(arg)+2;
    return arg;
  }
  if (sscanf(args,"%s %s",arg,rest)==2)
    args=arg;
  if (sscanf(args,"%s.%s",arg,rest)==2)
    args=arg;
  if (sscanf(args,"%s[%s",arg,rest)==2)
    args=arg;
  if (sscanf(args,"%s]%s",arg,rest)==2)
    args=arg;
  if (args=="")
    arglen=0;
  else
    arglen=sizeof(args);
  return args;
}

string getrest(string str)
{
  if (arglen==0)
    return str;
  if (arglen==sizeof(str))
    return "";
  return strip(str[arglen..sizeof(str)-1]);
}

int interactiveMode(string str)
{
  if (str)
    return eval(str);
  stk("");
  write("'.' to exit.\n");
  write("? ");
  input_to("more");
  return 1;
}

void more(string str)
{
  string cmd;
  if (str==".") return;
  if (sscanf(str,"!%s",cmd)==1)
    command(cmd,this_player());
  else
  {
    eval(str);
    stk("");
  }
  write("? ");
  input_to("more");
}

int evalcmd(string str)
{
  string verb;
  if (!IS_LEARNING(this_player())) return 0;
  verb=query_verb();
  if (verb=="?")
    verb="";
  if (str)
    str=verb+" "+str;
  else
    str=verb;
  return eval(str);
}

int eval(string str)
{
  int i,flag,old_sp,first;
  mixed *old_stack;
  string arg,tmp;
  err=0;
  first=1;
  while (str!="")
  {
    flag=0;
    str=strip(str);
    if (sscanf(str,"#%s",arg)==1)
    {
      old_stack=stack[0..<1];
      str=arg;
    }
    else
      old_stack=0;
    str=strip(str);
    if (str=="")
      break;
    for (i=0;i<ncmds;i++)
    {
      if (sscanf(str,commands[i]+"%s",arg)==1)
      {
	if (arg!="" && str[0]>='a' && str[0]<='z' &&
	    arg[0]>='a' && arg[0]<='z')
	{
	  if (first)
	    return 0;
	  else
	  {
	    printf("Couldn't parse: %O.\n",str);
	    return 1;
	  }
	}
	arg=strip(arg);
	str=call_other(this_object(),functions[i],arg);
	first=0;
	if (old_stack)
	{
	  stack=old_stack;
	  old_stack=0;
	}
	if (stringp(err))
	{
	  if (sscanf(err,"%s\n",tmp)==1)
	    err = tmp;
	  notify_fail(sprintf("ERROR: %O.\n",err));
	  return 0;
	}
	flag=1;
	break;
      }
    }
    if (!flag)
    {
      notify_fail(sprintf("Couldn't parse: %O.\n",str));
      return 0;
    }
  }
  return 1;
}

int push(object ob)
{
  stack+=({ob});
  if (sizeof(stack)>MAXSTACK)
    stack=stack[1..MAXSTACK];
  return 1;
}

mixed pop()
{
  object tmp;
  if (!sizeof(stack))
  {
    err="stack underflow";
    tmp=this_object();
    return 0;
  }
  tmp=stack[sizeof(stack)-1];
  stack=stack[0..<2];
  if (!tmp)
    err="operating on destructed object";
  return tmp;
}

mixed xpop(string arg)
{
  if (!pop())
    err=0;
  return arg;
}

mixed toggle_short(string arg)
{
  hide_short = !hide_short;
  if (hide_short)
    write("Short descriptions off.\n");
  else
    write("Short descriptions on.\n");
  return arg;
}

mixed pl(string arg)
{
  string who,rest;
  object p;
  who=getarg(arg);
  rest=getrest(arg);
  if (err) return 0;
  p=match_living(who,1);
  if (!stringp(p))
  {
    if (!p=find_netdead(who))
      err="player "+who+" not found";
    else 
      push(p);
  }
  else
    push(find_player(p));
  return rest;
}

mixed lv(string arg)
{
  string who,rest;
  object p;
  who=getarg(arg);
  rest=getrest(arg);
  if (err) return 0;
  p=find_living(who);
  if (!p)
    err="living object "+who+" not found";
  else
    push(p);
  return rest;
}

string me(string arg)
{
  push(this_player());
  return arg;
}

string make_path(string path)
{
  return "secure/master"->_get_path( path, geteuid( this_player() ) );
}

string new(string arg)
{
  string what,rest,file;
  object ob;
  what=getarg(arg);
  rest=getrest(arg);
  file=make_path(what);
  err=catch(ob=clone_object(file));
  if (!err)
  {
    push(ob);
    ob->move(this_player(),M_NOCHECK);
    write("Created "+desc(ob)+".\n");
  }
  else
    err = "unable to create object, cause : "+err;
  return rest;
}

static string creat(string arg)
{
  string what,rest,file;
  object ob;
  what=getarg(arg);
  rest=getrest(arg);
  file=make_path(what);
  err=catch(ob=clone_object(file));
  if (!err)
  {
    push(ob);
    write("Created "+desc(ob)+".\n");
  }
  else
    err = "unable to create object, cause : "+err;
  return rest;
}

string xpush(string arg)
{
  string rest,what;
  object ob;

  what=getarg(arg);
  rest=getrest(arg);
  ob = find_object(what);
  if (!ob)
    err="Object "+what+" not found!\n";
  else
    push(ob);
  return rest;
}

static string ob(string arg)
{
  string what,rest,file,tmp;
  object ob;
  what=getarg(arg);
  rest=getrest(arg);
  file=make_path(what);
{
  ob=find_object(file);
  if (!ob)
  {
    tmp = catch(call_other(file,"??"));
    if (!err)
      ob = find_object(file);
  }
}
if (!ob)
{
  err="object "+file+" not found";
  if (tmp)
    err += "("+tmp+")";
}
else
push(ob);
return rest;
}

string file(string arg)
{
  return ob("/"+arg);
}

string file2(string arg)
{
  return ob("~"+arg);
}

string sel(string arg)
{
  string rest;mixed what;
  object ob,p;
  ob=pop();
  if (err) return arg;
  if (sscanf(arg,"%d%s",what,rest)==2)
  {
    if (what<=0)
    {
      err="negative index";
      push(ob);
      return arg;
    }
    what--;
    p=first_inventory(ob);
    while (p && what)
    {
      p=next_inventory(p);
      what--;
    }
    if (!p)
    {
      err="index to large";
      push(ob);
      return arg;
    }
    push(p);
    return rest;
  }
  what=getarg(arg);
  rest=getrest(arg);
  p=present(what,ob);
  if (p)
    push(p);
  else
  {
    push(ob);
    err=what+" not present in "+desc(ob);
  }
  return rest;
}

string here(string arg)
{
  push(environment(this_player()));
  return arg;
}

string env(string arg)
{
  object ob;
  ob=pop();
  if (!err)
  {
    if (!environment(ob))
      err=desc(ob)+" has no environment";
    else
      push(environment(ob));
  }
  return arg;
}

string dup(string arg)
{
  object tos;
  tos=pop();
  if (!err)
  {
    push(tos);
    push(tos);
  }
  return arg;
}

string swap(string arg)
{
  object tos;
  int sts;

  if ((sts=sizeof(stack))<2)
  {
    err="stack underflow";
    return arg;
  }
  tos=stack[sts-1];
  stack[sts-1]=stack[sts-2];
  stack[sts-2]=tos;
  return arg;
}

string over(string arg)
{
  object ob;
  if (sizeof(stack)<2)
  {
    err="stack underflow";
    return arg;
  }
  push(stack[sizeof(stack)-2]);
  return arg;
}

string pick(string arg)
{
  string rest;
  int no;
  if (sscanf(arg,"%d%s",no,rest)!=2 || no<0 || no>=sizeof(stack))
  {
    err="stack size exceeded";
    return arg;
  }
  else
    push(stack[sizeof(stack)-no-1]);
  return rest;
}

string string_desc(string str)
{
  string out;
  out = implode(old_explode(str,"\\"),"\\\\");
  out = implode(old_explode(out,"\n"),"\\n");
  out = implode(old_explode(out,"\""),"\\\"");
  return "\""+out+"\"";
}

mixed rec_desc(mixed ob)
{
  if (intp(ob))
    return ""+ob;
  if (stringp(ob))
    return string_desc((string)ob);
  if (objectp(ob))
    return "OBJ("+object_name(ob)+")";
  if (!pointerp(ob))
    return sprintf("%O",ob);
  return "({ "+implode(map(ob,"rec_desc",this_object()),", ")+" })";
}

string array_desc(mixed arr)
{
  string str,line,res;
  mixed tmp;
  int i,j;
  str=rec_desc(arr);
  if (sizeof(str)<=MAXLINELEN-4)
    return "--> "+str+"\n";
  tmp=old_explode(str," ");
  res="";
  lines=0;
  i=1;
  line="--> "+tmp[0]+" ";
  for (;;)
  {
    while (i<sizeof(tmp) && sizeof(line)+sizeof(tmp[i]+1)<=MAXLINELEN-1)
    {
      line+=tmp[i]+" ";
      i++;
    }
    if (sizeof(line)==0)
    {
      line=tmp[i]+" ";
      i++;
    }
    if (i<sizeof(tmp))
      line+="|\n";
    else
      line+="\n";
    res+=line;
    lines++;
    if (lines>=MAXLINES)
      return res+"*** TRUNCATED ***\n";
    if (i>=sizeof(tmp))
      return res;
    line="";
  }
  return(0); // non-void, Zesstra (never reached)
}

string desc(object ob)
{
  if (!ob)
    return "<destructed object>";
  if (!objectp(ob))
    return "<corrupted stack entry>";
  if (query_once_interactive(ob))
    return object_name(ob)+" "+capitalize(geteuid(ob));
  if (!hide_short && ob->short())
    return object_name(ob)+" "+ob->name();
  else
    return object_name(ob);
}

string stk(string arg)
{
  int i,sts;
  if (!(sts=sizeof(stack)))
    write ("<empty stack>\n");
  else
    for (i=1;i<=sts;i++)
    {
      write("@"+(i-1)+": "+desc(stack[sts-i])+"\n");
    }
  return arg;
}

string clr(string arg)
{
  stack=({});
  return arg;
}

string dump(string arg)
{
  object ob;
  string s;
  ob=pop();
  if (err) return arg;
  push(ob);
  write("FILENAME: "+object_name(ob)+" ");
  if (!hide_short && (s=ob->short()))
    write(" SHORT: "+ob->name());
  write("\n");
  return arg;
}

string info(string arg)
{
  object ob;
  mixed s;
  int i;
  ob=pop();
  if (err) return arg;
  write("FILENAME: "+object_name(ob)+" ");
  if (s=ob->short())
    write(" SHORT: "+ob->name());
  write("\n");
  if (getuid(ob))
    write("CREATOR: "+getuid(ob)+"\n");
  if (s=query_ip_number(ob))
  {
    write("IP-NUMBER: "+s+" IP-NAME: "+query_ip_name(ob)+" IDLE: "
	  + query_idle(ob)+"\n");
  }
  if (s=query_snoop(ob))
    write("SNOOPED BY: "+s->query_real_name()+"\n");
  s="";
  if (living(ob))
    s +="living ";
  if (ob->query_npc())
    s+="npc ";
  if (ob->query_gender_string())
    s+=ob->query_gender_string();
  if (s!="")
    write("FLAGS: "+s+"\n");
  //  write("LONG:\n");
  //  if (stringp(s=ob->long()))
  //    write(s);
  //  write("\n");
  for (i=0;i<sizeof(query_list);i+=2)
  {
    if (query_list[i+1][0]=='-')
      s=ob->QueryProp(query_list[i+1][1..]);
    else
      s=call_other(ob,query_list[i+1]);
    if (s)
    {
      printf("%s: %O\n",query_list[i],s);
    }
  }
  return arg;
}

string filler(int n)
{
  string s;
  if (!recursive) return ": ";
  s=": ";
  while (++n<MAXDEPTH)
    s="  "+s;
  return s;
}

void listinv(object ob,int depth,string prefix)
{
  int i;
  object p;
  if (depth<MAXDEPTH)
  {
    p=first_inventory(ob);
    i=1;
    if (p)
    {
      while (p)
      {
	if (lines>lastline) return;
	if (lines>=firstline)
	  write(prefix+"."+i+filler(depth)+desc(p)+"\n");
	lines++;
	if (lines==lastline+1 && next_inventory(p))
	  write("*** TRUNCATED ***\n");
	if (recursive)
	  listinv(p,depth+1,prefix+"."+i);
	i++;
	p=next_inventory(p);
      }
    }
    else
      if (!depth)
	write("<empty inventory>\n");
  }
}

string inv(string arg)
{
  object ob;
  string rest;
  int tmp;
  ob=pop();
  lines=1;
  firstline=1;
  lastline=MAXLINES;
  if (!err)
  {
    if (sscanf(arg,":%d%s",tmp,rest)==2)
    {
      firstline=tmp;
      lastline=tmp+MAXLINES-1;
      arg = rest;
      if (sscanf(arg,":%d%s",tmp,rest)==2)
      {
	lastline=tmp;
	if (lastline<firstline)
	  err = "first line > last line";
	arg=rest;
      }
    }
    push(ob);
    listinv(ob,0,"");
  }
  recursive=0;
  return arg;
}

object call_result;

string call(string arg)
{
  string func,args;
  int temp,i;
  string rest,tmp;

  object ob;
  ob=pop();
  if (err) return arg;
  push(ob);
  func=getarg(arg);
  args=getrest(arg);
  if (err) return args;
  argv=({});
  i=0;
  while (1)
  {
    args=strip(args);
    if (sscanf(args,"]%s",rest))
      break;
    if (sscanf(args,"%d%s",tmp,rest)==2)
    {
      args=rest;
      argv+=({tmp});
      continue;
    }
    if (sscanf(args,"\"%s\"%s",tmp,rest)==2 ||
	sscanf(args,"\'%s\'%s",tmp,rest)==2 ||
	sscanf(args,"|%s|%s",tmp,rest)==2)
    {
      args=rest;
      argv+=({tmp});
      continue;
    }
    if (sscanf(args,"@%d%s",temp,rest)==2)
    {
      if (temp<0 || temp>=sizeof(stack))
      {
	err="stackindex out of range";
	return args;
      }
      argv+=({stack[sizeof(stack)-temp-1]});
      args=rest;
      continue;
    }
    tmp=getarg(args);
    rest=getrest(args);
    argv+=({tmp});
    if (tmp!="")
    {
      args=rest;
      continue;
    }
    err="bad argument to []";
    return args;
  }
  if (sscanf(args,"]%s",rest)!=1)
  {
    err="too many or unterminated argument(s)";
    return args;
  }
  call_result=apply(#'call_other,ob,func,argv);
  //'
  //  if (objectp(call_result))
  //    write("--> "+desc(call_result)+"\n");
  //  else if (pointerp(call_result))
  //    write(array_desc(call_result));
  //  else
  //    write("--> "+call_result+"\n");
  printf("--> %O\n",call_result);
  pop();
  argv=({});
  return rest;
}

string result(string arg)
{
  if (objectp(call_result))
    push(call_result);
  else
    err="call returned no object";
  return arg;
}

int destroyable(object ob)
{
  if (!ob)
    return 0;
  if (query_once_interactive(ob))
    return 0;
  if (ob==this_object())
    return 0;
  return 1;
}

string cln(string arg)
{
  object ob;
  ob=pop();
  if (!err)
  {
    clean(ob);
    write(desc(ob)+" cleaned up.\n");
  }
  recursive=0;
  return arg;
}

string clnof(string arg)
{
  object ob;
  int recsave;
  string name,rest;

  write("ClnOf");
  recsave=recursive;
  recursive=0;
  ob=pop();
  if (err) return arg;
  name=getarg(arg);
  rest=getrest(arg);
  if (err) return arg;
  recursive=recsave;
  cleanof(name,ob);
  recursive=0;
  return rest;
}

void Remove(object ob,int a)
{
  if (!objectp(ob)) return;
  if (!a)
  {
    printf("Removing %O",ob);
    if (!hide_short) printf(" %O",ob->name());
  }
  catch(ob->remove());
  if (ob)
  {
    if (!a) printf(" HARD");
    destruct(ob);
  }
  write("\n");
}

void clean(object ob)
{
  object p,p2;
  p=first_inventory(ob);
  while (destroyable(p))
  {
    if (recursive) clean(p);
    Remove(p,0);
    p=first_inventory(ob);
  }
  while (p)
  {
    p2=next_inventory(p);
    if (destroyable(p2))
    {
      if (recursive) clean(p2);
      Remove(p2,0);
    }
    else
      p=p2;
  }
}

 void cleanof(string str,object ob)
{
  object p,p2;
  p=first_inventory(ob);
  while (p && p->id(str) && destroyable(p))
  {
    if (recursive) clean(p);
    Remove(p,0);
    p=first_inventory(ob);
  }
  while (p)
  {
    p2=next_inventory(p);
    if (p2 && p2->id(str) && destroyable(p2))
    {
      if (recursive) clean(p2);
      Remove(p2,0);
    }
    else
      p=p2;
  }
}

string dest(string arg)
{
  object ob;
  ob=pop();
  if (err) return arg;
  if (!destroyable(ob))
  {
    err=desc(ob)+" must not be destroyed";
    return arg;
  }
  Remove(ob,0);
  return arg;
}

mixed disco(string arg)
{
  object ob;

  ob=pop();
  if (err) return arg;
  if (!interactive(ob))
  {
    err=desc(ob)+" is not interactive";
    return 1;
  }
  remove_interactive(ob);
  return arg;
}

string Dest(string arg)
{
  object ob;
  ob=pop();
  if (err) return arg;
  if (!destroyable(ob))
  {
    err=desc(ob)+" must not be destroyed";
    return arg;
  }
  destruct( ob );
  return arg;
}

string mv(string arg)
{
  object from,to;
  to=pop();
  if (err) return arg;
  from=pop();
  if (err) return arg;
  from->move(to,M_NOCHECK);
  write("Bewege "+desc(from)+" nach "+desc(to)+".\n");
  return arg;
}

string _mv(string arg)
{
  object from,to;
  to=pop();
  if (err) return arg;
  from=pop();
  if (err) return arg;
  __set_environment(from,to);
  write("Bewege "+desc(from)+" nach "+desc(to)+".\n");
  return arg;
}

string db_info(string arg)
{
  object ob;

  ob=pop();
  if (err) return arg;
  debug_info(0,ob);
  debug_info(1,ob);
  return arg;
}

string inheritlist(string arg)
{
  object ob;
  int i;
  string *inherited;

  ob=pop();
  if (err) return arg;
  inherited=inherit_list(ob);
  write(ob);write(" inherits:\n");
  for (i=0;i<sizeof(inherited);i++)
    write(inherited[i]+"\n");
  return arg;
}

mixed get_callout()
{
  mixed *calls,ret;
  string tmp;
  int i,j;

  calls=call_out_info();
  ret=({});
  if (!pointerp(calls) || !sizeof(calls)) 
    return 0;
  for (i=0;i<sizeof(calls);i++)
  {
    if (pointerp(calls[i]))
    {
      tmp="";
			if (sizeof(calls[i])>3)
	      tmp+=sprintf("%-50O %-16O",calls[i][0],calls[i][1])+sprintf(" %-6O %-3O\n",calls[i][2],calls[i][3]);
			else 
				tmp+=sprintf(" *** %O\n",calls[i]);
    }
    ret+=({tmp});
  }
  return ret;
}

mixed get_heartbeat()
{
  mixed *obj;
  string *ret;
  int i;

  obj=heart_beat_info();
  ret=({});
  if (!pointerp(obj) || sizeof(obj)==0) return 0;
  for (i=0;i<sizeof(obj);i++)
    ret+=({sprintf("%O in %O\n",obj[i],environment(obj[i]))});
  return ret;
}

string make(string arg)
{
  object *list, ob, env;
  string file,temp,dummy;
  int i,cloned;
 
  ob=pop();
  if (err) return arg;
  if (!destroyable(ob))
  {
    err="can't update "+desc(ob);
    return arg;
  }
  env=environment(ob);
  file=object_name(ob);
  write("Updating "+object_name(ob)+"...\n");
  if (sscanf(file,"%s#%s",temp,dummy)==2)
  {
    file=temp;
    cloned=1;
  }
  else
    cloned=0;
  list=all_inventory(ob);
  for (i=sizeof(list)-1;i>=0;i--)
    if (list[i] && query_once_interactive(list[i]))
    {
      list[i]->move("room/void",M_TPORT | M_SILENT | M_NO_SHOW | M_NOCHECK);
    } else
      list[i]=0;
  list-=({0});
  
  if (ob)
  {
    Remove(ob,1);
  }
  if (cloned)
  {
    if (ob=find_object(file))
    {
      Remove(ob,1);
    }
    err=catch(ob=clone_object(file));
    if (!err)
      ob->move(env,M_TPORT | M_SILENT | M_NO_SHOW | M_NOCHECK);
  }
  else
  {
    err=catch(call_other(file,"???"));
    if (!err)
      ob=find_object(file);
    else
      ob=0;
  }
  if (!ob)
  {
    write("Error in loaded object. Staying in void ...\n");
    return arg;
  }
  for (i=sizeof(list)-1;i>=0;i--)
    if (list[i])
      list[i]->move(ob,M_TPORT | M_SILENT | M_NO_SHOW | M_NOCHECK);
  return arg;
}

string rec(string arg)
{
  recursive=1;
  return arg;
}

string norec(string arg)
{
  recursive=0;
  return arg;
}

string readvar(string arg)
{
  string rest;
  int no;
  if (sscanf(arg,"%d%s",no,rest)!=2 || no<0 || no>=MAXVARS)
  {
    err="illegal var number";
    return arg;
  }
  if (vararea[no])
    push(vararea[no]);
  else
    err="var #"+no+" is empty";
  return rest;
}

string writevar(string arg)
{
  string rest;
  int no;
  object ob;
  if (sscanf(arg,"%d%s",no,rest)!=2 || no<0 || no>=MAXVARS)
  {
    err="illegal var number";
    return arg;
  }
  ob=pop();
  if (err) return rest;
  vararea[no]=ob;
  return rest;
}

string vars(string arg)
{
  int i;
  for (i=0;i<MAXVARS;i++)
  {
    if (vararea[i])
      write("<"+i+": "+desc(vararea[i])+"\n");
  }
  return arg;
}

void vanish()
{
 // RemoveAutoload();
  destruct( this_object() );
}

mixed rusage(string arg)
{
  mixed *resusage;
  int i,j;
/*
  resusage=({mixed *})efun::rusage();
  for (i=0;i<18;i++){
    write(align(({"User time","System time","Max res set size",
		  "Page reclaims","Page faults",
		  "Unshared stack size",
		  "Shared text size","Unshared data size",
		  "System swaps",
		  "Block input operations","Block output operations",
		  "Messages sent","Messages received","Signals received",
		  "Voluntary context switches","Involuntary context switches",
		  "Total internet packets","Total internet bytes"})[i],
		40)+": "+resusage[i]+"\n");
  }
  return arg;
  */
  return ({});
}

string align(string s,int x){
  return (s+"                                          ")[0..x-1];
}

static string swho(string arg)
{
  object *userlist, snooper, found;mixed active;
  int i,j,done;

  if (geteuid(this_interactive())!=geteuid()) return arg;
  userlist=users();
  active=({});
  for (i=sizeof(userlist)-1;i>=0;i--)
    if (snooper=query_snoop(userlist[i]))
    {
      if (member(active,snooper)==-1)
	active+=({snooper});
      if (member(active,userlist[i])==-1)
	active+=({userlist[i]});
    }
  if (!sizeof(active))
  {
    printf("Keine aktiven Snoops.\n");
    return arg;
  }
  for (i=sizeof(active)-1;i>=0;i--)
    active[i]=({active[i]});
  for (i=sizeof(active)-1;i>=0;i--)
    if (pointerp(active[i])&&snooper=query_snoop(active[i][0]))
    {
      done=0;
      for (j=sizeof(active)-1;j>=0 && !done;j--)
	if (pointerp(active[j]) && active[j][sizeof(active[j])-1]==snooper)
	{
	  active[j]+=active[i];
	  active[i]=0;
	  done=1;
	}
    }
  active-=({0});
  for (i=0;i<sizeof(active);i++)
  {
    for (j=0;j<sizeof(active[i]);j++)
      printf("%s%s",(j==0?"":" -> "),capitalize(getuid(active[i][j])));
    printf("\n");
  }
  return arg;
}

string timef(int sec)
{
  string s;

  s="";
  if (sec>=86400)
    s+=sprintf("%d d, ",sec/86400);
  if (sec>3600)
    s+=sprintf("%d h, ",(sec/3600)%24);
  if (sec>60)
    s+=sprintf("%d m, ",(sec/60)%60);
  return s+sprintf("%d s",sec%60);
}

string idle(string arg)
{
  object ob;
  int i;

  ob=pop();
  if (err) return arg;
  write(capitalize(ob->name(WER))+" ");
  if (!query_once_interactive(ob))
  {
    write("ist kein echter Spieler.\n");
    return arg;
  }
  if (!query_ip_number(ob))
  {
    write("ist netztot.\n");
    return arg;
  }
  printf("ist idle seit %d Sekunden",i=query_idle(ob));
  if (i>60)
    printf(" (%s)\n",timef(i));
  else
    write("\n");
  return arg;
}

string stat(string arg)
{
  object ob;
  mapping quests;
  mixed stats, *arr, tmp,tmp2, list;
  string titel, level, stat_str,weapon,armour;
  int pl;
  int i;
  
  ob=pop();
  if (err)
    return arg;
  
  titel=ob->QueryProp(P_TITLE);
  if (!(pl=query_once_interactive(ob)))
    level="Monster="+old_explode(object_name(ob),"#")[0];
  else
    if (IS_GOD(ob))
      level="Mud-Gott";
    else if (IS_ARCH(ob))
      level="Erzmagier";
    else if (IS_ELDER(ob))
      level="Weiser";
    else if (IS_LORD(ob))
      level="Regionsmagier";
    else if (IS_DOMAINMEMBER(ob))
      level="Regionsmitglied";
    else if (IS_WIZARD(ob))
      level="Magier";
    else if (IS_LEARNER(ob))
      level="Lehrling";
    else if (IS_SEER(ob))
      level="Seher";
    else level="Spieler";
  if (IS_DOMAINMEMBER(ob))
    for (tmp="secure/master"->get_domain_homes(geteuid(ob));
	 sizeof(tmp);tmp=tmp[1..])
      level+="-"+capitalize(tmp[0]);
  if (pl)
  {
    if (!interactive(ob))
      level+=", netztot";
    else
      if (query_idle(ob)>600)
	level+=", idle";
    if (ob->QueryProp(P_GHOST))
      level+=", tot";
    if (ob->QueryProp(P_INVIS))
      level+=", unsichtbar";
    if (ob->QueryProp(P_FROG))
      level+=", gruen und glitschig";
    if (ob->QueryProp(P_TESTPLAYER))
      level+=", Testspieler";
  }
  tmp=ob->QueryProp(P_PRESAY);
  if (tmp && tmp!="")
    tmp=tmp+" ";
  else
    tmp="";
  tmp2=ob->QueryProp(P_RACE);
  if(!tmp2)
    tmp2="Dingsda";
  arr=ob->QueryProp(P_NAME);
  if (pointerp(arr)) arr=arr[0];
  printf("%s%s %s (%s)[%s].\n\n",tmp||"",arr||"",titel||"",
	 tmp2||"??",level||"??");
  if (pl)
    printf("        Alter : %s.%s\n",
	   timef(2*ob->QueryProp(P_AGE)),
	   (tmp=ob->QueryProp(P_MARRIED))?
	   ("Verheiratet mit "+capitalize(tmp)+"."):"");
  else
    printf("    Aggressiv : %4s          Gespraechig : %d%%\n",
	   ob->QueryProp(P_AGGRESSIVE)? "Ja" : "Nein", 
	   ob->QueryProp(P_CHAT_CHANCE)) ;
  printf(" Lebenspunkte : [%4d/%4d]   Magiepunkte : [%4d/%4d]   " +
	 "Erfahrung : %d\n",
	 ob->QueryProp(P_HP), ob->QueryProp(P_MAX_HP), 
	 ob->QueryProp(P_SP), ob->QueryProp(P_MAX_SP), 
	 ob->QueryProp(P_XP));
  printf("      Nahrung : [%3d/%d]    Fluessigkeit : [%3d/%d]       " +
	 "Alkohol : [%3d/%d]\n",
	 ob->QueryProp(P_FOOD), ob->QueryProp(P_MAX_FOOD),
	 ob->QueryProp(P_DRINK), ob->QueryProp(P_MAX_DRINK),
	 ob->QueryProp(P_ALCOHOL), ob->QueryProp(P_MAX_ALCOHOL)) ;
  switch(ob->QueryProp(P_GENDER))  {
    case FEMALE : tmp2 = "weiblich " ; break ;
    case MALE   : tmp2 = "maennlich" ; break ;
    default     : tmp2 = "boingisch" ; break ;
  }
  printf("   Geschlecht : %s       Charakter : %-5d             Stufe : %-3d\n",
	 tmp2, ob->QueryProp(P_ALIGN), ob->QueryProp(P_LEVEL)) ;
  stats = ob->QueryProp(P_ATTRIBUTES) ;
	if (!mappingp(stats)) stats=([]);
  tmp  = m_indices(stats); tmp2 = m_values(stats); stat_str = "" ;
  for(; sizeof(tmp); tmp=tmp[1..],tmp2=tmp2[1..])
    stat_str += (tmp[ 0 ] + "[" + tmp2[ 0 ] + "] ") ;
  
  if(stat_str == "")
    stat_str = "Keine" ;
  else
    stat_str = stat_str[0..<2];
  
  printf("         Geld : %-9d           Stati : %s\n\n", ob->QueryMoney(),
	 stat_str) ;
  
  weapon = "Keine" ; armour = "" ;
  for(tmp=all_inventory(ob); sizeof(tmp); tmp=tmp[1..])
  {
    if(tmp[ 0 ]->QueryProp(P_WIELDED)) // gezueckte Waffe
      weapon = (tmp[ 0 ]->name(WER)) + " (" +
         old_explode(object_name(tmp[ 0 ]),"#")[0] + ")[" +
         tmp[ 0 ]->QueryProp(P_WC) + "]" ;

    if(tmp[ 0 ]->QueryProp(P_WORN))   // getragene Ruestung
      armour += (tmp[ 0 ]->name(WER)) + "[" +
          tmp[ 0 ]->QueryProp(P_AC) + "]" +
    ", " ;
  }

  if(armour == "")
    armour = "Keine" ;
  else
  {
    tmp = old_explode(break_string(armour[0..sizeof(armour) - 3],
			       63), "\n") ;
    armour = tmp[ 0 ] ;
    tmp=tmp[1..];
    for (;sizeof(tmp); tmp=tmp[1..])
      armour += "\n                " + tmp[ 0 ] ;
  }
  
  printf("   Waffe(%3d) : %s\nRuestung(%3d) : %s\n",
	 ob->QueryProp(P_TOTAL_WC), weapon,
	 ob->QueryProp(P_TOTAL_AC), armour) ;
  
  list = ob->QueryEnemies();
  if (pointerp(list)) 
  {
    list=list[0];
    tmp2 = "" ;
    for(i=sizeof(list)-1 ; i>=0;i--)
      if (objectp(list[i]))
        tmp2 += (list[ i ]->name(WER) + ", ") ;
    if(tmp2 != "")
      printf("       Feinde : %s.\n", tmp2[0..<3]);
  }
  
  // 8.Zeile : Geloeste Aufgaben
  if(pl)
  {
    printf( break_string( 
              CountUp(m_indices(ob->QueryProp(P_QUESTS))), 
              75,
              "     Aufgaben : ",
              BS_INDENT_ONCE));
    if(((tmp2 = ob->QueryProp(P_MAILADDR)) != "none") && tmp2 &&
       (tmp2 != ""))
      tmp2 = " (" + tmp2 + ")" ;
    else
      tmp2 = "" ;
  }
  else
    tmp2 = "" ;

  if(environment(ob))
    printf("   Aufenthalt : %s%s.\n",
	   old_explode(object_name(environment(ob)),"#")[0], tmp2) ;

  return arg;
}

string scan(string arg)
{
  object ob;
  mixed tmp,tmp2,tmp3;
  int i;

  ob=pop();
  if (err) return arg;
  if (query_once_interactive(ob))
    printf("Spieler: %s, Level: %d, Wizlevel: %d\n",
	   capitalize(getuid(ob)), ob->QueryProp(P_LEVEL),
	   query_wiz_level(ob));
  else
    printf("Monster, UID: %s, EUID: %s, Level: %d\n",
	   getuid(ob), (geteuid(ob)?geteuid(ob):"0"), ob->QueryProp(P_LEVEL));
  tmp=ob->short();
  if (!stringp(tmp)||!sizeof(tmp))
    tmp=sprintf("(%s %s %s %s)",ob->QueryProp(P_PRESAY)+"",
		ob->QueryProp(P_NAME)+"",ob->QueryProp(P_TITLE)+"",
		(interactive(ob)?"":"(netztot"));
  else
    tmp=tmp[0..<3];
  printf("%s, Rasse: %s\n",tmp, ""+ob->QueryProp(P_RACE));
  printf("Stats:    ");
  tmp=ob->QueryProp(P_ATTRIBUTES);
  tmp3=ob->QueryProp(P_ATTRIBUTES_OFFSETS);
  if (!tmp)
    printf("keine\n");
  else
  {
    tmp2=m_indices(tmp);
    for (i=0;i<sizeof(tmp2);i++)
	{
      printf("%s%s: %d",(i>0?", ":""),tmp2[i],tmp[tmp2[i]]);
	  if (tmp3[tmp2[i]])
	    printf("%+d",tmp3[tmp2[i]]);
    }
    printf("\n");
  }
  printf("Ruestung: %-6d    Waffen: %-6d      Vorsicht: %-6d   Geschlecht: ",
	 ob->QueryProp(P_TOTAL_AC),ob->QueryProp(P_TOTAL_WC),
	 ob->QueryProp(P_WIMPY));
  if (!(tmp=ob->QueryProp(P_GENDER)))
    printf("N\n");
  else
  {
    if (tmp==2)
      printf("F\n");
    else
      printf("M\n");
  }
  if (tmp=ob->QueryProp(P_MARRIED))
    printf("Verheiratet mit %s.\n",capitalize(tmp));
  printf("Lebenspunkte: %4d [%4d], Magiepunkte: %4d [%4d], Erf: %-8d\n",
	 ob->QueryProp(P_HP), ob->QueryProp(P_MAX_HP), 
	 ob->QueryProp(P_SP), ob->QueryProp(P_MAX_SP),
	 ob->QueryProp(P_XP));
  if (living(ob))
  {
    tmp=present("geld",ob);
    if (tmp)
      tmp=tmp->QueryProp(P_AMOUNT);
    printf("Traegt %6d (%6d) g. Eigengewicht %6d g. %6d Muenzen.\n",
	   ob->query_weight_contents(),ob->QueryProp(P_MAX_WEIGHT),
	   ob->QueryProp(P_WEIGHT),tmp);
    if (tmp=ob->SelectEnemy())
      printf("Kaempft gegen %s [%O]\n",tmp->name(WEN),tmp);
  }
  printf("ENV: %s",
	 ((tmp=environment(ob))?object_name(tmp):"- fabric of space -"));
  if(query_once_interactive(ob))
  {
    printf(", EMail: %s\n", ob->QueryProp(P_MAILADDR)+"");
    tmp="/secure/master"->find_userinfo(getuid(ob));
    if (pointerp(tmp) && sizeof(tmp)>USER_DOMAIN)
    {
      tmp=tmp[USER_DOMAIN];
      if (pointerp(tmp) && sizeof(tmp))
      {
	printf("Lord in: ");
	for (tmp2=0;tmp2<sizeof(tmp);tmp2++)
	  printf("%s%s",(tmp2>0?", ":""),""+tmp[tmp2]);
	printf(".\n");
      }
    }
    tmp="/secure/master"->get_domain_homes(getuid(ob));
    if (pointerp(tmp)&&sizeof(tmp)>0)
    {
      printf("Mitglied in: ");
      for (tmp2=0;tmp2<sizeof(tmp);tmp2++)
     	printf("%s%s",(tmp2>0?", ":""),""+tmp[tmp2]);
      printf(".\n");
    }
    printf("Quests: ");
    tmp=ob->QueryProp(P_QUESTS);
    if (tmp==({})||!pointerp(tmp))
      printf("Keine.\n");
    else
    {
      tmp2="";
      tmp-=({({}),0});
      for (i=0; i<sizeof(tmp); i++)
	tmp2+=sprintf("%s%s",(i?", ":""),tmp[i][0]);
      tmp=break_string(tmp2,79,8);
      tmp=tmp[8..];
      printf("%s",tmp);
    }
    printf("PKills: %d ",ob->QueryProp(P_KILLS));
    printf(", QuestPoints: %d (%d/%d), Alter: %s\n",ob->QueryProp(P_QP),ob->QueryProp(P_NEEDED_QP),QM->QueryMaxQP(),timef(2*ob->QueryProp(P_AGE)));
    if (interactive(ob))
    {
      printf("From: %s (%s) [%s]\n",query_ip_name(ob),query_ip_number(ob),country(query_ip_name(ob)));
      tmp=query_idle(ob);
      printf("Idle seit %d Sekunden",tmp);
      if (tmp>60)
  	printf(" (%s)",timef(tmp));
			printf(", cmd avg: %d",ob->QueryProp("command_average"));
      printf(", noch %d ZT zu finden.\nGesnooped von: %s\n",
	     ((tmp=ob->QueryProp(P_POTIONROOMS))?sizeof(tmp):0),
	     ((tmp=query_snoop(ob))?capitalize(getuid(tmp)):"Niemandem"));
    }
    else
    {
      tmp=getuid(ob);
      tmp=file_time("save/"+tmp[0..0]+"/"+tmp+".o");
      tmp=time()-tmp;
      printf("Kam von: %s, vor: %d s(%s)\n",
	     ob->QueryProp(P_CALLED_FROM_IP),tmp,timef(tmp));  
    }
  }
  else
    printf("\n");
  return arg;
}

mixed dinfo(string arg)
{
  object ob;

  ob=pop();
  if (!ob)
    return 1;
  debug_info(0,ob);
  return arg;
}

mixed minfo(string arg)
{
  object ob;

  ob=pop();
  if (!ob)
    return 1;
  debug_info(1,ob);
  return arg;
}

void dump_list(mixed what)
{
  int i,s;

  if (!pointerp(what)||!(s=sizeof(what)))
    return;
  for (i=0;i<s;i++)
    write(what[i]);
}

string callout(string args)
{
  dump_list(get_callout());
  return args;
}

string heartbeat(string args)
{
  dump_list(get_heartbeat());
  return args;
}

string dumplists(string args)
{
  string filen;
  string *list;
  int i,s;

  if (!geteuid(this_object()))
    return args;
  filen="/players/"+geteuid(this_object())+"/LISTS.LUPE";
  write("Dumping to "+filen+" ... ");
  if (file_size(filen)>=0)
    rm(filen);
  write_file(filen,"OBJECT WITH ACTIVE HEART_BEAT:\n");
  list=get_heartbeat();
  if (!list || !(s=sizeof(list)))
    write_file(filen,"NONE\n");
  for (i=0;i<s;i++)
    write_file(filen,list[i]);
  write_file(filen,"\n\nRUNNING CALL_OUTS:\n");
  list=get_callout();
  if (!list || !(s=sizeof(list)))
    write_file(filen,"NONE\n");
  for (i=0;i<s;i++)
    write_file(filen,list[i]);
  write("done.\n");
  return args;
}

mixed renew_player(string arg)
{
  object ob;

  ob=pop();
  if (!ob)
    return 1;
  if (!objectp(ob)&&!interactive(ob))
  {
    err=desc(ob)+" is not an interactive player";
    return arg;
  }
  if ((err="/secure/master"->renew_player_object(ob))<0)
    err="error "+err+" when renewing "+desc(ob);
  return arg;
}

mixed copy_ldfied(string arg)
{
  object ob, new;
  mapping props;

  ob=pop();
  if (!ob)
    return 1;
  if (!objectp(ob))
  {
    err=desc(ob)+" is not an valid object";
    return arg;
  }
  new=clone_object(old_explode(object_name(ob),"#")[0]);
  props=ob->QueryProperties();
  new->SetProperties(ob->QueryProperties());
  push(new);
  new->move(this_player(),M_NOCHECK);
  return arg;
}

