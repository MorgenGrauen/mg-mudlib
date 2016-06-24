// (c) by Padreic (Padreic@mg.mud.de)

#include "../files.h"
#include <combat.h>
#include <moving.h>
#include <new_skills.h>
#include "/p/zauberer/zauberer.h"

inherit NPC("stdnpc");

string info_tarnzauber()
{
   object ob;
   if (present("\ntarnzauber", PL))
      return "sagt: Ich habe Dich doch bereist verzaubert.\n";
   ob=clone_object(OBJ("tarnzauber"));
   ob->move(PL, M_NOCHECK);
   ob->Initialize(PL);
   return "sagt: Nun gut, so sei es...\n"
         +"macht eine weite Handbewegung und Du fuehlst wie Du Dich verwandelst.\n"
         +"sagt: Viel Glueck!";
}

void create()
{
   ::create();
   SetProp(P_SHORT, "Ulinia die Waldfee");
   SetProp(P_LONG,
      BS("Ueber dem Teich schwebt Ulinia die Waldfee. Ihr langes goldenes Haar weht "
     +"leicht im Wind und das einzige was sie bedeckt, ist ein leichtes Blaetterkleid "
     +"das ihre weiblichen Rundungen ausserordentlich gut zur Geltung bringt. "
     +"In der Hand haelt sie einen kleinen Zauberstab, mit dem sie hoch ueber allem thront."));
   SetProp(P_NAME, "Ulinia");
   SetProp(P_ARTICLE, 0);
   SetProp(P_GENDER, FEMALE);
   SetProp(P_LEVEL, 80);
   SetProp(P_ATTRIBUTES, (["int":80,"con":40,"str":16,"dex":50]) );
   SetProp(P_DISABLE_ATTACK, -10000); // is nich :)
   SetProp(P_BODY, 80);
   SetProp(P_MAX_SP, 2000);
   SetProp(P_SP, 2000);
   SetProp(P_MAX_HP, 800);
   SetProp(P_HP, 800);
   SetProp(P_ALIGN, 400);
   SetProp(P_RACE, "Fee");
   SetProp(P_SIZE, 160+random(16));
   SetProp(P_ALCOHOL, 0);
   SetProp(P_CORPSE, OBJ("feenleiche"));
   SetProp(P_MAX_ALCOHOL, 0); // kein rauschen :)
   AddAdjective("wunderschoen");
   AddId(({"fee", "waldfee", "ulinia", WALDID("fee")}));
   SetProp(P_HANDS, ({" mit ihren zarten Haenden", 150, DT_BLUDGEON}) );
   SetProp(P_SKILL_ATTRIBUTE_OFFSETS, ([SA_SPELL_PENETRATION: 250]));
   SetProp(P_XP, 800*200*5*2);
   SetProp(P_GUILD, "zauberer");
   SetProp(P_Z_NO_MATERIAL, 1);
   SetProp(P_DEFAULT_INFO, "schweigt und laechelt Dich einfach nur freundlich an.\n");
   AddInfo(({"quest", "hilfe", "aufgabe", "dunkelelfenamulett", "delfenamulett"}),
     "Ohh in der Tat haette ich da eine Aufgabe fuer Dich, aber ich muss Dich "
    +"warnen, das sie nicht ganz ungefaehrlich ist.\n"
    +"Ein befreundeter Kobold berichtete mir von einem lange verschollenen Buch, "
    +"dass in der Bibliothek der Dunkelelfen wieder aufgetaucht ist. In diesem "
    +"Buch ist unter anderem von einem alten Amulett die Rede, mit dessen Hilfe "
    +"die Dunkelelfen ihre alte Macht wiedererlangen und erneut Furcht und "
    +"Schrecken ueber das gesamte Morgengrauen bringen koennten. "
    +"Wenn Du die Gefahr nicht scheust, waere es ausserordentlich nett wenn Du "
    +"Dich nach Zzerenzzen begeben wuerdest und dort nach dem Amulett suchst.\n"
    +"Solltest Du das Amulett tatsaechlich finden, so ist es wichtig das es "
    +"auf keinen Fall in die Hand der Dunkelelfen geraet und zerstoert wird.",
    "antwortet: ");
   AddInfo("gefahr",
     "Nun Du wirst Dich mitten unter die Dunkelelfen begeben muessen um "
    +"zu ihrer Bibliothek gelangen zu koennen. In Zzerenzzen wirst Du "
    +"hierbei voellig auf Dich allein gestellt sein.\n"
    +"Alles was ich hierbei fuer Dich tun kann, ist einen Tarnzauber zu "
    +"sprechen der Dich aeusserlich in einen Dunkelelfen verwandelt. "
    +"Aber vorsicht, spaetestens wenn Du Dich in einen Kampf verwickelst, "
    +"wird sich der Zauber loesen.", "sagt: ");
   AddInfo("tarnzauber", #'info_tarnzauber, "");
   AddInfo(({"furcht", "schrecken", "macht"}),
     "Ja frueher waren die Dunkelelfen sehr sehr maechtig und alle "
    +"friedliebenden Geschoepfe dieser Welt sollten hoffen das sie "
    +"nie wieder ihre alte Macht erlangen werden.", "antwortet: ");  
   AddInfo("amulett",
     "Nun leider weiss ich auch nichts genaues ueber das Amulett. Ich bin "
    +"nicht mal sicher ob es existiert oder je existiert hat. Aber sollte "
    +"es noch existieren, muss es auf jeden Fall zerstoert werden!", "sagt: ");
   AddInfo("kobold",
     "Ich habe versprochen ihn nicht zu verraten und so kann ich Dir nichts "
    +"weiter ueber ihn sagen.", "antwortet: ");
   AddInfo(({"buch", "bibliothek"}),
     "Ich denke am besten begibst Du Dich in die Hoehle des Loewen und versuchst "
    +"in der Bibliothek der Dunkelelfen selbst ein Blick in dies Buch zu werfen.\n"
    +"Wenn ich das noch recht in Erinnerung habe, handelt es sich hierbei um "
    +"das Buch eines Dunkelelfen names Teyrion.", "sagt: ");
   AddInfo("loewen", "Das ist doch bloss eine Redewendung.", "sagt: ");
   AddInfo("redewendung", "Na das sagt man halt so...", "sagt: ");
   AddInfo(({"hoehle", "zzerenzzen"}),
     "Der Eingang nach Zzerenzzen liegt in der naehe von Wilhelmsburg gut hinter "
    +"einem Wasserfall versteckt.", "sagt: ");
   ModifySkill("verletze",
      ([SI_SKILLABILITY:10000, SI_SPELLCOST:0,
        SI_SPELLFATIGUE:0, SI_NO_CONSEQUENCES:10000,
        SI_SKILLRESTR_USE:([P_GUILD_LEVEL:0,SR_FREE_HANDS:0]),
        SI_ARMOUR_RESTR:0]),0,"zauberer");
   SetProp(P_GUILD, 0);
   SetProp(P_RESISTANCE_STRENGTHS,
      ([DT_UNHOLY: 0.25,
        DT_POISON: 0.15, 
        DT_MAGIC: -0.3]) );

   AddItem(OBJ("zauberstab"), CLONE_WIELD);
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
   command("verletze mit "+({"feuer", "magie", "eis", "wasser", "gift",
                             "wind", "saeure", "laerm"})[random(8)] );
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
  log_file("padreic/kill", ctime(time())+" "+capitalize(getuid(who))+" getoetet von /zauberwald/ulinia\n");
}

void die()
{
  log_file("padreic/kill", ctime(time())+" Ulinia wurde von "+get_killer()+" getoetet.\n");
  ::die();
}
