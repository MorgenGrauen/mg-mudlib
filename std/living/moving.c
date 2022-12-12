// MorgenGrauen MUDlib
//
// living/moving.c -- moving of living objects
//
// $Id: moving.c 9448 2016-01-22 17:52:28Z Zesstra $
#pragma strict_types
#pragma save_types
#pragma range_check
#pragma no_clone

inherit "/std/thing/moving";

#define NEED_PROTOTYPES
#include <hook.h>
#include <living/moving.h>
#include <living/skills.h>
#include <thing/properties.h>
#include <thing/description.h>
#include <moving.h>
#include <new_skills.h>
#include <living.h>

#undef NEED_PROTOTYPES

#include <config.h>
#include <properties.h>
#include <language.h>
#include <wizlevels.h>
#include <defines.h>


protected void create()
{
    if (object_name(this_object()) == __FILE__[0..<3])
    {
      return;
    }
    offerHook(H_HOOK_MOVE,1);
}

public void AddPursuer(object ob)
{
  mixed *pur;

  if (!objectp(ob))
    return;

  if (!pointerp(pur=Query(P_PURSUERS)))
    pur=({0,({})});
  else if (member(pur[1],ob)!=-1)
    return;
  
  SetProp(P_PURSUERS,({ pur[0], pur[1]+({ob})-({0}) }));
  ({void})ob->_SetPursued(ME);
}

public void RemovePursuer(object ob)
{
  mixed *pur;

  if (pointerp(pur=Query(P_PURSUERS,F_VALUE)) 
      && member(pur[1],ob)!=-1)
  {
    pur[1]-=({ob,0});
    if (ob)
      ({void})ob->_RemovePursued(ME);
    if (!pur[0]&&!sizeof(pur[1]))
      pur=0;
    SetProp(P_PURSUERS,pur);
  }
}

public void _SetPursued(object ob)
{
  mixed *pur;

  if (!pointerp(pur=Query(P_PURSUERS)))
    pur=({0,({})});
  else
    if (objectp(pur[0]))
      ({void})pur[0]->RemovePursuer(ME);
  pur[0]=ob;
  pur[1]-=({0});
  Set(P_PURSUERS,pur);
}

public void _RemovePursued(object ob)
{
  mixed *pur;

  if (!pointerp(pur=Query(P_PURSUERS)) || pur[0]!=ob)
    return;
  pur[0]=0;
  pur[1]-=({0});
  if (!sizeof(pur[1]))
    pur=0;
  Set(P_PURSUERS,pur);
}


private void kampfende( object en ) {
  if (!objectp(en)) return;
  tell_object( ME, capitalize(({string})en->name()) +
      " ist jetzt hinter Dir her.\n" );
  tell_object( en, "Du verfolgst jetzt " + name(WEN) + ".\n" );      
  ({int})en->InsertSingleEnemy(ME);
}

private int _is_learner(object pl) {
  return IS_LEARNER(pl); 
}


// a) Pruefungen, ob das move erlaubt ist.
// b) zum Ueberschreiben
protected int PreventMove(object dest, object oldenv, int method) {

  // M_NOCHECK? -> Bewegung eh erlaubt (und Rueckgabewert wuerde ignoriert),
  // aber PreventInsert/PreventLeave() rufen und ignorieren.
  if ((method&M_NOCHECK)) {
      // erst PreventLeaveLiving() rufen...
      if(environment())        
          ({int})environment()->PreventLeaveLiving(this_object(), dest);
      // dann PreventInsertLiving() im Ziel-Env.
      ({int})dest->PreventInsertLiving(this_object());
      // und raus...
      return(0);
  }

  // bei Lebewesen muss die Bewegungsmethode M_GO und M_TPORT sein. Dies ist
  // gleichzeigt die Restriktion gegen das Nehmen von Lebewesen, da dort
  // M_GET/M_GIVE/M_PUT etc. verwendet wuerde. Bei M_GO und M_TPORT findet
  // keine Pruefung statt, ob das Objekt ins Ziel 'reinpasst' (Gewicht, Anzahl
  // Objekte usw.).
  // Ich finde es etwas merkwuerdig gebaut (Zesstra).
  if ( !(method & (M_GO | M_TPORT)) )
      return ME_PLAYER;
  
  // alte und neue Umgebung auf NO_TPORT pruefen.
  if ( (method & M_TPORT) ) {
    if ( environment() &&
        (({int})environment()->QueryProp(P_NO_TPORT) & (NO_TPORT_OUT|NO_TPORT)) )
          return ME_CANT_TPORT_OUT;
    else if ( ({int})dest->QueryProp(P_NO_TPORT) & (NO_TPORT_IN|NO_TPORT) )
          return ME_CANT_TPORT_IN;
  }

  // erst PreventLeaveLiving() testen...
  if( environment() && ({int})environment()->PreventLeaveLiving(this_object(), dest))
      return ME_CANT_LEAVE_ENV;
  // dann PreventInsertLiving() im Ziel-Env
  if (({int})dest->PreventInsertLiving(this_object())) 
      return ME_CANT_BE_INSERTED;

  return 0;
}

// Krams nach dem Move machen und nebenbei zum Ueberschreiben.
protected void NotifyMove(object dest, object oldenv, int method) {
  mixed res;
  object enem;

  // Begruessungsschlag fuer die Gegener
  if ( !(method & M_NO_ATTACK) )
      InitAttack();

  if (!objectp(ME)) return;

  // Verfolger nachholen.
  if ( pointerp(res = Query(P_PURSUERS)) && sizeof(res[1]) ) {
      while ( remove_call_out( "TakeFollowers" ) >= 0 );

      call_out( "TakeFollowers", 0 );
  }

  // und noch das Team nachholen.
  if ( oldenv != dest
      && objectp(ME)
      && QueryProp(P_TEAM_AUTOFOLLOW)
      && objectp( enem = IsTeamLeader() ) )
      ({void})enem->StartFollow(oldenv); // Teamverfolgung

}

varargs public int move( object|string dest, int method, string direction,
                         string textout, string textin )
{
    int para, invis, tmp;
    object oldenv;
    string fn,vc;
    mixed res;
    mixed hookData, hookRes;

    if (!objectp(dest) && !stringp(dest))
      raise_error(sprintf("Wrong argument 1 to move(). 'dest' must be a "
            "string or object! Argument was: %.100O\n",
            dest));

    // altes Env erstmal merken.
    oldenv = environment();
    
    //erstmal den richtigen Zielraum suchen, bevor irgendwelche Checks gemacht
    //werden...
    // Ist der Spieler in einer Parallelwelt?
    if ( (para = QueryProp(P_PARA)) && intp(para) ) {
        fn = objectp(dest) ? object_name(dest) : dest;

        // Falls der Zielraum nicht schon explizit in der Parallelwelt ist,
        // neuen Zielraum suchen. Aber nur, wenn fn kein # enthaelt (also kein
        // Clone ist), sonst wuerde eine Bewegung nach raum#42^para versucht,
        // was dann buggt. ;-) Problem wird offenbar, wenn ein Para-Lebewesen
        // im create() eines VC-Raums in Para in den Raum bewegt wird, da
        // dieser dann noch nicht vom Driver umbenannt wurde und raum#42
        // heisst.
        if ( !sizeof(regexp( ({ fn }), "\\^[1-9][0-9]*$" )) &&
            strrstr(fn,"#")==-1 )
        {
            fn += "^" + para;

          // Der Parallelwelt-Raum muss existieren und fuer Spieler
          // freigegeben sein, damit er zum neuen Ziel wird. Ansonsten
          // duerfen nur NPCs, Testspieler und Magier herein.
          if ( (find_object(fn) 
                || ((file_size(fn+".c")>0 ||
                    (file_size(vc=implode(explode(fn,"/")[0..<2],"/")+
                          "/virtual_compiler.c")>0 &&
                    !catch(tmp=({int})call_other(vc,"QueryValidObject",fn);
                           publish) && tmp>0)) &&
                    !catch(load_object(fn);publish) )) &&
                  (!interactive(ME) || !({int})fn->QueryProp(P_NO_PLAYERS) || 
                  (method & M_NOCHECK) || IS_LEARNER(ME) ||
                  (stringp(res = QueryProp(P_TESTPLAYER)) &&
                   IS_LEARNER( lower_case(res) ))) )
          {
              dest = fn;
          }
          else
          {
              // Wir bleiben in der Normalwelt.
              para = 0;
          }
        }
    }

    // jetzt erstmal Hooks abpruefen, da sie ggf. die Daten aendern.
    // alten P_TMP_MOVE_HOOK pruefen.
    if ( res = QueryProp(P_TMP_MOVE_HOOK) ){
        if ( pointerp(res) && sizeof(res) >= 3
             && intp(res[0]) && time()<res[0]
             && objectp(res[1]) && stringp(res[2]) ){
            if ( res = ({mixed})call_other( res[1], res[2], dest, method, direction,
                                   textout, textin ) ){
                if ( pointerp(res) && sizeof(res) == 5 ){
                    dest = res[0];
                    method = res[1];
                    direction = res[2];
                    textout = res[3];
                    textin = res[4];
                }
                else if ( intp(res) && res == -1 )
                    return ME_CANT_LEAVE_ENV;
            }
        } else
            SetProp( P_TMP_MOVE_HOOK, 0 );
    }
    // move hook nach neuem Hooksystem triggern.
    hookData=({dest,method,direction,textout,textin});
    hookRes=HookFlow(H_HOOK_MOVE,hookData);
    if(hookRes && pointerp(hookRes) && sizeof(hookRes)>H_RETDATA) {
      if(hookRes[H_RETCODE]==H_CANCELLED) {
        return ME_CANT_LEAVE_ENV;
      }
    else if(hookRes[H_RETCODE]==H_ALTERED && hookRes[H_RETDATA] &&
          pointerp(hookRes[H_RETDATA]) && sizeof(hookRes[H_RETDATA])>=5 ){
      dest = hookRes[H_RETDATA][0];
      method = hookRes[H_RETDATA][1];
      direction = hookRes[H_RETDATA][2];
      textout = hookRes[H_RETDATA][3];
      textin = hookRes[H_RETDATA][4];
      }
    }

    // dest auf Object normieren
    if (stringp(dest)) dest=load_object(dest);

    // jetzt Checks durchfuehren, ob das Move durchgefuehrt werden darf.
    if (tmp=PreventMove(dest, oldenv, method)) {
      // auf gueltigen Fehler pruefen, wer weiss, was Magier da evtl.
      // versehentlich zurueckgeben.
      if (VALID_MOVE_ERROR(tmp))
        return(tmp);
      else
        return(ME_DONT_WANT_TO_BE_MOVED);
    }
  
    if ( invis = QueryProp(P_INVIS) )
        method |= M_SILENT;

    if ( objectp(oldenv) ) {
        if ( !(method & M_SILENT) ) {
            string *mout;
            if ( !textout ){
                if ( method & M_TPORT )
                    textout = ({string}) QueryProp(P_MMSGOUT) ||
                        ({string}) QueryProp(P_MSGOUT);
                else 
                    textout = (mout = explode( ({string})
                                                QueryProp(P_MSGOUT) || "",
                                                      "#" ))[0]
                         || ({string})QueryProp(P_MMSGOUT);
            }

            if ( !sizeof(direction) )
                direction = 0;

            foreach (object who : all_inventory(environment())) {
              if (who != this_object() && living(who) && !who->CannotSee(1))
                who->ReceiveMsg(sprintf("%s %s%s%s.",
                                Name(WER,2),
                                textout,
                                direction ? " " + direction : "",
                                sizeof(mout) > 1 ? mout[1] : ""),
                    MT_LOOK, MA_MOVE_OUT);
            }

        }
        // Magier sehen auch Bewegungen, die M_SILENT sind
        else if ( interactive(ME) ){
            if ( invis )
                fn = "(" + capitalize(getuid(ME)) + ") verschwindet "
                    "unsichtbar.\n";
            else
                fn = capitalize(getuid(ME)) + " verschwindet ganz leise.\n";

            foreach (object who : all_inventory(environment())) {
              if (who != this_object() && _is_learner(who))
                tell_object(who, fn);
            }
        }

        // Nackenschlag beim Fluechten:
        if ( !(method & M_NO_ATTACK) && objectp(ME) )
            ExitAttack();
        //falls nach ExitAttack() das Living nicht mehr existiert, muss das
        //move() auch nicht mehr fortgesetzt werden. Weiter unten gibt es auch
        //min. eine Stelle, die nicht prueft und ggf. buggt. Daher erfolgt
        //hier ggf. ein Abbruch. 15.11.06 Zesstra
        if (!objectp(ME)) return(ME_WAS_DESTRUCTED);

        // Nackenschlag kann ME in den Todesraum bewegt haben...
        if ( oldenv == environment() ) {
            // Fuer alle anwesenden gegner kampfende() aufrufen
            filter((QueryEnemies()[0] & all_inventory(oldenv))-({0}),
                #'kampfende);
            // Bugs im exit() sind ohne catch() einfach mist.
            catch(({void})environment()->exit(ME, dest);publish);
        }
    }

    // irgendwas kann das Objekt zerstoert haben, z.B. env->exit().
    if (!objectp(ME)) return(ME_WAS_DESTRUCTED);

    if ( oldenv != environment() )
        // Der Nackenschlag oder exit() koennen einen schon bewegt haben.
        // Und wenn es in den Todesraum ist. ;^)
        return MOVE_OK;
    
    SetProp( P_PREPARED_SPELL, 0 ); // Spruchvorbereitung abgebrochen
    SetProp( P_LAST_MOVE, time() ); // Zeitpunkt der letzten Bewegung
    
    move_object(ME, dest);
    if (!objectp(ME))
      return(ME_WAS_DESTRUCTED);

    dest = environment();

    UseSkill(SK_NIGHTVISION);

    // Meldungen an nicht-Blinde ausgeben, falls keine 'stille' Bewegung
    if ( !(method & M_SILENT) ) {
      if ( !textin ) {        
        if ( method & M_TPORT )
              textin = ({string}) QueryProp(P_MMSGIN);
        else
              textin = ({string}) QueryProp(P_MSGIN);
      }

      foreach (object who : all_inventory(environment())) {
        if (who != this_object() && living(who) && !who->CannotSee(1))
          who->ReceiveMsg(Name(WER,0)+" "+textin+".", MT_LOOK, MA_MOVE_IN);
      }
    }
    // sonst: Magier sehen auch M_SILENT-Bewegungen, hier also nur an Magier
    // ausgeben, alle anderen sehen eh nix.
    else if ( interactive(ME) ) {
      if ( invis )
        fn = "(" + capitalize(getuid(ME)) + ") taucht unsichtbar auf.\n";
      else
        fn = capitalize(getuid(ME)) + " schleicht leise herein.\n";

        foreach (object who : all_inventory(environment())) {
          if (who != this_object() && _is_learner(who))
            tell_object(who, fn);
        }
    }

    // "Objekt" ueber das Move informieren.
    NotifyMove(dest, oldenv, method);

    // InitAttack() in NotifyMove() kann das Objekt zerstoert haben.
    if (!objectp(ME))
      return(ME_WAS_DESTRUCTED);

    //scheint wohl geklappt zu haben.
    return MOVE_OK;
}

public void TakeFollowers()
{
  mixed *f,env;
  int meth,r;

  f=Query(P_PURSUERS);
  if (!pointerp(f))
    return;
  env=environment();
  if(object_name(env) == "/room/netztot") return;
  foreach(object follower: f[1]-({0}) ) {
    // die pruefung auf objectp ist nicht verrueckt, es kann theo. sein, dass
    // Verfolger im PreventFollow() oder in ihrem move/init andere Verfolger
    // zerstoeren.
    if (objectp(follower) && environment(follower)!=env) {
      //meth=M_NOCHECK;
      meth=M_GO;
      if (({int})follower->Query(P_FOLLOW_SILENT))
          meth|=M_SILENT|M_NO_SHOW;
      catch(r=({int})follower->PreventFollow(env);publish);
      if (!r)
          ({int})follower->move(env,meth);
      else if (r==2)
          RemovePursuer(follower);
    }
  }
}

varargs public int remove()
{ object team;

  if (environment())
  {
    if ( objectp(team=Query(P_TEAM)) )
      catch(({int})team->RemoveMember(ME);publish);

    ({void})environment()->NotifyRemove(ME);
  }
  destruct(ME);
  return 1;
}

