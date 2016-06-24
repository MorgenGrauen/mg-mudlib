#pragma strong_types,save_types

#include <properties.h>

object caster;
string*ids,name;
int gender,article,plural;
string lgdesc;
string min,mout,mmin,mmout;
mixed hands;

void Initialize(object _caster,
                string*_ids,
                string _name,
                int _gender,
                int _article,
                int _plural)
{ if(!objectp(_caster)    ||
     !interactive(_caster)||
     !stringp(_name)      ||
     _name==""            ||
     _gender<0            ||
     _gender>2)
  { destruct(this_object());
    return;
  }
  caster=_caster;
  ids=_ids;
  name=_name;
  gender=_gender;
  article=_article;
  plural=_plural;
  lgdesc=0;
  shadow(caster,1);
}

void SetLongDesc(string txt)
{ if(!stringp(txt)||txt=="")
    return;
  lgdesc=txt;
}

string* _query_ids()
{ return caster->Query(P_IDS)+ids;
}

// nicht alle Verben mit veraendertem Aussehen
int special_verb()
{ string verb;
  verb=query_verb();
  if(!stringp(verb)||verb=="")
    return 0;
  if(verb[0]=='-')
    return 1;
  if(member(({"ruf","rufe","mruf","mrufe",
              "teil","teile","erzaehl","erzaehle"}),verb)!=-1)
    return 1;
  return 0;
}

string _query_name()
{ if(!special_verb())
    return name;
  return capitalize(caster->Query(P_NAME));
}

string _query_short()
{ if(!special_verb())
    return caster->Name();
  return caster->Query(P_SHORT);
}

string _query_long()
{ if(!special_verb())
  { string str;
    if(lgdesc)
      return lgdesc;
    str=caster->name();
    return break_string(capitalize(str)+" ist "+str+" ist "+str+".",78);
  }
  return caster->Query(P_LONG);
}

int _query_gender()
{ if(!special_verb())
    return gender;
  return caster->Query(P_GENDER);
}

int _query_article()
{ if(!special_verb())
    return article;
  return caster->Query(P_ARTICLE);
}

int _query_plural()
{ if(!special_verb())
    return plural;
  return caster->Query(P_PLURAL);
}

string _query_race()
{ if(!special_verb())
    return name;
  return caster->Query(P_RACE);
}

varargs int remove(int silent) {
  unshadow();
  destruct(this_object());
  return 1;
}

void stop_shadow()
{ 
  remove();
}

string _query_presay()
{ return"";
}

string _query_title()
{ return"";
}

string _set_msgin(string val)
{ return min=val;
}

string _query_msgin()
{ return min;
}

string _set_msgout(string val)
{ return mout=val;
}

string _query_msgout()
{ return mout;
}

string _set_mmsgin(string val)
{ return mmin=val;
}

string _query_mmsgin()
{ return mmin;
}

string _set_mmsgout(string val)
{ return mmout=val;
}

string _query_mmsgout()
{ return mmout;
}

mixed _set_hands(mixed val)
{ return hands=val;
}

mixed _query_hands()
{ return hands;
}

varargs int Defend(int dam,mixed dam_type,mixed spell,object enemy)
{ object ob;
  if(!enemy ||  // Silvana 26.1.2002
     (!query_once_interactive(previous_object())&&
      !query_once_interactive(enemy)))
    return caster->Defend(dam,dam_type,spell,enemy);
  else
  { enemy->StopHuntFor(caster);
    caster->StopHuntFor(enemy);
  }
  if(objectp(ob=present("zauberer\nshadow",caster)))
    ob->remove();
  if(this_object())
    remove();
  return 0;
}

int Kill(object enemy)
{ object ob;
  if(!query_once_interactive(enemy))
    return caster->Kill(enemy);
  if(objectp(ob=present("zauberer\nshadow",caster)))
    ob->remove();
  if(this_object())
    remove();
  return 0;
}

int InsertEnemy(object enemy)
{ object ob;
  if(!query_once_interactive(enemy))
    return caster->InsertEnemy(enemy);
  else {
    enemy->StopHuntFor(caster);
    caster->StopHuntFor(enemy);
  }
  if(objectp(ob=present("zauberer\nshadow",caster)))
    ob->remove();
  if(this_object())
    remove();
  return 0;
}

string short()
{ if(load_name(previous_object()) == "/obj/werliste")
    return capitalize(geteuid(caster))+" verkleidet als "+caster->short();
  return caster->short();
}

varargs string long()
{ if(lgdesc)
    return lgdesc;
  return caster->long();
}
