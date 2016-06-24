inherit "std/npc";
#include <properties.h>
#include <language.h>

int warte;
 
create()
{
  if (!clonep(this_object())) return;
  ::create();
  SetProp(P_NAME, "Testmonster");
  SetProp(P_GENDER, NEUTER);
  SetProp(P_SHORT, "Das Testmonster - nervtoetend -");
  SetProp(P_ALIGN, 100);
  SetProp(P_LONG, 
  "Dies ist Testie, das Testmonster. Es hatte alle moeglichen, uebel aussehenden\n"+
  "Testgeraete dabei. Leg Dich lieber nicht mit ihm an ...\n");
  AddId("monster");
  AddId("testmonster");
  AddId("testie");
  SetProp(P_RACE, "tester");
  SetProp(P_LEVEL, 19);
  SetProp(P_MAX_HP, 275);
  SetProp(P_HP, 275);
  SetProp(P_XP, 0);
  SetProp(P_HANDS, ({" mit einem Spannungspruefer", 5}) );
  SetProp(P_AC, 1);
  seteuid(getuid(this_object()));
  AddSpell(1,5,"Ein Testmonster schlaegt Dir ein Testheft um die Ohren.\n",
               "Ein Testmonster schlaegt @WEN mit einem Testheft.\n");
  AddSpell(1,8,"Ein Testmonster testet Deine Reaktion auf Hitze.\n",
               "Ein Testmonster haelt ein Feuerzeug unter @WESSEN Hintern.\n");
  AddSpell(1,11,"Ein Testmonster testet Deine Reaktion auf elektrischen Strom.\n",
                "Ein Testmonster verpasst @WEM einen Elektroschock.\n");
  AddSpell(1,15,"Ein Testmonster testet Deine Saeurebestaendigkeit.\n",
                "Ein Testmonster ueberschuettet @WEN mit konzentrierter Saeure.\n");
  AddSpell(1,10,"Ein Testmonster nimmt eine Blutprobe von Dir.\n",
                "Ein Testmonster sticht @WEN mit einer gigantischen Nadel.\n");
  SetProp(P_SPELLRATE,50);
  warte=10;
}

give_notify(obj)
{
  write("Danke fuer "+obj->name(WEN, 1)+".\n");
  return 1;
}

catch_tell(str)
{
  string s1, s2;

  if (!str || str=="") return;
  if (sscanf(str,"%s tritt%sTestmonster",s1,s2))
  {
    say_str(capitalize(name())+" sagt: Aua, wieso trittst Du mich, "+s1+"?\n");
    return;
  }
  if (sscanf(str,"%s sagt: %s",s1,s2))
  {
    if (!s2) s2=" ";
    s2=old_explode(s2,"\n");
    if (sizeof(s2))
      s2=implode(s2," ");
    else
      return;
    say_str(capitalize(name())+" sagt: Wieso sagst Du \""+s2+"\", "+s1+"? \n");
    return;
  }
}

say_str(str)
{
  call_out("do_say",0,str);
}

do_say(str)
{
  say(str);
}

heart_beat()
{
  ::heart_beat();
  warte--;
  if (warte) return;
  warte=8;
  Flee();
}
