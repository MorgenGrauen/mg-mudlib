#pragma strong_types,save_types
#pragma no_shadow,no_clone

#include <living/team.h>
#include <wizlevels.h>
#define ME this_object()
#define PO previous_object()

static mapping team_names;
static mapping team_names_reverse;

void create() {

  team_names=([]);         // Objekt -> Name
  team_names_reverse=([]); // Name -> Objekt
}

static mixed DoRegisterTeam(object ob, string name) {
  mixed old;

  old=team_names[ob];
  m_delete(team_names_reverse,old); // Namen Freigeben
  if (!name) { // UnRegister
    m_delete(team_names,ob);
    return name;
  }
  team_names[ob]=name;
  team_names_reverse[name]=ob;
  return name;
}

varargs string RegisterTeam(string name) {
  int i,min;

  if (member(inherit_list(PO),TEAM_OBJECT+".c")<0)
    return 0;
  if (stringp(name)){
    if (!team_names_reverse[name]) // Name noch nicht vergeben
      return DoRegisterTeam(PO,name);
    return 0;
  }
  min=sizeof(m_indices(team_names_reverse))+2;
  for (i=1;i<min;i++)
    if (!team_names_reverse[sprintf("%d",i)]) {
      min=i;
      break;
    }
  name=sprintf("%d",i);
  return DoRegisterTeam(PO,name);
}

void UnregisterTeam() {
  if (member(inherit_list(PO),TEAM_OBJECT+".c")>=0)
    DoRegisterTeam(PO,0);
}

object *ListTeamObjects() {
  return(m_indices(team_names)-({0}));
}

string *ListTeamNames() {
  return(m_indices(team_names_reverse)-({0}));
}

string *SortedListTeamNames() {
  return(sort_array(ListTeamNames(),#'<));
}

void ShowTeamInfos() {
  object ob;
  string *ind;
  int i;
  
  write(" --- Teamliste: ---\n");
  ind=SortedListTeamNames();
  for (i=sizeof(ind)-1;i>=0;i--)
    if (objectp(ob=team_names_reverse[ind[i]]))
      ob->ShowTeamInfo();
  write(" --- (Ende Teamliste) ---\n");
}

varargs int remove(int silent) {
  return 0;
}
