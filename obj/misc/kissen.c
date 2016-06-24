inherit "std/thing";
#include <properties.h>
#include <language.h>

string cstr;  

init()
{
  int colour;
  ::init();
  colour = random(8);
  if (colour==0) cstr = "gruenes";
  if (colour==1) cstr = "rotes";
  if (colour==2) cstr = "blaues";
  if (colour==3) cstr = "gelbes";
  if (colour==4) cstr = "schwarz-weiss-kariertes";
  if (colour==5) cstr = "orange-pink-gestreiftes";
  if (colour==6) cstr = "lila-gruen-getuepfeltes";
  if (colour==7) cstr = "silbergraues";
  add_action("throw_it","wirf"); 
  add_action("ww", "ww");
}

short()
{
  return "Ein " + cstr + " Kissen\n";
}

long()
{
  return "Dies ist ein wunderschoenes, " + cstr + " Kissen.\n"+
          "Du kannst versuchen es zu werfen.\n";
}

create()
{
  ::create();
  cstr = "von Boing handgenaehtes";
  AddId("kissen");
  SetProp(P_WEIGHT, 1);
  SetProp(P_VALUE, 1);
  SetProp(P_NAME, "Kissen");
  SetProp(P_GENDER, 0);
}

ww(str)
{
  if (str)
    return throw_it("kissen nach "+str);
  else
  {
    object *all, *liv;
    int i;

    all=all_inventory(environment(this_player()));
    for (i=0; i<sizeof(all); i++)
      if (query_once_interactive(all[i]))
        if (!liv)
	  liv = ({all[i]});
        else
          liv = liv+({all[i]});
    i = random(sizeof(liv));
    return throw_it("kissen nach "+lower_case(liv[i]->query_real_name()));
  }
}

throw_it(str)
{
  int treffer;
  string werfer_name;
  string victim_string,werfer_string;
  string was,at,wen;
  int fehler;
  object victim;

  if (!str) return 0;
  fehler = 0;
  if (sscanf(str,"%s %s %s",was,at,wen) != 3)
    fehler = 1;
  if (!id(was) || at != "nach")
    fehler = 1;
  if (fehler)
  { 
     notify_fail("Zum Werfen: wirf kissen nach <spielername>\n");
     return 0;
  }
  victim = present(wen,environment(this_player()));
  if (!victim)
  { 
     write("Dieser Spieler ist nicht hier!\n");                                
     return 1;                                                                 
  }                                                                           
  werfer_name = this_player()->name();
  treffer = random(5);
  write("Du schmeisst ein Kissen nach " + victim->name(WEM) +".\n");
  say(werfer_name + " wirft ein Kissen nach " + victim->name(WEM) + ".\n");
  if (treffer==0)
  {
    werfer_string = "Du verfehlst " + victim->name(WEN) + " meilenweit.\n";
    victim_string = werfer_name + " verfehlt Dich meilenweit.\n";
    this_object()->move(environment(this_player()));
  }
  if (treffer==1)
  {
    werfer_string = "Das Kissen streift " + victim->name(WEN)+ " am Ohrlaeppchen.\n";
    victim_string = "Das Kissen streift Dich am Ohrlaeppchen.\n";
  }
  if (treffer==2)
  {
    werfer_string = "PAFF! Du triffst " + victim->name(WEN) + " mitten ins Gesicht.\n";
    victim_string = "PAFF! Das Kissen trifft Dich mitten im Gesicht!\n";
  }
  if (treffer==3)
  {
    werfer_string = "WUSCH! Das Kissen trifft " + victim->name(WESSEN) + " Bauch mit voller Wucht.\n";
    victim_string = "WUSCH! Das Kissen fliegt mit voller Wucht gegen Deinen Bauch.\n";
  }
  if (treffer==4)
  {
    werfer_string =  victim->name() + " versucht wegzurennen, doch das Kissen trifft den Hinterkopf.\n";
    victim_string = "Du versuchst wegzurennen, aber das Kissen trifft Dich am Hinterkopf.\n";
  }
  write(werfer_string);
  tell_object(victim,victim_string);
  if (treffer) this_object()->move(victim);
  return 1;
}

