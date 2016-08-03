// MorgenGrauen MUDlib
//
// spellbook.c -- Grundlegende Funktionen fuer Zaubersprueche
//
// $Id: spellbook.c 9142 2015-02-04 22:17:29Z Zesstra $
#pragma strict_types
#pragma save_types
#pragma no_shadow
#pragma no_clone
#pragma pedantic
#pragma range_check

//#define NEED_PROTOTYPES
inherit "/std/thing";
inherit "/std/restriction_checker";
inherit "/std/player/pklog";

#include <thing/properties.h>
#include <properties.h>
#include <wizlevels.h>
#include <new_skills.h>
#include <spellbook.h>
#define ME this_object()

void create() {
  seteuid(getuid());
  // diese Prop sollte von aussen nicht direkt geaendert werden koennen.
  Set(P_SB_SPELLS,([]),F_VALUE);
  Set(P_SB_SPELLS,PROTECTED,F_MODE_AS);

  ::create();
}

mapping _query_sb_spells() {
    // Kopie liefern, da sonst versehentlich schnell eine Aenderung des
    // originalen Mappings hier passieren kann.
    return(deep_copy(Query(P_SB_SPELLS, F_VALUE)));
}

mapping QuerySpell(string spell) {
  mapping spi;

  //Query-Methode umgehen, damit nur ein Spellmapping kopiert werden muss und
  //nicht das ganzen Mapping mit allen Spells.
  if (!spell
      || !(spi=Query(P_SB_SPELLS))
      || !(spi=spi[spell]))
    return 0;
  return deep_copy(spi);
}

varargs int
AddSpell(string verb, int kosten, mixed ski) {
  int level;
  mapping spells;

  if (!verb || kosten<0)
    return 0;
  
  verb=lower_case(verb);
  level=(intp(ski))?ski:0;

  if (!mappingp(ski)) ski=([]);
  // Zur Sicherheit eine Kopie machen, wer weiss, was da uebergeben wird, bzw.
  // was der Setzende mit ski hinterher noch macht.
  else ski=deep_copy(ski);

  ski=AddSkillMappings(QueryProp(P_GLOBAL_SKILLPROPS),ski);
  ski+=([SI_SPELLCOST:kosten]);
  // SI_SPELL-Submapping hinzufuegen, wenn nicht da.
  if (!member(ski,SI_SPELL))
      ski+=([SI_SPELL:([]) ]);
  // ski[SI_SPELL][SP_NAME] ergaenzen, ggf. aus ski verschieben
  if (!stringp(ski[SI_SPELL][SP_NAME]) && stringp(ski[SP_NAME])) {
      ski[SI_SPELL][SP_NAME] = ski[SP_NAME];
      m_delete(ski, SP_NAME);
  }
  else if (!stringp(ski[SI_SPELL][SP_NAME])) {
      ski[SI_SPELL][SP_NAME] = verb;
  }
  if (stringp(ski[SP_NAME])) {
      m_delete(ski, SP_NAME);
  }

  if (level)
    ski=AddSkillMappings(ski,([SI_SKILLRESTR_LEARN:([P_LEVEL:level])]));
  
  // Abfrage per Query(), um das Mapping als Referenz, nicht als Kopie zu
  // bekommen, das spart etwas Zeit.
  if (!(spells=Query(P_SB_SPELLS,F_VALUE))) {
    spells=([]);
    SetProp(P_SB_SPELLS,spells);
  }
  // Spell setzen...
  spells[verb]=ski;
  // Set() nicht noetig, da Query() an spell das Originalmapping geliefert hat
  // und dieses bereits jetzt geaendert wurde. ;-)
  //Set(P_SB_SPELLS,spells+([verb:ski]),F_VALUE);

  // hat wohl geklappt...
  return(1);
}

int
TryAttackSpell(object victim, int damage, mixed dtypes,
               mixed is_spell, object caster, mapping sinfo) {
  mixed no_attack;
  int nomag;

  if (no_attack = (mixed)victim->QueryProp(P_NO_ATTACK)) {
    if (stringp(no_attack))
      write(no_attack);
    else
      write(victim->Name(WER,1)+" laesst sich nicht angreifen!\n");
    return 0;
  }
  if (victim->QueryProp(P_GHOST)) {
    write(victim->Name(WER,1)+" ist ein Geist!\n");
    return 0;
  }

  damage=(damage*(int)caster->QuerySkillAttribute(SA_DAMAGE))/100;

  // ggf. Loggen von PK-Versuchen und ggf. ausserdem Angriff abbrechen.
  // BTW: Aufruf von InsertEnemy() gibt 0 zurueck, wenn der Gegner nicht
  // eingetragen wurde. Allerdings tut es das auch, wenn der Gegner schon
  // Feind war. Daher muss noch geprueft werden, ob nach dem InsertEnemy() die
  // beiden Spieler Feinde sind. Wenn nicht, wird der Spell auch abgebrochen
  // und eine Meldung ausgegeben.
  if (query_once_interactive(caster) && query_once_interactive(victim)
      && CheckPlayerAttack(caster, victim, 
        sprintf("Spellbook: %O, Spell: %O\n",
                object_name(this_object()),
                ((mappingp(sinfo) && is_spell) ? sinfo[SP_NAME]  : "")))
      && !caster->IsEnemy(victim)
      && !caster->InsertEnemy(victim)
      )
  {
    tell_object(ME, "Ein goettlicher Einfluss schuetzt Deinen Gegner.\n");
    return 0;
  }

  nomag=(int)victim->QueryProp(P_NOMAGIC);
  if ((sinfo[SI_NOMAGIC] < nomag) &&
      nomag*100 > random(100)*(int)caster->QuerySkillAttribute(SA_ENEMY_SAVE)) {
    printf("%s wehrt Deinen Zauber ab.\n", capitalize((string)victim->name(WER, 1)));
    return 0;
  }
  else {
    return (int)victim->Defend(damage, dtypes, is_spell, caster);
  }
}

varargs int
TryDefaultAttackSpell(object victim, object caster, mapping sinfo, 
                      mixed si_spell) {
  object team;
  int row;
  
  if (!si_spell) si_spell=sinfo[SI_SPELL];
  // Wenn der Spieler in einem Team ist, die Teamreihen-Boni
  // beruecksichtigen. Wenn nicht, eben nicht.
  if (!team=((object)caster->QueryProp(P_TEAM)))
  return TryAttackSpell(victim,
                        GetRandFValueO(SI_SKILLDAMAGE,sinfo,caster),
                        GetData(SI_SKILLDAMAGE_TYPE,sinfo,caster),
                        si_spell,
                        caster,
                        sinfo);
  else
  {
	  row=(int)caster->PresentPosition();
	  return TryAttackSpell(victim,
		  GetRandFValueO(SI_SKILLDAMAGE,sinfo,caster)+
		  // Nur wenn SI_SKILLDAMAGE_BY_ROW ein mapping ist, 
		  // randomisiert addieren
		  (mappingp(sinfo[SI_SKILLDAMAGE_BY_ROW])?
		   random(sinfo[SI_SKILLDAMAGE_BY_ROW][row]):0)+
		  // Nur wenn das OFFSET davon ein mapping ist,
		  // nicht randomisiert addieren.
		  (mappingp(sinfo[OFFSET(SI_SKILLDAMAGE_BY_ROW)])?
		   sinfo[OFFSET(SI_SKILLDAMAGE_BY_ROW)][row] : 0),
		   GetData(SI_SKILLDAMAGE_TYPE,sinfo,caster),
		   si_spell,caster,sinfo);
   }
}

#define SMUL(x,y) ((x<0 && y<0)?(-1*x*y):(x*y))
int
SpellSuccess(object caster, mapping sinfo) {
  int cval,abil,res;

  abil=sinfo[SI_SKILLABILITY];
  cval=(int)caster->UseSkill(SK_CASTING);
  res=abil + (SMUL(abil,cval)) / MAX_ABILITY - random(MAX_ABILITY);
  if (cval && res>MAX_ABILITY) // besonders gut gelungen?
    caster->LearnSkill(SK_CASTING,1+(res-MAX_ABILITY)/2000);
  return res;
}

int
CanTrySpell(object caster, mapping sinfo) {
  mapping rmap;
  string res;

  if (caster->QueryProp(P_GHOST)) {
    write("Als Geist kannst Du nicht zaubern.\n");
    return 0;
  }
  if ((rmap=sinfo[SI_SKILLRESTR_USE])
      && (res=check_restrictions(caster,rmap))) {
    write(res);
    return 0;
  }
  return 1;
}

void
Learn(object caster, string spell, mapping sinfo) {
  int val,diff,attval;
  mapping attr;
 
  // gar nicht lernen?
  if (sinfo[SI_NO_LEARN])
    return;

 // Attribut sollte int sein, wenn nicht anders angegeben
 if (mappingp(sinfo[SI_LEARN_ATTRIBUTE]))
 {
	attr=sinfo[SI_LEARN_ATTRIBUTE];
	// Direkt in einem durch berechnen.
	// Ich gehe davon aus, dass wir nie nie nie
	// ein weiteres Attribut ins MG einfuegen.
	// Wir berechnen den Mittelwert
	attval=((int)caster->QueryAttribute(A_INT)*attr[A_INT]+
		(int)caster->QueryAttribute(A_DEX)*attr[A_DEX]+
		(int)caster->QueryAttribute(A_STR)*attr[A_STR]+
		(int)caster->QueryAttribute(A_CON)*attr[A_CON])
		/(attr[A_CON]+attr[A_INT]+attr[A_STR]+attr[A_DEX]);
		
	 
 } else {
	 attval=(int)caster->QueryAttribute(A_INT);
 }

  
  val=((attval/2)*GetFValue(SI_SKILLLEARN,sinfo,caster)+
       GetOffset(SI_SKILLLEARN,sinfo,caster));
  if (!(diff=GetFValueO(SI_DIFFICULTY,sinfo,caster)))
    diff=GetFValueO(SI_SPELLCOST,sinfo,caster);
  caster->LearnSkill(spell,val,diff);
}

void
Erfolg(object caster, string spell, mapping sinfo) {
  object env;
  if(env=environment(caster))
     env->SpellInform(caster,spell,sinfo);
}

void
Misserfolg(object caster, string spell, mapping sinfo) {
  write("Der Zauberspruch ist missglueckt.\n");
  write("Du lernst aus Deinem Fehler.\n");
  Learn(caster,spell,sinfo);
}

string
SelectSpell(string spell, mapping sinfo) {
  if (sinfo && sinfo[SI_SKILLFUNC])
    return sinfo[SI_SKILLFUNC];
  return spell;
}

varargs void
prepare_spell(object caster, string spell, mapping sinfo) {
  string txt;

  if (!caster || !spell) return ;
  if (!mappingp(sinfo) || !stringp(txt=sinfo[SI_PREPARE_MSG]))
    txt="Du bereitest Dich auf den Spruch ``%s'' vor.\n";
  tell_object(caster,sprintf(txt,spell));
}

varargs int
UseSpell(object caster, string spell, mapping sinfo) {
  mapping ski,tmp;
  string spellbook,sname,fname,txt;
  int res,fat,cost;
  mixed ps;

  if (!caster || !spell)
    return 0;
  // Spell kann in der Gilde anderen Namen haben
  sname=SelectSpell(spell,sinfo);
  if (!(ski=QuerySpell(sname))) // Existiert dieser Spell?
    return 0;
  // Gildeneigenschaften sollen Spelleigenschaften ueberschreiben koennen
  ski=AddSkillMappings(ski,sinfo);
  // Fuer verschiedene Rassen unterschiedliche Eigenschaften
  ski=race_modifier(caster,ski);

  // printf("Spellinfo: %O\n",ski);

  if (!CanTrySpell(caster, ski))
    return 1;

  if (caster->QueryProp(P_SP) < (cost=GetFValueO(SI_SPELLCOST,ski,caster))) {
    if(txt=ski[SI_SP_LOW_MSG])write(txt);
    else write("Du hast zu wenig Zauberpunkte fuer diesen Spruch.\n");
    return 1;
  }
  // printf("cost: %d\n",cost);

  if (mappingp(ski[SI_X_SPELLFATIGUE])) {
    // fuer jeden Key die Spellfatigue pruefen, wenn das Mapping hinterher
    // nicht leer ist, ist man zu erschoepft.
    tmp = filter(ski[SI_X_SPELLFATIGUE],
        function int (string key, int val)
        { return (int)caster->CheckSpellFatigue(key); } );
    if (sizeof(tmp)) {
        write(ski[SI_TIME_MSG] ||
            "Du bist noch zu erschoepft von Deinem letzten Spruch.\n");
      return 1;
    }
  }
  else {
    if (caster->CheckSpellFatigue()) {
        write(ski[SI_TIME_MSG] ||
            "Du bist noch zu erschoepft von Deinem letzten Spruch.\n");
      return 1;
    }
  }

  if (!(ski[SI_NO_ATTACK_BUSY]&NO_ATTACK_BUSY_QUERY) &&
      caster->QueryProp(P_ATTACK_BUSY)) {
    if (txt=ski[SI_ATTACK_BUSY_MSG]) write(txt);
    else write("Du bist schon zu sehr beschaeftigt.\n");
    return 1;
  }

  // Spruchvorbereitung
  if (pointerp(ps=(mixed)caster->QueryProp(P_PREPARED_SPELL)) // Ausstehender Spruch
      && sizeof(ps)>=3 && intp(ps[0] && stringp(ps[1]))) {
    if (ps[1]==spell) { // Dieser Spruch wird noch vorbereitet
      if (time()<ps[0]) {
        if (!stringp(txt=ski[SI_PREPARE_BUSY_MSG]))
          txt="Du bist noch mit der Spruchvorbereitung beschaeftigt.\n";
        write(txt);
        return 1;
      }
    }
    else { // Andere Sprueche brechen die Vorbereitung ab
      if (!mappingp(tmp=QuerySpell(ps[1])) || // richtige Meldung holen...
          !stringp(txt=tmp[SI_PREPARE_ABORT_MSG]))
        txt="Du brichst die Spruchvorbereitung fuer `%s' ab.\n";
      printf(txt,ps[1]);
      if (fat=GetValue(SI_PREPARE_TIME,ski,caster)) {
        // Spruch braucht vorbereitungszeit
        caster->SetProp(P_PREPARED_SPELL,({time()+fat,spell,ski[SI_SKILLARG]}));
        prepare_spell(caster,spell,ski);
        return 1;
      }
    }
  }
  else {
    if (fat=GetValue(SI_PREPARE_TIME,ski,caster)) {
      // Spruch braucht vorbereitungszeit
      caster->SetProp(P_PREPARED_SPELL,({time()+fat,spell,ski[SI_SKILLARG]}));
      prepare_spell(caster,spell,ski);
      return 1;
    }
  }
  if (ps)
    caster->SetProp(P_PREPARED_SPELL,0);
  
  // Funktion kann anderen Namen haben als Spell
  if (!(fname=sinfo[SI_SKILLFUNC]))
    fname=sname;

  if((ski[SI_NOMAGIC] < environment(caster)->QueryProp(P_NOMAGIC)) &&
      random(100) < environment(caster)->QueryProp(P_NOMAGIC)) {
    if (txt=ski[SI_NOMAGIC_MSG])
      write(txt);
    else
      write("Dein Zauberspruch verpufft im Nichts.\n");
    res=ABGEWEHRT;
  }
  else {
    // Spruch ausfuehren.
    res=(int)call_other(this_object(),fname,caster,ski);
  }
  // TODO: Wenn die ausgefuehrte Spellfunktion eine 0 zurueckgibt, sollen jetzt
  // noch notify_fails zum Zuge kommen koennen. Daher in diesem Fall auch 0
  // zurueckgeben.
  if (!res || !caster)
    return 1;

  if (!(ski[SI_NO_ATTACK_BUSY]&NO_ATTACK_BUSY_QUERY))
  	{
	if (!ski[SI_ATTACK_BUSY_AMOUNT])  
    		caster->SetProp(P_ATTACK_BUSY,1);
	else
		caster->SetProp(P_ATTACK_BUSY,ski[SI_ATTACK_BUSY_AMOUNT]);
  	}

  caster->restore_spell_points(-1*cost);

  if (mappingp(ski[SI_X_SPELLFATIGUE])) {
    // fuer jeden Key die Spellfatigue setzen. Keys mit Dauer 0 loesen keine
    // Spellfatigue aus.
    filter(ski[SI_X_SPELLFATIGUE],
        function int (string key, int val)
        { return (int)caster->SetSpellFatigue(val, key); } );
  }
  else {
    if ((fat=GetFValueO(SI_SPELLFATIGUE,ski,caster))<0)
      fat=1;
    caster->SetSpellFatigue(fat);
  }


  if (res==ERFOLG)
    Erfolg(caster,spell,ski);
  else
  if (res==MISSERFOLG)
    Misserfolg(caster,spell,ski);
  return 1;
}

object *
FindGroup(object pl, int who) {
  object *res,ob,env,team;
  int p1,p2;
  closure qp;

  res=({});
  if (!pl || !(env=environment(pl))) return res;
  p1 = query_once_interactive(pl) ? 1 : -1;
  team=(object)pl->QueryProp(P_TEAM);
  for (ob=first_inventory(env);ob;ob=next_inventory(ob)) {
    if (!living(ob)) continue;
    qp=symbol_function("QueryProp",ob);
    if (pl->IsEnemy(ob)) // Feinde sind immer Gegner
      p2=-1*p1;
    else if (objectp(team) && funcall(qp,P_TEAM)==team)
      p2=p1; // Teammitglieder sind immer auf Seite des Spielers
    else
      p2 = (query_once_interactive(ob)||funcall(qp,P_FRIEND)) ? 1 : -1;
    if (p2>0 && !interactive(ob) && query_once_interactive(ob))
      continue; // keine Netztoten.
    if (funcall(qp,P_GHOST))
      continue;
    if ( who<0 && (funcall(qp,P_NO_ATTACK) || funcall(qp,P_NO_GLOBAL_ATTACK)) )
      continue;
    if (IS_LEARNING(ob) &&
        (funcall(qp,P_INVIS) || (who<0 && !pl->IsEnemy(ob))))
      continue;
    if (p1*p2*who >=0)
      res+=({ob});
  }
  return res;
}

object *
FindGroupN(object pl, int who, int n) {
  if (!pl) return ({});
  n=(n*(int)pl->QuerySkillAttribute(SA_EXTENSION))/100;
  if (n<1) n=1;
  return FindGroup(pl,who)[0..(n-1)];
}

object *
FindGroupP(object pl, int who, int pr) {
  object *res,*nres;
  int i;

  nres=({});
  if (!pl) return nres;
  pr=(pr*(int)pl->QuerySkillAttribute(SA_EXTENSION))/100;
  if (pr<0) return nres;
  res=FindGroup(pl,who);
  for (i=sizeof(res)-1;i>=0;i--)
    if (pr>=random(100))
      nres+=({res[i]});
  return nres;
}

// Findet feindliche und freundliche GRUPPEN im angegebenen Bereich.
varargs mixed
FindDistantGroups(object pl, int dist, int dy, int dx) {
  mapping pos;
  object ob,enteam,myteam;
  int p1,min,max,i;
  mixed *b_rows,x;
  closure is_enemy, qp;

  if (!objectp(pl) || !environment(pl))
    return ({({}),({})});
  if (!dy) dy=100;
  if (!dx) dx=MAX_TEAM_ROWLEN*100;
  x=(int)pl->QuerySkillAttribute(SA_EXTENSION);
  dx=(dx*x)/100;dy=(dy*x)/100;
  dist=(dist*(int)pl->QuerySkillAttribute(SA_RANGE))/100;
  min=dist-dy/2;
  max=dist+dy/2;

  pos=([]);
  p1=query_once_interactive(pl) ? 1 : -1;
  is_enemy=symbol_function("IsEnemy",pl); // zur Beschleunigung
  myteam=(object)pl->QueryProp(P_TEAM);
  for (ob=first_inventory(environment(pl));ob;ob=next_inventory(ob)) {
    if (!living(ob)) continue;
    qp=symbol_function("QueryProp",ob); // zur Beschleunigung

    // Zuerst mal die Position feststellen:
    if (!objectp(enteam=funcall(qp,P_TEAM)))
      pos[ob]=1;
    else if (!pos[ob] && mappingp(x=(mapping)ob->PresentTeamPositions()))
      pos+=x;
    // PresentTeamPositions wird nur einmal pro Team ausgerechnet, weil
    // anschliessend jedes anwesende Teammitglied pos[ob]!=0 hat.

    pos[ob]=(2*pos[ob])-1;
    // Reihen sollen Abstand 2 haben, auch Reihen 1 und -1 (nach Umrechnung)

    // Feindliche Reihen an Y-Achse spiegeln, also gegenueber hinstellen:
    if (funcall(is_enemy,ob))
      pos[ob]*=-1; // Ist auf jeden Fall Feind
    else if (objectp(myteam) && myteam==enteam)
        ;            // Teammitglieder sind immer auf eigener Seite
    else
      pos[ob]*=(p1*((int)(query_once_interactive(ob)||
                          funcall(qp,P_FRIEND))?1:-1));

    // Den Spieler auf keinen Fall entfernen
    if (ob==pl)
      continue;
    // Netztote, Geister  und unsichtbare Magier nicht beruecksichtigen,
    // nicht (global) angreifbare Monster und nicht-feindliche Magier
    // von feindlicher Seite entfernen.
    if ((!interactive(ob) && query_once_interactive(ob)) // Netztote raus
        || funcall(qp,P_GHOST)
        || (IS_LEARNING(ob) && funcall(qp,P_INVIS))   // Bin nicht da :-)
        || (pos[ob]<0 && (funcall(qp,P_NO_GLOBAL_ATTACK) // Nicht angreifen
                          || funcall(qp,P_NO_ATTACK)
                          || (IS_LEARNING(ob) && !funcall(is_enemy,ob)))))
      m_delete(pos,ob);
  }

  // Reihen aufstellen
  // Kampfreihe: |   5     4     3     2     1 ||  1     2     3     4     5|
  // Arrays:     |0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19|
  //             |<----------- Freunde ------->||<--------- Feinde -------->|
  b_rows=EMPTY_TEAMARRAY+EMPTY_TEAMARRAY+EMPTY_TEAMARRAY+EMPTY_TEAMARRAY;
  x=m_indices(pos);
  for (i=sizeof(x)-1;i>=0;i--) {
    p1=2*MAX_TEAMROWS-pos[ob=x[i]];
    pos[ob]=p1;
    if (p1>=0 && p1<4*MAX_TEAMROWS) {
      if (random(100)<50)             // Damit gut gemischt wird...
        b_rows[p1]=b_rows[p1]+({ob}); // zufaellig hinten anfuegen
      else
        b_rows[p1]=({ob})+b_rows[p1]; // oder vorne anfuegen.
    }
  }

  // Umrechnung von Min, Max auf Reihen
  // ... -2: -124..-75, -1:-74..-25, 0:-24..24, 1:25..74 2:75..124 ...
  // Das muss man machen, wenn man keine vernuenftige Rundungsfunktion hat:
  if (min<0) min=-1*((25-min)/50); else min=(25+min)/50;
  if (max<0) max=-1*((25-max)/50); else max=(25+max)/50;
  // Relativ zur Position des Spielers verschieben:
  p1=pos[pl];min+=p1;max+=p1;
  // Umrechnung von Breite auf Anzahl:
  dx=((dx+50)/100)-1;if (dx<0) dx=0;

  x=({({}),({})});
  for (i=0;i<2*MAX_TEAMROWS;i++)
    if (i>=min && i<=max)
      x[1]+=b_rows[i][0..dx]; // Freunde
  for (i=2*MAX_TEAMROWS+1;i<4*MAX_TEAMROWS;i++)
    if (i>=min && i<=max)
      x[0]+=b_rows[i][0..dx]; // Feinde

  return x;
}

// Findet feindliche oder freundliche Gruppe (oder Summe beider) im
// angegebenen Bereich.
varargs object *
FindDistantGroup(object pl, int who, int dist, int dy, int dx) {
  mixed *x;

  x=FindDistantGroups(pl,dist,dy,dx);
  if (who<0)
    return x[0];
  else if (who>0)
    return x[1];
  return x[0]+x[1];
}


static varargs object
find_victim(string wen, object pl) {
  object victim;

  if (!pl) return 0;
  if (!sizeof(wen)) {
    if (victim = (object)pl->SelectEnemy())
      return victim;
    else
      return 0;
  }
  if (victim = present(wen, environment(pl)))
    return victim;
  else if (victim = present(wen, pl))
    return victim;
  else
    return 0;
}
varargs object
FindVictim(string wen, object pl, string msg) {
  object vic;

  if (!(vic=find_victim(wen,pl)) && msg)
    write(msg);
  return vic;
}
varargs object
FindLivingVictim(string wen, object pl, string msg) {
  object vic;

  if (!(vic=FindVictim(wen,pl,msg)))
    return 0;
  if (!living(vic) || vic->QueryProp(P_GHOST)) {
    printf("%s lebt doch nicht!\n", capitalize((string)vic->name()));
    return 0;
  }
  return vic;
}

private varargs object
DoFindEnemyVictim(string wen, object pl, string msg,
                  mixed func, int min, int max) {
  object vic;
  mixed no_attack;
  int row;

  if (!(vic=FindLivingVictim(wen,pl,msg))) {
    if ((stringp(wen) && wen!="") || !objectp(pl))
      return 0;
    if (pointerp(func)) { // Soll einer DIESER Gegner genommen werden?
      if (!(vic=(object)pl->SelectEnemy(func))) // Dann daraus auswaehlen
        return 0;
    } else {
      if (!stringp(func))
        func="SelectEnemy";
      if (!(vic=(object)call_other(pl,func,0,min,max)))
        return 0;
    }
    func=0; // kein zweites Mal pruefen.
  }
  if (no_attack = (mixed)vic->QueryProp(P_NO_ATTACK)) {
    if (stringp(no_attack))
      write(no_attack);
    else
      write(vic->Name(WER,1)+" laesst sich nicht angreifen.\n");
    return 0;
  }
  if (vic==pl) {
    write("Du koenntest Dir dabei wehtun.\n");
    return 0;
  }
  if (stringp(func)) {
    switch(func) {
    case "SelectNearEnemy":
      if (pl->PresentPosition()>1) {
        write("Du stehst nicht in der ersten Kampfreihe.\n");
        return 0;
      }
      if (vic->PresentPosition()>1) {
        write(vic->Name(WER,1)+" ist in einer hinteren Kampfreihe.\n");
        return 0;
      }
      break;
    case "SelectFarEnemy":
      if (row=(int)vic->PresentPosition())
        row--;
      if (row>=min && row<=max)
        break;
      if (row<min)
        write(vic->Name(WER,1)+" ist zu nahe.\n");
      else if (row>max)
        write(vic->Name(WER,1)+" ist zu weit weg.\n");
      else
        write(vic->Name(WER,1)+" ist unerreichbar.\n");
      return 0;
    default:;
    }
  } else if (pointerp(func)) {
    if (member(func,vic)<0) {
      write(vic->Name(WER,1)+" ist unerreichbar.\n");
      return 0;
    }
  }

  if (!pl->IsEnemy(vic)) // War es bisher kein Feind?
    pl->Kill(vic);       // Dann ist es jetzt einer.
  return vic;
}

varargs object
FindEnemyVictim(string wen, object pl, string msg) {
  return DoFindEnemyVictim(wen,pl,msg,"SelectEnemy");
}

// Wie FindEnemyVictim, aber nur im Nahkampf erreichbare Feinde werden
// gefunden.
varargs object
FindNearEnemyVictim(string wen, object pl, string msg) {
  return DoFindEnemyVictim(wen,pl,msg,"SelectNearEnemy");
}

// Wie FindEnemyVictim, aber nur Feinde im Bereich der angegebenen Reihen
// (min,max) werden gefunden.
varargs object
FindFarEnemyVictim(string wen, object pl, string msg,
                   int min, int max) {
  return DoFindEnemyVictim(wen,pl,msg,"SelectFarEnemy",min,max);
}

// Wie FindEnemyVictim, findet aber nur Feinde in
// FindDistantGroup(GEGNER,entfernung,abweichung)
varargs object
FindDistantEnemyVictim(string wen, object pl, string msg,
                       int dist, int dy) {
  return DoFindEnemyVictim(wen,pl,msg,
                           FindDistantGroup(pl,-1,dist,dy,10000));
}

varargs int
TryGlobalAttackSpell(object caster, mapping sinfo, int suc,
                     int damage, mixed dt, mixed is_spell,
                     int dist, int depth, int width) {
  int i;
  mixed x,coldam;
  object *obs,ob;

  if (!suc) suc=random(sinfo[SI_SKILLABILITY]);
  if (!dt) dt=GetData(SI_SKILLDAMAGE_TYPE,sinfo,caster);
  if (!is_spell) is_spell=GetData(SI_SPELL,sinfo,caster);
  if (!dist) dist=GetRandFValueO(SI_DISTANCE,sinfo,caster);
  if (!depth) depth=GetRandFValueO(SI_DEPTH,sinfo,caster);
  if (!width) width=GetRandFValueO(SI_WIDTH,sinfo,caster);

  if (!depth && width) depth=width;
  if (!width && depth) width=depth;
  if (!mappingp(is_spell)) is_spell=([]);
  is_spell[SP_GLOBAL_ATTACK]=1;

  x=FindDistantGroups(caster,dist,depth,width);
  sinfo[SI_NUMBER_ENEMIES]=sizeof(x[0]);
  sinfo[SI_NUMBER_FRIENDS]=sizeof(x[1]);

  obs=x[0];
  for (i=sizeof(obs)-1;i>=0;i--)
    if (objectp(ob=obs[i]) && suc>=ob->SpellDefend(caster,sinfo))
      TryAttackSpell(ob,(damage?random(damage):
                         GetRandFValueO(SI_SKILLDAMAGE,sinfo,caster)),
                     dt,is_spell,caster,sinfo);

  if (!intp(coldam=sinfo[SI_COLLATERAL_DAMAGE]) || !coldam)
    return 1;
  obs=x[1];
  for (i=sizeof(obs)-1;i>=0;i--)
    if (objectp(ob=obs[i]) && suc>=ob->SpellDefend(caster,sinfo))
      ob->reduce_hit_points(((damage?random(damage):
                              GetRandFValueO(SI_SKILLDAMAGE,sinfo,caster))
                             *coldam)/10);
  // 10 statt 100 ist Absicht, weil reduce_hit_points schon um Faktor
  // 10 staerker wirkt als Defend.

  return 1;
}
