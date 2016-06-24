// (c) by Padreic (Padreic@mg.mud.de)

#include <properties.h>
#include <attributes.h>
#include <combat.h>
#include <guard.h>
#include <moving.h>
#include <new_skills.h>

// Ein NPC wird nicht von /std/thing direkt geerbt, sondern von /std/npc.
// Da /std/npc aber seinerseits wieder von /std/thing erbt, sind im NPC auch
// saemtliche Funktionen definiert, die bereits in /std/thing enthalten sind.
inherit "/std/npc";

void create()
{
   ::create();
   // create_default_npc() setzt schon mal einige wichtige Properties wie
   // P_ATTRIBUTES automatisch...
   create_default_npc(20, 300);

   // Keine Angst: Es muessen keineswegs immer alle diese Properties benutzt
   // werden, auch wenn ich sie jetzt einmal alle angebe, um eine Uebersicht
   // zu geben, was man alles ueber Properties steuern kann...
   SetProp(P_SHORT, "Ein fieser Ork");
   SetProp(P_LONG, "Der Ork schaut Dich grimmig an.\n");
   SetProp(P_NAME, "Ork");
   SetProp(P_NAME_ADJ, "fies");
   SetProp(P_GENDER, MALE);
   SetProp(P_MAGIC, 0);
   SetProp(P_MATERIAL, MAT_MISC_LIVING);

   // nun komen einige wirklich neue Properties
   SetProp(P_INFO, "Der Ork ist ein ganz gemeiner... :)\n");
   SetProp(P_ALIGN, -500); // Alignment des Gegners
   SetProp(P_RACE, "Ork"); // Rasse des NPCs
   SetProp(P_AGGRESSIVE, 1); // greift aggressiv jeden an
   SetProp(P_ATTRIBUTES, ([A_INT:5, A_DEX:10, A_STR:25, A_CON:10]));
   SetProp(P_SIZE, 134); // Groesse in cm
   SetProp(P_MAX_HP, 300); // max. Lebenspunkte
   SetProp(P_HP, 300); // Lebenspunkte
   SetProp(P_MAX_SP, 200); // max. Magiepunkte
   SetProp(P_SP, 200); // Magiepunkte
   SetProp(P_BODY, 80); // Grundruestungsschutz des Koerpers
   SetProp(P_MAX_HANDS, 2); // kaempft mit zwei Haenden...
   SetProp(P_HANDS, ({" mit blossen Haenden", 150}));
   SetProp(P_NOMAGIC, 20); // 20% Resistenz gegen Spells
   SetProp(P_GUARD, 20); // Der NPC laesst sich ggf. weglocken
   SetProp(P_NO_GLOBAL_ATTACK, 0); // wird von 'toete alles' erfasst
   SetProp(P_NO_ATTACK, 0);  // kann man den NPC ueberhaupt angreifen?
   SetProp(P_FRIEND, 0); // soll der NPC in Spells als Freund erfasst
                         // werden?
   SetProp(P_RESISTANCE, DT_FIRE);  // Resistenzen
   SetProp(P_VULNERABILITY, DT_HOLY);  // Empfindlichkeiten
   SetProp(P_FOOD, 0);
   SetProp(P_MAX_FOOD, 100);
   SetProp(P_DRINK, 0);
   SetProp(P_MAX_DRINK, 100);
   SetProp(P_ALCOHOL, 0);
   SetProp(P_MAX_ALCOHOL, 100);
   SetProp(P_XP, QueryProp(P_MAX_HP)*QueryProp(P_HANDS)[0]*5); // Erfahrung
   SetProp(P_MURDER_MSG, "Ich komme doch eh wieder!\n"); // Moerdermeldung
   SetProp(P_KILL_NAME, "Ein fieser Testork"); // noetig falls != P_NAME
   SetProp(P_KILL_MSG, "Das kommt davon wenn man sich mit mir anlegt.\n");
   SetProp(P_DIE_MSG, "Der Ork schreit ein letztes mal laut auf.\n");
   SetProp(P_NOCORPSE, 0); // soll der NPC eine Leiche hinterlassen?
   SetProp(P_HEAL, -10); // wieviel LP bekommt man beim Leichen essen...

   // der Ork ist Mitglied der Abenteurergilde...
   SetProp(P_GUILD, "abenteurer"); // in die Abenteuergilde mit ihm...
   SetProp(P_GUILD_LEVEL, 20);
   ModifySkill("feuerball",([SI_SKILLABILITY:10000]),1,"abenteurer");

   // nun noch eine andere Art zu zaubern...
   SetProp(P_SPELLRATE, 10);
   AddSpell(100, 400,
     "Der Ork tritt Dich einmal feste zwischen die Beine.\n",
     "Der Ork tritt @WEN einmal feste zwischen die Beine.\n", DT_BLUDGEON);

   // was soll der NPC mit ihm gegebenen Gegenstaenden machen...
   SetProp(P_REJECT, ({REJECT_DROP, "Damit kann ich nichts anfangen.\n"}));

   // und natuerlich auch noch eine id fuer den ork
   AddId("ork");

   // Der NPC hat immer einige Juwelen bei sich.
   AddItem("/doc/beispiele/Praxis-Kurs/juwelen");
}

// Diese Funktion wird aufgerufen, wenn man einen Gegenstand bekommt...
public void give_notify(object ob)
{
   if (ob->id("milchflasche")) // Milch annehmen und einen Spruch aufsagen
     write("Der Ork sagt: Ohh.... Danke fuer die leckere Milch.\n");
   else ::give_notify(ob); // Gegenstand fallen lassen
}

// Diese Funktion wird aufgerufen, wenn uns jemand im Kampf Schaden zufuegen
// moechte...
public varargs int Defend(int dam, mixed dam_type, mixed spell, object enemy)
{
   if (!random(4))
     write("Der Ork weicht Deinem Angriff geschickt aus.\n");
   else
     return ::Defend(dam, dam_type, spell, enemy);
}

public void Attack(object enemy)
{
  // das Kommando 'Feuerball' eingeben. Da der NPC Mitglied der
  // Abenteurergilde ist (s.o.), spricht er also den Spell, falls er genug
  // Magiepunkt dafuer hat.
  if (!random(3)) command_me("feuerball");
  ::Attack(enemy);
}
