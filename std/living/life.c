// MorgenGrauen MUDlib
//
// living/life.c -- life variables
//
// $Id: life.c 9426 2016-01-03 10:02:57Z Zesstra $

// living object life variables
//
//  P_ALIGN         -- alignment value
//  P_NPC           -- if living is an NPC
//  P_HP            -- HitPoints
//  P_SP            -- SpellPoints
//  P_ALCOHOL       -- value of intoxication
//  P_DRINK         -- value of soakness
//  P_FOOD          -- value of stuffness
//  P_XP            -- experience
//  P_POISON        -- level of poison
//  P_CORPSE        -- corpse-object
//  P_DEAF          -- if living is deaf
#pragma strict_types,save_types,rtt_checks
#pragma range_check
#pragma no_clone

#define NEED_PROTOTYPES
#include <hook.h>
#include <living/skills.h>
#include <thing/properties.h>
#include <living/life.h>
#include <living/moving.h>
#include <living/combat.h>
#include <living/attributes.h>
#include <thing/description.h>
#include <thing/language.h>
#undef NEED_PROTOTYPES
#include <health.h>
#include <defines.h>
#include <new_skills.h>
#include <scoremaster.h>
#include <defuel.h>
#include <properties.h>
#include <events.h>
#include <wizlevels.h>

#define ALCOHOL_VALUE(n) n

#include <debug_info.h> //voruebergehend


// 'private'-Prototypen
private void DistributeExp(object enemy, int exp_to_give);

// Variablen
nosave int delay_alcohol; /* time until next alcohol effect */
nosave int delay_drink;   /* time until next drink effect */
nosave int delay_food;    /* time until next food effect */
nosave int delay_heal;    /* time until next heal effect */
nosave int delay_sp;      /* time until next sp regeneration */
nosave int delay_poison;  /* time until next poison effect */
nosave int drop_poison;
nosave mapping enemy_damage;
nosave mapping hp_buffer;
nosave mapping sp_buffer;
nosave int remove_me;
int nextdefueltimefood;
int nextdefueltimedrink;


protected void create()
{
  Set(P_GHOST, SAVE, F_MODE);
  Set(P_FROG, SAVE, F_MODE);
  Set(P_ALIGN, SAVE, F_MODE);
  Set(P_HP, SAVE, F_MODE);
  Set(P_SP, SAVE, F_MODE);
  Set(P_XP, SAVE, F_MODE);
  Set( P_LAST_XP, ({ "", 0 }) );
  Set( P_LAST_XP, PROTECTED, F_MODE_AS );

  Set(P_ALCOHOL, SAVE, F_MODE);
  Set(P_DRINK, SAVE, F_MODE);
  Set(P_FOOD, SAVE, F_MODE);
  Set(P_POISON, SAVE, F_MODE);
  Set(P_DEAF, SAVE, F_MODE);

  SetProp(P_FOOD_DELAY, FOOD_DELAY);
  SetProp(P_DRINK_DELAY, DRINK_DELAY);
  SetProp(P_ALCOHOL_DELAY, ALCOHOL_DELAY);
  SetProp(P_HP_DELAY,HEAL_DELAY);
  SetProp(P_SP_DELAY,HEAL_DELAY);
  SetProp(P_POISON_DELAY,POISON_DELAY);
  // default fuer alle Lebewesen (NPC + Spieler):
  SetProp(P_MAX_POISON, 10);
  SetProp(P_CORPSE, "/std/corpse");

  nextdefueltimefood=time()+QueryProp(P_DEFUEL_TIME_FOOD);
  nextdefueltimedrink=time()+QueryProp(P_DEFUEL_TIME_DRINK);

  enemy_damage=([:2 ]);
  hp_buffer=([]);
  sp_buffer=([]);

  SetProp(P_DEFUEL_LIMIT_FOOD,1);
  SetProp(P_DEFUEL_LIMIT_DRINK,1);
  SetProp(P_DEFUEL_TIME_FOOD,1);
  SetProp(P_DEFUEL_TIME_DRINK,1);
  SetProp(P_DEFUEL_AMOUNT_FOOD,1);
  SetProp(P_DEFUEL_AMOUNT_DRINK,1);

  offerHook(H_HOOK_DIE,1);

  offerHook(H_HOOK_FOOD,1);
  offerHook(H_HOOK_DRINK,1);
  offerHook(H_HOOK_ALCOHOL,1);
  offerHook(H_HOOK_POISON,1);
  offerHook(H_HOOK_CONSUME,1);
}

// Wenn der letzte Kampf lang her ist und das Lebewesen wieder vollgeheilt
// ist, wird P_ENEMY_DAMAGE zurueckgesetzt.
protected void ResetEnemyDamage() {
  if (time() > QueryProp(P_LAST_COMBAT_TIME) + __RESET_TIME__ * 4
      && QueryProp(P_HP) == QueryProp(P_MAX_HP))
    enemy_damage=([:2 ]);
}

private void DistributeExp(object enemy, int exp_to_give) {
  int total_damage, tmp, ex;
  mapping present_enemies;

  if ( exp_to_give<=0 )
    return;

  mapping endmg=deep_copy(enemy_damage);

  // Mitglieder im Team des Killers bekommen:
  //
  //                  Gesamtanteil des Teams
  // Eigenen Anteil + ----------------------
  //                  Anzahl  Teammitglieder
  // ---------------------------------------
  //                2
  //
  object *inv = ({object*})enemy->TeamMembers();
  if ( pointerp(inv) )
  {
    present_enemies=m_allocate(sizeof(inv), 1);
    foreach(object ob: inv)
    {
      if ( objectp(ob) && (environment(ob)==environment()) )
      {
        tmp=endmg[object_name(ob)];
        total_damage+=tmp;
        present_enemies[ob] = tmp/2;
        m_delete(endmg,object_name(ob)); //s.u.
      }
    }
    int mitglieder = sizeof(present_enemies);
    if ( mitglieder )
    {
      tmp=total_damage/(2*mitglieder);
      foreach(object ob, int punkte: &present_enemies)
        punkte += tmp;
    }
  }
  else {
    // ohne Team wird trotzdem ein Mapping gebraucht. Da Groessenveraenderung
    // rel. teuer sind, kann einfach mal fuer 3 Eintraege Platz reservieren.
    present_enemies=m_allocate(3, 1);
  }
  // Und noch die Lebewesen im Raum ohne Team.
  foreach(object ob: all_inventory(environment()))
  {
    if ( tmp=endmg[object_name(ob)] )
    {
      total_damage += tmp;
      present_enemies[ob] = tmp;
      m_delete(endmg,object_name(ob)); // Nur einmal pro Leben Punkte :)
    }
  }
  if ( !total_damage )
  {
    ({int})enemy->AddExp(exp_to_give);
  }
  else
  {
    foreach(object ob, int damage: present_enemies)
    {
      if ( !objectp(ob) )
        continue;
      if ( query_once_interactive(ob) && ( !interactive(ob)
              || (query_idle(ob)>600) ) )
        continue;
      //exp_to_give*present_enemies[i][1]/total_damage gibt bei viel Schaden
      //einen numerical overflow. Daher muessen wir hier wohl doch
      //zwischenzeitlich mit floats rechnen, auch wenn das 0-1 XP Verlust
      //durch float->int-Konversion gibt. (ceil() lohnt sich IMHO nicht.)
      ex = (int)(exp_to_give*((float)damage/(float)total_damage));
      ob->AddExp(ex);
    }
  }
}

/*
 * This function is called from other players when they want to make
 * damage to us. But it will only be called indirectly.
 * We return how much damage we received, which will
 * change the attackers score. This routine is probably called from
 * heart_beat() from another player.
 * Compare this function to reduce_hit_points(dam).
 */
public int do_damage(int dam, object enemy)
{ int hit_point,al,al2;

  if ( extern_call()
      && objectp(enemy)
      && living(enemy)
      && !QueryProp(P_ENABLE_IN_ATTACK_OUT))
  {
    al=time()-({int})enemy->QueryProp(P_LAST_MOVE);
    if (al<3)      // Erste Kampfrunde nach Betreten des Raumes?
      dam/=(4-al); // Gegen Rein-Feuerball-Raus-Taktik
  }

  if ( QueryProp(P_GHOST) || QueryProp(P_NO_ATTACK) || (dam<=0)
      || ( objectp(enemy)
          && ( ({int})enemy->QueryProp(P_GHOST)
              || ({int|string})enemy->QueryProp(P_NO_ATTACK) ) ) )
    return 0;

  hit_point = QueryProp(P_HP)-dam;

  if ( QueryProp(P_XP) && objectp(enemy) )
  {
    if ( !QueryProp(P_NO_XP) )
      ({int})enemy->AddExp(dam*({int})QueryProp(P_TOTAL_WC)/10);
  }

  if (living(enemy)) {
      string enname = object_name(enemy);
      // Hmpf. Blueprints sind doof. Die Chance ist zwar gering, aber koennte
      // sein, dass ein Unique-NPC mit zwei verschiedenen Spielern am gleichen
      // NPC metzelt.
      // TODO: MHmm. wie gross ist das Risiko wirklich?
      //if (!clonep(enemy))
      //    enname = enname + "_" + to_string(object_time(enemy));
      // nur wenn gegner NPC ist und noch nicht drinsteht: Daten aus
      // P_HELPER_NPC auswerten
      if (!member(enemy_damage,enemy) && !query_once_interactive(enemy)) {
          mixed helper = ({mixed})enemy->QueryProp(P_HELPER_NPC);
          if (pointerp(helper) && objectp(helper[0]))
              enemy_damage[enname,1] = helper[0];
      }
      enemy_damage[enname,0]+=dam;
  }

  SetProp(P_HP, hit_point);

  if ( hit_point<0 )
  {
    //TODO: Warum nicht das ganze Zeug ins die() verlegen?
    if ( enemy )
    {
      ({int})enemy->StopHuntFor(ME,1);
      if ( !QueryProp(P_NO_XP) )
        DistributeExp(enemy,QueryProp(P_XP)/100);
      if ( !query_once_interactive(ME) )
        log_file ("NPC_XP", sprintf(
	    "[%s] %s, XP: %d, HP*WC: %d, Killer: %s\n",
	    dtime(time()), object_name(ME), (QueryProp(P_XP)/100),
                  QueryProp(P_TOTAL_WC)*QueryProp(P_MAX_HP)/10,
                  ({string})enemy->name()||"NoName" ));
      al = QueryProp(P_ALIGN)/50 + ({int})enemy->QueryProp(P_ALIGN)/200;
      if (al>20)
        al=20;
      else if(al<-20)
        al=-20;
      ({int})enemy->SetProp(P_ALIGN,({int})enemy->QueryProp(P_ALIGN)-al);
    }
    SetProp(P_KILLER, enemy);
    
    die();
  }
  return dam;
}


private void _transfer( object *obs, string|object dest, int flag )
{   int i;

    i = sizeof(obs);

    // Eine Schleife ist zwar langsamer als filter() o.ae., aber
    // selbst mit einer noch so schnellen Loesung kann leider nicht
    // ausgeschlossen werden, dass irgendwo ein too-long-eval-Bug dazwischen
    // kommt. Dazu sind die Kaempfe mit Gilden-NPCs etc. einfach zu teuer ...
    // Pruefung auf zerstoerte Objekte, da einige sich evtl. im NotifyPlayerDeath() 
    // zerstoeren.
   while ( i && get_eval_cost() > 300000 )
        if ( objectp(obs[--i]) && !({int})obs[i]->QueryProp(P_NEVERDROP) )
        // Jetzt wird's noch etwas teurer mit catch() - aber manche Sachen
        // duerfen einfach nicht buggen
            catch( ({int})obs[i]->move( dest, flag );publish );

    if ( i > 0 )
        // Zuviel Rechenzeit verbraten, es muessen noch Objekte bewegt werden
        call_out( #'_transfer, 0, obs[0..i-1], dest, flag );
    else {
        if ( remove_me )
            remove();
    }
}


public varargs void transfer_all_to( string|object dest, int isnpc ) {
    int flags;
    object *obs;

    if ( !objectp(ME) )
        return;

    // Das Flag "isnpc" ist fuer NPCs gedacht. Deren Ausruestung darf nicht
    // mit M_NOCHECK bewegt werden, da Spieler das bei Nicht-Standard-Leichen
    // sonst u.U. ausnutzen koennten.
    if ( isnpc )
        flags = M_SILENT;
    else
        flags = M_SILENT|M_NOCHECK;

    obs = all_inventory(ME) || ({});

    // unnoetig, weil _transfer() auch auf P_NEVERDROP prueft. Zesstra
    //obs -= filter_objects( obs, "QueryProp", P_NEVERDROP );

    _transfer( obs, dest, flags );
}


protected varargs void create_kill_log_entry(string killer, object enemy) {
  int level,lost_exp;

  if ( (level=QueryProp(P_LEVEL))<20 || !IS_SEER(ME) )
    lost_exp = QueryProp(P_XP)/3;
  else
    lost_exp = QueryProp(P_XP)/(level-17);
  
  log_file("KILLS",sprintf("%s %s (%d,%d) %s\n", strftime("%e %b %H:%M"),
               capitalize(REAL_UID(ME)), level, lost_exp/1000, killer)); 
}

// Liefert im Tod (nach dem toetenden do_damage()) das Spielerobjekt, was den
// Tod wohl zu verantworten hat, falls es ermittelt werden kann. Es werden vor
// allem registrierte Helfer-NPC und einige Sonderobjekte beruecksichtigt.
protected object get_killing_player()
{
  object killer=QueryProp(P_KILLER);
  // koennte sein, wenn ausserhalb des Todes gerufen oder eine Vergiftung uns
  // umgebracht hat.
  if (!objectp(killer))
    return 0;

  while (killer && !query_once_interactive(killer))
    killer = ({object})killer->QueryUser();

  return killer;
}

protected object GiveKillScore(object pl, int npcnum)
{
  // Stufenpunkt fuer den Kill vergeben.
  // Falls der Killer den Punkt schon hat, wird
  // zufaellig ein Mitglied seines Teams ausgewaehlt
  // und diesem der Punkt gegeben.
  object *obs,ob;
  mixed *fr;
  int i,j,sz;

  if ( pointerp(obs=({object*})pl->TeamMembers()) && (member(obs,pl)>=0) )
  {
    if ( !pointerp(fr=({mixed})pl->PresentTeamRows())
        || !sizeof(fr)
        || !pointerp(fr=fr[0])) // Erste Reihe des Teams
      fr=({});
    fr-=({pl,0});
    obs-=({pl,0});
    obs-=fr;
    i=sz=sizeof(obs); // restliche Teammitglieder in zufaelliger Reihenfolge:
    for ( --i ; i>=0 ; i-- )
    {
      j=random(sz);
      ob=obs[j];
      obs[j]=obs[0];
      obs[0]=ob;
    }
    i=sz=sizeof(fr);  // Erste Reihe in zufaelliger Reihenfolge:
    for ( --i ; i>=0 ; i-- )
    {
      j=random(sz);
      ob=fr[j];
      fr[j]=fr[0];
      fr[0]=ob;
    }

    obs+=fr;     // Erste Reihe wird vor Rest getestet
    obs+=({pl}); // Killer wird als erstes getestet
  }
  else
  {
    obs=({pl});
  }
  for ( i=sizeof(obs)-1 ; i>=0 ; i-- )
    if ( objectp(ob=obs[i] )
        && interactive(ob)     // Nur netztot dabei stehen gilt nicht :)
        && query_idle(ob)<600  // gegen Leute die sich nur mitschleppen lassen
        && environment(ob)==environment(pl) // Nur anwesende Teammitglieder
        && !IS_LEARNER(ob)
//        && !({int|string})ob->QueryProp(P_TESTPLAYER)
        && !(({int})SCOREMASTER->HasKill(ob,ME)) )
      return ({int})SCOREMASTER->GiveKill(ob,npcnum),ob;

  return ({int})SCOREMASTER->GiveKill(pl,npcnum),pl;
}

// zum ueberschreiben in Spielern
public int death_suffering() {
  return 0; // NPC haben keine Todesfolgen
}

// kein 2. Leben fuer Nicht-Spieler. ;-)
varargs protected int second_life( object corpse ) {
    return 0;
}

public varargs void die( int poisondeath, int extern )
{   object corpse;
    string die_msg, tmp;
    mixed res;
    mixed hookData;
    mixed hookRes;

    if ( !objectp(this_object()) || QueryProp(P_GHOST) )
        return; // Ghosts dont die ...

    // direkt von extern aufgerufen und nicht ueber heart_beat() oder
    // do_damage() hierher gelangt?
    if (extern_call() && previous_object() != this_object()) {
      extern=1;
      SetProp(P_KILLER, previous_object());
    }

    if ( res = QueryProp(P_TMP_DIE_HOOK) ){
        if ( pointerp(res) && sizeof(res)>=3
            && intp(res[0]) && time()<res[0]
            && objectp(res[1]) && stringp(res[2]) )
        {
            if ( res = ({mixed})call_other( res[1], res[2], poisondeath ) ) {
              SetProp(P_KILLER,0);
              return;
            }
        }
        else
            SetProp(P_TMP_DIE_HOOK,0);
    }

    // trigger die hook
    hookData=poisondeath;
    hookRes=HookFlow(H_HOOK_DIE,hookData);
    if (pointerp(hookRes) && sizeof(hookRes)>H_RETDATA){
      if(hookRes[H_RETCODE]==H_CANCELLED) {
        SetProp(P_KILLER,0);
        return;
      }
      else if (hookRes[H_RETCODE]==H_ALTERED)
          poisondeath = hookRes[H_RETDATA];
    }

    if ( IS_LEARNING(ME) && query_once_interactive(ME) ){
       tell_object( ME, "Sei froh dass Du unsterblich bist, sonst waere es "
                        "eben Dein Ende gewesen.\n");
       SetProp(P_KILLER,0);
       return;
    }

    // Gegner befrieden.
    map_objects( QueryEnemies()[0], "StopHuntFor", ME, 1 );
    StopHuntingMode(1);

    // Falls die() direkt aufgerufen wurde und dies ein Spieler ist, muss das
    // die() noch Eintraege in /log/KILLS via create_kill_log_entry bzw. in
    // /log/KILLER erstellen.
    if ( query_once_interactive(ME) && extern )
    {
      object killer = QueryProp(P_KILLER) 
                     || previous_object() || this_interactive() || this_player();
      if ( killer && !query_once_interactive(killer) )
      {
          tmp = explode( object_name(killer), "#")[0] + " (direkt !)";

          create_kill_log_entry( tmp + " (" + REAL_UID(killer) + ")", killer );
      }
      else if ( killer && !QueryProp(P_TESTPLAYER) && !IS_LEARNER(ME) )
      {
          log_file( "KILLER", sprintf( "%s %s (%d/%d) toetete %s (%d/%d)\n",
                                       ctime(time()),
                                       capitalize(getuid(killer)),
                                       query_wiz_level(killer),
                                       ({int})killer->QueryProp(P_LEVEL),
                                       capitalize(getuid(ME)),
                                       query_wiz_level(ME),
                                       QueryProp(P_LEVEL) ) );

          ({int})killer->SetProp( P_KILLS, -1 );
      }
    }

  // Bei NPC EKs vergeben und ggf. in der Gilde des Killers und im Raum
  // NPC_Killed_By() rufen.
  if ( !query_once_interactive(ME) )
  {
    object killer = (({object}) QueryProp(P_KILLER)) || previous_object() ||
      this_interactive() || this_player();

    if ( killer && query_once_interactive(killer) )
    {
      if (stringp(res=({string})killer->QueryProp(P_GUILD))
          && objectp(res=find_object("/gilden/"+res)))
        ({void})res->NPC_Killed_By(killer);

      if (environment())
          ({void})environment()->NPC_Killed_By(killer);

      res = QueryProp(P_XP);
      res = (res < SCORE_LOW_MARK) ? 0 : ((res > SCORE_HIGH_MARK) ? 2 : 1);
      if ( !QueryProp(P_NO_SCORE) && !IS_LEARNER(killer) &&
           // !killer->QueryProp(P_TESTPLAYER) &&
           pointerp( res = ({mixed})SCOREMASTER->QueryNPC(res)) )
        GiveKillScore( killer, res[0] );
    }
  }

  if( !(die_msg = QueryProp(P_DIE_MSG)) )
    if (QueryProp(P_PLURAL))
      die_msg = " fallen tot zu Boden.\n";
    else
      die_msg = " faellt tot zu Boden.\n";

  if ( poisondeath )
  {
      Set( P_LAST_DAMTYPES, ({ DT_POISON }) );
      Set( P_LAST_DAMTIME, time() );
      Set( P_LAST_DAMAGE, 1 );
      die_msg = " wird von Gift hinweggerafft und kippt um.\n";
  }

  say( capitalize(name(WER,1)) + die_msg );

  // Wenn keine Leiche, dann Kram ins Env legen.
  if ( QueryProp(P_NOCORPSE) || !(tmp = QueryProp(P_CORPSE))
      || catch(corpse = clone_object(tmp);publish) 
      || !objectp(corpse) )
  {
      // Magier oder Testspieler behalten ihre Ausruestung.
      // Sonst kaemen u.U. Spieler an Magiertools etc. heran
      if ( !(IS_LEARNER(ME) || (tmp = Query(P_TESTPLAYER)) &&
             (!stringp(tmp) || IS_LEARNER( lower_case(tmp) ))) )
          transfer_all_to( environment(), 0 );
      else
          // Aber sie ziehen sich aus.
          filter_objects(QueryProp(P_ARMOURS),"DoUnwear",M_NOCHECK,0);
  }
  else
  // sonst in die Leiche legen.
  {
      ({void})corpse->Identify(ME);
      ({int})corpse->move( environment(), M_NOCHECK|M_SILENT );
      // Magier oder Testspieler behalten ihre Ausruestung.
      // Sonst kaemen u.U. Spieler an Magiertools etc. heran
      if ( !(IS_LEARNER(ME) || (tmp = Query(P_TESTPLAYER)) &&
             (!stringp(tmp) || IS_LEARNER( lower_case(tmp) ))) )
          transfer_all_to( corpse, !query_once_interactive(ME) );
      else
          // Aber sie ziehen sich aus.
          filter_objects(QueryProp(P_ARMOURS),"DoUnwear",M_NOCHECK,0);
  }

  if ( query_once_interactive(ME) ) {
      Set( P_DEADS, Query(P_DEADS) + 1 );
      // Spieler-Tod-event ausloesen
      ({int})EVENTD->TriggerEvent(EVT_LIB_PLAYER_DEATH, ([
      E_OBJECT: ME, E_PLNAME: getuid(ME),
      E_ENVIRONMENT: environment(), E_TIME: time(),
      P_KILLER: QueryProp(P_KILLER),
      P_LAST_DAMAGE: QueryProp(P_LAST_DAMAGE),
      P_LAST_DAMTYPES: copy(QueryProp(P_LAST_DAMTYPES)),
      E_EXTERNAL_DEATH: extern,
      E_POISON_DEATH: poisondeath, 
      E_CORPSE: (objectp(corpse)?corpse:0) ]) );
  }
  else {
      // NPC-Todes-Event ausloesen. Div. Mappings/Arrays werden nicht kopiert,
      // weil der NPC ja jetzt eh zerstoert wird.
      mapping data = ([
            E_OBNAME: object_name(ME),
            E_ENVIRONMENT: environment(), E_TIME: time(),
            P_NAME: QueryProp(P_NAME),
            P_KILLER: QueryProp(P_KILLER),
            P_ENEMY_DAMAGE: QueryProp(P_ENEMY_DAMAGE),
            P_LAST_DAMAGE: QueryProp(P_LAST_DAMAGE),
            P_LAST_DAMTYPES: QueryProp(P_LAST_DAMTYPES),
            E_EXTERNAL_DEATH: extern,
            E_POISON_DEATH: poisondeath,
            E_CORPSE: (objectp(corpse)?corpse:0),
            P_XP: QueryProp(P_XP),
            P_ATTRIBUTES: QueryProp(P_ATTRIBUTES),
            P_MAX_HP: QueryProp(P_MAX_HP),
            P_HANDS: QueryProp(P_HANDS),
            P_ALIGN: QueryProp(P_ALIGN),
            P_RACE: QueryProp(P_RACE),
            P_CLASS: QueryProp(P_CLASS),
            ]);
      ({int})EVENTD->TriggerEvent(EVT_LIB_NPC_DEATH(""), data);
      ({int})EVENTD->TriggerEvent(
          EVT_LIB_NPC_DEATH(load_name(ME)), data);
  }

  // transfer_all_to() ist evtl. (wenn zuviele Objekte bewegt werden mussten)
  // noch nicht ganz fertig und wird per call_out() den Rest erledigen.
  // Sollte die Leiche dann nicht mehr existieren, verbleiben die restlichen
  // Objekte im Spieler.
  // Es bleiben aber auf jeden Fall noch rund 300k Eval-Ticks ueber, damit
  // kein Spieler dank "evalcost too high" ungeschoren davon kommt.
  if ( !(second_life(corpse)) )
  {
      Set( P_GHOST, 1 ); // Fuer korrekte Ausgabe auf Teamkanal.

      if ( find_call_out(#'_transfer) == -1 )
          // Falls kein call_out() mehr laeuft, sofort destructen ...
          remove();
      else
          // ... ansonsten vormerken
          remove_me = 1;
  }
}

public void heal_self(int h)
{
  if ( h<=0 )
    return;
  SetProp(P_HP, QueryProp(P_HP)+h);
  SetProp(P_SP, QueryProp(P_SP)+h);
}


//--------------------------------------------------------------------------
//
//   int defuel_food( /* void */ )
//
//   Enttankt den Spieler um einen gewissen Essens-Wert.
//   Sollte nur von Toiletten aufgerufen werden.
//
//--------------------------------------------------------------------------
public int defuel_food()
{
  int food;

  food=QueryProp(P_FOOD);

// wenn spieler kein food hat: return 0
  if ( !food )
   return NO_DEFUEL;

// wenn spieler unter enttank-grenze: return -1
  if ( food < QueryProp(P_DEFUEL_LIMIT_FOOD) )
   return DEFUEL_TOO_LOW;

// wenn letztes enttanken nicht lange genug zurueckliegt: return -2
  if ( time() < nextdefueltimefood )
   return DEFUEL_TOO_SOON;

  food=to_int(((food*QueryProp(P_DEFUEL_AMOUNT_FOOD))/2));
  food+=random(food);

// sicherheitshalber
  if ( food > QueryProp(P_FOOD) )
   food=QueryProp(P_FOOD);

  SetProp(P_FOOD,(QueryProp(P_FOOD)-food));

  nextdefueltimefood=time()+QueryProp(P_DEFUEL_TIME_FOOD);

  return food;
}


//--------------------------------------------------------------------------
//
//   int defuel_drink( /* void */ )
//
//   Enttankt den Spieler um einen gewissen Fluessigkeits-Wert.
//   Gleichzeitig wird eine gewisse Menge Alkohol reduziert.
//   Sollte nur von Toiletten aufgerufen werden.
//
//--------------------------------------------------------------------------
public int defuel_drink()
{
  int alc, drink;

  drink=QueryProp(P_DRINK);

// wenn spieler kein drink hat: return 0
  if ( !drink )
   return NO_DEFUEL;

// wenn spieler unter enttank-grenze: return -1
  if ( drink < QueryProp(P_DEFUEL_LIMIT_DRINK) )
   return DEFUEL_TOO_LOW;

// wenn letztes enttanken nicht lange genug zurueckliegt: return -2
  if ( time() < nextdefueltimedrink )
   return DEFUEL_TOO_SOON;
    
  drink=to_int(((drink*QueryProp(P_DEFUEL_AMOUNT_DRINK))/2));
  drink+=random(drink);

// sicherheitshalber
  if ( drink > QueryProp(P_DRINK) )
   drink=QueryProp(P_DRINK);

  SetProp(P_DRINK,(QueryProp(P_DRINK)-drink));

// jedes fluessige Enttanken macht auch etwas nuechterner :^)
// bei sehr kleinen Mengen enttankt man keinen Alkohol
// ansonsten in Abhaengigkeit von enttankter Menge, P_ALCOHOL und P_WEIGHT

  if ( drink > 9 && QueryProp(P_ALCOHOL) > 0 )
   {
    alc=(to_int(exp(log(1.1)*(drink)))*
         to_int(exp(log(0.67)*(QueryProp(P_ALCOHOL)))))/
         (QueryProp(P_MAX_DRINK)*QueryProp(P_MAX_ALCOHOL))*
         (to_int(QueryProp(P_WEIGHT)/1000));

     SetProp(P_ALCOHOL,QueryProp(P_ALCOHOL)-(alc+random(alc)));
   }

  nextdefueltimedrink=time()+QueryProp(P_DEFUEL_TIME_DRINK);
 
  return drink;
}


public void reduce_spell_points(int h)
{
  SetProp(P_SP, QueryProp(P_SP)-h);
}

public void restore_spell_points(int h)
{
  SetProp(P_SP, QueryProp(P_SP)+h);
}

/* Reduce hitpoints. Log who is doing it. */
public int reduce_hit_points(int dam)
{ object o;
  int i;

#ifdef LOG_REDUCE_HP
  if (this_player()!=ME)
  {
    log_file("REDUCE_HP", name()+" by ");
    if(!this_player()) log_file("REDUCE_HP","?\n");
    else {
      log_file("REDUCE_HP",({string})this_player()->name());
      o=previous_object();
      if (o)
        log_file("REDUCE_HP", " " + object_name(o) + ", " +
                 ({string})o->name(WER,0) + " (" + creator(o) + ")\n");
      else
        log_file("REDUCE_HP", " ??\n");
    }
  }
#endif
  if ((i=QueryProp(P_HP)) <= dam)
    return SetProp(P_HP,1);
  return SetProp(P_HP, i - dam);
}

public int restore_hit_points(int heal)
{
  return reduce_hit_points(-heal);
}

public varargs int drink_alcohol(int strength,int testonly, string mytext)
{ int alc,add,res;

  add=ALCOHOL_VALUE(strength);
  res=UseSkill(SK_BOOZE,([
      SI_SKILLARG : add,
      SI_TESTFLAG : 1])); // Kann der Spieler gut saufen?
  if (intp(res) && res>0) add=res;
  alc=QueryProp(P_ALCOHOL)+add;
  if ((alc >= QueryProp(P_MAX_ALCOHOL)) && !IS_LEARNING(this_object())){
    if(!testonly)
      tell_object(ME,mytext||"So ein Pech, Du hast alles verschuettet.\n");
    return 0;
  }
  if(testonly)return 1;
  UseSkill(SK_BOOZE,([ SI_SKILLARG : ALCOHOL_VALUE(strength) ]));
  if(alc < 0) alc = 0;
  if(!alc) tell_object(ME, "Du bist stocknuechtern.\n");
  SetProp(P_ALCOHOL, alc);
  return 1;
}

public varargs int drink_soft(int strength, int testonly, string mytext)
{ int soaked;

  soaked = QueryProp(P_DRINK);
  if((soaked + strength > QueryProp(P_MAX_DRINK)) &&
     !IS_LEARNING(this_object())){
    if(!testonly)
     tell_object(ME, mytext||
       "Nee, so viel kannst Du momentan echt nicht trinken.\n" );
    return 0;
  }
  if(testonly)return 1;
  if((soaked += DRINK_VALUE(strength)) < 0) soaked = 0;
  if(!soaked) tell_object(ME, "Dir klebt die Zunge am Gaumen.\n");
  SetProp(P_DRINK, soaked);
  return 1;
}

public varargs int eat_food(int strength, int testonly, string mytext)
{ int stuffed;

  stuffed = QueryProp(P_FOOD);
  if ((stuffed + strength > QueryProp(P_MAX_FOOD)) &&
      !IS_LEARNING(this_object()))
  {
    if(!testonly)
        tell_object(ME,
            mytext || "Das ist viel zu viel fuer Dich! Wie waers mit etwas "
            "leichterem?\n");
    return 0;
  }
  if(testonly)return 1;
  stuffed += FOOD_VALUE(strength);
  if(stuffed < 0) stuffed = 0;
  if(!stuffed) tell_object(ME, "Was rumpelt denn da in Deinem Bauch?\n");
  SetProp(P_FOOD, stuffed);
  return 1;
}

public int buffer_hp(int val,int rate)
{
  int dif;

  if(val<=0 || rate<=0)return 0;
  if(val < rate)rate = val;
  if(rate>20) rate=20;

  /* Check for BufferOverflow */
  if((dif=(hp_buffer[0]+val)-QueryProp(P_MAX_HP)) > 0)val-=dif;
  if(val<=0)return 0;

  hp_buffer[0] += val;
  hp_buffer[1+rate] += val;
  if(rate > hp_buffer[1])hp_buffer[1] = rate;

  return hp_buffer[0];
}

public int buffer_sp(int val,int rate)
{
  int dif;

  if(val<=0 || rate<=0)return 0;
  if(val < rate)rate = val;
  if(rate>20) rate=20;

  /* Check for BufferOverflow */
  if((dif=(sp_buffer[0]+val)-QueryProp(P_MAX_SP)) > 0)val-=dif;
  if(val<=0)return 0;

  sp_buffer[0] += val;
  sp_buffer[1+rate] += val;
  if(rate > sp_buffer[1])sp_buffer[1] = rate;

  return sp_buffer[0];
}

protected void update_buffers()
{ int i, rate, max;

  rate=0;
  max=0;
  for(i=1;i<=20;i++){
    if(member(hp_buffer, i+1))
      if(hp_buffer[i+1]<=0)
        hp_buffer = m_delete(hp_buffer,i+1);
      else{
        max+=hp_buffer[i+1];
        rate=i;
      }
  }

  hp_buffer[0]=max;
  hp_buffer[1]=rate;
  rate=0;
  max=0;
  for(i=1;i<=20;i++){
    if(member(sp_buffer, i+1))
      if(sp_buffer[i+1]<=0)
        sp_buffer = m_delete(sp_buffer,i+1);
      else{
        max+=sp_buffer[i+1];
        rate=i;
      }
  }
  sp_buffer[0]=max;
  sp_buffer[1]=rate;
}

public int check_timed_key(string key) {

  // keine 0 als key (Typ wird per RTTC geprueft)
  if (!key)
    return 0;
  mapping tmap=Query(P_TIMING_MAP, F_VALUE);
  if (!mappingp(tmap)) {
    tmap=([]);
    Set(P_TIMING_MAP, tmap, F_VALUE);
  }
  // Wenn key noch nicht abgelaufen, Ablaufzeitpunkt zurueckgeben.
  // Sonst 0 (key frei)
  return (time() < tmap[key]) && tmap[key];
}

public int check_and_update_timed_key(int duration,string key) {

  // wenn key noch gesperrt, die zeit der naechsten Verfuegbarkeit
  // zurueckgeben.
  int res = check_timed_key(key);
  if (res) {
    return res;
  }

  // duration <= 0 ist unsinnig. Aber key ist nicht mehr gesperrt, d.h. time()
  // ist ein sinnvoller Rueckgabewert.
  if (duration <= 0)
    return time();
 
  mapping tmap = Query(P_TIMING_MAP,F_VALUE);
  tmap[key]=time()+duration;
  
  // speichern per SetProp() unnoetig, da man das Mapping direkt aendert,
  // keine Kopie.
  //SetProp(P_TIMING_MAP, tmap);
  
  return -1; // Erfolg.
}

protected void expire_timing_map() {
  
  mapping tmap = Query(P_TIMING_MAP, F_VALUE);
  if (!mappingp(tmap) || !sizeof(tmap))
    return;
  foreach(string key, int endtime: tmap) {
    if (endtime < time())
      m_delete(tmap, key);
  }
  // speichern per SetProp() unnoetig, da man das Mapping direkt aendert,
  // keine Kopie.
}

protected void heart_beat()
{
    if ( !this_object() )
        return;

    attribute_hb();

    // Als Geist leidet man nicht unter so weltlichen Dingen wie
    // Alkohol, Gift&Co ...
    if ( QueryProp(P_GHOST) )
        return;

    int hpoison = QueryProp(P_POISON);
    int rlock = QueryProp(P_NO_REGENERATION);
    int hp = QueryProp(P_HP);
    int sp = QueryProp(P_SP);
    int alc;

    // Wenn Alkohol getrunken: Alkoholauswirkungen?
    if ( (alc = QueryProp(P_ALCOHOL)) && !random(40) ){
        int n;
        string gilde;
        object ob;

        n = random( 5 * (alc - 1)/QueryProp(P_MAX_ALCOHOL) );

        switch (n){
        case ALC_EFFECT_HICK:
            say( capitalize(name( WER, 1 )) + " sagt: <Hick>!\n" );
            write( "<Hick>! Oh, Tschuldigung.\n" );
            break;
            
        case ALC_EFFECT_STUMBLE:
            say( capitalize(name( WER, 1 )) + " stolpert ueber " +
                 QueryPossPronoun( FEMALE, WEN ) + " Fuesse.\n" );
            write( "Du stolperst.\n" );
            break;
            
        case ALC_EFFECT_LOOKDRUNK:
            say( capitalize(name( WER, 1 )) + " sieht betrunken aus.\n" );
            write( "Du fuehlst Dich benommen.\n" );
            break;
            
        case ALC_EFFECT_RUELPS:
            say( capitalize(name( WER, 1 )) + " ruelpst.\n" );
            write( "Du ruelpst.\n" );
            break;
        }
       
        // Gilde und Environment informieren ueber Alkoholauswirkung.
        if ( stringp(gilde = QueryProp(P_GUILD))
             && objectp(ob = find_object( "/gilden/" + gilde )) )
            ({void})ob->InformAlcoholEffect( ME, n, ALC_EFFECT_AREA_GUILD );
        
        if ( environment() )
            ({void})environment()->InformAlcoholEffect( ME, n, ALC_EFFECT_AREA_ENV );
    }
    
    // Alkohol abbauen und etwas extra heilen, falls erlaubt.
    if ( alc && (--delay_alcohol < 0)
         && !(rlock & NO_REG_ALCOHOL) ){

        SetProp( P_ALCOHOL, alc - 1 );
        
        if ( !hpoison ){
            hp++;
            sp++;
        }
        
        delay_alcohol = QueryProp(P_ALCOHOL_DELAY);
    }

    // P_DRINK reduzieren, falls erlaubt.
    if ( (--delay_drink < 0) && !(rlock & NO_REG_DRINK) ){
        delay_drink = QueryProp(P_DRINK_DELAY);
        SetProp( P_DRINK, QueryProp(P_DRINK) - 1 );
    }

    // P_FOOD reduzieren, falls erlaubt.
    if ( (--delay_food < 0) && !(rlock & NO_REG_FOOD) ){
        delay_food = QueryProp(P_FOOD_DELAY);
        SetProp( P_FOOD, QueryProp(P_FOOD) - 1 );
    }

    // Regeneration aus dem HP-Puffer
    // Hierbei wird zwar nur geheilt, wenn das erlaubt ist, aber der Puffer
    // muss trotzdem abgearbeitet/reduziert werden, da Delfen sonst eine
    // mobile Tanke kriegen. (Keine Heilung im Hellen, Puffer bleibt sonst
    // konstant und kann bei Bedarf ueber dunkelmachende Items abgerufen
    // werden.)
    int val;
    if (hp_buffer[0]) {
        int rate = hp_buffer[1];
        val = hp_buffer[rate + 1];
    
        if ( val > rate )
            val = rate;
        hp_buffer[0] -= val;
        hp_buffer[rate + 1] -= val;
        if ( hp_buffer[rate + 1] <= 0 )
            update_buffers();
    }
    // Jetzt Regeneration aus dem Puffer durchfuehren, aber nur wenn erlaubt.
    if ( val && !(rlock & NO_REG_BUFFER_HP) )
        hp += val; 
    // normales Heilen, falls keine Regeneration aus dem Puffer erfolgte und
    // es erlaubt ist.
    else if ( (--delay_heal < 0) && !(rlock & NO_REG_HP) ){
        delay_heal = QueryProp(P_HP_DELAY);
        if ( !hpoison )
            hp++;
    }

    // Gleiches Spiel jetzt fuer den SP-Puffer (s.o.)
    val=0;
    if ( sp_buffer[0] ) {
        int rate = sp_buffer[1];
        val = sp_buffer[rate + 1];
        
        if ( val > rate )
            val = rate;
        
        sp_buffer[0] -= val;
        sp_buffer[rate + 1] -= val;
 
        if ( sp_buffer[rate + 1] <= 0 )
            update_buffers();
    }
    // Regeneration erlaubt?
    if ( val && !(rlock & NO_REG_BUFFER_SP) )
         sp += val;
    // Wenn nicht, normales Hochideln versuchen.
    else if ( (--delay_sp < 0) && !(rlock & NO_REG_SP) ){
        delay_sp = QueryProp(P_SP_DELAY);
        if ( !hpoison )
            sp++;
    }

    if ( hpoison && (interactive(ME) || !query_once_interactive(ME)) ){
        // Vanion, 26.10.03
        // Wenn _set_poison() per SET_METHOD ueberschrieben wird, kann
        // nicht sichergestellt werden, dass poison immer groesser 0 ist
        // Daher muss hier ein Test rein, so teuer das auch ist :(
        if (--hpoison < 0)
          hpoison=0;
        
        if ( --delay_poison < 0 ){
            delay_poison = QueryProp(P_POISON_DELAY)
                + random(POISON_MERCY_DELAY);
            hp -= hpoison;

            if ( hp < 0 ){
                tell_object( ME, "Oh weh - das Gift war zuviel fuer Dich!\n"
                             + "Du stirbst.\n" );
                
                if ( query_once_interactive(ME) ){
                    create_kill_log_entry( "Vergiftung", 0 );
                    
                    // Beim Gifttod gibt es keinen Killer. Aber auf diese Art
                    // erkennt der Todesraum die Ursache korrekt und gibt die
                    // richtige Meldung aus.
                    SetProp( P_KILLER, "gift" );
                }
                
                die(1);
                return;
            }
            
            if ( (hpoison < 3 || !query_once_interactive(ME) )
                 && --drop_poison < 0)
            {
                // Giftlevel eins reduzieren. hpoison wurde oben schon
                // reduziert, d.h. einfach hpoison in P_POISON schreiben.
                // dabei wird dann auch ggf. drop_poison richtig gesetzt.
                SetProp( P_POISON, hpoison );
                if ( !hpoison )
                    tell_object( ME, "Du scheinst die Vergiftung "
                                     "ueberwunden zu haben.\n" );  
            }
        }
        
        if ( hpoison && !random(15) )
            switch ( hp*100/QueryProp(P_MAX_HP) ){
            case 71..100 :
                write( "Du fuehlst Dich nicht gut.\n" );
                say( capitalize(name(WER)) +
                     " sieht etwas benommen aus.\n" );
                break;
                
            case 46..70 :
                write( "Dir ist schwindlig und Dein Magen revoltiert.\n" );
                say( capitalize(name(WER)) + " taumelt ein wenig.\n" );
                break;
                
            case 26..45 :
                write( "Dir ist heiss. Du fuehlst Dich schwach. Kopfweh "
                       "hast Du auch.\n" );
                say( capitalize(name(WER)) + " glueht direkt und scheint "
                     "grosse Schwierigkeiten zu haben.\n" );
                break;
                
            case 11..25 :
                write( "Du fuehlst Dich beschissen. Alles tut weh, und Du "
                       "siehst nur noch unscharf.\n" );
                say( capitalize(name(WER)) + " taumelt und stoehnt und "
                     "kann gerade noch vermeiden hinzufallen.\n" );
                break;
                
            case 0..10 :
                write( break_string( "Du siehst fast nichts mehr und kannst "
                                     "Dich nur noch unter groessten Schmerzen "
                                     "bewegen. Aber bald tut nichts mehr weh"
                                     "...", 78 ) );
                say( break_string( capitalize(name(WER)) + " glueht wie "
                                   "im Fieber, kann sich kaum noch ruehren "
                                   "und hat ein schmerzverzerrtes Gesicht.\n",
                                   78 ) );
                break;
            }
    }

    SetProp( P_HP, hp );
    SetProp( P_SP, sp );
}

public int AddExp( int e )
{
  int experience;
  string fn;
  mixed last;

  experience = QueryProp(P_XP);

  if ( QueryProp(P_KILLS) > 1 && e > 0 )
      return experience;

  fn = implode( explode( object_name( environment() || this_object() ),
                               "/" )[0..<2], "/" );

  if ( pointerp(last = Query(P_LAST_XP)) && sizeof(last) == 2 && last[0] == fn )
      Set( P_LAST_XP, ({ fn, last[1]+e }) );
  else
      Set( P_LAST_XP, ({ fn, e }) );

  if ( (experience += e) < 0 )
      experience = 0;

  return SetProp( P_XP, experience );
}

static <string|int>* _set_last_xp( <int|string>* last )
{
    if ( !pointerp(last) || sizeof(last) != 2 || !stringp(last[0]) ||
         !intp(last[1]) )
        return Query(P_LAST_XP);
    else
        return Set( P_LAST_XP, last );
}


static int _set_align(int a)
{
  if (a<-1000) a = -1000;
  if (a>1000) a = 1000;
  return Set(P_ALIGN, a);
}


static int _set_hp( int hp )
{
    if ( QueryProp(P_GHOST) )
        return QueryProp(P_HP);

    if ( hp < 0 )
        return Set( P_HP, 0 );

    if ( hp > QueryProp(P_MAX_HP) )
        return Set( P_HP, QueryProp(P_MAX_HP), F_VALUE );

    return Set( P_HP, hp, F_VALUE );
}

static int _set_sp( int sp )
{
    if ( QueryProp(P_GHOST) )
        QueryProp(P_SP);

    if ( sp < 0 )
        return Set( P_SP, 0 );

    if ( sp > QueryProp(P_MAX_SP) )
        return Set( P_SP, QueryProp(P_MAX_SP), F_VALUE );

    return Set( P_SP, sp, F_VALUE );
}

static int _set_alcohol(int n)
{
  if (QueryProp(P_GHOST))
    return Query(P_ALCOHOL, F_VALUE);

  // nur Aenderungen und Werte >=0 werden gesetzt...
  n = n < 0 ? 0 : n;
  int old = Query(P_ALCOHOL, F_VALUE);
  if ( old == n)
    return old;

  // Hooks aufrufen
  int *ret = HookFlow(H_HOOK_ALCOHOL, n);
  // Bei Abbruch alten Wert zurueckgeben
  switch (ret[H_RETCODE]) {
    case H_CANCELLED:
      return old;
    case H_ALTERED:
      // sonst neuen Wert setzen
      if(!intp(ret[H_RETDATA]))
          raise_error(sprintf(
            "_set_alcohol(): data from HookFlow() != <int>: %.50O\n",
            ret[H_RETDATA]));
      n = ret[H_RETDATA];
      n = n < 0 ? 0 : n;

    // H_NO_MOD is fallthrough
  }

  return Set(P_ALCOHOL, n, F_VALUE);
}

static int _set_drink(int n)
{
  if (QueryProp(P_GHOST))
    return Query(P_DRINK, F_VALUE);

  // nur Aenderungen und Werte >=0 werden gesetzt...
  n = n < 0 ? 0 : n;
  int old = Query(P_DRINK, F_VALUE);
  if ( old == n)
    return old;

  // Hooks aufrufen
  int *ret = HookFlow(H_HOOK_DRINK, n);
  // Bei Abbruch alten Wert zurueckgeben
  switch (ret[H_RETCODE]) {
    case H_CANCELLED:
      return old;
    case H_ALTERED:
      // sonst neuen Wert setzen
      if(!intp(ret[H_RETDATA]))
          raise_error(sprintf(
            "_set_drink(): data from HookFlow() != <int>: %.50O\n",
            ret[H_RETDATA]));
      n = ret[H_RETDATA];
      n = n < 0 ? 0 : n;

    // H_NO_MOD is fallthrough
  }

  return Set(P_DRINK, n, F_VALUE);
}

static int _set_food(int n)
{
  if (QueryProp(P_GHOST))
    return Query(P_FOOD, F_VALUE);

  // nur Aenderungen und Werte >=0 werden gesetzt...
  n = n < 0 ? 0 : n;
  int old = Query(P_FOOD, F_VALUE);
  if ( old == n)
    return old;

  // Hooks aufrufen
  int *ret = HookFlow(H_HOOK_FOOD, n);
  // Bei Abbruch alten Wert zurueckgeben
  switch (ret[H_RETCODE]) {
    case H_CANCELLED:
      return old;
    case H_ALTERED:
      // sonst neuen Wert setzen
      if(!intp(ret[H_RETDATA]))
          raise_error(sprintf(
            "_set_food(): data from HookFlow() != <int>: %.50O\n",
            ret[H_RETDATA]));
      n = ret[H_RETDATA];
      n = n < 0 ? 0 : n;

    // H_NO_MOD is fallthrough
  }

  return Set(P_FOOD, n, F_VALUE);
}

static int _set_poison(int n)
{
  if (QueryProp(P_GHOST))
    return Query(P_POISON, F_VALUE);

  int mp = QueryProp(P_MAX_POISON);
  n = (n<0 ? 0 : (n>mp ? mp : n));

  // nur >=0 zulassen.
  n = n < 0 ? 0 : n;

  int old = Query(P_POISON, F_VALUE);
  if ( old == 0 && n == 0)
    return old;

  // Hooks aufrufen
  int *ret = HookFlow(H_HOOK_POISON, n);
  // Bei Abbruch alten Wert zurueckgeben
  switch (ret[H_RETCODE]) {
    case H_CANCELLED:
      return old;
    case H_ALTERED:
      // sonst neuen Wert setzen
      if(!intp(ret[H_RETDATA]))
          raise_error(sprintf(
            "_set_poison(): data from HookFlow() != <int>: %.50O\n",
            ret[H_RETDATA]));
      n = ret[H_RETDATA];
      n = n < 0 ? 0 : n;

    // H_NO_MOD is fallthrough
  }

  // Fuer die Selbstheilung.
  switch(n) {
  case 1:
    drop_poison = 40+random(16);
    break;
  case 2:
    drop_poison = 25+random(8);
    break;
  case 3:
    drop_poison = 18+random(4);
    break;
  default:
    // nur relevant fuer NPC, da Spieler bei >3 Gift nicht mehr regegenieren.
    drop_poison = 22 - 2*n + random(43 - 3*n);
    break;
  }

  // fuer Setzen der Prop von aussen ein Log schreiben
  if (previous_object(1) != ME)
    log_file("POISON", sprintf("%s - %s: %d von %O (%s)\n",
           dtime(time())[5..],
           (query_once_interactive(this_object()) ?
             capitalize(geteuid(this_object())) :
             capitalize(name(WER))),
           n,
           (previous_object(2) ? previous_object(2) : previous_object(1)),
           (this_player() ? capitalize(geteuid(this_player())) : "???")));

  return Set(P_POISON, n, F_VALUE);
}

static int _set_xp(int xp) { return Set(P_XP, xp < 0 ? 0 : xp, F_VALUE); }

static mixed _set_die_hook(mixed hook)
{
  if(hook && query_once_interactive(this_object()))
    log_file("DIE_HOOK",
             sprintf("%s : DIE_HOOK gesetzt von %O in %O (%s)\n",
                     dtime(time())[5..],
                     (previous_object(2) ? previous_object(2):previous_object(1)),
                     this_object(),getuid(this_object())));
  return Set(P_TMP_DIE_HOOK,hook, F_VALUE);
}

static mapping _query_enemy_damage()
{
  return copy(enemy_damage);
}

// nur ne Kopie liefern, sonst kann das jeder von aussen aendern.
static mapping _query_timing_map() {
  return copy(Query(P_TIMING_MAP));
}

/****************************************************************************
 * Consume-Funktion, um zentral durch konsumierbare Dinge ausgeloeste
 * Aenderungen des Gesundheitszustandes herbeizufuehren.
 ***************************************************************************/

/* Konsumiert etwas
 *
 * Rueckgabewert
 *      1       erfolgreich konsumiert
 *      0       fehlende oder falsche Parameter
 *     <0       Bedingung fuer konsumieren nicht erfuellt, Bitset aus:
 *      1       Kann nichts mehr essen
 *      2       Kann nichts mehr trinken
 *      4       Kann nichts mehr saufen
 *      8       Abgebrochen durch Hook H_HOOK_CONSUME
 */
public varargs int consume(mapping cinfo, int testonly)
{
  int retval = 0;
  // nur was tun, wenn auch Infos reinkommen
  if (mappingp(cinfo) && sizeof(cinfo)) {
    // Hooks aufrufen, sie aendern ggf. noch was in cinfo.
    mixed *hret = HookFlow(H_HOOK_CONSUME, ({cinfo, testonly}) );
    switch(hret[H_RETCODE])
    {
        case H_CANCELLED:
          return -HC_HOOK_CANCELLATION;
        case H_ALTERED:
          // testonly kann nicht geaendert werden.
          cinfo = hret[H_RETDATA][0];
    }
    // Legacy-Mappings (flache) neben strukturierten Mappings zulassen
    // flache Kopien erzeugen (TODO?: und fuer Teilmappings nicht relevante
    // Eintraege loeschen)
    mapping conditions;
    if (mappingp(cinfo[H_CONDITIONS])) {
      conditions = copy(cinfo[H_CONDITIONS]);
    } else {
      conditions = copy(cinfo);
    }
    mapping effects;
    if (mappingp(cinfo[H_EFFECTS])) {
      effects = filter(cinfo[H_EFFECTS], (: member(H_ALLOWED_EFFECTS, $1) > -1 :));
    } else {
      effects = filter(cinfo, (: member(H_ALLOWED_EFFECTS, $1) > -1 :));
    }

    // Bedingungen pruefen
    if (mappingp(conditions) && sizeof(conditions)) {
      // Bedingungen fuer Konsum auswerten
      if (conditions[P_FOOD] && !eat_food(conditions[P_FOOD], 1))
        retval |= HC_MAX_FOOD_REACHED;
      else if (conditions[P_DRINK] && !drink_soft(conditions[P_DRINK], 1))
        retval |= HC_MAX_DRINK_REACHED;
      else if (conditions[P_ALCOHOL] && !drink_alcohol(conditions[P_ALCOHOL], 1))
        retval |= HC_MAX_ALCOHOL_REACHED;
      // retval negativ machen, damit Fehler leicht erkennbar ist
      retval = -retval;
    }
    // Bedingungen wurden abgearbeitet, jetzt die Heilung durchfuehren
    if (!retval) {
      if (!testonly) {
        // Bedingungen erfuellen, wenn alles passt und kein Test
        if (conditions[P_ALCOHOL])
          drink_alcohol(conditions[P_ALCOHOL]);
        if (conditions[P_DRINK])
          drink_soft(conditions[P_DRINK]);
        if (conditions[P_FOOD])
          eat_food(conditions[P_FOOD]);
        // Und jetzt die Wirkungen
        if (effects[P_POISON])
          SetProp(P_POISON, QueryProp(P_POISON) + effects[P_POISON]);
        // Und nun wirklich heilen
        switch (cinfo[H_DISTRIBUTION]) {
        case HD_INSTANT:
          map(effects, (: SetProp($1, QueryProp($1) + $2) :));
          break;
        case 1..50:
          buffer_hp(effects[P_HP], cinfo[H_DISTRIBUTION]);
          buffer_sp(effects[P_SP], cinfo[H_DISTRIBUTION]);
          break;
        default:
          buffer_hp(effects[P_HP], HD_STANDARD);
          buffer_sp(effects[P_SP], HD_STANDARD);
          break;
        }
      }
      retval = 1;
    }
  }
  return retval;
}
