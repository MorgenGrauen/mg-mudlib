#include <properties.h>
#include <language.h>
#include <seil.h>
inherit "/std/thing";

#pragma strict_types,rtt_checks

object tied_to_ob;
string tied_name;

void create(){
  ::create();

  AddId( ({"seil", "\nseil"}) );
  SetProp(P_NAME, "Seil");

  SetProp(P_GENDER, NEUTER);
  SetProp(P_LONG,break_string("Du siehst ein ganz normales Seil. Du kannst es an "+
                  "Gegenstaenden festbinden und wieder loesen. Das Seil besteht "+
                  "aus solider fester Steinbeisserwolle.",78));
  AddDetail(({"wolle","steinbeisserwolle"}),"Diese Wolle ist eine besonders feste und stabile Wolle.\n");
  SetProp(P_VALUE, 15);
  SetProp(P_TIE_AUTO,1);
  SetProp(P_WEIGHT, 300);

  SetProp(P_MATERIAL,({MAT_MISC_STONE,MAT_WOOL}));
  AddCmd( ({"binde","bind","knote","befestige","mach","mache"}), "tie" );

  // der Befehle loesen wird auf die anderen Befehle umgebogen und dient nur
  // der besseren benutzbarkeit des seiles.
  AddCmd( ({"loese" }), "loesen" );

}

// dieses Ding wird u.a. geerbt. Damit die BP beim Erben auch initialisiert
// wird (fuer die Laeden), muss create_super() das create() rufen.
protected void create_super() {
  create();
}

string _query_short()
{
 string artikel;
 switch( QueryProp(P_GENDER) )
 {
  case NEUTER : artikel="das";break;
  case MALE   : artikel="der";break;
  case FEMALE :	artikel="die";break;
  default     : artikel="das";
 }

 if (tied_to_ob)
  return this_object()->Name(WER)+", "+artikel+" an "
     + (tied_name?tied_name:(string)(tied_to_ob->name(WEM)))
     + " festgebunden ist";
 return (string)this_object()->Name(WER);

}

mixed _query_noget()
{
 if (tied_to_ob)
  return "Das geht nicht, solange "+name(WER)+" festgebunden ist.\n";
 return 0;
}

int tie(string str)
{
  string t1, t2;
  object ob;
  string verb;
  mapping tied_map;

  _notify_fail("Binde was?\n");

  // zunaechst den User notieren, damit spaeter immer drauf 
  // zugegriffen werden kann.
  // Wegen kompatibilitaet wird das nicht direkt an tie()/untie()
  // uebergeben.
  SetProp(P_TIE_USER,this_player());

  if (!str||!stringp(str)) return 0;

  verb = query_verb();
  
  //automatischer aus objecten/raeumen generierter aufruf
  if(QueryProp(P_TIE_VERB)) verb=QueryProp(P_TIE_VERB); 
  	
  if (sscanf(str, "%s an %s", t1, t2) != 2 && 
      sscanf(str,"%s los",t1) != 1 &&
      sscanf(str,"%s fest",t1) != 1 )
   return 0;

  if(id(t1) && str==(t1+" fest")) str="seil an raum";

  if(sscanf(str,"%s an %s",t1,t2)==2)
  {
   if(tied_to_ob)
   {
    write("Es ist bereits irgendwo festgebunden.\n");
    return 1;
   }

   if (!id(t1)) return 0;

   if (t2 == "mich")
   {
    _notify_fail("Warum willst Du das denn machen?\n");
    return 0;
   }

   ob = present(t2, this_player());		

   if (!ob) ob = present(t2, environment(this_player()));

   if (!ob)
   {
    if (call_other(environment(this_player()), "id", t2))
    ob = environment(this_player());
   }
		
   if(!ob && str == "seil an raum")
   {
    ob=environment(this_player());
   }

   _notify_fail( "Soetwas findest Du hier nicht.\n" );
   if (!ob) return 0;

   if (!(tied_name=(string)call_other(ob, "tie", t2)))
   {
    if(ob->QueryProp(P_INVIS)) return 0;
    
    if (ob != environment(this_player()))
    {
     _notify_fail("Du kannst "+name(WER)+" nicht an "+ob->name(WEM)+
                 " festbinden.\n");
     return 0;
    }
    else
    {
     _notify_fail("Du kannst "+name(WER)+" nicht daran festbinden.\n");
     return 0;
    }

   }
		
  /* Is he carrying the rope ? */
  if (environment() == this_player()) 
  {
   this_object()->move(environment(this_player()));
  }

  tied_to_ob = ob;

  if (tied_to_ob != environment(this_player()) || 
      environment(this_player())->name()!=0 )
   {
    tied_name = (string)(tied_to_ob->name(WEM));
   }
  else
   {
    if( !stringp(tied_name) ) tied_name="etwas";
   }

  if(QueryProp(P_TIE_VERB))
  {
    // das seil wird gesteuert
    tell_room(environment(),QueryProp(P_TIE_MESSAGE));

    // seil wieder in roh-zustand versetzen.
    SetProp(P_UNTIE_MESSAGE,0);
    SetProp(P_TIE_VERB,0);
  }
  else
  {
   write("Du bindest "+name(WER)+" an " + tied_name + " fest.\n");
   say(this_player()->name(WER) + " bindet "+name(WER)+" an "
   + tied_name + " fest.\n");
  }

  // den object mitteilen, an wen es gebunden ist.
  if(tied_to_ob->QueryProp(P_TIED)==0) tied_to_ob->SetProp(P_TIED,([]) );
  tied_to_ob->SetProp(P_TIED,tied_to_ob->QueryProp(P_TIED)+([this_object(): 
     ([
       "player":this_player(),
       "time"  :time()
     ]) ]));

  return 1;
 }

 if( (member( ({
         "binde",
         "bind",
         "knote",
         "mach",
         "mache",
         "loese" 
         }),verb                 
    )!=-1) && 
    sscanf(str,"%s los",t1)==1)
  {
   if (!tied_to_ob)
   {
    write(Name(WER)+" ist nirgendwo festgebunden.\n");
    return 1;
   }

   if (!call_other(tied_to_ob, "untie"))
   {
    write("Das klappt leider nicht.\n");
    return 1;
   }

   if(QueryProp(P_TIE_VERB))
   {
    // das seil wird gesteuert
    tell_room(environment(),QueryProp(P_UNTIE_MESSAGE));

    // wieder in roh-zustand versetzen.
    SetProp(P_UNTIE_MESSAGE,0);
    SetProp(P_TIE_VERB,0);
   }
   else
   {
    write("Du bindest "+name(WER)+" los.\n");
    say(this_player()->name()+" bindet "+name(WER)+" los.\n");
   }

  tied_map=([])+tied_to_ob->QueryProp(P_TIED);
  tied_map=m_copy_delete(tied_map,this_object());

  tied_to_ob->SetProp(P_TIED,tied_map);

  tied_to_ob = (object) 0;
 
  return 1;
 }
 return 0;
}

int loesen(string str)
{
 if(str == "seil"  ||
    str == lower_case(QueryProp(P_NAME)) ||
    id(str)
 )
 {
  return tie("seil los");
 }
 _notify_fail("Was moechtest Du loesen?\n");
 return 0;
}


object query_tied_to_ob()
{
  return tied_to_ob;
}

varargs int binde_seil(string ziel,string msg)
{
 if(!QueryProp(P_TIE_AUTO)) return 0;

 // diese funktion bindet ein Seil und kann von einem object aus 
 // aufgerufen werden.
 SetProp(P_TIE_VERB,"binde");

 if(!msg) msg = Name(WER)+" wird auf magische Art und Weise festgebunden.\n";

 SetProp(P_TIE_MESSAGE,msg);
 return tie("seil an "+ziel); 
}

varargs int loese_seil(string msg)
{
 if(!QueryProp(P_TIE_AUTO)) return 0;

 if(!msg) msg = Name(WER)+" loest sich.\n";
 
 SetProp(P_TIE_VERB,"binde");
 SetProp(P_UNTIE_MESSAGE,msg);
 return tie("seil los");
}

