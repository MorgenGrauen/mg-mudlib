// MorgenGrauen MUDlib
//
// fileview.c
//
// $Id: fileview.c 9142 2015-02-04 22:17:29Z Zesstra $
#pragma strict_types, rtt_checks
#pragma range_check
#pragma no_clone

#include <ansi.h>
#include <player/base.h>
#include <wizlevels.h>
#include <shells.h>
#include <daemon/mand.h>
#include <udp.h>
#include <files.h>
#include <rtlimits.h>

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
    case FSIZE_DIR: tmp=sprintf(colorstrings[DIR],tmp);
             maxlen-=colorstrings[DIR,1]; break;
    case FSIZE_NOFILE: tmp=sprintf(colorstrings[VC],tmp);
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
  maxlen=max(maxlen,sizeof(base));
  return 1;
}

private string _ls_output_long(mixed filedata, int flags,closure valid_read,
                               closure valid_write,closure creator_file)
{
  string base=filedata[BASENAME];
  if (!(sizeof(base)) || ( (!(flags&LS_A)) && (base[0]=='.')) )
    return 0;
  int size=filedata[FILESIZE];
  string path=filedata[PATHNAME];
  string full=filedata[FULLNAME];
  int dir=(size==FSIZE_DIR);
  object ob=find_object(full);
  int ftime=filedata[FILEDATE];
  string date;
  if ((time()-ftime)>31536000) // ein Jahr
    date=strftime("%b %e %Y", ftime);
  else
    date=strftime("%b %e %H:%M", ftime);

  string creator="";
  string group="";
  if (flags&LS_U)
  {
    creator=({string})call_other(master(),"creator_file", full);
    switch(creator)
    {
      case ROOTID: creator="root"; break;
      case BACKBONEID: creator="std"; break;
      case MAILID: creator="mail"; break;
      case NEWSID: creator="news"; break;
      case NOBODY: creator="nobody"; break;
      case POLIZEIID: creator="polizei"; break;
      case DOCID: creator="doc"; break;
      case GUILDID: creator="gilde"; break;
      case ITEMID: creator="items"; break;
      default: if(!creator) creator="none"; break;
    }
  }
  if (flags&LS_G)
  {
    string *tmp=explode(path, "/") - ({""});
    if (sizeof(tmp))
    {
      switch(tmp[0])
      {
      case WIZARDDIR: group="magier"; break;
      case NEWSDIR: group="news"; break;
      case MAILDIR: group="mail"; break;
      case FTPDIR: group="public"; break;
      case PROJECTDIR: group="project"; break;
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
      if (size==FSIZE_NOFILE)
        base=sprintf(colorstrings[VC],base);
      else
        base=sprintf(colorstrings[OBJ],base);
    }
  }
  return sprintf(("%c%c%c%c %3d" + ((flags&LS_U) ? " %-24.24s" : "%-0.1s")
                 +((flags&LS_G) ? " %-8.8s" : "%0.1s") + " %8s %s %s\n"),
                 (dir ? 'd' : '-'),
                 (!funcall(valid_read,full,getuid(),
                           "read_file",this_object()) ? '-' : 'r'),
                 (!funcall(valid_write,full,getuid(),
                           "write_file",this_object()) ? '-' : 'w'),
                 (ob ? 'x' : '-'),
                 (dir ? (sizeof((get_dir(full+"/*")||({}))-({".",".."}))) : 0),
                 creator, group,
                 (dir ? "-" : size==FSIZE_NOFILE ? "<vc>" : to_string(size)),
                 date, base);
}


static int _ls(string cmdline)
{
  int flags,cmp,i,arg_size,size;
  int maxlen,counter,maxcount;
  mixed* args;
  string output;
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

  if ( !cmp && !(flags&LS_R) || cmp && (flags&LS_R) )
    sort_fun = function int (mixed* a, mixed* b) {
                 return (a[cmp] > b[cmp]);
               };
  else
    sort_fun = function int (mixed* a, mixed* b) {
                 return (a[cmp] < b[cmp]);
               };
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
    if (size==FSIZE_DIR)
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
  More(arg[0],1,#'_more_file,
       (sizeof(arg)>1 ? ({ arg[1..]}) : ({})) );
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
  if (arg[FILESIZE]==FSIZE_DIR)
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
    if (sizeof(args) > 1)
      args=args[1..];
    else
      break;
  }
  return 1;
}

//                              #######
//############################### MAN ###################################
//                              #######

static int _man(string cmdline)
{
  int i, flags;
  string *input;

  string* args = parseargs(_unparsed_args(), &flags, MAN_OPTS, 0);

  if (flags==-1 ||
      (sizeof(args)!=1 &&
       (sizeof(args)<2 || sizeof(args[1])>1 || !(i=to_int(args[1])))))
    return USAGE("man [-" MAN_OPTS "] <hilfeseite>");

  input = explode(args[0], "/");

  if (oldman_result && sizeof(input)==1 && sizeof(args)==1 &&
      sizeof(input[0])==1 && (i=to_int(input[0])) &&
      member(oldman_result,i))
  {
    input = ({oldman_result[i,0], oldman_result[i,1]});
    i = 0;
  }
  else if (!(flags&(MAN_M|MAN_R)) && sizeof(input)>1)
  {
    if (file_size(MAND_DOCDIR+args[0]) >= 0)
      input = ({input[<1], args[0]});
    else
      input = ({});
  }
  else
  {
    if (flags&MAN_R)
    {
      flags &= (~MAN_M);
      if (catch(regexp(({""}), args[0])) || !regexp(({""}), args[0]))
      {
        printf("man: Ungueltiger Ausdruck in Maske.\n");
        return 1;
      }
    }
    input = ({string *})MAND->locate(args[0], flags&(MAN_M|MAN_R));
  }

  oldman_result = 0;

  if(i && sizeof(input)>2 && sizeof(input) >= i*2)
    input = input[(i*2-2)..(i*2-1)];

  switch (sizeof(input))
  {
    case 0:
      printf("Keine Hilfeseite gefunden fuer '%s'.\n", args[0]);
      break;

    case 2:
       /*
         if (flags&MAN_I)
         {
         // BRANCH TO MANUALD
         return 1;
         }
       */
      printf("Folgende Hilfeseite wurde gefunden: %s\n", input[1]);
      More(MAND_DOCDIR+input[1], 1);
      return 1;
    default:
      i = sizeof(input)/2;
      string* output = allocate(i);
      oldman_result = m_allocate(i, 2);
      while (i)
      {
        output[i-1] = input[i*2-2];
        oldman_result[i,0] = input[i*2-2];
        oldman_result[i,1] = input[i*2-1];
        i--;
      }

      // Sortierung case-insensitive, ggf. vorhandene Pfade dabei ignorieren
      output = sort_array(output, function int (string t1, string t2)
             {
               t1 = explode(t1, "/")[<1];
               t2 = explode(t2, "/")[<1];
               return lower_case(t1) > lower_case(t2);
             });

      // Numerierung ergaenzen
      foreach(int j : sizeof(output))
      {
        output[j] = sprintf("%d: %s", j+1, output[j]);
      }

      int tty_cols = QueryProp(P_TTY_COLS)-2;
      string list = "Es wurden die folgenden, potentiell passenden Seiten "
        "gefunden:\n";

      // Wer keine Grafik sehen will, bekommt eine andere Anzeige.
      if (QueryProp(P_NO_ASCII_ART))
      {
        // @ als geschuetztes Leerzeichen verwenden, um einen Umbruch
        // nach den Nummern zu verhindern.
        output = map(output, #'regreplace, ": ", ":@", 1);
        list += break_string(implode(output, "  "), tty_cols);
        list = regreplace(list, ":@", ": ", 1);
      }
      else
      {
        // Anzahl Spalten ausrechnen: Terminalbreite / Laenge des laengsten
        // Elements in <output>. Kann der Spaltenmodus von sprintf() an sich
        // selbst, das liefert aber nicht immer so guenstige Ergebnisse.
        int maxwidth = max(map(output, #'sizeof));
        int tablecols = tty_cols/maxwidth;
        list += "-"*tty_cols+"\n"+
                sprintf("%#-*.*s\n", tty_cols, tablecols, implode(output,"\n"))+
                "-"*tty_cols+"\n";
      }
      printf(list);
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
        ({string})call_other(UDP_CMD_DIR+"man","send_request",args[1],args[0]));
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
      if (catch(load_object(str)))
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
   list=map(list,(: return $1[5..<2]+"h"; :));
   list+=map(list,(: return explode($1,"/")[<1]; :));
   list=map(m_indices(mkmapping(list)),(: return "/sys/"+$1; :));
   list=filter(list,(: return file_size($1)>0; :));
   list=sort_array(list, #'<);
   ausgabe="";
   for (i=sizeof(list);i--;)
   {
     str=implode(filter(explode(read_file(list[i]),"\n"),
                              (: return $1[0..9]=="#define P_";:)),"\n");
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

//
// grep_file: Datei greppen
// rexpr: Regular Expression
// flags: Flags
//

private int grep_file(mixed filedata, string rexpr, int flags)
{
  string fullname=filedata[FULLNAME];
  if ((flags&GREP_F)&&fullname=="/players/"+getuid()+"/grep.out")
  {
    write_file("/players/"+getuid()+"/grep.out",
               "Uebergehe grep.out ...\n");
    return RET_FAIL;
  }
  switch(filedata[FILESIZE])
  {
    case FSIZE_DIR: return RET_FAIL;
    case FSIZE_NOFILE: return ERROR(DOESNT_EXIST,fullname,RET_FAIL);
    case 0:  return RET_FAIL;
    default: break;
  }
  if (!MAY_READ(fullname))
    return ERROR(NO_READ,fullname,RET_FAIL);

  // Bei case-insensitiver Suche das Pattern in Kleinschreibung wandeln
  if (flags&GREP_I)
    rexpr=lower_case(rexpr);

  // Portionsweise das komplette File einlesen.
  int maxlen = query_limits()[LIMIT_BYTE];
  int ptr;
  bytes read_buffer;
  bytes data = b"";
  while (sizeof(read_buffer = read_bytes(fullname, ptr, maxlen)))
  {
    data += read_buffer;
    ptr += sizeof(read_buffer);
    // die Schleifenbedingung erkennt zwar auch, wenn das File vollstaendig
    // ist, aber wir koennen den Speicherzugriff auch einsparen und schauen,
    // ob wir schon alles haben.
    if (ptr >= filedata[FILESIZE])
      break;
  }
  // In string konvertieren, wir gehen davon aus, das File ist UTF8-kodiert.
  string text = to_text(data, "UTF-8");
  string *lines = explode(text, "\n");
  int count; // Anzahl Zeilen mit Treffern
  <string|string*> result = ({}); // zutreffende Zeilen
  int linecount = 1;
  foreach(string line: lines)
  {
    string orig_line = line;
    // Suche case-insensitive?
    if (flags&GREP_I)
      line = lower_case(line);
    int match = regmatch(line, rexpr) != 0;
    if (flags&GREP_V) match = !match; // Match ggf. invertieren
    if (match)
    {
      // Ausgeben oder nicht?
      if (!(flags&GREP_C))
      {
        // Mit Zeilennummer?
        if (flags&GREP_N)
          result+=({ sprintf("%4d %s", linecount, orig_line)});
        else
          result+=({orig_line});
      }
      ++count;
    }
    ++linecount;
  }

  if (count)
  {
    // Bei -h werden die Dateinamen unterdrueckt.
    if (flags&GREP_H)
      fullname="";
    else
      fullname=sprintf("%s ",fullname);

    if (flags&GREP_C)
      result=sprintf("%s%d passende Zeile%s.\n",fullname, count,
                           (count==1?"":"n"));
    else
      result = ( (sizeof(fullname) ? fullname + "\n" : "")
                + implode(result,"\n") + "\n");

    // Ergebnis ausgeben in File oder an Magier
    if (flags&GREP_F)
      return write_file("/players/"+getuid()+"/grep.out",result);
    write(result);
  }
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
  args=(sizeof(args)>1 ? args[1..] : ({}) );
  if (flags&GREP_M)
  {
    mask=args[<1];
    if (sizeof(args) > 2)
    args = (sizeof(args) > 1 ? args[0..<2] : ({}) );
  }
  if (!sizeof(args))
      return USAGE("grep [-" GREP_OPTS
          "] <regexp> <datei/verz> [<datei2> ... ] [<maske>]");
  args=map(args,#'to_filename)-({0});
  args=file_list(args,MODE_GREP,(flags&GREP_R?1:0),"/",mask);
  if (!sizeof(args))
    return printf("Keine passenden Dateien gefunden.\n"),1;
  if (flags&GREP_I) rexpr=lower_case(rexpr);
  if (flags&GREP_F)
  {
    if (file_size("/players/"+getuid()+"/grep.out")==FSIZE_DIR
        || !MAY_WRITE("/players/"+getuid()+"/grep.out"))
      return printf("grep: Datei /players/%s/grep.out kann nicht "
                    "geschrieben werden.\n",getuid()),1;
    else
      write_file("/players/"+getuid()+"/grep.out",
                 "Ausgabe von \"grep " + _unparsed_args() + "\":\n"); 
  }
  asynchron(args,#'grep_file,rexpr,flags,0);
  return 1;
}
