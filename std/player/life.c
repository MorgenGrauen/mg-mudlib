// MorgenGrauen MUDlib
//
// player/life.c -- player life handling
//
// $Id: life.c 9397 2015-12-11 20:29:26Z Zesstra $

// Defines some things which are different than in living.c
// One example is the heart_beat().

// Properties
//  P_AGE           -- Age

#pragma strong_types, save_types, rtt_checks
#pragma range_check
#pragma no_clone

inherit "/std/living/life";

#include <rtlimits.h>
#include <debug_info.h>

#define NEED_PROTOTYPES
#include <thing/properties.h>
#include <player/base.h>
#include <player/life.h>
#include <moving.h>
#include <player/comm.h>
#include <player/gmcp.h>
#undef NEED_PROTOTYPES

#include <config.h>
#include <wizlevels.h>
#include <defines.h>
#include <language.h>
#include <properties.h>
#include <hook.h>

#include <living/life.h>
#include <player/pklog.h>
#include <player/combat.h>
#include <health.h>
#include <living/combat.h>
#include <attributes.h>
#include <defuel.h>
#include <new_skills.h>

// Die Folgen eines Todes wirken 4 Stunden lang nach
#define SUFFER_TIME 7200
#define P_DEATH_INFO "death_info"

int age;                  /* Number of heart beats of this character. */

private int suffer_time;
static int time_to_save;  /* when to next save player */
private nosave int die_in_progress; // semaphore fuer die()-Aufrufe.

static int _set_playerkills( int val );

protected void create()
{
    ::create();
    Set(P_AGE, -1, F_SET_METHOD);
    Set(P_AGE, PROTECTED, F_MODE);
    Set( P_KILLS, SAVE|SECURED, F_MODE_AS );
    Set( P_GHOST, SAVE, F_MODE_AS );
    Set( P_TIMING_MAP, SAVE|SECURED, F_MODE_AS );
    Set( P_LAST_DEATH_TIME, SAVE|SECURED, F_MODE_AS );
    Set( P_DEATH_INFO, SAVE|SECURED, F_MODE_AS );
    Set( P_DEFUEL_LIMIT_FOOD,PROTECTED,F_MODE_AS);
    Set( P_DEFUEL_LIMIT_DRINK,PROTECTED,F_MODE_AS);
    Set( P_DEFUEL_TIME_FOOD,PROTECTED,F_MODE_AS);
    Set( P_DEFUEL_TIME_DRINK,PROTECTED,F_MODE_AS);
    Set( P_DEFUEL_AMOUNT_FOOD,PROTECTED,F_MODE_AS);
    Set( P_DEFUEL_AMOUNT_DRINK,PROTECTED,F_MODE_AS);
    offerHook(H_HOOK_HP,1);
    offerHook(H_HOOK_SP,1);
    // P_TIMING_MAP aufraeumen, aber zeitverzoegert, weil jetzt die Daten noch
    // nicht aus dem Savefile geladen wurden.
    call_out(#'expire_timing_map, 4);
}

// called from base.c in Reconnect()
protected void reconnect() {
  expire_timing_map();
}

static int _set_playerkills( int val )
{
    string tmp;
    int playerkills;

    // ist der Setzer in einer Arena/Schattenwelt. Dann nicht. (Ja, Bug ist,
    // dass EMs aus Schattenwelt/Arena heraus auch das PK-Flag nicht
    // zuruecksetzen koennen.)
    if ( previous_object(1) && environment(previous_object(1)) &&
         (tmp = object_name(environment(previous_object(1)))) &&
         CheckArenaFight(previous_object(1)) )
        return 0;

    tmp = sprintf( "%O: %s %O %s",
                   previous_object(1) || this_interactive() || this_player(),
                   getuid(ME), val, dtime(time()) );

    playerkills = Query(P_KILLS);

    if( intp(val) && val >= 0 )
        if( previous_object(1) && IS_ARCH(getuid(previous_object(1))) &&
            this_interactive() && IS_ARCH(this_interactive()) )
            playerkills = val;
        else
            tmp += " ILLEGAL!";
    else
        playerkills++;

    if ( !previous_object(1) || !query_once_interactive(previous_object(1)) ||
         IS_LEARNER(previous_object(1)) )
        log_file( "SET_KILLS", tmp + "\n" );

    return Set( P_KILLS, playerkills );
}


public int death_suffering()
{
    if ( suffer_time <= 0 )
    return suffer_time = 0;

    return 1 + (10 * suffer_time) / SUFFER_TIME;
}


protected void heart_beat()
{
    mapping di, mods;

    ++age; // erstmal altern ;-)

    ::heart_beat();

    if ( age > time_to_save ){
        save_me(1);
        time_to_save = age + 500;
    }

    // als geist hat man mit den meisten weltlichen Dingen nicht allzuviel zu
    // tun.
    if ( QueryProp(P_GHOST) )
        return;

    if ( suffer_time > 0 )
        suffer_time--;

    // Todesfolgen nur alle 20 Sekunden (10 HB) checken.
    // Das ist immer noch oft genug und spart Rechenzeit.
    if ( (age % 10) || !mappingp(di = QueryProp(P_DEATH_INFO)) )
        return;

    mods = QueryProp(P_ATTRIBUTES_MODIFIER)["#death"];
    if (!mappingp(mods)) return;

    if ( mods[A_STR] && --di[A_STR] <= 0) {
        // einen Attributspunkt regenerieren
        if ( mods[A_STR] < -1 ) {
            mods[A_STR]++;
            di[A_STR] = (110 + 5 * (di[A_STR, 1] + mods[A_STR])) / 10;
        }
        else {
            m_delete( mods, A_STR );
            m_delete( di, A_STR );
        }
    }

    if ( mods[A_CON] && --di[A_CON] <= 0) {
        // einen Attributspunkt regenerieren
        if ( mods[A_CON] < -1 ){
            mods[A_CON]++;
            di[A_CON] = (110 + 5 * (di[A_CON, 1] + mods[A_CON])) / 10;
        }
        else {
            m_delete( mods, A_CON );
            m_delete( di, A_CON );
        }
    }

    if ( mods[A_DEX] && --di[A_DEX] <= 0) {
        // einen Attributspunkt regenerieren
        if ( mods[A_DEX] < -1 ){
            mods[A_DEX]++;
            di[A_DEX] = (110 + 5 * (di[A_DEX, 1] + mods[A_DEX])) / 10;
        }
        else {
            m_delete( mods, A_DEX );
            m_delete( di, A_DEX );
        }
    }

    if ( mods[A_INT] && --di[A_INT] <= 0) {
        // einen Attributspunkt regenerieren
        if ( mods[A_INT] < -1 ){
            mods[A_INT]++;
            di[A_INT] = (110 + 5 * (di[A_INT, 1] + mods[A_INT])) / 10;
        }
        else {
            m_delete( mods, A_INT );
            m_delete( di, A_INT );
        }
    }

    if ( sizeof(di) && sizeof(mods))
        SetProp( P_DEATH_INFO, di );
    else
        SetProp( P_DEATH_INFO, 0 );

    SetProp( P_ATTRIBUTES_MODIFIER, ({ "#death", mods }) );
}


public void force_save() {
    time_to_save = 0;
}


nomask public int do_damage( int dam, object enemy )
{
    int hit_point;

    if( QueryProp(P_GHOST) || dam <= 0 )
        return 0;

    hit_point = QueryProp(P_HP);

    if ( query_once_interactive(ME) && dam >= hit_point && IS_LEARNING(ME) ){
        tell_object( ME, "Deine magischen Kraefte verhindern Deinen Tod.\n" );
        return 0;
    }

    if ( !objectp(enemy) )
        enemy = previous_object() || this_interactive() || this_player();

    hit_point -= dam;

    if( hit_point < 0 ){
        if ( !interactive(ME) )
            // Netztote sterben nicht
            hit_point = 10;
        else {
            if ( objectp(enemy) && interactive( enemy ) && enemy != ME &&
                 !QueryProp(P_TESTPLAYER) && !IS_WIZARD(ME) &&
                 !CheckArenaFight(ME) ) {
                if ( QueryPlAttacked(enemy) )
                    hit_point = 1;
                else {
                    hit_point = 0;
                    enemy->SetProp( P_KILLS, -1 );
                }

                log_file( "KILLER",
                          sprintf( "%s %s(%d/%d) toetete %s(%d/%d)%s\n",
                                   ctime(time()),
                                   getuid(enemy), query_wiz_level(enemy),
                                   enemy->QueryProp(P_LEVEL), getuid(ME),
                                   query_wiz_level(ME), QueryProp(P_LEVEL),
                                   (hit_point ? " NOTWEHR=>KEIN PK" : "") ) );
            }
            else { 
                string killername;
                if (objectp(enemy))
                    killername=sprintf("%s (%s)",
                        BLUE_NAME(enemy), REAL_UID(enemy));
                else
                    killername="??? (???)"; 

                if ( !QueryProp(P_TESTPLAYER) )
                    create_kill_log_entry(killername, enemy );
            }

            if ( enemy )
                enemy->StopHuntFor( ME, 1 );

            map_objects( QueryEnemies()[0], "StopHuntFor", ME, 1 );
            StopHuntingMode(1);

            Set( P_KILLER, enemy );
            die();
        }
    }

    SetProp( P_HP, hit_point );
    return dam;
}


// Loescht im sterbenden Spieler die 'koerperabhaengigen' Properties
private void reset_my_properties()
{
  // Loeschen der Properties
  if ( QueryProp(P_POISON) )
  {
     // Don't die twice 'cause of the same poison
     Set( P_POISON, 0, F_SET_METHOD );
     Set( P_POISON, 0, F_QUERY_METHOD );
     SetProp( P_POISON, 0 );
  }

  Set( P_FROG, 0, F_QUERY_METHOD ); 
  Set( P_FROG, 0, F_SET_METHOD );
  SetProp( P_FROG, 0 ); // Damit die Attribute auch stimmen.
  Set( P_ALCOHOL, 0, F_QUERY_METHOD );
  Set( P_ALCOHOL, 0, F_SET_METHOD );
  SetProp(P_ALCOHOL, 0 );
  Set( P_DRINK, 0, F_QUERY_METHOD );
  Set( P_DRINK, 0, F_SET_METHOD );
  SetProp(P_DRINK, 0 );
  Set( P_FOOD, 0, F_QUERY_METHOD );
  Set( P_FOOD, 0, F_SET_METHOD );
  SetProp(P_FOOD, 0 );
  Set( P_BLIND, 0, F_QUERY_METHOD );
  Set( P_BLIND, 0, F_SET_METHOD );
  SetProp(P_BLIND, 0 );
  Set( P_DEAF, 0, F_QUERY_METHOD );
  Set( P_DEAF, 0, F_SET_METHOD );
  SetProp(P_DEAF, 0 );
  Set( P_MAX_HANDS, 0, F_QUERY_METHOD );
  Set( P_MAX_HANDS, 0, F_SET_METHOD );
  SetProp( P_MAX_HANDS, 2 );
  Set( P_HANDS_USED_BY, 0, F_QUERY_METHOD );
  Set( P_HANDS_USED_BY, 0, F_SET_METHOD );
  SetProp( P_HANDS_USED_BY, ({}) );
  Set( P_PARA, 0 );
  Set( P_NO_REGENERATION, 0, F_QUERY_METHOD );
  Set( P_NO_REGENERATION, 0, F_SET_METHOD );
  SetProp(P_NO_REGENERATION, 0 );
  Set( P_TMP_MOVE_HOOK, 0, F_QUERY_METHOD );
  Set( P_TMP_MOVE_HOOK, 0, F_SET_METHOD );
  SetProp(P_TMP_MOVE_HOOK, 0 );
  Set( P_LAST_DEATH_TIME , time() );
  // damit der Teddy o.ae. mitbekommt, dass man jetzt tot ist ]:->
  SetProp( P_HP, 0 );
  SetProp( P_SP, 0 );
}

varargs protected int second_life( object corpse )
{
    int lost_exp, level;
    // Es gibt Funktionen, die sollte man nicht per Hand aufrufen duerfen ;-)
    if ( extern_call() && previous_object() != ME )
        return 0;

    if ( query_once_interactive(ME) && IS_LEARNING(ME) ){
        tell_object( ME, "Sei froh, dass Du unsterblich bist, sonst waere "
                     "es eben zu Ende gewesen.\n" );
        return 1;
    }

    if ( !IS_SEER(ME) || (level = QueryProp(P_LEVEL)) < 20 )
        lost_exp = QueryProp(P_XP) / 3;
    else
        lost_exp = QueryProp(P_XP) / (level - 17);

    AddExp(-lost_exp);


    // Todesfolgen setzen....
    //SetProp( P_DEATH_INFO, 1);
    if ( !IS_LEARNING(ME) && !QueryProp(P_TESTPLAYER) ) {

        mapping attr = QueryProp(P_ATTRIBUTES);
        mapping mods = QueryProp(P_ATTRIBUTES_MODIFIER)["#death"] || ([]);

        // Attribute auf 75% vom aktuellen Wert senken
        mods[A_STR] = -attr[A_STR] + (3 * (attr[A_STR] + mods[A_STR]) / 4);
        mods[A_CON] = -attr[A_CON] + (3 * (attr[A_CON] + mods[A_CON]) / 4);
        mods[A_DEX] = -attr[A_DEX] + (3 * (attr[A_DEX] + mods[A_DEX]) / 4);
        mods[A_INT] = -attr[A_INT] + (3 * (attr[A_INT] + mods[A_INT]) / 4);

        SetProp( P_ATTRIBUTES_MODIFIER, ({ "#death", mods }) );

        int offs = 220;  // 220 heart_beats == 7min20
        // Die 220 HB sind fix, dazu kommen noch 5 HB pro realem 
        // Attributspunkt. Geteilt wird das ganze noch durch 10, weil im HB
        // nur alle 10 HBs die TF gecheckt werden. Da wird dann alle 10 HB ein
        // Punkt abgezogen und wenn 0 erreicht ist, wird das Attribut um eins
        // regeneriert.
        SetProp( P_DEATH_INFO, ([ 
            A_STR: (offs + 5 * (attr[A_STR] + mods[A_STR]))/10; attr[A_STR],
            A_CON: (offs + 5 * (attr[A_CON] + mods[A_CON]))/10; attr[A_CON],
            A_DEX: (offs + 5 * (attr[A_DEX] + mods[A_DEX]))/10; attr[A_DEX],
            A_INT: (offs + 5 * (attr[A_INT] + mods[A_INT]))/10; attr[A_INT]
                  ]) );

        // die suffer_time wird via death_suffering() von
        // QuerySkillAttribute() abgefragt und geht dann als Malus in
        // SA_QUALITY mit ein.
        if ( suffer_time <= 2*SUFFER_TIME )
            suffer_time += SUFFER_TIME - 1;
        else
            suffer_time = 3 * SUFFER_TIME - 1;
    }

    // Die verschiedenen NotifyPlayerDeath-Funktionen koennen u.U. schlecht
    // programmiert sein und zuviel Rechenzeit ziehen. Deshalb werden sie mit
    // einem Limits von 150k bzw. 80k aufgerufen. Ausserdem werden sie nur
    // gerufen, solange noch min. 25k Ticks da sind.
    int *limits=query_limits();
    limits[LIMIT_EVAL] = 150000;
    limits[LIMIT_COST] = LIMIT_DEFAULT;

    string|object killer = QueryProp(P_KILLER);
    string|object gi = QueryProp(P_GUILD);
    if (stringp(gi))
        gi = find_object("/gilden/"+gi);
    // jedes Objekt nur einmal, aber nicht via m_indices(mkmapping)) wegen
    // Verlust der Reihenfolge.
    object* items = ({});
    if (objectp(killer))
        items = ({killer});
    else
    {
      // (Fast) alle NotifyPlayerDeath() gehen davon aus, dass <killer> nur
      // ein Objekt ist. Daher wird im Falle von Gifttoden - wenn <killer>
      // ein string ist, immer 0 uebergehen.
      killer = 0;
    }
    if (environment() != killer)
        items += ({environment()});
    if (gi != killer && gi != environment())
        items += ({gi});
    foreach(object item: items) {
        if (get_eval_cost() < limits[LIMIT_EVAL] + 20000)
            break;
        // falls ein NPD() implizit andere Objekt zerstoert hat.
        if (objectp(item)) {
            catch(limited(#'call_other, limits, item, "NotifyPlayerDeath",
                  ME, killer, lost_exp);publish);
        }
    }
    // jetzt den Rest.
    limits[LIMIT_EVAL] = 80000;
    foreach(object item: (environment() ? all_inventory(environment()) : ({}))
                        + deep_inventory(ME)
                        + (objectp(corpse) ? deep_inventory(corpse) : ({}))
                        - items ) {
        // wenn nicht mehr genug Ticks, haben die restlichen Pech gehabt.
        if (get_eval_cost() < limits[LIMIT_EVAL] + 20000)
            break;
        // NPD() koennen andere Objekt zerstoeren.
        if (objectp(item)) {
            catch(limited(#'call_other, limits, item, "NotifyPlayerDeath",
                                    ME, killer, lost_exp);publish);
        }
    }

    // Properties zuruecksetzen, sollte nach dem NotifyPlayerDeath()
    // passieren, weil Objekte sich darin evtl. erstmal noch aufraeumen und
    // props manipulieren.
    reset_my_properties();
    UpdateAttributes(); // Beim Tod werden Dinge entfernt, Attribute pruefen
    
    // Auch Bewegung erst nach den NPD(), da diese den Spieler bewegen
    // koennten, was eine Bewegung aus dem Todesraum waere, wenn die Bewegung
    // vor dem NPD() stattfaende.
    SetProp( P_GHOST, 1 ); // nach reset_my_properties() !
    clone_object( "room/death/death_mark" )->move( ME, M_NOCHECK ); 

    return 1;
}


public int AddHpHook( object ob )
{
    object *hooks;

    if ( !objectp(ob) )
        return 0;

    if ( !pointerp(hooks = Query(P_HP_HOOKS)) ){
        Set( P_HP_HOOKS, ({ ob }) );
        return 1;
    }

    if ( member( hooks, ob ) >= 0 )
        return 0;

    Set( P_HP_HOOKS, (hooks - ({0})) + ({ ob }) );
    return 1;
}


public int RemoveHpHook( object ob )
{
    object *hooks;

    if ( !pointerp(hooks = Query(P_HP_HOOKS)) )
        return 0;

    Set( P_HP_HOOKS, hooks - ({ ob, 0 }) );
    return 1;
}


static int _query_age() {
    return Set(P_AGE, age, F_VALUE);
}

static int _set_hp( int hp )
{
    object *hooks;
    int ret, i, old;

    if ( (old = Query(P_HP, F_VALUE)) == hp )
        return old;

    ret = life::_set_hp(hp);

    if ( ret == old )
        return ret;

    // Call old hooks in all objects... destructed objects will be ignored.
    if (pointerp(hooks = Query(P_HP_HOOKS)))
      call_other(hooks, "NotifyHpChange");

    // Call new hooks.
    HookFlow(H_HOOK_HP,ret);

    // Report-ausgabe
    status_report(DO_REPORT_HP, ret);

    return ret;
}


static int _set_sp( int sp )
{
    object *hooks;
    int ret, i, old;

    if ( (old = Query(P_SP,F_VALUE)) == sp )
        return old;

    ret = life::_set_sp(sp);

    if ( ret == old )
        return ret;

    // Call old hooks in all objects... destructed objects will be ignored.
    if (pointerp(hooks = Query(P_HP_HOOKS)))
      call_other(hooks, "NotifyHpChange");

    // Call new hooks.
    HookFlow(H_HOOK_SP,ret);

    // Report-ausgabe
    status_report(DO_REPORT_SP, ret);

    return ret;
}

static int _set_poison(int n)
{
  int old = Query(P_POISON, F_VALUE);
  if (old == n )
      return old;
  n = life::_set_poison(n);
  if ( n == old )
      return n;
  // ggf. Statusreport ausgeben
  if (interactive(ME))
    status_report(DO_REPORT_POISON, n);
  return n;
}

static int _set_max_poison(int n)
{
  if (n >= 0)
  {
    Set(P_MAX_POISON, n, F_VALUE);
    int maxp=QueryProp(P_MAX_POISON); // koennte ne Querymethode drauf sein...
    if (QueryProp(P_POISON) > maxp)
      SetProp(P_POISON, maxp);
  }
  GMCP_Char( ([P_MAX_POISON: n]) );
  return n;
}

static int _set_max_hp( int hp )
{
  if (hp >= 0)
  {
    Set(P_MAX_HP, hp, F_VALUE);
    int maxhp=QueryProp(P_MAX_HP); // koennte ne Querymethode drauf sein...
    if (QueryProp(P_HP) > maxhp)
      SetProp(P_HP, maxhp);
  }
  GMCP_Char( ([P_MAX_HP: hp]) );
  return hp;
}

static int _set_max_sp( int sp )
{
  if (sp >= 0)
  {
    Set(P_MAX_SP, sp, F_VALUE);
    int maxsp=QueryProp(P_MAX_SP); // koennte ne Querymethode drauf sein...
    if (QueryProp(P_SP) > maxsp)
      SetProp(P_SP, maxsp);
  }
  GMCP_Char( ([P_MAX_SP: sp]) );
  return sp;
}

static int _set_ghost( int g ) {
    object team;

    if(!g && query_hc_play()>1)
    {
      write("HAHAHA, DU BIST AUF EWIG MEIN.\n");
      return Query(P_GHOST);
    }

    g = Set( P_GHOST, g );

    if ( g && objectp(team = Query(P_TEAM)) )
        team->RemoveMember(ME);

    return g;
}


public int undie()
{
    mixed x, di;
    mapping attr, mods;

    if ( !this_interactive() || !previous_object() )
        return 0;

    if ( !IS_ARCH(this_interactive()) || !IS_ARCH(getuid(previous_object())) ||
         process_call() )
        log_file( "UNDIE", sprintf( "%s %O -> %O\n", dtime(time())[5..16],
                                    this_interactive(), ME ) );

    if ( x = Query(P_DEADS) )
        x--;

    Set( P_DEADS, x );

    x = QueryProp(P_XP);

    if ( (di = QueryProp(P_LEVEL)) < 20 || !IS_SEER(ME) )
        x = (int)(x * 1.5);
    else
        // Umweg ueber float, weil bei hohen XP+Level sonst 32Bit nicht
        // mehr ausreichen -> negative XP
        x = (int) ( x * ((float) (di - 17) / (di - 18)) );

    Set( P_XP, x );

    attr = QueryProp(P_ATTRIBUTES) || ([]);
    mods = QueryProp(P_ATTRIBUTES_MODIFIER)["#death"] || ([]);

    if ( mappingp(di = QueryProp(P_DEATH_INFO)) ){
        // Beim naechsten heart_beat checken
        // Zesstra: Wieso eigentlich? Die Modifier werden doch direkt hier
        // geloescht. So expired man auch einen Teil von nicht-undie-ten Toden
        // vorzeitig...? Mal auskommentiert. 29.10.2007
        //di[A_STR] = 1;
        //di[A_DEX] = 1;
        //di[A_INT] = 1;
        //di[A_CON] = 1;
    }
    else
      di = ([]);
    
    mods[A_STR] = ((4 * (attr[A_STR] + mods[A_STR])) / 3) - attr[A_STR];
    mods[A_DEX] = ((4 * (attr[A_DEX] + mods[A_DEX])) / 3) - attr[A_DEX];
    mods[A_INT] = ((4 * (attr[A_INT] + mods[A_INT])) / 3) - attr[A_INT];
    mods[A_CON] = ((4 * (attr[A_CON] + mods[A_CON])) / 3) - attr[A_CON];

    if ( mods[A_STR] >= 0 ) {
        m_delete( mods, A_STR );
        m_delete( di, A_STR);
    }
    if ( mods[A_DEX] >= 0 ) {
        m_delete( mods, A_DEX );
        m_delete( di, A_DEX);
    }
    if ( mods[A_INT] >= 0 ) {
        m_delete( mods, A_INT );
        m_delete( di, A_INT);
    }
    if ( mods[A_CON] >= 0 ) {
        m_delete( mods, A_CON );
        m_delete( di, A_CON);
    }

    SetProp( P_ATTRIBUTES_MODIFIER, ({ "#death", mods }) );
    if (sizeof(di))
      SetProp( P_DEATH_INFO, di );
    else
      SetProp( P_DEATH_INFO, 0);

    suffer_time -= ((SUFFER_TIME)-1);

    if ( suffer_time < 0 )
        suffer_time = 0;

    Set( P_GHOST, 0 );
    return 1;
}


varargs public void die( int poisondeath, int extern)
{
    // laeuft schon ein die()? Fehler ausloesen, Ursache rekursiver die() soll
    // gefunden werden. DINFO_EVAL_NUMBER wird in jedem Top-Level Call im
    // driver erhoeht, d.h. gleiche Zahl signalisiert ein rekursives die().

    if (die_in_progress == debug_info(DINFO_EVAL_NUMBER))
    {
      // TODO: ist das die_in_progress aus dem letzten Backend-Cycle?
      raise_error(sprintf(
            "die() in %O gerufen, aber die() laeuft bereits!\n",
            this_object()));
    }
    die_in_progress = debug_info(DINFO_EVAL_NUMBER);
    
    // Fuer HC-Player ists jetzt gelaufen...
    if(query_hc_play()==1)
    {
      set_hc_play(capitalize(geteuid(ME)),time());
      SetDefaultHome("/room/nirvana");
      SetDefaultPrayRoom("/room/nirvana");
      SetProp(P_START_HOME,"/room/nirvana");
      log_file("HCDEAD",dtime(time())+" "+capitalize(geteuid(ME))
          +" geht in das Nirvana ein!\n");
    }

    // Wenn das die() direkt von aussen gerufen wurde, muss P_KILLER hier
    // gespeichert werden.
    if (extern_call())
        SetProp(P_KILLER, previous_object());

    // Sichern der zu loeschenden Properties. Diese Props werden im Verlauf des
    // Todes zurueckgesetzt. Einige Magier wollen diese Daten aber spaeter
    // noch haben und fragen teilweise P_LAST_DEATH_PROPS im
    // NotifyPlayerDeath() ab. Daher wird der Kram jetzt hier schonmal
    // gesichert.
    // BTW: Props mit Mappings/Arrays sollten kopiert werden.
    SetProp(P_LAST_DEATH_PROPS, ([
      P_POISON          : QueryProp(P_POISON),
      P_FROG            : QueryProp(P_FROG),
      P_ALCOHOL         : QueryProp(P_ALCOHOL),
      P_DRINK           : QueryProp(P_DRINK),
      P_FOOD            : QueryProp(P_FOOD),
      P_BLIND           : QueryProp(P_BLIND),
      P_DEAF            : QueryProp(P_DEAF),
      P_MAX_HANDS       : QueryProp(P_MAX_HANDS),
      P_PARA            : QueryProp(P_PARA),
      P_NO_REGENERATION : QueryProp(P_NO_REGENERATION),
      P_HP              : QueryProp(P_HP),
      P_SP              : QueryProp(P_SP),
      P_LAST_DEATH_TIME : QueryProp(P_LAST_DEATH_TIME )
    ]) );

    // call the inherited die() with 10 Mio Ticks which will be accounted as 1
    // Tick... ;-)
    int *limits = query_limits();
    limits[LIMIT_EVAL] == 10000000;
    limits[LIMIT_COST] == LIMIT_UNLIMITED;
    limited(#'::die, limits, poisondeath, (extern_call() ? 1 : 0)); 

    // nach dem Tod sollte man auch keine LP mehr haben.
    SetProp(P_HP, 0);

    // naechster Tod kann kommen. Dekrementierung, da 0 ein gueltiger Wert
    // fuer DINFO_EVAL_NUMBER waere. abs(), um nicht  -__INT_MAX__ zu
    // dekrementieren.
    die_in_progress = abs(die_in_progress) - 1;
}

int defuel_food()
{
    int ret;
    object prev;
    
    ret=::defuel_food();
    prev=previous_object();
    if(!prev || !objectp(prev))
    {
        prev=this_object();
    }
    
    if(ret<=0)
    {
            call_other(FUELSTAT,"addDefuelStatEntry",prev,this_object(),0,1,0,1);
    }
    else
    {
            call_other(FUELSTAT,"addDefuelStatEntry",prev,this_object(),0,0,ret,1);
    }
    return ret;
}

int defuel_drink()
{
    int ret;
    object prev;
    
    ret=::defuel_drink();
    prev=previous_object();
    if(!prev || !objectp(prev))
    {
        prev=this_object();
    }
    
    if(ret<=0)
    {
            call_other(FUELSTAT,"addDefuelStatEntry",prev,this_object(),0,1,0,0);
    }
    else
    {
            call_other(FUELSTAT,"addDefuelStatEntry",prev,this_object(),0,0,ret,0);
    }
    return ret;
}

public void show_age()
{ int i,j;

    write("Alter:\t");
    i = QueryProp(P_AGE);
    if ((j=i/43200))
    {
      write(j + " Tag"+(j==1?" ":"e "));
      i %= 43200;
    }
    if ((j=i/1800))
    {
      write(j + " Stunde"+(j==1?" ":"n "));
      i %= 1800;
    }
    if ((j=i/30))
    {
      write(j + " Minute"+(j==1?" ":"n "));
      i %= 30;
    }
    write(i*2 + " Sekunden.\n");
}

