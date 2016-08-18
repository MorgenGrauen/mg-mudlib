/* Humni, 2005-07-01 : habbich mal gebaut */

/* Verwendung: 1. Erben
	       2. Entweder die Einzelfunktionen aufrufen oder einfach
	          BuildUp(int spielerlevel) aufrufen und danach noch
		  bei Bedarf modifizieren.
*/

inherit "/std/npc";

#include <properties.h>
#include <language.h>
#include <new_skills.h>
#include <combat.h>

// In das Kommando-Array kommen die ganzen Befehle, die der NPC kann.
string* attack_kommandos;
string* defend_kommandos;
mapping row_kommandos;


create()
{
	::create();

	attack_kommandos=({});
	defend_kommandos=({});
	row_kommandos=([]);
	SetProp(P_GUILD,"abenteurer");
	seteuid(getuid(this_object()));
}

/* Mal als simpel-Methode eingebaut, vielleicht kann das ja irgend ein NPC
   auch noch so brauchen.*/
AddAttackCommand(string str)
{
	attack_kommandos+=({str});
}

AddDefendCommand(string str)
{
	defend_kommandos+=({str});
}

AddRowCommand(int row, string str)
{
	string * com;
	if (!pointerp(row_kommandos[row]))
	{
		row_kommandos+=([row:({str})]);
	}
	else
	{
		com=row_kommandos[row];
		com+=({str});
		row_kommandos+=([row:com]);
	}
}

/* Sehr simpel gemacht, gebe ich gerne zu.*/
AddPreferedRow(int row)
{
	AddDefendCommand(sprintf("team reihe %d",row));
}

/* BuildSkills: Baut die Skills im NPC auf.

   Idee: Level genauso gross wie Mindeslevel fuer Spell: Ability 1000. Danach pro Level>Mindestlevel +1000, bis zu 10000.

   Kampfspells: Saeurestrahl, Feuerball, Schnell, Ausweichen, Kampfschrei.
   */
BuildSkills(int level)
{
	int val;
	if (level>=11)
	{
		val=((level>21)?10000:(level-10)*1000);
		ModifySkill("pfeil",val);
		AddAttackCommand("pfeil");
		AddAttackCommand("pfeil");
		AddRowCommand(2,"pfeil");
		AddRowCommand(2,"pfeil");
	}
	if (level>=15)
	{
		val=((level>25)?10000:(level-14)*1000);
		ModifySkill("feuerball",val);
		AddAttackCommand("feuerball");
		AddAttackCommand("feuerball");
		AddRowCommand(3,"feuerball");
		AddRowCommand(3,"feuerball");
		AddRowCommand(2,"feuerball");
		AddRowCommand(3,"feuerball");
		AddRowCommand(4,"feuerball");
	}
	if (level>=30)
	{
		val=((level>40)?10000:(level-30)*1000);
		ModifySkill("saeurestrahl",val);
		AddAttackCommand("saeurestrahl");
		AddAttackCommand("saeurestrahl");
		AddRowCommand(4,"saeurestrahl");
		AddRowCommand(4,"saeurestrahl");
		AddRowCommand(4,"saeurestrahl");
		AddRowCommand(4,"saeurestrahl");
		AddRowCommand(3,"saeurestrahl");
	}
	if (level>=17)
	{
		val=((level>27)?10000:(level-16)*1000);
		ModifySkill("schnell",val);
		AddRowCommand(1,"schnell");
		AddRowCommand(1,"schnell");
	}
	if (level>=12)
	{
		val=((level>27)?10000:(level-16)*1000);
		ModifySkill("ausweichen",val);
		AddRowCommand(1,"ausweichen");
		AddRowCommand(1,"ausweichen");
		AddRowCommand(1,"ausweichen");
	}
	if (level>=13)
	{
		val=((level>13)?10000:(level-12)*1000);
		ModifySkill("kampfschrei",val);
		AddAttackCommand("kampfschrei");
		AddAttackCommand("kampfschrei");
	}

	ModifySkill(SK_FIGHT,([SI_SKILLABILITY:((level>50)?10000:level*200)]),150,"ANY");
}

//Nachbau...geklaut aus /std/player/skills.c

StdSkill_Fight(object me, string sname, mapping sinfo)
{
  int val, w;
  object enemy;

  if (!mappingp(sinfo) || !objectp(sinfo[P_WEAPON]))
    return 0;

  w = ([WT_KNIFE : 8,
        WT_SWORD : 5,
        WT_AXE   : 4,
        WT_SPEAR : 6,
        WT_CLUB  : 1,
        WT_WHIP  : 9,
        WT_STAFF : 7])[sinfo[P_WEAPON]->QueryProp(P_WEAPON_TYPE)];


  val = sinfo[SI_SKILLABILITY]*(sinfo[P_WEAPON]->QueryProp(P_WC)*
                                (w*QueryAttribute(A_DEX)+
                                 (10-w)*QueryAttribute(A_STR))/800)
        /MAX_ABILITY;

  sinfo[SI_SKILLDAMAGE]+=val;

  //Naja, gut. Lernen hab ich nicht uebernommen, das tut der NPC nicht.

}

void SpellAttack(object enemy)
{
	int row;
	string* cmds;
	row=PresentPosition();
	cmds=attack_kommandos;
	if (pointerp(row_kommandos[row]))
		cmds+=row_kommandos[row];
	if (sizeof(cmds)>0)
		command(cmds[random(sizeof(cmds))]);
	::SpellAttack(enemy);
}

int Defend(int dam, mixed dam_type, mixed spell, object enemy)
{
	if (sizeof(defend_kommandos)>0)
		command(defend_kommandos[random(sizeof(defend_kommandos))]);
	::Defend(dam,dam_type,spell,enemy);
}

BuildUp(int spielerlevel)
{
	// Sehr simpel, zugegeben.
	create_default_npc(spielerlevel);
	BuildSkills(spielerlevel);
	AddPreferedRow(spielerlevel>30?4:(spielerlevel>15?3:(spielerlevel>11?2:1)));
}
