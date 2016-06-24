// MorgenGrauen MUDlib
//
// newsclient.c -- Nachrichtenbretter
//
// $Id: newsclient.c 8349 2013-02-04 21:15:28Z Zesstra $

/* 1992 JOF */
#pragma strong_types
#pragma save_types
#pragma no_clone
#pragma range_check
#pragma pedantic

inherit "/std/thing";
inherit "/std/more";

#include <config.h>
#include <properties.h>
#include <news.h>
#include <wizlevels.h>
#include <language.h>
#include <defines.h>
#include <input_to.h>

string GROUP;
string writer;
string usemsg;

void SetInUseMessage(string str)
{
  usemsg=str;
}

string SetGroup(string str)
{
  return GROUP=str;
}

protected void create()
{
  thing::create();
  writer=="";

}

void thinginit() /* If someone doesnt want our add_action he may call this */
{
  thing::init();
}

varargs int remove(int silent)
{
  return thing::remove();
}

void init()
{
  thing::init();
  add_action("schreib","schreib",1);
  add_action("lies","lies");
  add_action("loesche","loesch",1);
}

string ladj(string str, int weite)
{
  return((str+"                                            ")[0..weite-1]);
}

string QueryLong()
{
  mixed *messages;
  string desc;
  int i;

  desc=QueryProp(P_LONG);
  desc+="\n";
  messages=NEWSSERVER->GetNotes(GROUP);
  if (!pointerp(messages) || !sizeof(messages)) 
    return desc+"Zur Zeit befinden sich keine Nachrichten am Brett.\n";
  if (sizeof(messages)==1)
    desc+="Zur Zeit befindet sich eine Nachricht am Brett:\n\n";
  else
    desc+="Zur Zeit befinden sich "+sizeof(messages)+" Nachrichten am Brett:\n\n";
  for (i=0;i<sizeof(messages);i++)
  {
    if (i<9) desc+=" ";
    desc+=""+(i+1)+". ";
    desc+=(ladj(messages[i][M_TITLE],45)+" ("+
	   ladj(messages[i][M_WRITER]+",",13)+
	   dtime(messages[i][M_TIME])[4..24] + ")\n");
  }
  return desc;
}

varargs string long(int mode)
{
  return QueryLong();
}

mixed *message;

int schreib(string str)
{
  int err;

  if (!this_interactive() || !this_interactive()->query_real_name()) return 0;
  if (writer)
  {
    write("Das geht im Moment nicht, da "+capitalize(writer)+" gerade an diesem Brett schreibt.\n");
    if (usemsg) write(usemsg+"\n");
    return 1;
  }
  _notify_fail("Du musst einen Titel fuer die Notiz angeben !\n");
  if (!str) return 0;
  switch (err=NEWSSERVER->AskAllowedWrite(GROUP))
  {
    case  1: break;
	     case -1: write("Du darfst keine Notes an dieses Brett heften !\n"); return 1;
	     case -3: write("Das Brett ist leider voll !\n"); return 1;
	     default: write("Interner Fehler "+err+", Erzmagier verstaendigen !\n"); return 1;
	   }
  writer=this_interactive()->query_real_name();
  message=allocate(6);
  message[M_BOARD]=GROUP;
  message[M_TITLE]=str;
  message[M_MESSAGE]="";
  write("Ok, beende Deine Notiz mit '**' oder brich sie mit '~q' ab !\n");
  if (IS_WIZARD(this_player()))
    write("Als Magier"+(this_player()->QueryProp(P_GENDER)==FEMALE ? "in" : "")+
	  " kannst Du per '~r<filename>' ein File in die Notiz einlesen.\n");
  input_to("get_note_line", INPUT_PROMPT, ":");
  return 1;
}

int get_note_line(string str)
{
  int err;
  if (str=="**" || str=="~.")
  {
    writer=0;
    switch (err=(NEWSSERVER->WriteNote(message)))
    {
      case 1:  write("Ok, Nachricht aufgehaengt.\n");
	       say(message[M_WRITER]+" hat eine Nachricht ans Brett gehaengt.\n");
	       return 1;
	       case -1: write("Du darfst leider keine Nachrichten hier aufhaengen.\n");
	       return 1;
	       case -3: write("Das Brett ist leider voll !\n");
	       return 1;
	       default: write("Interner Fehler "+err+" aufgetreten ! Bitte Erzmagier verstaendigen.\n");
	       return 1;
	     }
  }
  if (str=="~q")
  {
    writer=0;
    write("Notiz nicht angeheftet.\n");
    return 1;
  }
  if (str[0..1]=="~r" && IS_WIZARD(this_player())) {
    str=str[2..<1];
    if (str[0]==' ') str=str[1..<1];
    if (!str || catch(err=file_size(str);publish) || err<0) {
      write("File nicht gefunden.\n");
      input_to("get_note_line", INPUT_PROMPT, ":");
      return 1;
    }
    str=read_file(str);
    if (!str){
      write("Zu gross!\n");
      input_to("get_note_line", INPUT_PROMPT, ":");
      return 1;
    }
    write("Ok.\n");
  }
  if (message[M_MESSAGE] && message[M_MESSAGE]!="") message[M_MESSAGE]+="\n";
  message[M_MESSAGE]+=str;
  input_to("get_note_line", INPUT_PROMPT, ":");
  return 1;
}

int lies(string str)
{
  int num;
  mixed *messages;

  if (!str || str=="" || sscanf(str,"%d",num)!=1 || num<=0)
  {
    _notify_fail("WELCHE Nachricht willst Du lesen ?\n");
    return 0;
  }
  num--;
  messages=(NEWSSERVER->GetNotes(GROUP));
  if (sizeof(messages)<=num)
  {
    _notify_fail("So viele Nachrichten sind da nicht !\n");
    return 0;
  }
  this_player()->More(messages[num][M_TITLE]+" ("+messages[num][M_WRITER]+", "+
      dtime(messages[num][M_TIME])[5..26] + "):\n" + 
       messages[num][M_MESSAGE]);
  return 1;
}

int loesche(string str)
{
  int num;

  if (!str || str=="" || sscanf(str,"%d",num)!=1 || num<=0)
  {
    _notify_fail("WELCHE Nachricht willst Du loeschen ?\n");
    return 0;
  }
  num--;
  switch (NEWSSERVER->RemoveNote(GROUP, num)){
    case 1: write("Ok.\n");
	    say(this_player()->name()+" entfernt eine Nachricht vom Brett.\n");
	    return 1;
	    case -1: write("Das darfst Du nicht.\n");
	    say(this_player()->name()+" versucht, eine Nachricht vom Brett zu reissen.\n");
	    return 1;
	    case -3: write("So viele Nachrichten sind da nicht !\n");
	    return 1;
	    default: write("Interner Fehler. Bitte Erzmagier verstaendigen !\n");
	    return 1;
	  }
  return 0;
}

void Crumble()
{
  if (environment() && living(environment()))
    return;
  say(capitalize(name(WER))+" zerfaellt zu Staub, der sich sofort verteilt.\n");
  write(capitalize(name(WER))+" zerfaellt zu Staub, der sich sofort verteilt.\n");
  remove();
}

varargs int move(object target,mixed method)
{
  if (objectp(target)&&!living(target)&&environment()&&living(environment()))
    call_out("Crumble",30+random(30));
  return ::move(target, method);
}
