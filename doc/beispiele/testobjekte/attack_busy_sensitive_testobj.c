inherit "/std/thing";
#include <properties.h>
#include <new_skills.h>
#include <sensitive.h>

private action_puste(string str);
private int counter;

void create() {
  if (!clonep(this_object())) return;
  ::create();
  counter = 2+random(4);
  
  SetProp(P_NAME, "Pusteblume");
  SetProp(P_SHORT, "Eine kleine Pustblume");
  SetProp(P_LONG, break_string(
    "Eine abgebluehte Pflanze, die jetzt wie ein kleiner, weisser Ball "
	"aussieht. Die fiedrigen Samen fliegen bestimmt prima.", 78));
  AddDetail("samen", "Er sieht sehr fein und leicht aus.\n");
  SetProp(P_GENDER,FEMALE);
  SetProp(P_MATERIAL, MAT_MISC_PLANT);
  SetProp(P_NOBUY, 1);
  SetProp(P_VALUE, random(10));

  SetProp(P_INFO, "Starker Wind taete ihr nicht gut.\n");

  AddId(({"blume", "pusteblume", "loewenzahn"}));
  SetProp(P_COMBATCMDS,(["puste loewenzahn":
                         ([C_HEAL: 5])]));

  SetProp(P_SENSITIVE,({({SENSITIVE_ATTACK, DT_AIR, 20}),
                        ({SENSITIVE_INVENTORY, DT_AIR, 20})}));
 
  AddCmd("puste&@ID", #'action_puste, "Puste wen oder was (an)?");
}

private action_puste(string str) {
  if(environment()!=this_player()) {
    notify_fail("Dazu solltest du "+name(WEN,1)+" haben.\n");
	return 0;
  }
  if (this_player()->QueryProp(P_ATTACK_BUSY)) {
    write("Du hast dafuer momentan einfach nicht mehr die Puste.\n");
    return 1;
  }
  this_player()->SetProp(P_ATTACK_BUSY, 1);

  if(counter<0) {
    write(break_string("Du pustest sinnlos auf "+name(WEN, 2)+".", 78));
	say(break_string(this_player()->Name(WER)+
	  " pustet wie daemlich gegen "+name(WEN, 0)+".", 78));
	return 1;
  } else {
    write(break_string(
      "Du pustest "+name(WEN, 2)+" vorsichtig an, einige Samen "
      "loesen sich und fliegen taumelnd in deinem Atem davon."+
	  (counter<0?" Es bleibt nur noch ein nutzloser Strunk.":""), 78));
    say(break_string(
      this_player()->Name(WER)+" pustet sachte gegen "+name(WEN, 0)+" und "
	  "du schaust verzueckt den davonfliegenden Samen nach.", 78));
  }
	
  object who = this_player()->QueryEnemy();
  
  if(objectp(who)) {
    if(!interactive(this_player())) {
      who->ModifySkillAttribute(SA_SPEED, 80+random(10), 6);
	  this_player()->heal_self(5);
	} else
	  who->ModifySkillAttribute(SA_SPEED, 90+random(10), 4);
  }

  counter--;
  if(counter<0) {
    call_out(#'remove, 10+random(60));
	AddId("strunk");
	SetProp(P_NAME, "Strunk");
	SetProp(P_SHORT, "Der Strunk einer Pusteblume");
	SetProp(P_LONG, "Ein haesslicher, leerer Strunk.\n");
	SetProp(P_GENDER, MALE);
  }
  return 1;
}

private void notify_env_destroy() {
  object ob = environment();
  while(ob && !living(ob)) ob = environment(ob);
  if(objectp(ob))
    tell_object(ob, "Der Wind zerblaest "+name(WEN, 2)+".\n");
  remove(1);
}

varargs void trigger_sensitive_attack() {
  notify_env_destroy();
}

varargs void trigger_sensitive_inv() {
  notify_env_destroy();
}

varargs int remove(int silent) {
  if(!silent && living(environment()))
    tell_object(environment(), "Du wirfst "+name(WEN, 2)+" weg.\n");
  return ::remove(silent);
}