// $Id: todo.c 9142 2015-02-04 22:17:29Z Zesstra $
#pragma strict_types
#pragma save_types
#pragma range_check
#pragma no_clone
#pragma pedantic

#include <wizlevels.h>
#include <defines.h>
#define NEED_PROTOTYPES
#include <magier.h>
#include <player.h>

#define SAVEFILENAME sprintf("/players/%s/.todoliste",getuid())

private nosave status todo_initialized;
private nosave mixed *todo_data;
mixed *todo_data_public;
static mixed _query_localcmds()
{
  return ({
    ({"todo","_todo",0,WIZARD_LVL})});
}

private void todo_save()
{

  int i,j;
  string a,b;
  a=SAVEFILENAME+".o";
  rm(a);
  if (j=sizeof(todo_data))
  {
#if !__EFUN_DEFINED__(save_value)   
    write_file(a,"#0:0\ntodo_data_public ({");
    for (i=0;i<j;i++)
    {
      b=regreplace(todo_data[i][1],"\"","\\\"",1);
      b=regreplace(b,"\n","\\n",1);
      write_file(a,sprintf("({%d,\"%s\",}),",
                           todo_data[i][0],b));
    }
    write_file(a,"})\n");
#else
    todo_data_public=todo_data;
    write_file(a,save_value(todo_data_public));
    todo_data_public=0;
#endif
  }
  return;

}

static void initialize()
{
  if(!IS_WIZARD(this_object())) return;
  if (!restore_object(SAVEFILENAME)) todo_data=({});
  else
    {
      todo_data=todo_data_public;
      todo_data_public=0;
    }
  return;
}

static void _todo_neu2(string input,string carry)
{
  if (input=="q"||(input=="."&&!sizeof(carry)))
  {
    printf("Abbruch!");
    return;
  }
  if (input!=".")
  {
    printf("]");
    input_to("_todo_neu2",0,carry+input+"\n");
    return;
  }
  todo_data+=({ ({ time(), carry }) });
  todo_save();
  printf("Eintrag angelegt. Laufende Nummer ist %d.\n",sizeof(todo_data));
  return;
}

private int _todo_neu(string cmdline)
{
  if (cmdline!="neu") return 0;
  printf("Was musst Du noch machen? (Abbruch mit 'q', "
         "Ende mit '.')\n]");
  input_to("_todo_neu2",0,"");
  return 1;
}

private int _todo_loeschen(string cmdline)
{
  int nr;
  if (!sscanf(cmdline,"loeschen %d",nr))
    return notify_fail("Welchen Eintrag moechtest Du loeschen?\n"),0;
  if (!sizeof(todo_data))
    return notify_fail("Deine Todo-Liste hat ja ueberhaupt keinen "
                       "Eintrag!\n"),0;
  if (nr>sizeof(todo_data))
  {
    printf("Deine Todo-Liste hat nur %d Eintra%s.\n",sizeof(todo_data),
           (sizeof(todo_data)==1?"g":"ege"));
    return 1;
  }
  todo_data[nr-1]=0;
  todo_data-=({0});
  todo_save();
  printf("Eintrag Nummer %d wurde geloescht. Da hast Du ja ordentlich "
         "was geschafft...\noder geschickt delegiert ;-)\n",nr);
  return 1;
}

private int _todo_anzeigen(string cmdline)
{
  int nr1,nr2;
  string output;
  if (!sizeof(todo_data))
    return notify_fail("Deine Todo-Liste hat keine Eintraege.\n"),0;
  switch(sscanf(cmdline,"anzeigen %d bis %d",nr1,nr2)) 
  {
  case 0: nr1=1; nr2=sizeof(todo_data); break;
  case 1: nr2=nr1; break;
  case 2: break;
  }
  if (nr1<0) nr1=sizeof(todo_data)+nr1+1;
  if (nr2<0) nr2=sizeof(todo_data)+nr2+1;
  if (nr1<1||nr2<1||nr1>sizeof(todo_data)||nr2>sizeof(todo_data))
    return notify_fail(sprintf("Deine Todo-Liste hat (gluecklicherweise) "
                               "nur %d Eintra%s.\n",sizeof(todo_data),
                               (sizeof(todo_data)>1?"ege":"g"))),0;
  output="\n-----------------------------------------------------------------------------\n";
  if (nr1>nr2) // Rueckwaerts
  {
    while(nr1>=nr2)
      output+=sprintf("Eintrag Nr. %d von %s:\n%s\n\n",nr1--,
           dtime(todo_data[nr1][0]),break_string(todo_data[nr1][1],78,5,
                                       BS_LEAVE_MY_LFS));
  }
  else
  {
    nr1--;
    while (nr1<nr2)
      output+=sprintf("\nEintrag Nr. %d von %s:\n%s\n",(nr1+1),
           dtime(todo_data[nr1][0]),break_string(todo_data[nr1++][1],78,5,
                                       BS_LEAVE_MY_LFS));
  }
  More(output+"-----------------------------------------------------------------------------\n");
  return 1;
}

private int _todo_verschieben(string cmdline)
{
  int from;
  mixed to,dummy;
  if (!(sscanf(cmdline,"verschieben %d nach %d",from,to)==2||
        sscanf(cmdline,"verschieben %d nach %s",from,to)==2))
    return 0;
  from--;
  if (stringp(to))
  {
    switch(to)
    {
      case "oben": to=from-1; break;
      case "unten": to=from+1; break;
      default: return 0;
    }
  }
  else to--;
  if (to==from) return notify_fail("Da ist der Eintrag ja schon!\n"),0;
  if (from<0||from>=sizeof(todo_data)) return notify_fail(
        "Wie willst Du einen nicht existenten Eintrag verschieben?\n"),0;
  if (to<0||to>=sizeof(todo_data)) return notify_fail(
        "Dahin kannst Du den Eintrag nicht verschieben.\n"),0;
  dummy=todo_data[from];
  if (to>from)
    while (from++<to) todo_data[from-1]=todo_data[from];
  else
    while(from-->to) todo_data[from+1]=todo_data[from];
  todo_data[to]=dummy;
  printf("Eintrag wurde verschoben.\n");
  return 1;
}

private int _todo_hilfe()
{
  printf(
  "-------------------------------------------------------------------------------\n\n"
  " Die Todo-Liste laesst sich mit den folgenden Befehlen steuern:\n\n"
  " todo neu                                - Legt einen neuen Eintrag an.\n"
  " todo loeschen <nummer>                  - Loescht einen Eintrag.\n"
  " todo anzeigen [<nummer> [bis <nummer>]] - Zeigt die Todo-Liste an.\n"
  " todo verschieben <nummer> nach <ziel>   - Verschiebt einen Eintrag.\n\n"
  " <ziel> kann eine Zahl oder die Worte 'oben' oder 'unten' sein.\n"
  "-------------------------------------------------------------------------------\n");
  return 1;
}


static int _todo(string cmdline)
{
  string *args;
  
  cmdline=_unparsed_args(1);
  notify_fail("Falsche Syntax. 'todo hilfe' zeigt eine Hilfe an.\n");
  if (sizeof(cmdline))
  {
    args=explode(cmdline," ");
    switch(args[0])
    {
      case "neu":         return _todo_neu(cmdline);
      case "loeschen":    return _todo_loeschen(cmdline);
      case "anzeigen":    return _todo_anzeigen(cmdline);
      case "verschieben": return _todo_verschieben(cmdline);
      case "hilfe":       return _todo_hilfe();
     default:            return 0;
    }
  }
  return 0;
}
