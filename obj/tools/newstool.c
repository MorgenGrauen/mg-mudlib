inherit "std/thing";

#include <properties.h>
#include <language.h>
#include <config.h>
#include <news.h>
#include <defines.h>

#define UA PL->_unparsed_args()

void create()
{
  string str;
  if (!clonep(this_object())) return;
  ::create();
  SetProp(P_LONG,"Ein News-Gruppen-Tool.\n\
Befehle: addg      <grname> <ownername>   : Gruppe erzeugen\n\
         removeg   <grname>               : Gruppe loeschen\n\
         addw      <grname> <username>    : Schreibberechtigten eintragen\n\
         removew   <grname> <username>    : Schreibberechtigten austragen\n\
         addd      <grname> <username>    : Loeschberechtigten eintragen\n\
         removed   <grname> <username>    : Loeschberechtigten austragen\n\
         addr      <grname> <username>    : Leseberechtigten eintragen\n\
         remover   <grname> <username>    : Leseberechtigten austragen\n\
         ginfo     <grname>               : Info ueber Gruppe abfragen\n\
         setexpire <grname> <tage>        : Expiretime setzen\n\
         setmaxmsg <grname> <anz>         : Maxmsg setzen\n\
         setg <grname> <loeschlev> <schreiblev> <leselev> <maxmsg> <expire>\n");
  str=capitalize(((str=getuid(this_player()))[<1]=='s'||str[<1]=='x'||
		  str[<1]=='z')?str+="'":str+="s");
  SetProp(P_SHORT,str+" NewsTool");
  SetProp(P_NAME,str+" NewsTool");
  SetProp(P_GENDER,NEUTER);
  SetProp(P_ARTICLE,0);
  AddId("newstool");
  AddId("newsgruppentool");
  AddId("news-gruppen-tool");
  SetProp(P_AUTOLOADOBJ,1);
  SetProp(P_NODROP,1);
}

void init()
{
  add_action("addg","addg");
  add_action("removeg","removeg");
  add_action("addw","addw");
  add_action("removew","removew");
  add_action("addd","addd");
  add_action("removed","removed");
  add_action("addr","addr");
  add_action("remover","remover");
  add_action("setg","setg");
  add_action("setexpire","setexpire");
  add_action("expire","expire");
  add_action("ginfo","ginfo");
  add_action("setmaxmsg","setmaxmsg");
}

int addr(string str)
{
  mixed *arr;
  string st1, st2;
  
  if ((str=UA)=="" || sscanf(str,"%s %s",st1,st2) !=2){
    write("addr <groupname> <username>\nAnschliessend darf der User unabhaengig vom Wizlevel die Gruppe lesen.\n");
    return 1;
  }
  
  arr=old_explode(st2," ")-({""});
  switch ("/secure/news"->AddAllowed(st1,({}),({}),arr)){
    case 0: write("Param err\n"); break;
    case 1: write("Ok.\n"); break;
    case -1: write("No permission\n"); break;
    case -2: write("No such board\n"); break;
  }
  return 1;
}

int remover(string str)
{
  mixed *arr;
  string st1, st2;

  if (!(str=UA) || sscanf(str,"%s %s",st1,st2) !=2){
    write("remover <gruppenname> <username>\nDanach darf der User die Gruppe nur noch lesen, wenn sein Wizlevel dies erlaubt.\n");
    return 1;
  }

  arr=old_explode(st2," ")-({""});
  switch ("/secure/news"->RemoveAllowed(st1,({}),({}),arr)){
    case 0: write("Param err\n"); break;
    case 1: write("Ok.\n"); break;
    case -1: write("No permission\n"); break;
    case -2: write("No such board\n"); break;
  }
  return 1;
}

int addd(string str)
{
  mixed *arr;
  string st1, st2;

  if (!(str=UA) || sscanf(str,"%s %s",st1,st2) !=2){
    write("addd <gruppe> <user>\nUser darf Artikel loeschen (unabh vom Wizlevel)\n");
    return 1;
  }

  arr=old_explode(st2," ")-({""});
  	switch ("/secure/news"->AddAllowed(st1,arr,({}),({}))){
    case 0: write("Param err\n"); break;
    case 1: write("Ok.\n"); break;
    case -1: write("No permission\n"); break;
    case -2: write("No such board\n"); break;
  }
  return 1;
}

int removed(string str)
{
  mixed *arr;
  string st1, st2;

  if (!(str=UA) || sscanf(str,"%s %s",st1,st2) !=2){
    write("removed <gruppe> <user>\nUser darf nur noch Artikel loeschen, wenn sein Wizlevel das erlaubt.\n");
    return 1;
  }

  arr=old_explode(st2," ")-({""});
  switch ("/secure/news"->RemoveAllowed(st1,arr,({}),({}))){
    case 0: write("Param err\n"); break;
    case 1: write("Ok.\n"); break;
    case -1: write("No permission\n"); break;
    case -2: write("No such board\n"); break;
  }
  return 1;
}

int addw(string str)
{
  mixed *arr;
  string st1, st2;

  if (!(str=UA) || sscanf(str,"%s %s",st1,st2) !=2){
    write("addw <gruppe> <user>\nUser darf Artikel schreiben (unabh vom Wizlevel)\n");
    return 1;
  }

  arr=old_explode(st2," ")-({""});
  switch ("/secure/news"->AddAllowed(st1,({}),arr,({}))){
    case 0: write("Param err\n"); break;
    case 1: write("Ok.\n"); break;
    case -1: write("No permission\n"); break;
    case -2: write("No such board\n"); break;
  }
  return 1;
}

int removew(string str)
{
  mixed *arr;
  string st1, st2;

  if (!(str=UA) || sscanf(str,"%s %s",st1,st2) !=2){
    write("removew <gruppe> <user>\nUser darf nur noch Artikel schreiben, wenn sein Wizlevel dies erlaubt.\n");
    return 1;
  }

  arr=old_explode(st2," ")-({""});
  switch ("/secure/news"->RemoveAllowed(st1,({}),arr,({}))){
    case 0: write("Param err\n"); break;
    case 1: write("Ok.\n"); break;
    case -1: write("No permission\n"); break;
    case -2: write("No such board\n"); break;
  }
  return 1;
}

int setg(string str)
{
  string name;
  int dlevel, wlevel, rlevel, maxmessages, expire;

  if (!(str=UA) ||
      sscanf(str,"%s %d %d %d %d %d",name,dlevel,wlevel,rlevel,maxmessages,expire) != 6) {
    write("setg <gruppe> <loeschlevel> <schreiblevel> <leselevel> <maxmessages> <expire>\nGruppenparameter setzen, Bedeutung (Defaultwerte in Klammern):\n\
    name              : Der Name der Newsgroup\n\
    loeschlevel  (20) : Wizlevel, ab dem User Artikel loeschen koennen\n\
    schreiblevel ( 0) : Wizlevel, ab dem User Artikel schreiben koennen\n\
    leselevel    ( 0) : Wizlevel, ab dem User Artikel lesen koennen\n\
    maxmessages  (40) : Maximale Anzahl Artikel in einer Newsgruppe\n\
    expire       (-1) : Expiretime in Tagen, -1 = nie\n");
    return 1;
  }
  expire=86400*expire;
  switch("/secure/news"->SetGroup(name, dlevel, wlevel, rlevel, maxmessages,expire)){
    case 1: write("Ok\n"); break;
    case 0: write("Param error.\n"); break;
    case -1: write("No permission\n"); break;
    case -2: write("No such group\n"); break;
  }
  return 1;
}

int setexpire(string str)
{
  string name;
  int expire;
  mixed *gr;

  if (!(str=UA)||sscanf(str,"%s %d",name,expire)!=2)
  {
    write("setexpire <gruppe> <tage>\n");
    return 1;
  }
  expire=86400*expire;
  if (!pointerp((gr="/secure/news"->GetGroup(name))))
  {
    write("Get group error\n");
    return 1;
  }
  switch("/secure/news"->SetGroup(name, gr[G_DLEVEL], gr[G_WLEVEL], gr[G_RLEVEL], gr[G_MAX_MSG],expire)){
    case 1: write("Ok\n"); break;
    case 0: write("Param error.\n"); break;
    case -1: write("No permission\n"); break;
    case -2: write("No such group\n"); break;
  }
  return 1;
}

int setmaxmsg(string str)
{
  string name;
  int maxmsg;
  mixed *gr;
  
  if (!(str=UA)||sscanf(str,"%s %d",name,maxmsg)!=2)
  {
    write("setexpire <gruppe> <tage>\n");
    return 1;
  }
  if (!pointerp((gr="/secure/news"->GetGroup(name))))
  {
    write("Get group error\n");
    return 1;
  }
  switch("/secure/news"->SetGroup(name, gr[G_DLEVEL], gr[G_WLEVEL], gr[G_RLEVEL], maxmsg, gr[G_EXPIRE])){
    case 1: write("Ok\n"); break;
    case 0: write("Param error.\n"); break;
    case -1: write("No permission\n"); break;
    case -2: write("No such group\n"); break;
  }
  return 1;
}

int ginfo(string str)
{
  mixed *gr;
  int i;
  
  if (!(str=UA))
  {
    write("Usage: ginfo <gname>\n");
    return 1;
  }
  if (!pointerp((gr="/secure/news"->GetGroup(str))))
  {
    write("Get group error\n");
    return 1;
  }
  if (gr[11]==-1)
  {
    write("Group \""+gr[0]+"\" does not exist.\n");
    return 1;
  }
        //         1         2         3         4         5         6         7 
        //123456789012345678901234567890123456789012345678901234567890123456789012345678
  printf("------------------------------------------------------------------------------\n");
  
  printf("Rubrik  : %s (%s) Savefile: %s\n\n",gr[0],capitalize(gr[1]),gr[2]);
  printf("Messages: %i/%i    Expire: %i Tage.\n\n",
		  (pointerp(gr[11])?sizeof(gr[11]):0),gr[10],gr[3]/86400);  
  
  printf( break_string((sizeof(gr[6])?"("+(CountUp(map(gr[6],#'capitalize)))+")":" "), 78, "Readers : "+gr[9]+" ",16));
  
  printf( break_string((sizeof(gr[5])?"("+(CountUp(map(gr[5],#'capitalize)))+")":" "), 78, "Writers : "+gr[8]+" ",16));
  
  printf( break_string((sizeof(gr[4])?"("+(CountUp(map(gr[4],#'capitalize)))+")":" "), 78, "Deleters: "+gr[7]+" ",16));
  
  printf("------------------------------------------------------------------------------\n");
  return 1;
}
    
int removeg(string str)
{
  if (!(str=UA))
  {
    write("removeg <gruppenname>\nGruppe LOESCHEN.\n");
    return 0;
  }
  switch("/secure/news"->RemoveGroup(str)){
    case 1: write("Ok. Bitte nicht vergessen, das Savefile der Newsgroup von Hand zu entfernen !\n"); break;
    case 0: write("Param error.\n"); break;
    case -1: write("No permission\n"); break;
    case -2: write("No such group"); break;
  }
  return 1;
}

int addg(string str)
{
  string name, owner;

  if (!(str=UA) || sscanf(str,"%s %s",name,owner)!=2)
  {
    write("addg <gruppe> <user>\nGruppe anlegen, <user> wird Besitzer der Gruppe\n");
    return 1;
  }
  switch ("secure/news"->AddGroup(name, owner)){
    case 1: write("Ok.\n"); break;
    case 0: write("Param error.\n"); break;
    case -1: write("No permission.\n"); break;
    case -2: write("Group already there\n"); break;
    case -3: write("Owner not found.\n"); break;
    case -4: write("Savefile already in use.\n"); break;
  }
  return 1;
}
  
int expire()
{
  "/secure/news"->expire_all();
  write("Expiring startet.\n");
  return 1;
}
