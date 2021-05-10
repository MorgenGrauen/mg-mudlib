// MorgenGrauen MUDlib
//
// living/combat.c -- Basis-Kampfmodul
//
// $Id: combat.c 9568 2016-06-05 18:53:10Z Zesstra $
#pragma strong_types
#pragma save_types
#pragma range_check
#pragma no_clone

inherit "/std/living/skill_utils";
inherit "/std/living/inventory";
inherit "/std/living/team";

#include <sys_debug.h>
#include <debug_message.h>

#define NEED_PROTOTYPES
#include <hook.h>
#include <living/skills.h>
#include <thing/properties.h>
#include <player/comm.h>
#include <living/skill_attributes.h>
#include <combat.h>
#include <living.h>
#undef NEED_PROTOTYPES

#include <config.h>
#include <properties.h>
#include <language.h>
#include <wizlevels.h>
#include <attributes.h>
#include <new_skills.h>

#include <defines.h>

#include <sensitive.h>

#define HUNTTIME 300 //300 HBs sind 10 Minuten
#define RNAME(x) capitalize(getuid(x))

// 'private'-Prototypes
private string _kill_alias( string str );

// globale Variablen
nosave mapping enemies;
private nosave int attack_busy;
nosave int no_more_attacks;
private nosave int remaining_heart_beats;
private nosave int att2_time;
private nosave string last_attack_msg;
private nosave object *missing_attacks;
// Cache fuer QueryArmourByType()
private nosave mapping QABTCache;

protected void create()
{
  Set(P_WIMPY, SAVE, F_MODE_AS);
  Set(P_TOTAL_AC, PROTECTED, F_MODE_AS);
  Set(P_HANDS, SAVE, F_MODE_AS);
  Set(P_SHOW_ATTACK_MSG,SAVE,F_MODE_AS);
  Set(P_RESISTANCE, ({}));
  Set(P_VULNERABILITY, ({}));
  Set(P_GUILD_PREPAREBLOCK, SAVE, F_MODE_AS);
  // Kein Setzen von P_ARMOURS von aussen per Set(). (Per SetProp() geht es
  // durch die Setmethode).
  Set(P_ARMOURS,PROTECTED,F_MODE_AS);
  SetProp(P_ARMOURS, ({}));
  attack_busy=100;
  att2_time=0;
  enemies=([]);
  team::create(); 
  offerHook(H_HOOK_DEFEND,1);
  offerHook(H_HOOK_ATTACK,1);
  offerHook(H_HOOK_ATTACK_MOD,1);
}

#undef DEBUG
#define DEBUG(x) if (find_object("zesstra")) \
  tell_object(find_player("zesstra"),x)

public void UpdateResistanceStrengths() { 
  mapping resmods, strmap;

  if ( !mappingp(resmods=Query(P_RESISTANCE_MODIFIER)) )
    return;

  //erstmal die alte Aufsummation loeschen
  m_delete(resmods,"me");

  //wenn jetzt leer: Abbruch, keine Res-Modifier da.
  if (!sizeof(resmods))
      return(Set(P_RESISTANCE_MODIFIER,0));

  strmap = ([]);

  // ueber alle gesetzten ResModifier gehen
  foreach(string mod, mapping resmap, object ob: resmods) {
    if ( !mappingp(resmap) || !sizeof(resmap)
        || !objectp(ob) ) {
      m_delete(resmods, mod);
      continue; // Resi ungueltig, weg damit.
    }
    // jetzt noch ueber die Submappings laufen, die die Resis der Objekte
    // beinhalten.
    foreach(string reskey, float resi: resmap) {
        strmap[reskey] = ((strmap[reskey]+1.0)*(resi+1.0))-1.0;
    }
  }

  if ( !sizeof(strmap) )
    Set(P_RESISTANCE_MODIFIER, 0);
  else
    Set(P_RESISTANCE_MODIFIER, resmods+([ "me" : strmap; 0 ]) );
}

public varargs int AddResistanceModifier(mapping mod, string add)
{ string  key;
  mapping res;

  if ( !mappingp(mod) || !sizeof(mod) || !previous_object() )
    return 0;

  key = explode(object_name(previous_object()),"#")[0];

  if ( add )
    key += ("#"+add);

  res = Query(P_RESISTANCE_MODIFIER);
  mod = deep_copy(mod);

  if ( !mappingp(res) )
    res = ([ key : mod; previous_object() ]);
  else
    res += ([ key : mod; previous_object() ]);

  Set(P_RESISTANCE_MODIFIER, res);
  UpdateResistanceStrengths();

  return 1;
}

public varargs void RemoveResistanceModifier(string add)
{ string  key;
  mapping res;

  if ( !previous_object() )
    return;

  key = explode(object_name(previous_object()),"#")[0];

  if ( add )
    key += ("#"+add);

  if ( !mappingp(res = Query(P_RESISTANCE_MODIFIER)) )
    return;

  m_delete(res, key);
  Set(P_RESISTANCE_MODIFIER, res);
  UpdateResistanceStrengths();
}

// veraltete Prop, aus Kompatibilitaetsgruenden noch vorhanden.
static mixed _set_resistance(mixed arg)
{ int     i;
  mapping resimap;
  mixed   old;

  if ( !pointerp(arg) )
    arg=({arg});

  if ( !mappingp(resimap=Query(P_RESISTANCE_STRENGTHS)) )
    resimap=([]);

  if ( pointerp(old=QueryProp(P_RESISTANCE)) )
    for ( i=sizeof(old)-1 ; i>=0 ; i-- )
      resimap[old[i]]=(1.0+resimap[old[i]])*2.0-1.0;

  for ( i=sizeof(arg)-1 ; i>=0 ; i-- )
    resimap[arg[i]]=(1.0+resimap[arg[i]])*0.5-1.0;

  SetProp(P_RESISTANCE_STRENGTHS,resimap);

  return Set(P_RESISTANCE,arg);
}

// veraltete Prop, aus Kompatibilitaetsgruenden noch vorhanden.
static mixed _set_vulnerability(mixed arg)
{ int     i;
  mapping resimap;
  mixed   old;

  if ( !pointerp(arg) )
    arg=({arg});

  if ( !mappingp(resimap=Query(P_RESISTANCE_STRENGTHS)) )
    resimap=([]);

  if ( pointerp(old=QueryProp(P_VULNERABILITY)) )
    for ( i=sizeof(old)-1 ; i>=0 ; i-- )
      resimap[old[i]]=(1.0+resimap[old[i]])*0.5-1.0;

  for ( i=sizeof(arg)-1 ; i>=0 ; i-- )
    resimap[arg[i]]=(1.0+resimap[arg[i]])*2.0-1.0;

  SetProp(P_RESISTANCE_STRENGTHS,resimap);

  return Set(P_VULNERABILITY,arg);
}


/** kill - Kampf starten.
 * Fuegt ob der Feindesliste hinzu.
 */
public int Kill(object ob)
{ int   res;
  int|string no_attack;

  if ( !objectp(ob) )
    return 0;

  if ( ob->QueryProp(P_GHOST) )
  {
    tell_object(ME,ob->Name(WER)+" ist doch schon tot!\n");
    return -1;
  }

  if ( no_attack = ob->QueryProp(P_NO_ATTACK) )
  {
    if ( stringp(no_attack) )
      tell_object(ME, no_attack);
    else
      tell_object(ME, ob->Name(WER,1)+" laesst sich nicht angreifen!\n");

    return -2;
  }

  if ( QueryProp(P_NO_ATTACK) )
    return -3;

  res=InsertEnemy(ob);
  if (res)
    tell_object(ME, "Ok.\n");
  else if (IsEnemy(ob))
    tell_object(ME, "Jajaja, machst Du doch schon!\n");
  //else //kein gueltiger Feind, ob wurde nicht eingetragen.

  if ( !res )
    return -4;  // nicht aendern ohne Kill in player/combat.c

  return 1;
}

public int InsertSingleEnemy(object ob)
{
    if ( !living(ob) )
        return 0;

    // Wie lange verfolgt dieses Living? Wenn Prop nicht gesetzt oder 
    // ungueltig, d.h. kein int oder <= 0, dann wird die Defaultzeit genommen.
    int hunttime = QueryProp(P_HUNTTIME);
    hunttime = (intp(hunttime) && hunttime > 0) ? 
                 hunttime / __HEART_BEAT_INTERVAL__ : HUNTTIME;

    // Auch wenn ein Objekt schon als Gegner eingetragen ist, trotzdem die
    // HUNTTIME erneuern. Das "return 0;" muss aber bleiben, da der Gegner
    // ja nicht neu ist.
    //
    // 09.12.2000, Tiamak
    if ( member( enemies, ob ) ) {
        enemies[ob,ENEMY_HUNTTIME] = hunttime;
        return 0;
    }

    if ( (ob==ME) || QueryProp(P_NO_ATTACK)
         || !objectp(ob) || (ob->QueryProp(P_NO_ATTACK))
         || (ob->QueryProp(P_GHOST)) )
        return 0;

    object team;
    if ( ( query_once_interactive(ME) || query_once_interactive(ob) )
         && objectp(team=Query(P_TEAM)) && (team==(ob->Query(P_TEAM))) )
        return 0;

    set_heart_beat(1);

    last_attack_msg=0;

    enemies[ob,ENEMY_HUNTTIME] = hunttime;
    Set(P_LAST_COMBAT_TIME,time());

    return 1;
}

public int InsertEnemy(object ob)
{ int res;

  if ( res=InsertSingleEnemy(ob) )
    InsertEnemyTeam(ob);

  return res;
}

public object QueryPreferedEnemy()
{ int    sz,r;
  <int|object>*  pref;
  object enemy;

  enemy=0;
  if ( pointerp(pref=QueryProp(P_PREFERED_ENEMY)) && ((sz=sizeof(pref))>1)
      && intp(r=pref[0]) && (random(100)<r) )
  {
    enemy=pref[1+random(sz-1)];

    if ( !objectp(enemy) )
    {
      pref-=({enemy});

      if ( sizeof(pref)<2 )
        pref=0;

      SetProp(P_PREFERED_ENEMY,pref);

      return 0;
    }

    if ( !IsEnemy(enemy) )
      return 0;
  }

  return enemy;
}

public object *PresentEnemies() {

  object *here=({});
  object *netdead=({});
  mixed no_attack;
  foreach(object pl: enemies) {
    if (no_attack = pl->QueryProp(P_NO_ATTACK)) {
      m_delete(enemies,pl);
      // Und auch im Gegner austragen, sonst haut der weiter zu und dieses
      // Living wehrt sich nicht. (Zesstra, 26.11.2006)
      if (stringp(no_attack)) {
        tell_object(ME, no_attack);
        pl->StopHuntFor(ME, 1);
      }
      else pl->StopHuntFor(ME, 0);
    }
    else if ( environment()==environment(pl) )
    {
      if ( interactive(pl) || !query_once_interactive(pl) )
        here+=({pl});
      else
        netdead+=({pl});
    }
  }

  if ( !sizeof(here) ) // Netztote sind nur Feinde, falls keine anderen da sind
    return netdead;

  return here;
}

public varargs object SelectEnemy(object *here)
{ object enemy;

  if ( !pointerp(here) )
    here=PresentEnemies();

  if ( !sizeof(here) )
    return 0;

  if ( !objectp(enemy=QueryPreferedEnemy())
      || (environment(enemy)!=environment()) )
    enemy=here[random(sizeof(here))];

  return enemy;
}

protected void heart_beat() {
  int hbs; 
  // leider rufen viele Leute einfach das geerbte heart_beat() obwohl sie
  // schon tot sind und damit das Objekt zerstoert ist.
  if (!living(this_object()))
      return;

  // Paralyse pruefen, ggf. reduzieren
  int dis_attack = QueryProp(P_DISABLE_ATTACK);
  if ( intp(dis_attack) && (dis_attack > 0) )
  {
      SetProp(P_DISABLE_ATTACK, --dis_attack);
  }

  // Attacken ermitteln: SA_SPEED + Rest aus dem letzten HB
  hbs = remaining_heart_beats + QuerySkillAttribute(SA_SPEED);
  if ( hbs <= 0 )
    hbs = 100; 

  // P_ATTACK_BUSY bestimmen
  if ( attack_busy > 99) 
   attack_busy = 100 + hbs;
  else
    attack_busy += 100 + hbs;
  // mehr fuer Seher...
  if ( IS_SEER(ME) )
    attack_busy+=(100+QueryProp(P_LEVEL)); 
  // max. 500, d.h. 5 Attacken
  if ( attack_busy>500 )
    attack_busy=500;

  // unganzzahligen Rest fuer naechsten HB speichern
  remaining_heart_beats = hbs % 100;
  hbs /= 100; // ganze Attacken. ;-)

  if ( hbs > 10 ) // nicht mehr als 10 Attacken.
    hbs = 10; 

  // Falls jemand seit dem letzten HB in diesem Living von aussen Attack()
  // gerufen hat (nicht ExtraAttack()), wird jetzt was abgezogen.
  hbs -= no_more_attacks;
  no_more_attacks = 0;

  // Wie lange verfolgt dieser NPC? Wenn Prop nicht gesetzt oder ungueltig,
  // d.h. kein int oder <= 0, dann wird die Defaultzeit genommen.
  int hunttime = QueryProp(P_HUNTTIME);
  hunttime = (intp(hunttime) && hunttime > 0) ? 
              hunttime / __HEART_BEAT_INTERVAL__ : HUNTTIME;
  // erstmal die Hunttimes der Gegner aktualisieren und nebenbei die
  // anwesenden Feinde merken. Ausserdem ggf. Kampf abbrechen, wenn
  // P_NO_ATTACK gesetzt ist.
  // eigentlich ist diese ganze Schleife fast das gleiche wie
  // PresentEnemies(), aber leider muessen ja die Hunttimes aktulisiert werde,
  // daher kann man nicht direkt PresentEnemies() nehmen.
  // TODO: Diese Schleife und PresentEnmies() irgendwie vereinigen.
  object *netdead=({});
  object *here=({});
  object enemy;
  mixed no_attack;
  foreach(enemy, int htime: &enemies) { // Keys in enemy
    // ggf. Meldungen ausgeben und Gegner austragen und dieses Living beim
    // Gegner austragen.
    if (no_attack=enemy->QueryProp(P_NO_ATTACK)) {
      m_delete(enemies,enemy);
        if ( stringp(no_attack) ) {
            write( no_attack );
            enemy->StopHuntFor( ME, 1 );
        }
        else
            enemy->StopHuntFor( ME, 0 );
    }
    else if ( environment()==environment(enemy) ) {
      // Gegner anwesend, als Feind merken...
      if ( interactive(enemy) || !query_once_interactive(enemy) )
        here+=({enemy});
      else
        netdead+=({enemy});
      // ... und Hunttime erneuern.
      htime = hunttime;
    }
    // Feind nicht anwesend. Timer dekrementieren und Feind austragen, wenn
    // Jagdzeit abgelaufen.
    else if ( (--htime) <= 0 )
      StopHuntFor(enemy);
  }
  // Netztote sind nur Feinde, wenn sonst keine da sind.
  if ( !sizeof(here) )
    here=netdead;

  // schonmal abfragen, ob ein Spell vorbereitet wird.
  mixed pspell=QueryProp(P_PREPARED_SPELL);

  //Da hbs 0 und no_more_attacks durch einen manuellen Aufruf von Attack() im
  //Spieler >0 sein kann, kann jetzt hbs <0 sein. (s.o.)
  if (hbs > 0 && sizeof(here)) {
    foreach(int i: hbs) {
      // Feind in Nahkampfreichweite finden
      if ( objectp(enemy=SelectNearEnemy(here)) ) {
        // Flucht erwuenscht?
        if ( QueryProp(P_WIMPY) > QueryProp(P_HP) ) {
          Flee();
          // Flucht gelungen?
          if ( enemy && (environment(enemy)!=environment()) ) {
            here = 0; // naechste Runde neue Feinde suchen
            enemy = 0;
            continue; // kein Kampf, Runde ueberspringen
          }
        }
      }
      else {
        // keine Feinde gefunden... Abbrechen.
        break;
      }
      // Paralyse gesetzt? -> Abbrechen. Dieses Pruefung muss hier passieren,
      // damit ggf. die automatische Flucht des Lebewesens durchgefuehrt wird,
      // auch wenn es paralysiert ist.
      if (dis_attack > 0) break;

      // Kampf durch Spell-Prepare blockiert?
      // Keine genaue Abfrage, wird spaeter noch genau geprueft.
      if ( pspell && QueryProp(P_GUILD_PREPAREBLOCK) )
        break; // keine Angriffe in diesem HB.
      // wenn Feind da: hit'em hard.
      if ( objectp(enemy) )
        Attack(enemy);
      // ggf. hat Attack() no_more_attacks gesetzt. Zuruecksetzen
      no_more_attacks = 0;
      // naechste Kampfrunde neue Feinde suchen
      here = 0;
    }  // foreach
  }

  no_more_attacks=0;

  // Ist ein Spell in Vorbereitung und evtl. jetzt fertig vorbereitet?
  if ( pointerp(pspell)
      && (sizeof(pspell)>=3) && intp(pspell[0]) && stringp(pspell[1]) ) {
    if ( time()>=pspell[0] ) // Kann der Spruch jetzt ausgefuehrt werden?
    {
      UseSpell(pspell[2],pspell[1]); // Dann los
      SetProp(P_PREPARED_SPELL,0);
    }
  }
  else if ( pspell ) // Unbrauchbarer Wert, loeschen
    SetProp(P_PREPARED_SPELL,0);

} // Ende heart_beat

// wird von NPC gerufen, die Heartbeats nachholen und dabei die Hunttimes um
// die Anzahl verpasster Heartbeats reduzieren muessen.
// Wird von Spielerobjekten gerufen, wenn sie nach 'schlafe ein' aufwachen, um
// die notwendige Anzahl an HB hier abzuziehen und ggf. die Feinde zu expiren,
// da Spieler ja netztot keinen Heartbeat haben.
protected void update_hunt_times(int beats) {
  if (!mappingp(enemies)) return;
  foreach(object en, int htime: &enemies) { // Mapping-Keys in en
    htime -= beats;
    if ( htime <= 0 )
      StopHuntFor(en);
  }
}

public int IsEnemy(object wer)
{
  return (member(enemies,wer));
}

public void StopHuntText(object arg)
{
  tell_object(arg,
    Name(WER,1)+" "+(QueryProp(P_PLURAL)?"jagen ":"jagt ")+
               (arg->QueryProp(P_PLURAL)?"Euch":"Dich")+" nicht mehr.\n");
  tell_object(ME,(QueryProp(P_PLURAL)?"Ihr jagt ":"Du jagst ")+
                 arg->name(WEN,1)+" nicht mehr.\n");
}

public varargs int StopHuntFor(object arg, int silent)
{
  if ( !objectp(arg) || !IsEnemy(arg) )
    return 0;

  if (!silent)
    StopHuntText(arg);

  m_delete(enemies,arg);
  last_attack_msg=0;

  return 1;
}

// Begruessungsschlag nur einmal pro HB
public void Attack2(object enemy)
{
  if ( att2_time > time() )
    return;

  att2_time=time() + __HEART_BEAT_INTERVAL__;
  Attack(enemy);
}

// Fuer evtl. Attack-Aenderungen, ohne dass man gleich das ganze
// Attack ueberlagern muss:
protected void InternalModifyAttack(mapping ainfo)
{
  return; // Vorerst!
/*
  int   fac;
  mixed res;

  fac=100;

  if (CannotSee(1))
    fac -= 50;

  // Weitere Mali?

  if (fac<100)
    ainfo[SI_SKILLDAMAGE] = ainfo[SI_SKILLDAMAGE]*fac/100;
  // Fertig
*/
}

// Ausfuehren Eines Angriffs auch wenn es bereits einen Angriff
// in dieser Runde gegeben hat. Dieser Angriff wird auch nicht
// gezaehlt. Die Nutzung dieser Funktion ist nur fuer Spezialfaelle
// gedacht und immer BALANCEPFLICHTIG!
varargs public void ExtraAttack(object enemy, int ignore_previous)
{
  int saved_no_more_attacks;

  // Erstschlagsinfo speichern.
  saved_no_more_attacks = no_more_attacks;

  // Bei Bedarf bisher schon durchgefuehrte Erstschlaege ignorieren
  if (ignore_previous) no_more_attacks=0;

  // Normalen Angriff durchfuehren
  Attack (enemy);

  // Gespeicherten Wert zuruecksetzen
  no_more_attacks = saved_no_more_attacks;

  return;
}

public void Attack(object enemy)
{
  closure cl;
  mixed   res;
  mapping ainfo;
  mapping edefendinfo; // erweiterte Defend-Infos
  mixed hookData;
  mixed hookRes;

  if ( no_more_attacks || QueryProp(P_GHOST) || 
      !objectp(enemy) || !objectp(this_object()) || 
      (QueryProp(P_DISABLE_ATTACK) > 0) || enemy->QueryProp(P_NO_ATTACK) || 
      (query_once_interactive(this_object()) && !interactive(this_object())) )
    return;

  edefendinfo=([]);

  Set(P_LAST_COMBAT_TIME,time());

  // inkrementieren. Diese Variable wird im HB nach den Angriffen genullt.
  // Diese Variable wird im naechsten Heartbeat von der Anzahl an verfuegbaren
  // Angriffen des Livings abgezogen. Dies beruecksichtigt also zwischen den
  // HBs (z.B. extern) gerufene Attacks().
  no_more_attacks++;

  // Wird das Attack durch einen temporaeren Attack-Hook ersetzt?
  if ( res=QueryProp(P_TMP_ATTACK_HOOK) )
  {
    if ( pointerp(res) && (sizeof(res)>=3) && intp(res[0]) && (time()<res[0])
        && objectp(res[1]) && stringp(res[2]) )
    {
      if ( !(res=call_other(res[1],res[2],enemy)) )
        return;
    }
    else
      SetProp(P_TMP_ATTACK_HOOK,0);
  }

  // trigger attack hook
  hookData=({enemy});
  hookRes=HookFlow(H_HOOK_ATTACK,hookData);
  if(hookRes && pointerp(hookRes) && sizeof(hookRes)>H_RETDATA){
    if(hookRes[H_RETCODE]==H_CANCELLED){
      return;
    }
  }

  ainfo = ([ SI_ENEMY : enemy,
             SI_SPELL : 0,
           ]);

  if ( objectp(ainfo[P_WEAPON]=QueryProp(P_WEAPON)) )
  {
    ainfo[P_WEAPON]->TakeFlaw(enemy);

    // Abfrage fuer den Fall, dass Waffe durch das TakeFlaw() zerstoert
    // wurde. Dann wird das Attack abgebrochen.
    if ( !objectp(ainfo[P_WEAPON]) )
      return;

    cl=symbol_function("name",ainfo[P_WEAPON]);
    ainfo[SI_SKILLDAMAGE_MSG] = (" mit "+funcall(cl,WEM,0));
    ainfo[SI_SKILLDAMAGE_MSG2] = (" mit "+funcall(cl,WEM,1));

    ainfo[SI_SKILLDAMAGE] = ainfo[P_WEAPON]->QueryDamage(enemy);

    cl=symbol_function("QueryProp",ainfo[P_WEAPON]);
    ainfo[SI_SKILLDAMAGE_TYPE] = funcall(cl,P_DAM_TYPE);
    ainfo[P_WEAPON_TYPE] = funcall(cl,P_WEAPON_TYPE);
    ainfo[P_NR_HANDS] = funcall(cl,P_NR_HANDS);
    ainfo[P_WC] = funcall(cl,P_WC);

    // Zweihaendige Waffe?
    if ( ainfo[P_NR_HANDS]==2
        && mappingp(res=UseSkill(SK_TWOHANDED,deep_copy(ainfo)))
        && member(res,SI_SKILLDAMAGE) )
    {
      // Nur den neuen Schadenswert uebernehmen.
      ainfo[SI_SKILLDAMAGE]=res[SI_SKILLDAMAGE];
    }
  }
  else // Keine Waffe gezueckt
  {
    /* Check if there is a magical attack */
    if ( mappingp(res=UseSkill(SK_MAGIC_ATTACK,([SI_ENEMY:enemy]))) )
    {
      ainfo[SI_SKILLDAMAGE]=res[SI_SKILLDAMAGE];
      ainfo[SI_SKILLDAMAGE_TYPE]=res[SI_SKILLDAMAGE_TYPE];

      if ( stringp(res[SI_SKILLDAMAGE_MSG]) )
        ainfo[SI_SKILLDAMAGE_MSG] = " mit "+res[SI_SKILLDAMAGE_MSG];
      else
        ainfo[SI_SKILLDAMAGE_MSG] = " mit magischen Faehigkeiten";

      if ( stringp(res[SI_SKILLDAMAGE_MSG2]) )
        ainfo[SI_SKILLDAMAGE_MSG2] = " mit "+res[SI_SKILLDAMAGE_MSG2];
      else
        ainfo[SI_SKILLDAMAGE_MSG2] = ainfo[SI_SKILLDAMAGE_MSG];

      if ( !(ainfo[P_WEAPON_TYPE]=res[P_WEAPON_TYPE]) )
        ainfo[P_WEAPON_TYPE]=WT_MAGIC;

      if ( member(res,SI_SPELL) )
        ainfo[SI_SPELL]=res[SI_SPELL];
    }
    else
    {
      /* Ohne (freie) Haende wird auch nicht angegriffen */
      if ( interactive(this_object())
          && (QueryProp(P_USED_HANDS) >= QueryProp(P_MAX_HANDS)) )
        return ;

      if ( !pointerp(res=QueryProp(P_HANDS)) || (sizeof(res)<2) )
        return;

      ainfo[SI_SKILLDAMAGE] = (( 2*random(res[1]+1)
                              + 10*(QueryAttribute(A_STR)) )/3);
      ainfo[SI_SKILLDAMAGE_TYPE] = res[2];
      ainfo[SI_SKILLDAMAGE_MSG] = ainfo[SI_SKILLDAMAGE_MSG2]
                                = res[0];
      ainfo[P_WEAPON_TYPE] = WT_HANDS;
      ainfo[P_WC] = res[1];
    }
  }  // besondere Faehigkeiten mit diesem Waffentyp?
  if ( mappingp(res=UseSkill(FIGHT(ainfo[P_WEAPON_TYPE]),
                             deep_copy(ainfo))) )
    SkillResTransfer(res,ainfo);

  // besondere allgemeine Kampffaehigkeiten?
  if ( mappingp(res=UseSkill(SK_FIGHT,deep_copy(ainfo))) )
    SkillResTransfer(res,ainfo);

  // Veraenderungen durch einen Attack-Modifier?
  if ( (res=QueryProp(P_TMP_ATTACK_MOD)) )
  {
    if ( pointerp(res) && (sizeof(res)>=3) && intp(res[0])
        && (time()<res[0]) && objectp(res[1]) && stringp(res[2]) )
    {
      if ( !(res=call_other(res[1],res[2],deep_copy(ainfo)))
          || !mappingp(res) )
        return;
      else
        SkillResTransfer(res,ainfo);
    }
    else
      SetProp(P_TMP_ATTACK_MOD,0);
  }

  // trigger attack mod hook
  hookData=deep_copy(ainfo);
  hookRes=HookFlow(H_HOOK_ATTACK_MOD,hookData);
  if(hookRes && pointerp(hookRes) && sizeof(hookRes)>H_RETDATA){
    if(hookRes[H_RETCODE]==H_CANCELLED){
      return;
    }
    else if(hookRes[H_RETCODE]==H_ALTERED && hookRes[H_RETDATA] &&
             mappingp(hookRes[H_RETDATA])){
        SkillResTransfer(hookRes[H_RETDATA],ainfo);
    }
  }

  // Interne Modifikationen der Angriffswerte
  InternalModifyAttack(ainfo);

  if ( !objectp(enemy) )
    return;

  // hier mal bewusst nicht auf P_PLURAL zuerst testen. in 90% der Faelle
  // wird eh keine Angriffsmeldung mehr ausgegeben, also pruefen wir das
  // lieber zuerst. Plural testen zieht schon genug Rechenzeit :/
  // Nicht meine Idee! Nicht meine Idee! Nachtwind 7.7.2001
  if ( ainfo[SI_SKILLDAMAGE_MSG2]!=last_attack_msg )
  {
    last_attack_msg = ainfo[SI_SKILLDAMAGE_MSG2];
    if (QueryProp(P_PLURAL))
    {
      tell_object( ME, "  Ihr greift " + enemy->name(WEN,1)
                   + ainfo[SI_SKILLDAMAGE_MSG2] + " an.\n" );
      say("  "+(Name(WER,1))+" greifen "+(enemy->name(WEN,1))+
          ainfo[SI_SKILLDAMAGE_MSG]+" an.\n", enemy );
      tell_object( enemy, "  "+(Name(WER,1))+" greifen "+
                  (enemy->QueryProp(P_PLURAL)?"Euch":"Dich")+
                   ainfo[SI_SKILLDAMAGE_MSG2]+" an.\n" );
    }
    else
    {
      tell_object( ME, "  Du greifst " + enemy->name(WEN,1)
                   + ainfo[SI_SKILLDAMAGE_MSG2] + " an.\n" );
      say("  "+(Name(WER,2))+" greift "+(enemy->name(WEN,1))+
          ainfo[SI_SKILLDAMAGE_MSG]+" an.\n", enemy );
      tell_object( enemy, "  "+(Name(WER,1))+" greift "+
                  (enemy->QueryProp(P_PLURAL)?"Euch":"Dich")+
                   ainfo[SI_SKILLDAMAGE_MSG2]+" an.\n" );    }
  }
  else if ( Query(P_SHOW_ATTACK_MSG) )
    if (QueryProp(P_PLURAL))
      tell_object( ME, "  Ihr greift " + enemy->name(WEN,1)
                   + ainfo[SI_SKILLDAMAGE_MSG2] + " an.\n" );
    else
      tell_object( ME, "  Du greifst " + enemy->name(WEN,1)
                   + ainfo[SI_SKILLDAMAGE_MSG2] + " an.\n" );

  // ainfo in defendinfo merken
  edefendinfo[ ORIGINAL_AINFO]= deep_copy(ainfo);
  edefendinfo[ ORIGINAL_DAM]= ainfo[SI_SKILLDAMAGE];
  edefendinfo[ ORIGINAL_DAMTYPE]= ainfo[SI_SKILLDAMAGE_TYPE];
  edefendinfo[ CURRENT_DAM]= ainfo[SI_SKILLDAMAGE];
  edefendinfo[ CURRENT_DAMTYPE]= ainfo[SI_SKILLDAMAGE_TYPE];

  // ainfo[SI_SPELL] auf ein mapping normieren
  if ( intp(ainfo[SI_SPELL]) )
  {
    ainfo[SI_SPELL] = ([ SP_PHYSICAL_ATTACK : !ainfo[SI_SPELL],
               SP_SHOW_DAMAGE     : !ainfo[SI_SPELL],
               SP_REDUCE_ARMOUR   : ([ ])  ]);
  }

  // defendinfo anhaengen, falls ainfo[SI_SPELL] ein mapping ist
  if( mappingp(ainfo[SI_SPELL]))
  {
    ainfo[SI_SPELL][EINFO_DEFEND]=edefendinfo;
  }


  enemy->Defend(ainfo[SI_SKILLDAMAGE], ainfo[SI_SKILLDAMAGE_TYPE],
                ainfo[SI_SPELL],       this_object());


  //edefendinfo=([]);

  /* Done attacking */
}

public void AddDefender(object friend)
{ object *defs;

  if ( !objectp(friend) )
    return;

  if ( !pointerp(defs=QueryProp(P_DEFENDERS)) )
    defs=({});

  if ( member(defs,friend)>=0 )
    return;

  defs+=({friend});
  SetProp(P_DEFENDERS,defs);
}

public void RemoveDefender(object friend)
{ object *defs;

  if ( !objectp(friend) )
    return;

  if ( !pointerp(defs=QueryProp(P_DEFENDERS)) )
    defs=({});

  if ( member(defs,friend)==-1 )
    return;

  defs -= ({friend});
  SetProp(P_DEFENDERS,defs);
}

public void InformDefend(object enemy)
{
  UseSkill(SK_INFORM_DEFEND,([ SI_ENEMY  : enemy,
                               SI_FRIEND : previous_object() ]));
  // Oh, oh - ich hoffe mal, dass InformDefend wirklich NUR aus Defend
  // eines befreundeten livings aufgerufen wird... (Silvana)
  // This is only experimental... ;)
}

public <int|string*|mapping>* DefendOther(int dam, string|string* dam_type,
                                          int|mapping spell, object enemy)
{
  <int|string*|mapping>* res;

  if ( (res=UseSkill(SK_DEFEND_OTHER,([ SI_SKILLDAMAGE : dam,
                                        SI_SKILLDAMAGE_TYPE : dam_type,
                                        SI_SPELL : spell,
                                        SI_FRIEND : previous_object(),
                                        SI_ENEMY : enemy ])))
      && pointerp(res) )
    return res;

  return 0;
}

public void CheckWimpyAndFlee()
{
  if ( (QueryProp(P_WIMPY)>QueryProp(P_HP)) && !TeamFlee()
      && find_call_out("Flee")<0 )
    call_out(#'Flee,0,environment());
}

protected string mess(string msg,object me,object enemy)
{ closure mname, ename;
  string  *parts;
  int     i;

  mname = symbol_function("name", me);
  ename = symbol_function("name", enemy);

  parts=regexplode(msg,"@WE[A-Z]*[12]");
  for ( i=sizeof(parts)-2 ; i>=1 ; i-=2 )
  {
    switch(parts[i])
    {
      case "@WER1":    parts[i]=funcall(mname,WER,1);    break;
      case "@WESSEN1": parts[i]=funcall(mname,WESSEN,1); break;
      case "@WEM1":    parts[i]=funcall(mname,WEM,1);    break;
      case "@WEN1":    parts[i]=funcall(mname,WEN,1);    break;
      case "@WER2":    parts[i]=funcall(ename,WER,1);    break;
      case "@WESSEN2": parts[i]=funcall(ename,WESSEN,1); break;
      case "@WEM2":    parts[i]=funcall(ename,WEM,1);    break;
      case "@WEN2":    parts[i]=funcall(ename,WEN,1);    break;
      default: ;
    }
  }

  return break_string(capitalize(implode(parts,"")),78,"  ",1);
}

// Fuer evtl. Defend-Aenderungen, ohne dass man gleich das ganze
// Attack ueberlagern muss:
protected void InternalModifyDefend(int dam, string* dt, mapping spell, object enemy)
{
  return;
}

protected nomask void normalize_defend_args(int dam, string|string* dam_type,
                                            int|mapping spell, object enemy)
{
  // this_player(), wenn kein enemy bekannt...
  enemy ||= this_player();

  // Schadenstyp ueberpruefen
  if ( !pointerp(dam_type) )
    dam_type = ({ dam_type });

  // Und das Spellmapping pruefen, erzeugen, ergaenzen etc.
  if ( intp(spell) )
    spell = ([ SP_PHYSICAL_ATTACK : !spell,
               SP_SHOW_DAMAGE     : !spell,
               SP_REDUCE_ARMOUR   : ([ ]),
               EINFO_DEFEND       : ([ORIGINAL_DAM:dam,
                                      ORIGINAL_DAMTYPE:dam_type ])
             ]);
  else if ( mappingp(spell) )
  {
    // testen ob eine erweiterte defendinfo vorhanden ist
    if(!member(spell,EINFO_DEFEND))
    {
      // wenn nicht, koennen wir an den fehlenden Infos wenig machen, aber
      // zumindest ergaenzen wir es und schreiben die (hier) initialen dam und
      // dam_type rein.
      spell[EINFO_DEFEND] = ([ORIGINAL_DAM:dam,
                              ORIGINAL_DAMTYPE:dam_type]);
    }
    if ( !mappingp(spell[SP_REDUCE_ARMOUR]) )
      spell[SP_REDUCE_ARMOUR] = ([]);
  }
  else // Illegaler spell-Parameter
    raise_error(sprintf("Ungueltiger Typ des spell-Arguments: %d\n",
                        get_type_info(spell,0)));
}

public int Defend(int dam, string|string* dam_type, int|mapping spell, object enemy)
{
  mixed   res,res2;
  object  *armours,tmp;
  mixed hookData;
  mixed hookRes;
  int i;

  //  string  what, how;
  string enname, myname;

  normalize_defend_args(&dam, &dam_type, &spell, &enemy);

  // Testen, ob dieses Lebewesen ueberhaupt angegriffen werden darf
  if ( !this_object() || !enemy || QueryProp(P_NO_ATTACK)
      || ( query_once_interactive(enemy) && ! interactive(enemy) ) )
    return 0;

  spell[EINFO_DEFEND][CURRENT_DAMTYPE]=dam_type;
  spell[EINFO_DEFEND][CURRENT_DAM]=dam;

  // Testen, ob der Angreifer schon als Feind registriert worden ist.
  // Wenn nein, registrieren.
  if ( !IsEnemy(enemy) && !spell[SP_NO_ENEMY] )
  {
    spell[EINFO_DEFEND][ENEMY_INSERTED]=1;
    InsertEnemy(enemy);
  }

  // RFR-Taktik abfangen
  if ( !QueryProp(P_ENABLE_IN_ATTACK_OUT) )
  {
    i=time()-(enemy->QueryProp(P_LAST_MOVE));
    // Gegner hat sich bewegt, man selbst nicht
    if ( (i<3) && (time()-QueryProp(P_LAST_MOVE)>=5) )
    {
      // Bei Erster Kampfrunde wenige Schaden
      dam/=(4-i);
      spell[EINFO_DEFEND][RFR_REDUCE]=dam;
      spell[EINFO_DEFEND][CURRENT_DAM]=dam;
    }
  }

  // Man kann Verteidiger haben. Diese kommen als erste zum Zuge
  if ( res=QueryProp(P_DEFENDERS) )
  { object *defs,*defs_here;

    defs=({});
    defs_here=({});
    if ( !pointerp(res) )
      res=({res});
    // erst alle anwesenden finden.
    foreach(object defender: res) {
      if ( objectp(defender) && (member(defs,defender)<0) )
      {
        defs+=({defender});
        //  Verteidiger muessen im gleichen Raum oder im Living selber
        //  enthalten sein.
        if ( environment(defender) == environment()
            || environment(defender) == ME)
        {
          call_other(defender,"InformDefend",enemy);
          if (defender)
            defs_here += ({ defender });
        }
      }
    }
    //Anwesende Verteidiger eintragen.
    spell[EINFO_DEFEND][PRESENT_DEFENDERS]=defs_here;

    // P_DEFENDERS auch gleich aktualisieren
    if ( sizeof(defs)<1 )
      defs=0;
    SetProp(P_DEFENDERS,defs);

    if ( spell[SP_PHYSICAL_ATTACK] ) {
      // Bei physischen Angriffen nur Verteidiger aus Reihe 1
      // nehmen (z.B. fuer Rueckendeckung)
      foreach(object defender: defs_here) {
        if ( (defender->PresentPosition())>1 ) {
          defs_here-=({defender});
        }
      }
    }
 
    if ( (i=sizeof(defs_here)) )
    {
      mixed edefendtmp=({defs_here[random(i)],0,0,0}); 
      res=call_other(edefendtmp[DEF_DEFENDER],"DefendOther",
                     dam,dam_type,spell,enemy);
      if ( pointerp(res) && (sizeof(res)>=3) && intp(res[0])
          && pointerp(res[1]))
      {
        // Helfer koennen den Schaden oder Schadenstyp aendern,
        // z.B. Umwandlung von Feuer nach Eis oder so...
        dam=res[0];
        edefendtmp[DEF_DAM]=dam;
        dam_type=res[1];
        edefendtmp[DEF_DAMTYPE]=dam_type;

        if ( mappingp(res[2]) )
        {
          spell=res[2];
          // teuer, aber geht nicht anders (Rekursion vermeiden)
          edefendtmp[DEF_SPELL]=deep_copy(res[2]);
        }
        spell[EINFO_DEFEND][CURRENT_DAMTYPE]=dam_type;
        spell[EINFO_DEFEND][CURRENT_DAM]=dam;
      }
      spell[EINFO_DEFEND][DEFENDING_DEFENDER]=edefendtmp;
    }
  } // Ende Defender-Verarbeitung


  // Ueber einen P_TMP_DEFEND_HOOK werden z.B. Schutzzauber gehandhabt
  spell[EINFO_DEFEND][DEFEND_HOOK]=DI_NOHOOK;
  if ( res=QueryProp(P_TMP_DEFEND_HOOK) )
  {
    if ( pointerp(res) && (sizeof(res)>=3) && intp(res[0]) && (time()<res[0])
        && objectp(res[1]) && stringp(res[2]) )
    {
      if ( !(res=call_other(res[1],res[2],dam,dam_type,spell,enemy)) )
      {
        // Ein P_TMP_DEFEND_HOOK kann den Schaden vollstaendig abfangen,
        // das Defend wird dann hier abgebrochen *SICK* und es wird nur
        // noch getestet, ob man in die Flucht geschlagen wurde
        spell[EINFO_DEFEND][DEFEND_HOOK]=DI_HOOKINTERRUPT;
        CheckWimpyAndFlee();
        return 0;
      }
      else
      {
        spell[EINFO_DEFEND][DEFEND_HOOK]=DI_HOOK;
        if ( pointerp(res) && (sizeof(res)>=3)
            && intp(res[0] && pointerp(res[1])) )
        {
          mixed edefendtmp=({0,0,0});
          // Der P_TMP_DEFEND_HOOK kann ebenfalls Schadenshoehe und
          // -art sowie die Spell-Infos veraendern
          dam=res[0];
          edefendtmp[HOOK_DAM]=dam;
          dam_type=res[1];
          edefendtmp[HOOK_DAMTYPE]=dam_type;

          if ( mappingp(res[2]) )
          {
            spell=res[2];
            // Waeh. Teuer. Aber geht nicht anders.
            edefendtmp[HOOK_SPELL]=deep_copy(spell);
          }
          spell[EINFO_DEFEND][DEFEND_HOOK]=edefendtmp;
          spell[EINFO_DEFEND][CURRENT_DAMTYPE]=dam_type;
          spell[EINFO_DEFEND][CURRENT_DAM]=dam;
        }
      }
    }
    else
      SetProp(P_TMP_DEFEND_HOOK,0);
  } // P_TMP_DEFEND_HOOK

  // trigger defend hook
  hookData=({dam,dam_type,spell,enemy});
  hookRes=HookFlow(H_HOOK_DEFEND,hookData);
  if(hookRes && pointerp(hookRes) && sizeof(hookRes)>H_RETDATA){
    if(hookRes[H_RETCODE]==H_CANCELLED){
        spell[EINFO_DEFEND][DEFEND_HOOK]=DI_HOOKINTERRUPT;
        CheckWimpyAndFlee();
        return 0;
    }
    else if(hookRes[H_RETCODE]==H_ALTERED && hookRes[H_RETDATA]){
        spell[EINFO_DEFEND][DEFEND_HOOK]=DI_HOOK;
        if ( pointerp(hookRes[H_RETDATA]) && (sizeof(hookRes[H_RETDATA])>=3)
            && intp(hookRes[H_RETDATA][0] && pointerp(hookRes[H_RETDATA][1])) )
        {
          mixed edefendtmp=({0,0,0});
          // Der P_TMP_DEFEND_HOOK kann ebenfalls Schadenshoehe und
          // -art sowie die Spell-Infos veraendern
          dam=hookRes[H_RETDATA][0];
          edefendtmp[HOOK_DAM]=dam;
          dam_type=hookRes[H_RETDATA][1];
          edefendtmp[HOOK_DAMTYPE]=dam_type;

          if ( mappingp(hookRes[H_RETDATA][2]) )
          {
            spell=hookRes[H_RETDATA][2];
            // Teuer, teuer... :-(
            edefendtmp[HOOK_SPELL]=deep_copy(spell);
          }
          spell[EINFO_DEFEND][DEFEND_HOOK]=edefendtmp;
          spell[EINFO_DEFEND][CURRENT_DAMTYPE]=dam_type;
          spell[EINFO_DEFEND][CURRENT_DAM]=dam;
        }
    }
  } // Ende Hook-Behandlung

  // Es gibt auch Parierwaffen,
  if ( objectp(tmp=QueryProp(P_PARRY_WEAPON)) )
  {
    res2=tmp->QueryDefend(dam_type, spell, enemy);

    // Reduzierbare Wirksamkeit der Parierwaffe?
    if ( member(spell[SP_REDUCE_ARMOUR],P_PARRY_WEAPON)
        && intp(res=spell[SP_REDUCE_ARMOUR][P_PARRY_WEAPON]) && (res>=0) )
    {
      res2=(res2*res)/100;
    }

    dam-=res2;
    spell[EINFO_DEFEND][CURRENT_DAM]=dam;
  }

  // Jetzt kommen die Ruestungen des Lebewesens ins Spiel (wenn es denn
  // welche traegt)

  armours=QueryProp(P_ARMOURS)-({0});
  if ( (i=sizeof(armours))>0 ) { 
    string aty;

    tmp=armours[random(i)];

    if ( objectp(tmp) )
      //Uebergabe des Mappings eh als Pointer/Referenz, daher billig
      tmp->TakeFlaw(dam_type,spell[EINFO_DEFEND]);

    // pro Ruestung ein Key an Platz reservieren
    spell[EINFO_DEFEND][DEFEND_ARMOURS]=m_allocate(i,2);
    foreach(object armour : armours) {
      if ( objectp(armour) ) {
        aty=armour->QueryProp(P_ARMOUR_TYPE);

        if ( member(spell[SP_REDUCE_ARMOUR],aty)
            && intp(res2=spell[SP_REDUCE_ARMOUR][aty]) && (res2>=0) )
          dam -= (res2*armour->QueryDefend(dam_type, spell, enemy))/100;
        else
          dam -= armour->QueryDefend(dam_type, spell, enemy);
        // Schaden NACH DefendFunc vermerken. 
        // Schutzwirkung VOR DefendFunc (DEF_ARMOUR_PROT) vermerkt
        // das QueryDefend() selber.
        spell[EINFO_DEFEND][DEFEND_ARMOURS][armour,DEF_ARMOUR_DAM]=dam;
        // akt. Schaden vermerken und Schutz der aktuellen Ruestung (vor
        // DefendFunc) wieder auf 0 setzen fuer den naechsten Durchlauf.
        spell[EINFO_DEFEND][CURRENT_DAM]=dam;
        spell[EINFO_DEFEND][DEFEND_CUR_ARMOUR_PROT]=0;
      }
    }
  }

  // Manche Gilden haben einen Verteidigungsskill. Der kommt jetzt zum
  // tragen. Der Skill kann die Schadenshoehe und -art beeinflussen.
  spell[EINFO_DEFEND]+=([DEFEND_GUILD:({})]);
  if ( mappingp(res=UseSkill(SK_MAGIC_DEFENSE,
                            ([ SI_ENEMY            : enemy,
                               SI_SKILLDAMAGE      : dam,
                               SI_SKILLDAMAGE_TYPE : dam_type,
                               SI_SPELL            : spell     ]))) )
  {
    dam=res[SI_SKILLDAMAGE];

    if ( pointerp(res[SI_SKILLDAMAGE_TYPE]) )
        dam_type=res[SI_SKILLDAMAGE_TYPE];

    spell[EINFO_DEFEND][CURRENT_DAMTYPE]=dam_type;
    spell[EINFO_DEFEND][CURRENT_DAM]=dam;
    spell[EINFO_DEFEND][DEFEND_GUILD]=({dam,dam_type});
  }

  // Evtl. interne Modifikationen
  InternalModifyDefend(&dam, &dam_type, &spell, &enemy);

  spell[EINFO_DEFEND][CURRENT_DAMTYPE]=dam_type;
  spell[EINFO_DEFEND][CURRENT_DAM]=dam;

  // Testen, ob irgendwas im Inventory des Lebewesen auf den Angriff
  // "reagiert"
  CheckSensitiveAttack(dam,dam_type,spell,enemy);

  if ( !objectp(enemy) )
    return 0;

  // Angriffszeit im Gegner setzen
  enemy->SetProp(P_LAST_COMBAT_TIME,time());

  // Die Resistenzen des Lebewesens (natuerliche und durch Ruestungen
  // gesetzte) beruecksichtigen
  dam = to_int(CheckResistance(dam_type)*dam);
  spell[EINFO_DEFEND][DEFEND_RESI]=dam;

  spell[EINFO_DEFEND][CURRENT_DAM]=dam;

  // Bei physikalischen Angriffen wird die natuerliche Panzerung und die
  // Geschicklichkeit des Lebewesens beruecksichtigt
  object stat = find_object("/d/erzmagier/zesstra/pacstat"); // TODO: remove
  if ( spell[SP_PHYSICAL_ATTACK] )
  {
    // Schutz bestimmen, Minimum 1, aber nur wenn P_BODY > 0
    // Um Rundungsverluste zu reduzieren, wird P_BODY anfangs mit 10000
    // skaliert. Beim runterskalieren wird aufgerundet (Addition von
    // 5000 vor Division).
    int body = (QueryProp(P_BODY)+QueryAttribute(A_DEX)) * 10000;
    res2 = ((body/4 + random(body*3/4 + 1) + 5000)/10000) || 1;
    if (stat)
      stat->bodystat(body, res2, (random(body)+1)/10000);

    // Reduzierbare Wirksamkeit des Bodies?
    if ( member(spell[SP_REDUCE_ARMOUR], P_BODY)
        && intp(res=spell[SP_REDUCE_ARMOUR][P_BODY]) && (res>=0) )
      res2=(res2*res)/100;

    dam-=res2;
  }
  spell[EINFO_DEFEND][DEFEND_BODY]=dam;
  spell[EINFO_DEFEND][CURRENT_DAM]=dam;

  // Ist ueberhaupt noch etwas vom Schaden uebrig geblieben?
  if ( dam<0 )
    dam = 0;
  spell[EINFO_DEFEND][CURRENT_DAM]=dam;

  // fuer die Statistik
  // TODO: entfernen nach Test-Uptime
  if (stat)
    stat->damagestat(spell[EINFO_DEFEND]);

  // Die Anzahl der abzuziehenden Lebenspunkte ist der durch 10 geteilte
  // Schadenswert
  dam = dam / 10;
  spell[EINFO_DEFEND][DEFEND_LOSTLP]=dam;

  // evtl. hat entweder der Aufrufer oder das Lebewesen selber eigene
  // Schadensmeldungen definiert. Die vom Aufrufer haben Prioritaet.
  mixed dam_msg = spell[SP_SHOW_DAMAGE];
  // Wenn != 0 (d.h. Treffermeldungen grundsaetzlich erwuenscht), aber kein
  // Array, hier im Living fragen.
  if (dam_msg && !pointerp(dam_msg))
    dam_msg = QueryProp(P_DAMAGE_MSG);

  // In den meisten Faellen soll auch eine Schadensmeldung ausgegeben
  // werden, die die Hoehe des Schadens (grob) anzeigt
  if (spell[SP_SHOW_DAMAGE] && !pointerp(dam_msg)) {
    myname=name(WEN);
    enname=enemy->Name(WER);
    if (enemy->QueryProp(P_PLURAL)) {
      switch (dam) {
      case 0:
        tell_object(enemy,sprintf("  Ihr verfehlt %s.\n",myname));
        tell_object(this_object(),sprintf("  %s verfehlen Dich.\n",enname));
        tell_room(environment(enemy)||environment(this_object()),
           sprintf("  %s verfehlen %s.\n",enname,myname),
                  ({ enemy, this_object() }));
        break;
      case 1:
        tell_object(enemy,sprintf("  Ihr kitzelt %s am Bauch.\n",myname));
        tell_object(this_object(),
          sprintf("  %s kitzeln Dich am Bauch.\n",enname));
        tell_room(environment(enemy)||environment(this_object()),
          sprintf("  %s kitzeln %s am Bauch.\n",enname,myname),
                  ({ enemy, this_object() }));
        break;
      case 2..3:
        tell_object(enemy,sprintf("  Ihr kratzt %s.\n",myname));
        tell_object(this_object(),sprintf("  %s kratzen Dich.\n",enname));
        tell_room(environment(enemy)||environment(this_object()),
          sprintf("  %s kratzen %s.\n",enname,myname),
                  ({ enemy, this_object() }));
        break;
      case 4..5:
        tell_object(enemy,sprintf("  Ihr trefft %s.\n",myname));
        tell_object(this_object(),sprintf("  %s treffen Dich.\n",enname));
        tell_room(environment(enemy)||environment(this_object()),
          sprintf("  %s treffen %s.\n",enname,myname),
                  ({ enemy, this_object() }));
        break;
      case 6..10:
        tell_object(enemy,sprintf("  Ihr trefft %s hart.\n",myname));
        tell_object(this_object(),sprintf("  %s treffen Dich hart.\n",enname));
        tell_room(environment(enemy)||environment(this_object()),
          sprintf("  %s treffen %s hart.\n",enname,myname),
                  ({ enemy, this_object() }));
        break;
      case 11..20:
        tell_object(enemy,sprintf("  Ihr trefft %s sehr hart.\n",myname));
        tell_object(this_object(),
          sprintf("  %s treffen Dich sehr hart.\n",enname));
        tell_room(environment(enemy)||environment(this_object()),
          sprintf("  %s treffen %s sehr hart.\n",enname,myname),
                  ({ enemy, this_object() }));
        break;
      case 21..30:
        tell_object(enemy,
          sprintf("  Ihr schlagt %s mit dem Krachen brechender Knochen.\n",
                  myname));
        tell_object(this_object(),
          sprintf("  %s schlagen Dich mit dem Krachen brechender Knochen.\n",
                  enname));
        tell_room(environment(enemy)||environment(this_object()),
          sprintf("  %s schlagen %s mit dem Krachen brechender Knochen.\n",
                  enname,myname), ({ enemy, this_object() }));
        break;
      case 31..50:
        tell_object(enemy,
          sprintf("  Ihr zerschmettert %s in kleine Stueckchen.\n",myname));
        tell_object(this_object(),
          sprintf("  %s zerschmettern Dich in kleine Stueckchen.\n",
                                          enname));
        tell_room(environment(enemy)||environment(this_object()),
          sprintf("  %s zerschmettern %s in kleine Stueckchen.\n",
                  enname,myname), ({ enemy, this_object() }));
        break;
      case 51..75:
        tell_object(enemy,sprintf("  Ihr schlagt %s zu Brei.\n",myname));
        tell_object(this_object(),
          sprintf("  %s schlagen Dich zu Brei.\n",enname));
        tell_room(environment(enemy)||environment(this_object()),
          sprintf("  %s schlagen %s zu Brei.\n",enname,myname),
                  ({ enemy, this_object() }));
        break;
      case 76..100:
        tell_object(enemy,sprintf("  Ihr pulverisiert %s.\n",myname));
        tell_object(this_object(),sprintf("  %s pulverisieren Dich.\n",enname));
        tell_room(environment(enemy)||environment(this_object()),
          sprintf("  %s pulverisieren %s.\n",enname,myname),
                  ({ enemy, this_object() }));
        break;
      case 101..150:
        tell_object(enemy,sprintf("  Ihr zerstaeubt %s.\n",myname));
        tell_object(this_object(),sprintf("  %s zerstaeuben Dich.\n",enname));
        tell_room(environment(enemy)||environment(this_object()),
          sprintf("  %s zerstaeuben %s.\n",enname,myname),
                  ({ enemy, this_object() }));
        break;
      case 151..200:
        tell_object(enemy,sprintf("  Ihr atomisiert %s.\n",myname));
        tell_object(this_object(),sprintf("  %s atomisieren Dich.\n",enname));
        tell_room(environment(enemy)||environment(this_object()),
          sprintf("  %s atomisieren %s.\n",enname,myname),
                  ({ enemy, this_object() }));
        break;
      default:
        tell_object(enemy,sprintf("  Ihr vernichtet %s.\n",myname));
        tell_object(this_object(),sprintf("  %s vernichten Dich.\n",enname));
        tell_room(environment(enemy)||environment(this_object()),
          sprintf("  %s vernichten %s.\n",enname,myname),
                  ({ enemy, this_object() }));
        break;
      }

    }
    else {
      switch (dam) {
      case 0:
        tell_object(enemy,sprintf("  Du verfehlst %s.\n",myname));
        tell_object(this_object(),sprintf("  %s verfehlt Dich.\n",enname));
        tell_room(environment(enemy)||environment(this_object()),
          sprintf("  %s verfehlt %s.\n",enname,myname),
                  ({ enemy, this_object() }));
        break;
      case 1:
        tell_object(enemy,sprintf("  Du kitzelst %s am Bauch.\n",myname));
        tell_object(this_object(),
          sprintf("  %s kitzelt Dich am Bauch.\n",enname));
        tell_room(environment(enemy)||environment(this_object()),
          sprintf("  %s kitzelt %s am Bauch.\n",enname,myname),
                  ({ enemy, this_object() }));
        break;
      case 2..3:
        tell_object(enemy,sprintf("  Du kratzt %s.\n",myname));
        tell_object(this_object(),sprintf("  %s kratzt Dich.\n",enname));
        tell_room(environment(enemy)||environment(this_object()),
          sprintf("  %s kratzt %s.\n",enname,myname),
                  ({ enemy, this_object() }));
        break;
      case 4..5:
        tell_object(enemy,sprintf("  Du triffst %s.\n",myname));
        tell_object(this_object(),sprintf("  %s trifft Dich.\n",enname));
        tell_room(environment(enemy)||environment(this_object()),
          sprintf("  %s trifft %s.\n",enname,myname),
                  ({ enemy, this_object() }));
        break;
      case 6..10:
        tell_object(enemy,sprintf("  Du triffst %s hart.\n",myname));
        tell_object(this_object(),sprintf("  %s trifft Dich hart.\n",enname));
        tell_room(environment(enemy)||environment(this_object()),
          sprintf("  %s trifft %s hart.\n",enname,myname),
            ({ enemy, this_object() }));
        break;
      case 11..20:
        tell_object(enemy,sprintf("  Du triffst %s sehr hart.\n",myname));
        tell_object(this_object(),
          sprintf("  %s trifft Dich sehr hart.\n",enname));
        tell_room(environment(enemy)||environment(this_object()),
          sprintf("  %s trifft %s sehr hart.\n",enname,myname),
                  ({ enemy, this_object() }));
        break;
      case 21..30:
        tell_object(enemy,
          sprintf("  Du schlaegst %s mit dem Krachen brechender Knochen.\n",
                  myname));
        tell_object(this_object(),
          sprintf("  %s schlaegt Dich mit dem Krachen brechender Knochen.\n",
                  enname));
        tell_room(environment(enemy)||environment(this_object()),
          sprintf("  %s schlaegt %s mit dem Krachen brechender Knochen.\n",
                  enname,myname), ({ enemy, this_object() }));
        break;
      case 31..50:
        tell_object(enemy,
          sprintf("  Du zerschmetterst %s in kleine Stueckchen.\n",myname));
        tell_object(this_object(),
          sprintf("  %s zerschmettert Dich in kleine Stueckchen.\n",enname));
        tell_room(environment(enemy)||environment(this_object()),
          sprintf("  %s zerschmettert %s in kleine Stueckchen.\n",
                  enname,myname), ({ enemy, this_object() }));
        break;
      case 51..75:
        tell_object(enemy,sprintf("  Du schlaegst %s zu Brei.\n",myname));
        tell_object(this_object(),
          sprintf("  %s schlaegt Dich zu Brei.\n",enname));
        tell_room(environment(enemy)||environment(this_object()),
          sprintf("  %s schlaegt %s zu Brei.\n",enname,myname),
                  ({ enemy, this_object() }));
        break;
      case 76..100:
        tell_object(enemy,sprintf("  Du pulverisierst %s.\n",myname));
        tell_object(this_object(),sprintf("  %s pulverisiert Dich.\n",enname));
        tell_room(environment(enemy)||environment(this_object()),
          sprintf("  %s pulverisiert %s.\n",enname,myname),
                  ({ enemy, this_object() }));
        break;
      case 101..150:
        tell_object(enemy,sprintf("  Du zerstaeubst %s.\n",myname));
        tell_object(this_object(),sprintf("  %s zerstaeubt Dich.\n",enname));
        tell_room(environment(enemy)||environment(this_object()),
          sprintf("  %s zerstaeubt %s.\n",enname,myname),
                  ({ enemy, this_object() }));
        break;
      case 151..200:
        tell_object(enemy,sprintf("  Du atomisierst %s.\n",myname));
        tell_object(this_object(),sprintf("  %s atomisiert Dich.\n",enname));
        tell_room(environment(enemy)||environment(this_object()),
          sprintf("  %s atomisiert %s.\n",enname,myname),
                  ({ enemy, this_object() }));
        break;
      default:
        tell_object(enemy,sprintf("  Du vernichtest %s.\n",myname));
        tell_object(this_object(),sprintf("  %s vernichtet Dich.\n",enname));
        tell_room(environment(enemy)||environment(this_object()),
          sprintf("  %s vernichtet %s.\n",enname,myname),
                  ({ enemy, this_object() }));
        break;
      }
    }
  }

  // Man kann auch selbst-definierte Schadensmeldungen ausgeben lassen
  else if( spell[SP_SHOW_DAMAGE] && pointerp(dam_msg) )
  {
    for( i=sizeof(dam_msg) ; --i >= 0 ; )
    {
      if ( dam>dam_msg[i][0] )
      {
        tell_object(ME,mess(dam_msg[i][1],ME,enemy));
        tell_object(enemy,mess(dam_msg[i][2],ME,enemy));
        say(mess(dam_msg[i][3],ME,enemy), enemy);
        break;
      }
    }
  }
  // else (!spell[SP_SHOW_DAMAGE]) keine Schadensmeldung.

  // Informationen ueber den letzten Angriff festhalten
  Set(P_LAST_DAMTYPES, dam_type);
  Set(P_LAST_DAMTIME,  time());
  Set(P_LAST_DAMAGE,   dam);

  // Bei Angriffen mit SP_NO_ENEMY-Flag kann man nicht sterben ...
  if ( spell[SP_NO_ENEMY] )
    reduce_hit_points(dam);
  // ... bei allen anderen natuerlich schon
  else
    do_damage(dam,enemy);

  // evtl. ist dies Objekt hier tot...
  if (!objectp(ME)) return dam;

  // Testen, ob man in die Fucht geschlagen wird
  CheckWimpyAndFlee();

  // Verursachten Schaden (in LP) zurueckgeben
  return dam;
}

public float CheckResistance(string *dam_type) {
    //funktion kriegt die schadensarten uebergeben, schaut sich dieses
    //sowie P_RESISTENCE_STRENGTH und P_RESITENCE_MODFIFIER an und berechnet
    //einen Faktor, mit dem die urspruengliche Schadensmenge multipliziert
    //wird, um den Schaden nach Beruecksichtigung der Resis/Anfaelligkeiten zu
    //erhalten. Rueckgabewert normalerweise (s.u.) >=0.
  mapping rstren, mod;
  float   faktor,n;
  int     i;

  mod = Query(P_RESISTANCE_MODIFIER);
  if ( mappingp(mod) )
    mod = mod["me"];

  if ( !mappingp(rstren=Query(P_RESISTANCE_STRENGTHS)) )
  {
    if (!mappingp(mod))
      return 1.0;
    else
      rstren = ([]);
  }

  if ( !mappingp(mod) )
    mod = ([]);

  if ( (i=sizeof(dam_type))<1 )
    return 1.0;

  n=to_float(i);
  faktor=0.0;

  //dies hier tut nicht mehr das gewuenschte, wenn in P_RESISTENCE_STRENGTHS
  //Faktoren <-1.0 angegeben werden. Rueckgabewerte <0 sind dann moeglich und
  //leider werden die Schadensarten ohne Resis nicht mehr richtig verwurstet. :-/
  foreach(string dt: dam_type) {
    faktor = faktor + (1.0 + to_float(rstren[dt]))
           * (1.0 + to_float(mod[dt]))-1.0;
  }
  return 1.0+(faktor/n);
}

public varargs mapping StopHuntingMode(int silent)
{ mapping save_enemy;

  save_enemy=enemies;
  if ( !silent )
    walk_mapping(enemies, #'StopHuntText); //#');

  enemies=m_allocate(0,1);
  last_attack_msg=0;

  return save_enemy;
}

public <object*|int*>* QueryEnemies()
{
  return ({m_indices(enemies),m_values(enemies)});
}

public mapping GetEnemies()
{
  return enemies;
}

public mapping SetEnemies(<object*|int*>* myenemies)
{
  enemies=mkmapping(myenemies[0],myenemies[1]);
  return enemies;
}

private string _kill_alias( string str )
{
    return "\\" + str;
}

public varargs void Flee( object oldenv, int force )
{ mixed   *exits, exit, dex;
  mapping tmp;
  int     i;
  object  env;

  if ( !environment() )
    return;

  // mit 'force' kann man die Checks umgehen, damit der Spieler auf jeden
  // Fall fluechtet ...
  if ( !force && ( oldenv && (oldenv != environment()) ||
                   query_once_interactive(ME) && (!EnemyPresent() ||
                   (QueryProp(P_HP) >= QueryProp(P_WIMPY))) ) )
    return;

  // ... aber Magier fluechten zu lassen ist nicht ganz so einfach ;-)
  if ( query_once_interactive(this_object()) && IS_LEARNING(this_object()) )
    return;

  // Geister brauchen nicht um ihr Leben zu fuerchten
  if ( QueryProp(P_GHOST) )
    return;

  tell_object( ME, "Die Angst ist staerker als Du ... "+
                   "Du willst nur noch weg hier.\n");

  if ( TeamFlee() ) // Erfolgreiche Flucht in hintere Kampfreihe?
    return;

  env = environment();
  tmp = environment()->QueryProp(P_EXITS);
  exits = m_indices(tmp);
  tmp = environment()->QueryProp(P_SPECIAL_EXITS);
  exits += m_indices(tmp);

  if ( query_once_interactive(ME) )
        exits = map( exits, #'_kill_alias/*'*/ );

  // die Fluchtrichtung von Magiern wird aus Sicherheitsgruenden
  // nicht ausgewertet
  if ( interactive(ME) && IS_SEER(ME) && !IS_LEARNER(ME)
      && (dex=QueryProp(P_WIMPY_DIRECTION)) )
  {
    i = 60 + 4 * (QueryProp(P_LEVEL) - 30) / 3;
    exits += ({dex}); // bevorzugte Fluchtrichtung mindestens einmal
  }

  if ( !sizeof(exits) )
  {
    tell_object( ME, "Du versuchst zu fliehen, schaffst es aber nicht.\n" );

    return;
  }

  while ( sizeof(exits) && (environment()==env) )
  {
    if ( dex                         // Vorzugsweise Fluchtrichtung?
        && (member(exits,dex) >= 0)  // moeglich?
        && (random(100) <= i))       // und Wahrscheinlichkeit gross genug?
      exit = dex;
    else
      exit = exits[random(sizeof(exits))];

    catch(command(exit);publish);
    exits -= ({exit});
  }

  if ( environment()==env )
    tell_object( ME, "Dein Fluchtversuch ist gescheitert.\n" );
}

public object EnemyPresent()
{
  foreach(object en: enemies) {
    if (environment()==environment(en))
      return en;
  }
  return 0;
}

public object InFight()
{
  return EnemyPresent();
}

public varargs int StopHuntID(string str, int silent) { 

  if ( !stringp(str) )
    return 0;

  int j;
  foreach(object en: enemies) {
    if (en->id(str)) {
      StopHuntFor(en,silent);
      j++;
    }
  }

  return j;
}

public int SpellDefend(object caster, mapping sinfo)
{ int    re;
  mixed  res;
  string *ind;

  re = UseSkill(SK_SPELL_DEFEND,([ SI_SKILLARG : sinfo ,
                                   SI_ENEMY    : caster ]) );

  if ( (res=QueryProp(P_MAGIC_RESISTANCE_OFFSET)) && mappingp(res)
      && pointerp(sinfo[SI_MAGIC_TYPE]))
  {
    ind = m_indices(res) & sinfo[SI_MAGIC_TYPE];

    if (pointerp(ind) && sizeof(ind) ) {
      foreach(string index : ind)
        re+=res[index];
    }
  }
  else if(res && intp(res))
    re+=res;

  if ( (re>3333) && query_once_interactive(this_object()) )
    re=3333; /* Maximal 33% Abwehrchance bei Spielern */
  return re;
}

// **** this is property-like

static int _set_attack_busy(mixed val)
{
  if ( ((to_int(val))>5) && previous_object(1)
      && query_once_interactive(previous_object(1)) )
    log_file("ATTACKBUSY",sprintf("%s %d Taeter: %O Opfer: %O\n",
             dtime(time()),to_int(val),previous_object(1),this_object()));

  attack_busy-=to_int(val*100.0);

  if ( attack_busy<-2000 )
    attack_busy=-2000;

  return attack_busy;
}

static int _query_attack_busy()
{
  if (IS_LEARNING(ME))
    return 0;

  return (attack_busy<100);
}

// **** local property methods
static int _set_wimpy(int i)
{
  if ( !intp(i) || (i>QueryProp(P_MAX_HP)) || (i<0) )
    return 0;

  // ggf. Statusreport ausgeben
  if (interactive(ME))
    status_report(DO_REPORT_WIMPY, i);

  return Set(P_WIMPY, i);
}

static string _set_wimpy_dir(string s) {
  // ggf. Statusreport ausgeben
  if (interactive(ME))
    status_report(DO_REPORT_WIMPY_DIR, s);
  return Set(P_WIMPY_DIRECTION, s, F_VALUE);
}

static mixed _set_hands(mixed h)
{ 
  if ( sizeof(h)==2 )
    h += ({ ({DT_BLUDGEON}) });
  if (!pointerp(h[2]))
    h[2] = ({h[2]});
  return Set(P_HANDS, h, F_VALUE);
}

//TODO normalisieren/korrigieren in updates_after_restore().
static mixed _query_hands()
{
  mixed *hands = Query(P_HANDS);
  if ( !hands )
    return Set(P_HANDS, ({ " mit blossen Haenden", 30, ({DT_BLUDGEON})}));
  else if ( sizeof(hands)<3 )
    return Set(P_HANDS, ({hands[0], hands[1], ({DT_BLUDGEON})}));
  else if ( !pointerp(hands[2]) )
    return Set(P_HANDS, ({hands[0], hands[1], ({ hands[2] })}));

  return Query(P_HANDS);
}

static int _query_total_wc()
{ mixed res;
  int   totwc;

  if ( objectp(res=QueryProp(P_WEAPON)) )
    totwc = res->QueryProp(P_WC);
  else if ( pointerp(res=QueryProp(P_HANDS)) && sizeof(res)>1
           && intp(res[1]) )
    totwc=res[1];
  else
    totwc=30;

  totwc = ((2*totwc)+(10*QueryAttribute(A_STR)))/3;

  return Set(P_TOTAL_WC, totwc, F_VALUE);
}

static int _query_total_ac() {

  int totac = 0;
  object *armours = QueryProp(P_ARMOURS);
  object parry = QueryProp(P_PARRY_WEAPON);

  if ( member(armours,0)>=0 ) {
    armours -= ({ 0 }); 
  }

  foreach(object armour: armours)
    totac += armour->QueryProp(P_AC);

  if ( objectp(parry) )
    totac += parry->QueryProp(P_AC);

  totac += (QueryProp(P_BODY)+QueryAttribute(A_DEX));

  return Set(P_TOTAL_AC, totac, F_VALUE);
}

static mapping _query_resistance_strengths() {

  UpdateResistanceStrengths();

  mapping rstren = copy(Query(P_RESISTANCE_STRENGTHS, F_VALUE));
  mapping mod = Query(P_RESISTANCE_MODIFIER, F_VALUE);

  if ( !mappingp(rstren) )
    rstren = ([]);

  if ( !mappingp(mod) || !mappingp(mod=mod["me"]) || !sizeof(mod) )
    return rstren;

  foreach(string modkey, float modvalue : mod)
    rstren[modkey] = ((1.0+rstren[modkey])*(1.0+modvalue))-1.0;

  return rstren;
}

static int _set_disable_attack(int val)
{
  if (val<-100000)
           {
                  log_file("PARALYSE_TOO_LOW",
                          sprintf("Wert zu klein: %s, Wert: %d, "
                                  "Aufrufer: %O, Opfer: %O",
                                  ctime(time()),
                                          val,previous_object(1),
                                          this_object()));
           }
  if ( val>30 )
    val=30;
  if ( (val>=20) && previous_object(1)!=ME && query_once_interactive(ME) )
    log_file("PARALYSE",sprintf("%s %d Taeter: %O Opfer: %O\n",
                                ctime(time())[4..15],
                                val,previous_object(1),this_object()));
 
  if (time()<QueryProp(P_NEXT_DISABLE_ATTACK))
  {
    // gueltige Zeitsperre existiert.
    // Erhoehen von P_DISABLE_ATTACK geht dann nicht. (Ja, auch nicht erhoehen
    // eines negativen P_DISABLE_ATTACK)
    if (val >= QueryProp(P_DISABLE_ATTACK))
      return DISABLE_TOO_EARLY;
    // val ist kleiner als aktuelles P_DISABLE_ATTACK - das ist erlaubt, ABER es
    // darf die bestehende Zeitsperre nicht verringern, daher wird sie nicht
    // geaendert.
    return Set(P_DISABLE_ATTACK,val,F_VALUE);
  }
  // es existiert keine gueltige Zeitsperre - dann wird sie nun gesetzt.
  // (Sollte val < 0 sein, wird eine Zeitsperre in der Vergangenheit gesetzt,
  // die schon abgelaufen ist. Das ist ueberfluessig, schadet aber auch
  // nicht.)
  SetProp(P_NEXT_DISABLE_ATTACK,time()+val*2*__HEART_BEAT_INTERVAL__);
  return Set(P_DISABLE_ATTACK,val);
}

// Neue Verwaltung der Haende:

// P_HANDS_USED_BY enhaelt ein Array mit allen Objekten, die Haende
// belegen, jedes kommt so oft vor wie Haende belegt werden.

static mixed *_query_hands_used_by()
{
  return ((Query(P_HANDS_USED_BY, F_VALUE) || ({}) ) - ({0}));
}

static int _query_used_hands()
{
  return sizeof(QueryProp(P_HANDS_USED_BY));
}

static int _query_free_hands()
{
  return (QueryProp(P_MAX_HANDS)-QueryProp(P_USED_HANDS));
}

public varargs int UseHands(object ob, int num)
{ mixed *h;

  if ( !ob && !(ob=previous_object(1)) )
    return 0;

  if ( (num<=0) && ((num=ob->QueryProp(P_HANDS))<=0) )
    return 0;

  h=QueryProp(P_HANDS_USED_BY)-({ob});

  if ( (sizeof(h)+num)>QueryProp(P_MAX_HANDS) )
    return 0;

  foreach(int i: num)
    h+=({ob});

  SetProp(P_HANDS_USED_BY,h);

  return 1;
}

public varargs int FreeHands(object ob)
{
  if ( !ob && !(ob=previous_object(1)) )
    return 0;

  SetProp(P_HANDS_USED_BY,QueryProp(P_HANDS_USED_BY)-({ob}));

  return 1;
}

// Kompatiblitaetsfunktionen:

static int _set_used_hands(int new_num)
{ int    old_num, dif;
  object ob;

  old_num=QueryProp(P_USED_HANDS);

  if ( !objectp(ob=previous_object(1)) )
    return old_num;

  // Meldung ins Debuglog schreiben. Aufrufer sollte gefixt werden.
  debug_message(sprintf("P_USED_HANDS in %O wird gesetzt durch %O\n",
        this_object(), ob), DMSG_LOGFILE|DMSG_STAMP);

  if ( !(dif=new_num-old_num) )
    return new_num;

  if ( dif>0 )
    UseHands(ob,dif);
  else
    FreeHands(ob);

  return QueryProp(P_USED_HANDS);
}

// Funktionen fuer Begruessungsschlag / Nackenschlag:

public int CheckEnemy(object ob)
{
  return (living(ob) && IsEnemy(ob));
}

public varargs void ExecuteMissingAttacks(object *remove_attackers)
{
  if ( !pointerp(missing_attacks) )
    missing_attacks=({});

  if ( pointerp(remove_attackers) )
    missing_attacks-=remove_attackers;

  foreach(object ob : missing_attacks) {
    if ( objectp(ob) && (environment(ob)==environment()) )
      ob->Attack2(ME);
  }
  missing_attacks=({});
}

public void InitAttack()
{ object ob,next;
  closure cb;

  if ( !living(ME) )
    return;

  ExecuteMissingAttacks();
  //EMA kann das Living zerstoeren oder toeten...
  if (!living(ME) || QueryProp(P_GHOST)) return;

  if ( objectp(ob=IsTeamMove()) )
    cb=symbol_function("InitAttack_Callback",ob);
  else
    cb=0;

  for ( ob=first_inventory(environment()) ; objectp(ob) ; ob=next)
  {
    next=next_inventory(ob);

    if ( !living(ob) )
      continue;

    if (ob->IsEnemy(ME))
    {
      // Das ist nicht so sinnlos wie es aussieht. a) werden die Hunttimes
      // aktualisiert und b) werden Teammitglieder von mir bei diesem
      // InsertEnemy() ggf. erfasst.
      ob->InsertEnemy(ME);

      if ( closurep(cb) && funcall(cb,ob) ) // Wird ganzes Team gemoved?
        missing_attacks += ({ ob }); // Dann erstmal warten bis alle da sind
      else
        ob->Attack2(ME);

    }
    else if ( IsEnemy(ob) )
    {
      // Das ist nicht so sinnlos wie es aussieht. a) werden die Hunttimes
      // aktualisiert und b) werden Teammitglieder von ob bei diesem 
      // InsertEnemy() ggf. erfasst.
      InsertEnemy(ob);
      Attack2(ob);
    }
    //Attack2 kann dieses Objekt zerstoeren oder toeten. Wenn ja: abbruch
    if ( !living(ME) || QueryProp(P_GHOST)) break;
  }
}

public void ExitAttack()
{
  if ( !living(ME) )
    return;

  // Noch nachzuholende Begruessungsschlaege:
  ExecuteMissingAttacks();
}

public object|object*|mapping QueryArmourByType(string type)
{
  // Rueckgabewert:
  // DIE Ruestung vom Typ <type>, falls <type> nicht AT_MISC,
  // Array aller AT_MISC-Ruestungen falls <type> AT_MISC (auch leer),
  // Mapping mit allen oben genannten Infos, falls <type> Null

  object *armours;
  string typ2;

  // Wenn Cache vorhanden, dann Cache liefern.
  if (mappingp(QABTCache)) {
    if (type == AT_MISC)
      return QABTCache[AT_MISC] - ({0});
    else if (type)
      return QABTCache[type];
    else
      return copy(QABTCache);
  }

  if ( !pointerp(armours=QueryProp(P_ARMOURS)) )
    armours=({});

  // Cache erzeugen
  QABTCache = ([ AT_MISC: ({}) ]);
  foreach(object ob: armours - ({0}) ) {
    if ( !stringp(typ2=ob->QueryProp(P_ARMOUR_TYPE)) )
      continue;
    if ( typ2==AT_MISC )
      QABTCache[AT_MISC] += ({ob});
    else
      QABTCache[typ2] = ob;
  }
  // Und gewuenschtes Datum liefern.
  if (type)
    return QABTCache[type];
  else
    return copy(QABTCache);
}

//TODO: langfristig waers besser, wenn hier nicht jeder per SetProp() drauf
//los schreiben wuerde.
static object *_set_armours(object *armours) {
  if (pointerp(armours)) {
    // Cache wegwerfen
    QABTCache = 0;
    // armours ggf. unifizieren. Ausserdem Kopie reinschreiben.
    return Set(P_ARMOURS, m_indices(mkmapping(armours)), F_VALUE);
  }
  return QueryProp(P_ARMOURS);
}

/** Reduziert die Befriedezaehler pro Reset im Durchschnitt um 2.5.
  Berechnet ganzzahlige durchschnittliche Resets seit dem letzten Expire und
  erhoeht die letzte Expirezeit um Resets*__RESET_TIME__ (Standard Intervall
  fuer Reset ist momentan 3600s, im Durchschnitt kommen dann 2700 zwischen 2
  Resets bei raus). So wird der unganzzahlige Rest beim naechsten Aufruf
  beruecksichtigt.
  Diese Variante des Expires wurde gewaehlt, um zu vermeiden, combat.c einen
  reset() zu geben und in jedem Reset in jedem Lebewesen ein Expire machen zu
  muessen, auch wenn nur in sehr wenigen Lebewesen was gemacht werden muss.
  @param[in,out] ph Mapping aus P_PEACE_HISTORY. Wird direkt aktualisiert.
  @attention Muss ein gueltiges P_PEACE_HISTORY uebergeben bekommen, anderem
  Datentyp inkl. 0 wird die Funktion buggen.
  */
private void _decay_peace_history(mixed ph) {
  // Ganzzahlige resets seit letztem Expire ermitteln. (Durchschnitt)
  int resets = (time() - ph[0]) / (__RESET_TIME__*75/100);
  // auf Zeitstempel draufrechnen, damit der unganzzahlige Rest nicht
  // verlorengeht, der wird beim naechsten Expire dann beruecksichtigt.
  ph[0] += resets * (__RESET_TIME__ * 75 / 100);
  // pro Reset werden im Durchschnitt 2.5 Versuche abgezogen. (Hier werden
  // beim Expire mal 2 und mal 3 Versuche pro Reset gerechnet. Aber falls hier
  // viele Resets vergangen sind, ist es vermutlich eh egal, weil die Counter
  // auf 0 fallen.)
  int expire = resets * (random(2) + 2);
  // ueber alle Gilden
  mapping tmp=ph[1];
  foreach(string key, int count: &tmp ) {
    count-=expire;
    if (count < 0) count = 0;
  }
}

/** Pacify() dient zur Bestimmung, ob sich ein Lebewesen gerade 
 * befrieden lassen will.
 * Berechnet eine Wahrscheinlichkeit nach unten stehender Formel, welche die
 * Intelligenz dieses Lebenwesens, die Intelligenz des Casters und die
 * bisherige Anzahl erfolgreicher Befriedungsversuche dieser Gilde eingeht.
 * Anschliessend wird aus der Wahrscheinlichkeit und random() bestimmt, ob
 * dieser Versuch erfolgreich ist.
Formel: w = (INT_CASTER + 10 - ANZ*4) / (INT_ME + 10)
INT_CASTER: Caster-Intelligenz, INT_ME: Intelligenz dieses Livings
ANZ: Anzahl erfolgreicher Befriedungsversuche

Annahme: INT_CASTER === 22, alle Wahrscheinlichkeiten * 100

INT_ME   Erfolgswahrscheinlichkeiten je nach Anzahl erfolgreicher Versuche
              1       2       3       4       5       6       7       8
      0     280     240     200     160     120      80      40       0
      2  233,33     200  166,67  133,33     100   66,67   33,33       0
      4     200  171,43  142,86  114,29   85,71   57,14   28,57       0
      6     175     150     125     100      75      50      25       0
      8  155,56  133,33  111,11   88,89   66,67   44,44   22,22       0
     10     140     120     100      80      60      40      20       0
     12  127,27  109,09   90,91   72,73   54,55   36,36   18,18       0
     14  116,67     100   83,33   66,67      50   33,33   16,67       0
     16  107,69   92,31   76,92   61,54   46,15   30,77   15,38       0
     18     100   85,71   71,43   57,14   42,86   28,57   14,29       0
     20   93,33      80   66,67   53,33      40   26,67   13,33       0
     22    87,5      75    62,5      50    37,5      25    12,5       0
     24   82,35   70,59   58,82   47,06   35,29   23,53   11,76       0
     26   77,78   66,67   55,56   44,44   33,33   22,22   11,11       0
     28   73,68   63,16   52,63   42,11   31,58   21,05   10,53       0
     30      70      60      50      40      30      20      10       0
     32   66,67   57,14   47,62    38,1   28,57   19,05    9,52       0
     34   63,64   54,55   45,45   36,36   27,27   18,18    9,09       0
     35   62,22   53,33   44,44   35,56   26,67   17,78    8,89       0
     36   60,87   52,17   43,48   34,78   26,09   17,39     8,7       0
     38   58,33      50   41,67   33,33      25   16,67    8,33       0
     40      56      48      40      32      24      16       8       0
     42   53,85   46,15   38,46   30,77   23,08   15,38    7,69       0
     44   51,85   44,44   37,04   29,63   22,22   14,81    7,41       0
     46      50   42,86   35,71   28,57   21,43   14,29    7,14       0
     48   48,28   41,38   34,48   27,59   20,69   13,79     6,9       0
     50   46,67      40   33,33   26,67      20   13,33    6,67       0
     52   45,16   38,71   32,26   25,81   19,35    12,9    6,45       0
     54   43,75    37,5   31,25      25   18,75    12,5    6,25       0
     56   42,42   36,36    30,3   24,24   18,18   12,12    6,06       0
     58   41,18   35,29   29,41   23,53   17,65   11,76    5,88       0
     60      40   34,29   28,57   22,86   17,14   11,43    5,71       0
     62   38,89   33,33   27,78   22,22   16,67   11,11    5,56       0
     64   37,84   32,43   27,03   21,62   16,22   10,81    5,41       0
     66   36,84   31,58   26,32   21,05   15,79   10,53    5,26       0
     68    35,9   30,77   25,64   20,51   15,38   10,26    5,13       0
     70      35      30      25      20      15      10       5       0
     72   34,15   29,27   24,39   19,51   14,63    9,76    4,88       0
     74   33,33   28,57   23,81   19,05   14,29    9,52    4,76       0
     76   32,56   27,91   23,26    18,6   13,95     9,3    4,65       0
     78   31,82   27,27   22,73   18,18   13,64    9,09    4,55       0
     80   31,11   26,67   22,22   17,78   13,33    8,89    4,44       0
     82   30,43   26,09   21,74   17,39   13,04     8,7    4,35       0
     84   29,79   25,53   21,28   17,02   12,77    8,51    4,26       0
     86   29,17      25   20,83   16,67    12,5    8,33    4,17       0
     88   28,57   24,49   20,41   16,33   12,24    8,16    4,08       0
     90      28      24      20      16      12       8       4       0
     92   27,45   23,53   19,61   15,69   11,76    7,84    3,92       0
     94   26,92   23,08   19,23   15,38   11,54    7,69    3,85       0
     96   26,42   22,64   18,87   15,09   11,32    7,55    3,77       0
     98   25,93   22,22   18,52   14,81   11,11    7,41     3,7       0
    100   25,45   21,82   18,18   14,55   10,91    7,27    3,64       0
 * @return 1, falls Befrieden erlaubt ist, 0 sonst.
 * @param[in] caster Derjenige, der den Spruch ausfuehrt.
 * @attention Wenn acify() 1 zurueckliefert, zaehlt dies als
 * erfolgreicher Befriedungsversuch und in diesem Lebewesen wurde
 * StopHuntingMode(1) aufgerufen.
*/
public int Pacify(object caster) {
  
  // wenn das Viech keine Gegner hat, dann witzlos. ;-) Ohne Caster gehts auch
  // direkt raus.
  if (!mappingp(enemies) || !sizeof(enemies)
      || !objectp(caster)) {
    return 0;
  }

  // Wenn P_ACCEPT_PEACE gesetzt ist, altes Verhalten wiederspiegeln
  // -> der NPC ist einfach immer befriedbar. Gleiches gilt fuer den Caster
  // selber, der wird sich ja nicht gegen das eigene Befriede wehren. Und auch
  // im team wehrt man sich nicht gegen das Befriede eines Teamkollegen
  if (QueryProp(P_ACCEPT_PEACE)==1 || caster==ME
      || member(TeamMembers(), caster) > -1) {
    StopHuntingMode(1); // Caster/Gilde sollte eigene Meldung ausgeben
    return 1;
  }

  string gilde = caster->QueryProp(P_GUILD) || "ANY";

  // ggf. P_PEACE_HISTORY initialisieren
  <int|mapping>* ph = QueryProp(P_PEACE_HISTORY);
  if (!pointerp(ph))
    SetProp(P_PEACE_HISTORY, ph=({time(), ([]) }) );
  
  // ggf. die Zaehler reduzieren.
  if ( ph[0] + __RESET_TIME__ * 75/100 < time()) {
    _decay_peace_history(&ph);
  }
  
  float w = (caster->QueryAttribute(A_INT) + 10 - ph[1][gilde] * 4.0) / 
             (QueryAttribute(A_INT) + 10);
  // auf [0,1] begrenzen.
  if (w<0) w=0.0;
  else if (w>1) w=1.0;
  // w * n ist eine Zahl zwischen 0 und n, wenn w * n > random(n)+1,
  // darf befriedet werden. Da das Random fuer grosse Zahlen
  // besser verteilt ist, nehm ich n = __INT_MAX__ und vernachlaessige
  // ausserdem die +1 beim random().
  if (ceil(w * __INT_MAX__) > random(__INT_MAX__) ) {
    ph[1][gilde]++;
    StopHuntingMode(1);
    return 1;
  }
  // ein SetProp(P_PEACE_HISTORY) kann entfallen, da das Mapping direkt
  // geaendert wird. Sollte die Prop allerdings mal ne Querymethode haben,
  // welche eine Kopie davon liefert, muss das hier geaendert oder die
  // Prop per Query() abgefragt werden.
  return 0;
}

