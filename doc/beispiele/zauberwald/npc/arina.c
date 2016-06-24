// (c) by Padreic (Padreic@mg.mud.de)

#include "../files.h"
#include <combat.h>
#include <new_skills.h>
#include "/p/zauberer/zauberer.h"

inherit NPC("stdnpc");

void create()
{
   ::create();
   SetProp(P_SHORT, "Arina die Waldfee");
   SetProp(P_LONG,
      "Vor Dir steht Arina die weise Waldfee. Sie ist die Lehrerin des Waldes und\n"
     +"unterrichtet die Pixies. In ihrer Hand haelt sie einen kleinen Zeigestock mit\n"
     +"dem sie alles moegliche erklaert und Buchstaben in den Sand zeichnet. Auch\n"
     +"Arina ist, genauso wie ihre Schwesten, bildhuebsch, doch ihr gestrenger Blick\n"
     +"floesst jedem in ihrer Naehe grossen Respekt ein.\n");
   SetProp(P_NAME, "Arina");
   SetProp(P_ARTICLE, 0);
   SetProp(P_GENDER, FEMALE);
   SetProp(P_LEVEL, 80);
   SetProp(P_ATTRIBUTES, (["int":80,"con":40,"str":16,"dex":50]) );
   SetProp(P_DISABLE_ATTACK, -10000); // is nich :)
   SetProp(P_BODY, 80);
   SetProp(P_MAX_SP, 2000);
   SetProp(P_SP, 2000);
   SetProp(P_MAX_HP, 700);
   SetProp(P_HP, 700);
   SetProp(P_ALIGN, 400);
   SetProp(P_RACE, "Fee");
   SetProp(P_SIZE, 160+random(16));
   SetProp(P_ALCOHOL, 0);
   SetProp(P_CORPSE, OBJ("feenleiche"));
   SetProp(P_MAX_ALCOHOL, 0); // kein rauschen :)
   AddAdjective("wunderschoen");
   AddId(({"arina", "fee", "waldfee", WALDID("fee")}));
   SetProp(P_HANDS, ({" mit ihren zarten Haenden", 150, DT_BLUDGEON}) );
   SetProp(P_SKILL_ATTRIBUTE_OFFSETS, ([SA_SPELL_PENETRATION: 200]));
   SetProp(P_XP, 800*200*5*2);
   SetProp(P_GUILD, "zauberer");
   SetProp(P_Z_NO_MATERIAL, 1);
   SetProp(P_DEFAULT_INFO, "schweigt und laechelt Dich einfach nur freundlich an.\n");
   ModifySkill("verletze",
      ([SI_SKILLABILITY:9000, SI_SPELLCOST:0,
        SI_SPELLFATIGUE:0, SI_NO_CONSEQUENCES:10000,
        SI_SKILLRESTR_USE:([P_GUILD_LEVEL:0,SR_FREE_HANDS:0]),
        SI_ARMOUR_RESTR:0]),0,"zauberer");
   SetProp(P_GUILD, 0);
   SetProp(P_RESISTANCE_STRENGTHS,
      ([DT_POISON: 0.15,
        DT_MAGIC: -0.3 ]) );
   AddItem(OBJ("stock"), CLONE_WIELD);
   AddItem(OBJ("kleid"), CLONE_WEAR);
   if (file_size(SAECKCHEN)>0) AddItem(SAECKCHEN, CLONE_NEW);
}

void Attack(object enemy)
{
   int normal_speed;
   normal_speed=(enemy->QueryProp(P_SKILL_ATTRIBUTE_OFFSETS)||([]))[SA_SPEED]||100;
   if (enemy && enemy->QuerySkillAttribute(SA_SPEED) > normal_speed) {
      write("Die Fee macht eine beruhigende Handbewegung und ploetzlich fuehlst Du Dich\n"
           +"ruhig und Du merkst wie Du allmaehlich wieder langsamer wirst...\n");
      enemy->ModifySkillAttribute(SA_SPEED, -20, 180);
   }
   if (enemy) ::Attack(enemy);
   SetProp(P_GUILD, "zauberer");
   command("verletze mit "+({"feuer", "magie", "eis", "wasser", "wind"})[random(5)] );
   SetProp(P_GUILD, 0);
}

int Defend(int dam, mixed dts, mixed spell, object enemy)
{
   int normal_speed;
   normal_speed=(enemy->QueryProp(P_SKILL_ATTRIBUTE_OFFSETS)||([]))[SA_SPEED]||100;
   if (enemy && enemy->QuerySkillAttribute(SA_SPEED) > normal_speed) {
      write(Name(WER)+" macht eine beruhigende Handbewegung und ploetzlich fuehlst Du Dich\n"
           +"ruhig und Du merkst wie Du allmaehlich wieder langsamer wirst...\n");
      enemy->ModifySkillAttribute(SA_SPEED, -20, 180);
   }

   if ((!spell || (mappingp(spell) && spell[SP_PHYSICAL_ATTACK])) && !random(3)) {
      tell_room(environment(), Name(WER)+" weicht schnell einen Schritt zurueck und weicht Deinem Angriff aus.\n");
      dam=0;
   }
   return (int)::Defend(dam, dts, spell, enemy);
}

void NotifyPlayerDeath(object who, object killer, object lost_exp)
{
  if (!who || killer!=ME) return; // uninteressant
  log_file("padreic/kill", ctime(time())+" "+capitalize(getuid(who))+" getoetet von /zauberwald/arina\n");
}

void die()
{
  log_file("padreic/kill", ctime(time())+" Arina wurde von "+get_killer()+" getoetet.\n");
  ::die();
}
