inherit "/std/thing";

#include <properties.h>
#include <language.h>

int tm;
int gestopft = 0;
int rauchen  = 0;

string des;

string _query_long(){
  string s;
  s="Ein einfaches Holzpfeifchen: Ohne Verzierungen oder besondere "
    "Extras, sondern schlicht und stabil genug fuer ein Abenteurerleben. "
    "Es eignet sich hervorragend dazu, einmal zu entspannen und guten "
    "Tabak zu geniessen";
  if(gestopft) s+=". Das Pfeifchen ist gestopft";
  if(rauchen)  s+=" und angezuendet.";
  else         s+=".";
  return(break_string(s,78));
}

string _query_short(){
  if(rauchen)  return "Ein Pfeifchen (angezuendet)";
  if(gestopft) return "Ein Pfeifchen (gestopft)";
  else         return "Ein Pfeifchen";
}

protected void create(){
  if(!clonep(this_object()))return ;
  thing::create();
  tm=0;
  SetProp(P_GENDER,NEUTER);
  SetProp(P_NAME,"Pfeifchen");
  SetProp(P_WEIGHT,10);
  SetProp(P_VALUE,0);
  AddId(({"pfeifchen","holzpfeifchen"}));
  SetProp(P_NODROP,1);
  SetProp(P_NEVERDROP,1);
  SetProp(P_MATERIAL,MAT_IRON_WOOD);
  AddCmd(({"rauch","rauche"}),"cm_smoke");
  AddCmd(({"stopf","stopfe"}),"cm_stopf");
}

static int cm_stopf(string str){
  string s1,s2;
  object ob;
  _notify_fail("Syntax: stopfe pfeifchen mit <was>\n");
  if(!str) return 0;
  if(sscanf(str,"%s mit %s",s1,s2)==2){
    if(id(s1)){
      if(!ob=present(s2,this_player())){
        write("So etwas hast Du doch gar nicht bei Dir.\n");
        return 1;
       }
      if((ob->id("tabak")) || (ob->QueryMaterial(MAT_CANNABIS)>50)){
        if(rauchen){
          write(break_string("Du rauchst Dein Pfeifchen doch gerade. Damit "
           "musst Du schon warten, bis Du fertig bist.",78));
          return 1;
         }
        if(gestopft){
          write(break_string("Du hast Dein Pfeifchen bereits gestopft. Es "
           "liesse sich nun vortrefflich rauchen.",78));
          return 1;
         }
        else{
          write(break_string("Du stopfst Dein Pfeifchen mit "+
           ob->name(WEM,1)+
           " und freust Dich schon darauf, es zu rauchen.",78));
          tell_room(environment(this_player()),this_player()->Name()+
           " stopft ein Pfeifchen.\n",({this_player()}));
          if(!ob->GetSmokeDescription()) des="Es scheint sich um keinen "
           "besonders guten Tabak zu handeln. Nun, ja. Besser als nichts "
           "immerhin.";
          else des=ob->GetSmokeDescription();
          if(!ob->QueryProp(P_AMOUNT))
            ob->remove();
          else
            ob->AddAmount(-1);
          gestopft = 1;
          return 1;
         }
       }
      write(break_string("Das laesst sich aber schlecht rauchen, such "
       "Dir lieber einen ordentlichen Tabak.",78));
      return 1;
     }
   }
  return(0); //non-void Funktion, Zesstra
}

static int cm_smoke(string str){
  object ob;

  _notify_fail("Was moechtest Du rauchen?\n");
  if(!str) return 0;
  if(!id(str)) return 0;
  if(rauchen){
    write("Das machst Du doch bereits, nicht so hektisch.\n");
    return 1;
   }
  if(!gestopft){
    write("Vielleicht solltest Du Dein Pfeifchen erst mal stopfen?\n");
    return 1;
   }
  if(tm > time()){
    write("Nana, ein Pfeifchen geniesst man. Sei mal nicht so hektisch.\n");
    return 1;
   }
  tm = time()+20;
  rauchen = 1;
  tell_room(environment(this_player()),this_player()->Name()+" raucht in "
   +"Ruhe ein Pfeifchen.\n",({this_player()}));
  write("Du rauchst in Ruhe Dein Pfeifchen.\n");
  write(break_string(des,78));
  if(find_call_out("smoke2")==-1)
    call_out("smoke2",20,getuid(this_player()));
  return 1;
}

int smoke2(object pl){
  object ob;
  if(!rauchen) return 0;
  if(!ob=find_player(pl)) return 0;
  if(environment()!=ob) return 0;
  tell_object(ob,"Genuesslich pustest Du ein paar Rauchkringel in die Luft.\n");
  tell_room(environment(ob),ob->Name()+" pustet genuesslich ein paar "
   +"Rauchkringel in die Luft.\n",({ob}));
  if(find_call_out("smoke3")==-1)
    call_out("smoke3",20,getuid(environment()));
  return 1;
}

int smoke3(object pl){
  object ob;
  if(!rauchen) return 0;
  if(!ob=find_player(pl)) return 0;
  if(environment()!=ob) return 0;
  tell_object(ob,break_string("Genuesslich pustest Du ein paar Rauchkringel "
   "in die Luft, rauchst Dein Pfeifchen auf und klopfst es aus.",78));
  tell_room(environment(ob),break_string(ob->Name()+" pustet genuesslich ein "
   "paar Rauchkringel in die Luft, beendet das Rauchpaeuschen und klopft "
   +(ob->QueryProp(P_GENDER)==1?"sein":"ihr")+" Pfeifchen aus.",78),({ob}));
  rauchen = 0;
  gestopft = 0;
  return 1;
}
