#pragma strong_types,save_types

#include <defines.h>
#include <properties.h>

#define WEG() destruct(this_object())

object player;
int gender;
string desc;

void _tarn_turn_on(object pl,string txt,int gen)
{
  if (!objectp(pl)||!interactive(pl)) return WEG();
  if (!stringp(txt)||txt=="") return WEG();
  if (gen<0||gen>2) return WEG();
  player=pl;
  desc=capitalize(txt);
  gender=gen;
  shadow(pl,1);
}

int special_verb()
{
  string verb;
  
  verb=query_verb();
  if (!verb||verb=="") return 0;
  if (verb[0]=='-') return 1;
  if (verb=="ruf"||verb=="rufe"||verb=="teile"||verb=="teil"||verb=="mruf"||
      verb=="mrufe"||verb=="erzaehl"||verb=="erzaehle") return 1;
  return(0); // non-void funktion, Zesstra
}

int _query_gender()
{
  return gender;
}

string _query_name()
{
  if (!special_verb())
    return capitalize(desc);
  return player->Query(P_NAME);
}

string _query_short()
{
  return capitalize(player->name());
}

string _query_long()
{
  string str;

  str=player->name();
  return capitalize(str)+" ist "+str+" ist "+str+".\n";
}

string* _query_ids()
{
  return player->Query("ids")+({lower_case(desc)});
}

string _query_race()
{
  return desc;
}

void _tarn_turn_off()
{
  unshadow();
  destruct(this_object());
}

int _query_article()
{
  if (!special_verb())
    return 1;
  return(0); // non-void funktion, Zesstra
}

string _query_presay()
{
  return "";
}

string _query_title()
{
  return "";
}

void Defend(int dam,mixed dam_type,mixed spell,object enemy)
{
  object o;

  if (!query_once_interactive(previous_object()))
    player->Defend(dam, dam_type, spell, enemy);
  else
    previous_object()->StopHuntFor(player);
  if ((o=present("\ntarnhelm",player)))
        o->DoUnwear();
  if (this_object()) destruct(this_object());
}

int Kill(object ob)
{
  object o;

  if ((o=present("\ntarnhelm",player)))
      o->DoUnwear();
  if (this_object()) destruct(this_object());
  return(0); // non-void funktion, Zesstra
}

int InsertEnemy(object ob)
{
  object o;

  if (!query_once_interactive(ob))
    player->InsertEnemy(ob);
  else
    ob->StopHuntFor(player);
  if ((o=present("\ntarnhelm",player)))
      o->DoUnwear();
  if (this_object()) destruct(this_object());
  return 0;
}

string short()
{
  if (old_explode(object_name(previous_object()),"#")[0]=="/obj/werliste")
    return capitalize(geteuid(player)+" verkleidet als "+player->short());
  return player->short();
}

string QueryDisguise()
{
  return desc; 
}
