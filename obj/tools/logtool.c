// (c) by Padreic (Padreic@mg.mud.de)

inherit "std/secure_thing";

#include <properties.h>
#include <wizlevels.h>
#include <defines.h>
#include <moving.h>

#define SAVEFILE  "/players/"+geteuid()+"/.logtool"

// definieren von "inline" Funktionen:
#define allowed()   (!process_call() && PL==RPL && geteuid()==getuid(RPL))

mapping logs;

static void check_logs()
{
  filter(sort_array(m_indices(logs), #'>), lambda( ({ 'x }), ({#'?, 
    ({#'!=, ({#'[, logs, 'x, 2}), ({#'file_time, ({#'[, logs, 'x, 0}) }) }),
       ({#'tell_object, ({#'environment }), ({#'sprintf,
       "Neue Eintraege in <%s>.\n", 'x }) }), 0})));
}

void create()
{
  if (!clonep(ME)) return;
  ::create();
  if (!restore_object(SAVEFILE)) {
    logs=([ "repfile": sprintf("/log/report/%s.rep", geteuid()); 0; 0 ]);
    if (IS_ARCH(geteuid()))
      logs+=(["snooplog": "/log/SNOOP"; 0; 0, "killer": "/log/KILLER"; 0; 0]);
  }
  SetProp(P_SHORT, this_player()->Name(WESSEN)+" Logtool");
  SetProp(P_NAME, QueryProp(P_SHORT));
  SetProp(P_NODROP, 0);
  SetProp(P_NEVERDROP, 0);
  SetProp(P_AUTOLOADOBJ, 1);
  AddId("logtool");
  AddCmd("mark", "aktualisieren");
  AddCmd(m_indices(logs), "read_log");
  AddCmd("AddLog", "add_log");
  AddCmd("DelLog", "del_log");
}

void reset()
{
  ::reset();
  if (!clonep(ME) || !environment()) return;
  call_out("check_logs", 0);
  set_next_reset(2*__RESET_TIME__); // kein Zufall, max. Laenge
}

varargs int move(mixed dest, int method)
{
  int i;
  if (!objectp(dest)) dest=find_object(dest);
  if (!dest || !interactive(dest) || getuid(dest)!=geteuid()) 
    return ME_CANT_BE_TAKEN;
  i=::move(dest, method);
  if (i!=1) return i;
  if (environment()) call_out("check_logs", 0);
  return 1;
}

varargs string long() 
{
   return "Folgende Logfiles werden derzeit von Deinem Logtool verwaltet:\n\n"
         +implode(map(sort_array(m_indices(logs), #'>),
          lambda( ({ 'x }), ({#'sprintf, " %1s %-14s - %s", ({#'?, ({#'!=,
          ({#'[,logs,'x, 2}), ({#'file_time,({#'[,logs,'x, 0})})}),"*",""}),
          ({#'sprintf, "<%s>", 'x}), ({#'[, logs, 'x, 0})}) ) ), "\n")
         +"\n\nMit <mark> koennen alle Eintraege als gelesen markiert und "
         +"mit Add- und\nDelLog Logfiles in die Liste aufgenommen bzw. aus "
         +"der Liste entfernt werden.\n";
}

static int aktualisieren(string str)
{
  if (!allowed()) return 0;
  if (str && str!="") {
    if (!logs[str]) {
      notify_fail("Ein solches Logfile ist nicht in der Liste enthalten.\n");
      return 0;
    }
    logs[str, 1]=file_size(logs[str, 0]);
    logs[str, 2]=file_time(logs[str, 0]);
  }
  else filter(m_indices(logs), lambda( ({ 'x }),({#', ,
   ({#'=, ({#'[, logs, 'x, 1}), ({#'file_size, ({#'[, logs, 'x, 0}) }) }),
   ({#'=, ({#'[, logs, 'x, 2}), ({#'file_time, ({#'[, logs, 'x, 0}) })})})));
  write("Done!\n");
  save_object(SAVEFILE);
  return 1;
}

static int read_log(string str)
{
  int si;
  if (!allowed()) return 0;
  si=file_size(logs[query_verb(),0]);
  if (str)
    PL->more(logs[query_verb(),0]);
  else {
    if (si<0 || file_time(logs[query_verb(), 0])==logs[query_verb(), 2])
      write("Keine neuen Eintraege in <"+query_verb()+">.\n");
    else {
      write("Folgendes ist neu in <"+query_verb()+">.\n");
      if (si<logs[query_verb(),1])
        PL->more(logs[query_verb(),0]);
      else PL->More(read_bytes(logs[query_verb(),0],
	    logs[query_verb(),1],si-logs[query_verb(),1]));
    }
  }
  logs[query_verb(), 1]=si;
  logs[query_verb(), 2]=file_time(logs[query_verb(), 0]);
  save_object(SAVEFILE);
  return 1;
}

static int add_log()
{
  string key, path, str;
  str=PL->_unparsed_args();  
  if (!allowed() || !str || sscanf(str, "%s %s", key, path)!=2) {
    notify_fail("Syntax: AddLog <key> <path>\n");
    return 0;
  }
  key=lower_case(key);
  if (logs[key]) {
    notify_fail("Es gibt schon ein Logfile mit diesem Key in der Liste.\n");
    return 0;
  }
  logs+=([ key: path; 0; 0 ]);
  AddCmd(key, "read_log");
  save_object(SAVEFILE);
  write("Logfile <"+key+"> in die Liste aufgenommen.\n");
  return 1;
}

static int del_log(string str)
{
  if (!allowed() || !str) {
    notify_fail("WELCHES Logfile soll aus der Liste entfernt werden?\n");
    return 0;
  }
  if (!logs[str]) {
    notify_fail("Logfile nicht in Liste enthalten.\n");
    return 0;
  }
  logs=m_copy_delete(logs,str);
  RemoveCmd(str);
  save_object(SAVEFILE);
  write("Logfile <"+str+"> aus Liste entfernt.\n");
  return 1;
}
