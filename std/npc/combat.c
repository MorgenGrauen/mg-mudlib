// MorgenGrauen MUDlib
//
// npc/combat.c -- NPC-spezifische Kampffunktionen
//
// $Id: combat.c 9488 2016-02-19 21:24:04Z Arathorn $
#pragma strong_types
#pragma save_types
#pragma range_check
#pragma no_clone
#pragma pedantic

inherit "std/living/combat";

#include <combat.h>
#include <language.h>
#include <properties.h>
#include <wizlevels.h>
#include <health.h>
#include <new_skills.h>

#define NEED_PROTOTYPES 1
#include <living/life.h>
#undef NEED_PROTOTYPES

#define HB_CHECK 7
#define ME this_object()
#define STATMASTER "/p/service/rochus/guildstat/master"

nosave int heartbeat, beatcount;

private void catch_up_hbs();

protected void create() {
  ::create();
  beatcount=1;
  heartbeat=1;
}

protected void create_super() {
  set_next_reset(-1);
}

// aktuelles Lebewesen, fuer das dieser NPC gerade taetig ist. Default:
// Spieler, bei dem er als Helfer-NPC registriert ist.
public object QueryUser()
{
  mixed helperdata = QueryProp(P_HELPER_NPC);
  if (pointerp(helperdata) && objectp(helperdata[0]))
    return helperdata[0];
  return 0;
}

// ggf. Feinde expiren. Soll das Problem verringern, dass Spieler nach Tagen
// erst die Meldung kriegen, dass der NPC sie nicht mehr jagt, wenn der HB
// reaktivert wird.
void reset() {
  // ggf. die abgeschalteten HBs nachholen.
  if (!heartbeat)
    catch_up_hbs();
  // ggf. P_ENEMY_DAMAGE zuruecksetzen
  ResetEnemyDamage();
}

static void _set_max_hp(int i) {
  Set(P_MAX_HP,i);
  SetProp(P_HP,i);
}

static void _set_max_sp(int i) {
  Set(P_MAX_SP,i);
  SetProp(P_SP,i);
}


// Check-Funktion fuer die P_NO_ATTACK-QueryMethod
static mixed _check_immortality()
{
    int t;

    if ( !(t = Query("time_to_mortality")) || time() > t ){
        // Zeit ist abgelaufen - wieder angreifbar machen
        Set( P_NO_ATTACK, 0, F_QUERY_METHOD );
        heartbeat = 1;
        beatcount = 1;
        set_heart_beat(1);

        return 0;
    }

    // der NPC ist noch unangreifbar
    return break_string( capitalize(name( WER, 1 )) + " versteckt sich hinter "
                         "einem Fehler im Raum-Zeit-Gefuege und entgeht so "
                         "voruebergehend allen Angriffen.", 78 );
}


// wenn der HeartBeat buggt, wird diese Funktion vom Master aufgerufen
public void make_immortal()
{
    // fuer 5 Minuten unangreifbar machen
    Set( P_NO_ATTACK, #'_check_immortality, F_QUERY_METHOD );

    Set( "time_to_mortality", time() + 300, F_VALUE );

    // damit die Spieler keinen Vorteil durch den Bug haben, heilen
    heal_self(10000);

    // da nun der Heartbeat abgeschaltet ist und normalerweise erst
    // reaktiviert wird, sobald jemand nach 5min P_NO_ATTACK abfragt, muss man
    // aber auf Viecher achten, die immer nen Heartbeat haben wollen. In dem
    // fall per call_out selber die Prop abfragen.
    if (QueryProp(P_HB))
      call_out(#'QueryProp, 301, P_NO_ATTACK);
}


// Damit NPCs gegeneinander weiterkaempfen, auch wenn kein Spieler
// in der Naehe ist:
static int _query_hb()
{
    // TODO: return InFight() || Query(P_HB, F_VALUE), sobald InFight()
    // geaendert.
    return (InFight() || Query(P_HB,F_VALUE)) ? 1 : 0;
}


#define SPELL_TOTALRATE 0
#define SPELL_DAMAGE 1
#define SPELL_TEXT_FOR_ENEMY 2
#define SPELL_TEXT_FOR_OTHERS 3
#define SPELL_DAMTYPE 4
#define SPELL_FUNC 5
#define SPELL_ARG 6

varargs int AddSpell(int rate, int damage,
  string|<int|string>* TextForEnemy, string|<int|string>* TextForOthers,
  string|string* dam_type, string|closure func, int|mapping spellarg)
{
  mixed *spells;
  int total_rates;
  closure cl;

  if(rate<=0 || damage<0) return 0;

 // Tatsaechlich ist es immer ein nicht-physischer Angriff, wenn spellarg ein
  // int ist, weil wenn spellarg==0 ist der Default nicht-physischer Angriff
  // und bei spellarg!=0 auch. Nur mit einem mapping kann man einen phys.
  // Angriff erzeugen.
  if (intp(spellarg))
    spellarg = ([SP_PHYSICAL_ATTACK: 0]);

  if(stringp(dam_type))
    dam_type=({dam_type});
  else if(!pointerp(dam_type))
  {
    if(spellarg[SP_PHYSICAL_ATTACK])
      dam_type=({DT_BLUDGEON});
    else
      dam_type=({DT_MAGIC});
  }

  foreach(string s : dam_type)
  {
    if(!VALID_DAMAGE_TYPE(s))
    {
      catch(raise_error(
        "AddSpell(): Ungueltiger Schadenstyp: "+s);
        publish);
    }
  }

  // Falls func ein String ist eine Closure erstellen und diese speichern.
  if(stringp(func) && sizeof(func))
  {
    cl=symbol_function(func,this_object());
    if(!closurep(cl))
    {
      catch(raise_error(
        "AddSpell(): Es konnte keine Closure fuer "+func+" erstellt werden.");
        publish);
    }
  }
  else
  {
    // Leerstrings durch 0 ersetzen.
    if(stringp(func))
    {
      cl=0;
    }
    else
    {
      cl=func;
    }
  }
  
  if(damage==0 && !closurep(cl))
  {
    catch(raise_error(
      "AddSpell(): Bei damage=0 muss eine Funktion eingetragen werden.");
      publish);
    return 0;
  }

  if(!sizeof(TextForEnemy) ||
    (pointerp(TextForEnemy) && !sizeof(TextForEnemy[0])))
  {
    TextForEnemy=0;
  }
  else if(stringp(TextForEnemy))
  {
    TextForEnemy=({TextForEnemy,MT_LOOK});
  }
  else if(pointerp(TextForEnemy) && 
    (!stringp(TextForEnemy[0]) || !intp(TextForEnemy[1])))
  {
    raise_error(
      "AddSpell(): Falsche Datentypen fuer TextForEnemy");
  }
  
  if(!sizeof(TextForOthers) || 
    (pointerp(TextForOthers) && !sizeof(TextForOthers[0])))
  {
    TextForOthers=0;
  }
  else if(stringp(TextForOthers))
  {
    TextForOthers=({TextForOthers,MT_LOOK});
  }
  else if(pointerp(TextForOthers) && 
    (!stringp(TextForOthers[0]) || !intp(TextForOthers[1])))
  {
    raise_error(
      "AddSpell(): Falsche Datentypen fuer TextForOthers");
  }
  

  // Falls vorhanden, alte Syntax auf die von replace_personal() anpassen,
  // die im heart_beat() beim Ausgeben der Meldung verwendet wird.
  if ( strstr(TextForOthers[0], "@", 0) != -1 )
  {
    // Zeichen nach @WER & Co in runde Klammern einschliessen, damit es als
    // Sub-Pattern im Ausgabestring wiederholt werden kann. Ansonsten wuerde
    // es mit ersetzt.
    TextForOthers[0] = regreplace(TextForOthers[0], "@WER([^1-9QU])", "@WER1\\1", 1);
    TextForOthers[0] = regreplace(TextForOthers[0], "@WESSEN([^1-9QU])",
                               "@WESSEN1\\1", 1);
    TextForOthers[0] = regreplace(TextForOthers[0], "@WEM([^1-9QU])", "@WEM1\\1", 1);
    TextForOthers[0] = regreplace(TextForOthers[0], "@WEN([^1-9QU])", "@WEN1\\1", 1);
  }
  total_rates=Query("npc:total_rates")+rate;
  spells=Query(P_SPELLS);
  if (!pointerp(spells))
    spells=({});
  spells+=({({total_rates, damage, TextForEnemy, TextForOthers,
              dam_type, cl, spellarg})});
  Set(P_SPELLS,spells);
  Set("npc:total_rates",total_rates);
  return 1;
}

int AutoAttack(object ob) {
  mixed m;

  if (!query_once_interactive(ob))
    return 0;
  if (mappingp(m=QueryProp(P_AGGRESSIVE))) {
    mixed *ind,x,z;
    float f;
    int i,n;

    ind=m_indices(m)-({0});n=0;f=0.0;
    for (i=sizeof(ind)-1;i>=0;i--) {
      x=ind[i];
      if ((z=m[x][ob->QueryProp(x)]) || (z=m[x][0])) {
  f=f+(float)z;
  n++;
      }
    }
    if (n)
      m=f/((float)n);
    else
      m=m[0];
  }
  if (((int)(100*(m+ob->QueryProp(P_AGGRESSIVE))))<=random(100))
    return 0;
  if  (IS_LEARNER(ob)
       && (ob->QueryProp(P_INVIS)
     || ob->QueryProp(P_WANTS_TO_LEARN)))
    return 0;
  return 1;
}

void SpellAttack(object enemy) {
}

#if 0
TJ(string s) {
  object o;
  if (o=find_player("jof"))
    tell_object(o,sprintf("%O: %s\n",this_object(),s));
}
#else
#define TJ(x)
#endif

protected void heart_beat() {
  int r,i;
  mixed env,*spells, sinfo;
  object enemy;

  if ( --beatcount < 0 )
      beatcount = 0;
  
  if (!beatcount && !Query(P_HB)) {
    if (!environment()) {
      set_heart_beat(0);
      heartbeat = 0;
      if( clonep(this_object()) ) remove();
      return;
    }
    if (!QueryProp(P_POISON)) {
      // Spieler anwesend?
      env = filter(all_inventory(environment()), #'query_once_interactive);
      if (!sizeof(env)) {
	// Nein, HBs abschalten.
	set_heart_beat(0);
	heartbeat=0;
	TJ("OFF\n");
	beatcount=HB_CHECK;
	Set("npc:beat_off_num",absolute_hb_count());
	return;
      }
    }
  }
  ::heart_beat();
  if (!ME)
      return;
  enemy=SelectEnemy();
  if (QueryProp(P_AGGRESSIVE)
      && (!enemy || environment()!=environment(enemy))
      && !beatcount) {
    beatcount=HB_CHECK;
    env=filter(all_inventory(environment()),#'AutoAttack);
    if (!sizeof(env))
       return;
    i=random(sizeof(env));
    Kill(env[i]);
  }
  else if (!beatcount)
    beatcount=HB_CHECK;
  if (!objectp(enemy) ||QueryProp(P_DISABLE_ATTACK)>0)
    return;
  SpellAttack(enemy);

  if (!pointerp(spells=Query(P_SPELLS))
      || !sizeof(spells)
      || !objectp(enemy=SelectEnemy())
      || environment(enemy)!=environment()
      || (QueryProp(P_DISABLE_ATTACK)>0)
      || random(100)>Query(P_SPELLRATE))
    return;
  r=random(Query("npc:total_rates"));
  for (i=sizeof(spells)-1;(i>0 && spells[i-1][SPELL_TOTALRATE]>r);i--)
    ;
  <int|string>* akt_spell_mess=spells[i][SPELL_TEXT_FOR_ENEMY];
  // Nur, wenn ueberhaupt eine Meldung gesetzt wurde, muss diese verarbeitet
  // werden.
  if(pointerp(akt_spell_mess))
  {
    enemy->ReceiveMsg(
      akt_spell_mess[0],
      akt_spell_mess[1],
      MA_SPELL);
  }
  akt_spell_mess=spells[i][SPELL_TEXT_FOR_OTHERS];
  if(pointerp(akt_spell_mess))
  {
    akt_spell_mess[0]=replace_personal(akt_spell_mess[0], ({enemy}), 1);
    send_room(environment(),
      akt_spell_mess[0],
      akt_spell_mess[1],
      MA_SPELL,
      0,
      ({enemy,this_object()}),
      this_object());
  }
  sinfo = deep_copy(spells[i][SPELL_ARG]);
  if(!mappingp(sinfo))
    sinfo=([ SI_MAGIC_TYPE :({ MT_ANGRIFF }) ]);
  else if(!sinfo[SI_MAGIC_TYPE])
    sinfo[ SI_MAGIC_TYPE]=({ MT_ANGRIFF });
  if(!sinfo[SP_PHYSICAL_ATTACK] &&
     (enemy->SpellDefend(this_object(),sinfo) >
      random(MAX_ABILITY+QueryProp(P_LEVEL)*50))){
    enemy->ReceiveMsg(
      "Du wehrst den Spruch ab.",
      MT_NOTIFICATION,
      MA_SPELL);
    send_room(environment(),
      enemy->Name(WER,1)+" wehrt den Spruch ab.",
      MT_LOOK,
      MA_SPELL,
      0,
      ({ enemy, this_object()}));
    return ;
  }
  int damage;
  // Bei 0 sparen wir uns das Defend() und rufen nur die Funktion auf.
  if(spells[i][SPELL_DAMAGE])
  {
    damage=random(spells[i][SPELL_DAMAGE])+1;
    enemy->Defend(damage, spells[i][SPELL_DAMTYPE],
      spells[i][SPELL_ARG],
      this_object());
  }

  // Falls der Gegner (oder wir) im Defend stirbt, hier abbrechen
  if ( !objectp(ME) || !objectp(enemy) 
      || enemy->QueryProp(P_GHOST) ) return;
  
  closure cl = spells[i][SPELL_FUNC];
  if (cl)
  {
    if (closurep(cl))
      catch(funcall(cl, enemy, damage, spells[i][SPELL_DAMTYPE]);publish);
    else
      raise_error(sprintf("P_SPELL defekt: SPELL_FUNC in Spell %i ist keine "
                          "Closure.\n", i));
  }
}

// Heartbeats nachholen.
private void catch_up_hbs() {
  // gibt es HBs zum nachholen?
  int beat_off_num = Query("npc:beat_off_num");
  if (!beat_off_num)
    return; // nein.
  // wieviele HBs nachholen?
  beat_off_num = absolute_hb_count() - beat_off_num;
  
  if (beat_off_num>0) {
    // Nicht ausgefuehrtes HEILEN nachholen
    int rlock=QueryProp(P_NO_REGENERATION);
    int hp=QueryProp(P_HP);
    int sp=QueryProp(P_SP);
    int alc=QueryProp(P_ALCOHOL);
    if (!(rlock & NO_REG_HP)) {
      hp+=beat_off_num/HEAL_DELAY+alc/ALCOHOL_DELAY;
      SetProp(P_HP,hp);
    }
    if (!(rlock & NO_REG_SP)) {
      sp+=beat_off_num/HEAL_DELAY+alc/ALCOHOL_DELAY;
      SetProp(P_SP,sp);
    }
    alc-=beat_off_num/ALCOHOL_DELAY;
    if ( alc < 0 )
      alc = 0;
    SetProp(P_ALCOHOL,alc);
    int da = QueryProp(P_DISABLE_ATTACK);
    // Paralysen abbauen
    if ( da > 0 ) {
      da -= beat_off_num;
      if ( da < 0 )
	da = 0;
      SetProp( P_DISABLE_ATTACK, da );
    }
    // Hunttimes aktualisieren, Feinde expiren
    update_hunt_times(beat_off_num);
    if (!heartbeat)
      // HBs immer noch abgeschaltet, naechstes Mal HBs seit jetzt nachholen.
      Set("npc:beat_off_num",absolute_hb_count());
    else
      // HB laeuft wieder, nix mehr nachholen, bis zur naechsten Abschaltung.
      Set("npc:beat_off_num",0);
  }
}

void init() {

  // ggf. Heartbeats nachholen und wieder einschalten.
  if (!heartbeat) {
    set_heart_beat(1);
    heartbeat=1;
    catch_up_hbs();
  }

  if (AutoAttack(this_player()))
    Kill(this_player());
}

private nosave closure mod_att_stat;

int Defend(int dam, mixed dam_type, mixed spell, object enemy) {
  if (objectp(enemy=(enemy||this_player()))
      && query_once_interactive(enemy)
      && !IS_LEARNER(enemy)) {
    if (!objectp(get_type_info(mod_att_stat,2))) {
      object ma;
      if (!objectp(ma=find_object(STATMASTER)))
        return ::Defend(dam,dam_type,spell,enemy);
      // Keine Statistik wenn Master nicht geladen ist.
      mod_att_stat=symbol_function("ModifyAttackStat",ma);
    }
    funcall(mod_att_stat,
            enemy->QueryProp(P_GUILD),
            enemy->QueryProp(P_GUILD_LEVEL),
            dam,
            dam_type,
            spell);
  }

  return ::Defend(dam,dam_type,spell,enemy);
}
