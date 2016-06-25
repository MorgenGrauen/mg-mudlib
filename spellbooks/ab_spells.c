/* Gildenobjekt fuer Abenteurer - MorgenGrauen */
/* Boing, 1993-1998                                */
/* Sprueche:
     Lichtkugel herbeizaubern: licht
       Magiepunkte 5, Stufe 4
       Dauer: Level * 20 Sekunden
     Identifizieren: identifiziere
       Magiepunkte 10, Stufe 3
     Monster einschaetzen: schaetz <name>
       Magiepunkte 3, Stufe 10
     Pfeilangriff: pfeil <name>
       Schaden maximal 20, Magiepunkte 10, Stufe 11
     Gegenstaende untersuchen: identifiziere <objekt>
       Wenn P_INFO exisitiert, wird dies ausgegeben
       Magiepunkte 10, stufe 8
     Ausweichen: ausweichen
       Geht nur im Kampf, genau einer Attacke wird ausgewichen.
       Magiepunkte 10, Stufe 12
     Kampfschrei: kampfschrei <opfer> bzw. kampfschrei meldung <meldung>
       Der Kampfschrei wird ausgestossen und verlangsamt den Gegner.
       Magiepunkte 30, stufe 13
     Feuerball: feuerball <name>
       Schaden maximal 40, Magiepunkte 20, Stufe 15
     Schnelligkeit: schnell
       Gibt fuer eine bestimmte Zeit zusaetzliche Angriffe
       Magiepunkte 100, Stufe 17
     Saeurestrahl: saeurestrahl <name>
       Schaden maximal 60, Magiepunkte 25, Stufe 30
*/

#include <properties.h>
#include <language.h>
#include <attributes.h>
#include <combat.h>
#include <wizlevels.h>
#include <new_skills.h>

#define LICHTKUGEL "/gilden/files.abenteurer/lichtkugel"

inherit "/std/spellbook";

void create() {
  ::create();
  SetProp(P_GLOBAL_SKILLPROPS,
          ([SI_SPELLFATIGUE:2,       // 2 Sekunden kein neuer Spell
            SI_SKILLLEARN:5,         // 0.05% pro 2 A_INT
            OFFSET(SI_SKILLLEARN):10 // plus 0.1% in jedem Fall
            ]));

    AddSpell("steinschlag",5,
        ([SI_SKILLRESTR_LEARN:([P_LEVEL:1]),
         SI_SKILLDAMAGE:40,
         OFFSET(SI_SKILLDAMAGE):40,
         SI_SKILLDAMAGE_TYPE: ({DT_BLUDGEON}),
         SI_MAGIC_TYPE:({MT_ANGRIFF}),
         SI_SPELL:([SP_NAME:"steinschlag"])]));

  AddSpell("identifiziere",10,
       ([SI_SKILLRESTR_LEARN:([P_LEVEL:3]),
         SI_MAGIC_TYPE: ({ MT_HELLSICHT }),
         SI_SPELL: ([P_NAME: "identifiziere"]),
         ]) );
  AddSpell("licht",5,
      ([SI_SKILLRESTR_LEARN:([P_LEVEL:6]),
        SI_SPELL: ([P_NAME: "licht"]) 
       ]) );
  AddSpell("schaetz",3,
       ([SI_SKILLRESTR_LEARN:([P_LEVEL:8]),
         SI_MAGIC_TYPE: ({ MT_HELLSICHT }),
         SI_SPELL: ([SP_NAME: "schaetz"]),
         ]));
  AddSpell("pfeil",10,
           ([SI_SKILLRESTR_LEARN:([P_LEVEL:10]),
             // Um diesen Spruch lernen zu koennen muss
             // der Level mindestens 11 sein
             SI_SKILLDAMAGE:200,
             // Schaden: Random(200) ...
             SI_SKILLDAMAGE_TYPE:({DT_MAGIC}),
             // Damage Type: Magic
             SI_MAGIC_TYPE: ({ MT_ANGRIFF }),
             SI_SPELL: ([ SP_NAME: "pfeil" ]),
             // Boni für hintere Kampfreihen
             SI_SKILLDAMAGE_BY_ROW: ([2:110,3:50,4:10]),
             OFFSET(SI_SKILLDAMAGE_BY_ROW): ([2:50,3:20,4:5])
             ]));
  AddSpell("ausweichen", 10,
      ([SI_SKILLRESTR_LEARN:([P_LEVEL:13]),
        SI_SPELL: ([P_NAME: "ausweichen"]),
        ]));
  AddSpell("kampfschrei", 30,
       ([SI_SKILLRESTR_LEARN:([P_LEVEL:16]),
         SI_MAGIC_TYPE: ({ MT_PSYCHO }),
         SI_SPELL: ([SP_NAME: "kampfschrei"]),
         ]));
  AddSpell("feuerball",20,
           ([SI_SKILLRESTR_LEARN:([P_LEVEL:18]),
             SI_SKILLDAMAGE:300,
             // Schaden: Random(300) ...
             OFFSET(SI_SKILLDAMAGE):100,
             // ... plus 100
             SI_SKILLDAMAGE_TYPE:({DT_FIRE}),
             // Damage Type: Fire
            SI_MAGIC_TYPE: ({ MT_ANGRIFF }),
            SI_SPELL: ([SP_NAME: "feuerball"]),
            SI_SKILLDAMAGE_BY_ROW: ([2:80,3:150,4:80,5:20]),
            OFFSET(SI_SKILLDAMAGE_BY_ROW): ([2:30,3:80,4:30,5:10])
             ]));
  AddSpell("schnell",100,
      ([SI_SKILLRESTR_LEARN:([P_LEVEL:21]),
        SI_SPELL: ([P_NAME: "schnell"]) ]));
  AddSpell("saeurestrahl",25,
       ([SI_SKILLRESTR_LEARN:([P_LEVEL:28]),
             OFFSET(SI_SKILLLEARN):1,
             FACTOR(SI_SKILLLEARN):20,
             SI_SKILLDAMAGE:400,
             // Schaden: Random(400) ...
             OFFSET(SI_SKILLDAMAGE):200,
             // ... plus 200
             SI_SKILLDAMAGE_TYPE:({DT_ACID}),
             // Damage Type: Saeure
             SI_MAGIC_TYPE: ({ MT_ANGRIFF }),
             SI_SPELL: ([SP_NAME: "saeurestrahl"]),
             SI_SKILLDAMAGE_BY_ROW:([2:60,3:150,4:210,5:150,6:60]),
             OFFSET(SI_SKILLDAMAGE_BY_ROW):([2:30,3:80,4:120,5:80,6:30])
             ]));

}

int
steinschlag(object caster, mapping sinfo) {
  object victim;
  int hands;
  string vn, vnw, cn;
  int suc;

  victim=FindEnemyVictim(sinfo[SI_SKILLARG],caster,
         "Ueber wem willst Du einen Steinschlag niedergehen lassen?\n");
  if (!victim)
    return 0;
  if ((suc=SpellSuccess(caster,sinfo))<=0)
    return MISSERFOLG;
  if (suc < victim->SpellDefend(caster, sinfo))
  {
    tell_object(caster, victim->Name() + " wehrt Deinen Spruch ab.\n");
    return ERFOLG;
  }

  cn = caster->Name();
  vnw = victim->name(WEN, 2);
  say(sprintf("%s laesst kleine Kiesel auf %s fallen.\n",cn, vnw),
      victim);
  printf("Du konzentrierst Dich darauf, kleine Kiesel auf %s fallen zu lassen.\n", vnw);
  tell_object(victim,sprintf("%s laesst kleine Kiesel auf Dich fallen.\n",
                             cn));
  TryDefaultAttackSpell(victim,caster,sinfo,
            ([SP_NAME:"steinschlag", SP_PHYSICAL_ATTACK:1,SP_SHOW_DAMAGE:1]));
  return ERFOLG;
}

int
schaetz(object caster, mapping sinfo) {
   object victim;
   int hpdiff, wcdiff, acdiff;
   int hp, vhp, wc, vwc, ac, vac;
   string result;
   string vname, vnamegen;
   int suc;

   victim=FindLivingVictim(sinfo[SI_SKILLARG],caster,
                           "Wen moechtest Du einschaetzen?\n");
   if (!victim)
     return 0;
   if ((suc=SpellSuccess(caster,sinfo))<=0)
      return MISSERFOLG;
   if (suc < victim->SpellDefend(caster, sinfo))
   {
     tell_object(caster, victim->Name() + " wehrt Deinen Spruch ab.\n");
     return ERFOLG;
   }

   hp = caster->QueryProp(P_HP);    vhp = victim->QueryProp(P_HP);
   wc = caster->QueryProp(P_TOTAL_WC);  vwc = victim->QueryProp(P_TOTAL_WC);
   ac = caster->QueryProp(P_TOTAL_AC);  vac = victim->QueryProp(P_TOTAL_AC);
   hpdiff = hp - vhp;
   wcdiff = wc - vwc;
   acdiff = ac - vac;
   vname = victim->name(WER, 1);
   vnamegen = victim->name(WESSEN, 1);

   if (hpdiff > 10*vhp)
      result = capitalize(victim->name(WER, 1))+" sollte sich besser schon mal einen Sarg besorgen.\n";
   else if (hpdiff > 7*vhp)
      result = "Du bist " + victim->name(WEM, 1) + " haushoch ueberlegen.\n";
   else if (hpdiff > 4*vhp)
      result = "Du bist sehr viel staerker als " + vname + ".\n";
   else if (hpdiff > 2*vhp)
      result = "Du bist viel staerker als " + vname + ".\n";
   else if (2*hpdiff > 3*vhp)
      result = "Du bist spuerbar staerker als " + vname + ".\n";
   else if (hpdiff > vhp)
      result = "Du bist staerker als " + vname + ".\n";
   else if (hpdiff > vhp/5)
      result = "Du bist etwas staerker als " + vname + ".\n";
   else if (-hpdiff > 10*hp)
      result = "Besorg Dir besser schon mal einen Sarg.\n";
   else if (-hpdiff > 7*hp)
      result = capitalize(vname) + " ist Dir haushoch ueberlegen.\n";
   else if (-hpdiff > 4*hp)
      result = capitalize(vname) + " ist sehr viel staerker als Du.\n";
   else if (-hpdiff > 2*hp)
      result = capitalize(vname) + " ist viel staerker als Du.\n";
   else if (-2*hpdiff > 3*hp)
      result = capitalize(vname) + " ist spuerbar staerker als Du.\n";
   else if (-hpdiff > hp)
      result = capitalize(vname) + " ist staerker als Du.\n";
   else if (-hpdiff > hp/5)
      result = capitalize(vname) + " ist etwas staerker als Du.\n";
   else
      result = capitalize(vname) + " ist etwa gleich stark wie Du.\n";
   write(result);

   if (acdiff > 2*vac)
      result = "Deine Ruestung ist unglaublich viel besser als die "
     + vnamegen + ".\n";
   else if (acdiff > vac)
      result = "Deine Ruestung ist viel besser als die " + vnamegen + ".\n";
   else if (acdiff >vac/5)
      result = "Deine Ruestung ist besser als die " + vnamegen + ".\n";
   else if (-acdiff > 2*ac)
      result = "Die Ruestung " + vnamegen
     + " ist unglaublich viel besser als Deine.\n";
   else if (-acdiff > ac)
      result = "Die Ruestung " + vnamegen + " ist viel besser als Deine.\n";
   else if (-acdiff > ac/5)
      result = "Die Ruestung " + vnamegen + " ist besser als Deine.\n";
   else
      result = "Eure Ruestungen sind etwa gleich gut.\n";
   write(result);

   if (wcdiff > 2*vwc)
      result = "Deine Waffe ist eigentlich zu gut fuer diesen Gegner.\n";
   else if (wcdiff > vwc)
      result = "Deine Waffe ist viel besser als die " +vnamegen + ".\n";
   else if (wcdiff > vwc/5)
      result = "Deine Waffe ist besser als die " +vnamegen + ".\n";
   else if (-wcdiff > 2*wc)
      result = "Die Waffe " + vnamegen
     + " laesst Deine ganz mickrig aussehen.\n";
   else if (-wcdiff > wc)
      result = "Die Waffe " + vnamegen + " ist viel besser als Deine.\n";
   else if (-wcdiff > wc/5)
      result = "Die Waffe " + vnamegen + " ist besser als Deine.\n";
   else
      result = "Eure Waffen sind etwa gleich gut.\n";
   write(result);
   return ERFOLG;
}

int kampfschrei(object caster, mapping sinfo)
{
  object ob;
  string cmdline, meldung;
  int    suc, dauer;

  cmdline = this_player()->_unparsed_args();
  if (cmdline)
  {
    if (sscanf(cmdline, "meldung %s", meldung))
    {
      caster->Set("ab_kampfschrei", SAVE, F_MODE_AS);
      caster->SetProp("ab_kampfschrei", meldung);
      printf("Dein Kampfschrei ist jetzt: %s\n", meldung);
      return 0;
    }
  }
  ob = FindEnemyVictim(sinfo[SI_SKILLARG], caster,
          "Wem willst Du Deinen Kampfschrei entgegenschleudern?\n");

  if (!ob)
    return 0;

  if ((suc=SpellSuccess(caster, sinfo)) <= 0)
    return MISSERFOLG;
  if (suc < ob->SpellDefend(caster, sinfo))
  {
    tell_object(caster, ob->Name() + " wehrt Deinen Spruch ab.\n");
    return ERFOLG;
  }

  meldung = caster->QueryProp("ab_kampfschrei");
  if (!meldung)
    meldung = "Aaaaaaaiiiiieeeeeeeeeeeeeeeehhhhhhhhhhhhhhh!!!!!!!\n";

  write(break_string(meldung, 78, "Du kreischst: "));
  tell_room(environment(caster),
        break_string(meldung, 78, caster->Name()+" kreischt: "),
        ({caster}));
  tell_object(ob, "Du fuehlst Dich wie gelaehmt.\n");

  dauer = 1 + (suc/1000 + random(caster->QueryProp(P_LEVEL)/2))/2;
  if (dauer>25)
    dauer = 25;
  ob->ModifySkillAttribute(SA_SPEED, -50, dauer);

  return ERFOLG;
}

int ausweichen(object caster, mapping sinfo)
{
  if (!caster->InFight())
  {
    write("Ausweichen ausserhalb eines Kampfes ist ziemlich sinnlos.\n");
    return 0;
  }

  if (caster->QueryProp(P_TMP_DEFEND_HOOK))
  {
    write("Das geht momentan nicht, Du wunderst Dich wieso!\n");
    return 0;
  }

  if (SpellSuccess(caster,sinfo)<=0)
    return MISSERFOLG;

  caster->SetProp(P_TMP_DEFEND_HOOK, ({ time()+4, this_object(), "Deckung" }));
  return ERFOLG;
}

mixed Deckung(int dam, mixed dam_type, mixed spell, object enemy)
{
  if (!pointerp(dam_type))
    dam_type = ({ dam_type });
  if (sizeof(dam_type & m_indices(PHYSICAL_DAMAGE_TYPES)))
  {
    tell_object(previous_object(), "Du weichst gekonnt aus.\n");
    if (previous_object()->QueryProp(P_TMP_DEFEND_HOOK)[1] == this_object())
      previous_object()->SetProp(P_TMP_DEFEND_HOOK, 0);
    return 0;
  }
  tell_object(previous_object(), "Dein Ausweichversuch ist vergebens.\n");
  return ({ dam, dam_type, spell, enemy });
}

int
identifiziere(object caster, mapping sinfo) {
  object ob;
  string* fi;
  string cn;
  string result, info;
  int suc;
  ob=FindVictim(sinfo[SI_SKILLARG], caster,
                "Was moechtest Du identifizieren?\n");
  if (!ob)
    return 0;

  cn = caster->name();
  if ((suc=SpellSuccess(caster,sinfo))<=0)
    return MISSERFOLG;
  if (suc < ob->SpellDefend(caster, sinfo))
  {
    tell_object(caster, ob->Name() + " wehrt Deinen Spruch ab.\n");
    return ERFOLG;
  }

  result="Du identifizierst "+ob->name(WEN,1)+". "
        +capitalize(ob->QueryPronoun())
        +" besteh"
        +(ob->QueryProp(P_PLURAL) ? "en" : "t")
        +" aus "
        +(ob->MaterialList(WEM, caster))+". ";
  if (info=ob->QueryProp(P_INFO))
    result+="Ausserdem bemerkst Du folgendes:\n"+info;
  else result+="Du kannst sonst nichts besonderes an "+ob->QueryPronoun(WEM)
              +" erkennen.";
  write(break_string(result, 78, 0, 1));
  return ERFOLG;
}

int
licht(object caster, mapping sinfo)
{
   object l;

   tell_object(caster, "Du machst eine magische Bewegung.\n");
   tell_room(environment(caster),
       sprintf("%s macht eine magische Bewegung.\n",
           capitalize(caster->name(WER))), ({caster}));
   
   int suc=SpellSuccess(caster,sinfo);
   if (suc<=0)
      return MISSERFOLG;

   l = clone_object(LICHTKUGEL);
   if (l->move(this_player()) > 0)
   {
      int lvl;
      // fuer Spruchrollen-Support.
      if (!sinfo[SI_USR] || !(lvl=sinfo[SI_USR][P_LEVEL]) )
        lvl = caster->QueryProp(P_LEVEL);
      lvl = min(lvl,42);
      // wenn der Spell nicht ganz so gut gelingt, ist die Kugel auch etwas
      // dunkler.
      if (suc<6000)
      {
        l->SetProp(P_LIGHT,1);
        tell_object(caster,
            "Wie aus dem Nichts erscheint eine leuchtende Kugel.\n");
      }
      else
        tell_object(caster,
            "Wie aus dem Nichts erscheint eine hell leuchtende Kugel.\n");
      l->start_remove(lvl * 20 * min(suc+3333,10000) / 10000);
   }
   else
   {
      l->remove();
      tell_object(caster,
          "Eine Lichtkugel erscheint, zerplatzt aber sofort mit einem sanften 'plopp'.\n");
   }

   return ERFOLG;
}

int
saeurestrahl(object caster, mapping sinfo) {
  object victim;
  int hands;
  string vnw;
  int suc;

  victim=FindEnemyVictim(sinfo[SI_SKILLARG],caster,
                     "Wen willst Du mit einem Saeurestrahl beschiessen?\n");
  if (!victim)
    return 0;
  if ((suc=SpellSuccess(caster,sinfo))<=0)
    return MISSERFOLG;
  if (suc < victim->SpellDefend(caster, sinfo))
  {
    tell_object(caster, victim->Name() + " wehrt Deinen Spruch ab.\n");
    return ERFOLG;
  }

  vnw = victim->name(WEN, 2);
  say(break_string(
   sprintf(
   "%s fuchtelt seltsam in der Luft herum und schiesst einen Saeurestrahl auf %s.\n",
       caster->Name(), vnw), 78), victim);
  printf("Du schiesst einen Saeurestrahl auf %s.\n", vnw);
  tell_object(victim, sprintf("%s schiesst einen Saeurestrahl auf Dich.\n",
                              caster->Name()));

  TryDefaultAttackSpell(victim,caster,sinfo,
            ([SP_NAME:"Saeurestrahl", SP_SHOW_DAMAGE:1]));

  return ERFOLG;
}

int
feuerball(object caster, mapping sinfo) {
  object victim;
  int hands;
  string vnw;
  int suc;

  victim=FindEnemyVictim(sinfo[SI_SKILLARG],caster,
                         "Auf wen willst Du einen Feuerball schleudern?\n");
  if (!victim)
    return 0;
  if ((suc=SpellSuccess(caster,sinfo))<=0)
    return MISSERFOLG;
  if (suc < victim->SpellDefend(caster, sinfo))
  {
    tell_object(caster, victim->Name() + " wehrt Deinen Spruch ab.\n");
    return ERFOLG;
  }

  vnw = victim->name(WEM, 2);
  say(sprintf("%s wirft einen Feuerball nach %s.\n",
              caster->Name(), vnw), victim);
  printf("Du wirfst einen Feuerball nach %s.\n", vnw);
  tell_object(victim, sprintf("%s wirft einen Feuerball nach Dir.\n",
                              caster->Name()));

  TryDefaultAttackSpell(victim,caster,sinfo,
            ([SP_NAME:"Feuerball", SP_SHOW_DAMAGE:1]));

  return ERFOLG;
}

int
pfeil(object caster, mapping sinfo) {
  object victim;
  int hands;
  string vn, vnw, cn;
  int suc;

  victim=FindEnemyVictim(sinfo[SI_SKILLARG],caster,
                         "Nach wem willst Du einen magischen Pfeil werfen?\n");
  if (!victim)
    return 0;
  if ((suc=SpellSuccess(caster,sinfo))<=0)
    return MISSERFOLG;
  if (suc < victim->SpellDefend(caster, sinfo))
  {
    tell_object(caster, victim->Name() + " wehrt Deinen Spruch ab.\n");
    return ERFOLG;
  }

  cn = caster->Name();
  vnw = victim->name(WEN, 2);
  say(sprintf("%s feuert einen magischen Pfeil auf %s ab.\n",cn, vnw),
      victim);
  printf("Du feuerst einen magischen Pfeil auf %s ab.\n", vnw);
  tell_object(victim,sprintf("%s feuert einen magischen Pfeil auf Dich ab.\n",
                             cn));
  TryDefaultAttackSpell(victim,caster,sinfo,
            ([SP_NAME:"Pfeil", SP_SHOW_DAMAGE:1]));
  return ERFOLG;
}

int
schnell(object caster, mapping sinfo) {
  int dauer;

  if (SpellSuccess(caster,sinfo)<=0)
    return MISSERFOLG;
  write("Du fuehlst Dich ploetzlich deutlich schneller.\n");
  dauer = 2 * (caster->QueryAttribute(A_DEX) +
           random(sinfo[SI_SKILLABILITY]/1000));
  if (dauer>60)
    dauer = 60;
  caster->ModifySkillAttribute(SA_SPEED, 100, dauer);

  return ERFOLG;
}
