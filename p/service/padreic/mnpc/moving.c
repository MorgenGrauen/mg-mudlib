#pragma save_types,strong_types,rtt_checks

inherit "/std/living/moving";

#include <moving.h>
#include <defines.h>
#include <properties.h>
#include "/p/service/padreic/mnpc/mnpc.h"
#define NEED_PROTOTYPES
#include <living/combat.h>
#undef NEED_PROTOTYPES
#include <combat.h>

#define ENV    environment
#define PO     previous_object()

struct exit_s {
    string cmd;             // Kommando fuer Benutzung
    string|closure room;    // Pfad oder ausfzufuehrende Closure
    string msg;             // Bewegungsrichtung beim Benutzen
    int special;            // == 1 fuer special exits
};

// Letzter Spielerkontakt. Beim Laden auf Ladezeit faken.
//  0, wenn der MNPC inaktiv rumsteht
// -1, wenn der MNPC inaktiv zuhause rumsteht
nosave int meet_last_player = time();

static void mnpc_create()
{
  if (PO && member(inherit_list(PO), "/std/room.c")!=-1)
    SetProp(MNPC_HOME, object_name(PO));
  SetProp(P_MNPC, 1);
  SetProp(MNPC_AREA, ({}));
  SetProp(MNPC_DELAY, MNPC_DFLT_DELAY);
  SetProp(MNPC_FUNC, 0);
  SetProp(MNPC_RANDOM, 0);
  SetProp(MNPC_WALK_TIME, MNPC_DFLT_WALK);
  SetProp(MNPC_FLAGS, 0);
  SetProp(P_ENABLE_IN_ATTACK_OUT, 1);
}

protected void RegisterWalk()
{
  if ((QueryProp(MNPC_DELAY)+QueryProp(MNPC_RANDOM)) < MAX_MASTER_TIME)
  {
    if (!WALK_MASTER->Registration())
      WALK_MASTER->RegisterWalker(QueryProp(MNPC_DELAY),
                                  QueryProp(MNPC_RANDOM));
  }
  else
  {
    if (find_call_out("Walk") == -1)
      call_out("Walk", QueryProp(MNPC_DELAY)+random(QueryProp(MNPC_RANDOM)));
  }
}

// Can be used to manually restart the MNPC from a different object even if
// the MNPC had no player contact.
public int RestartWalk()
{
  int flags = QueryProp(MNPC_FLAGS);
  // Falls nicht laufend, wird gar nichts gemacht.
  if (flags & MNPC_WALK)
  {
    //Spielerkontakt simulieren
    meet_last_player=time();
    // Falls MNPC noch registriert ist oder noch einen Callout auf Walk hat,
    // muss nichts weiter gemacht werden.
    if (WALK_MASTER->Registration()
        || find_call_out("Walk") > -1)
      return -1;
    // ansonsten MNPC registrieren, falls geeignet.
    if ((QueryProp(MNPC_DELAY)+QueryProp(MNPC_RANDOM))<MAX_MASTER_TIME)
      WALK_MASTER->RegisterWalker(QueryProp(MNPC_DELAY), QueryProp(MNPC_RANDOM));
    // und mit kurzer Verzoegerung einmal laufen. (ja, absicht, hier
    // MNPC_DELAY zu nutzen - denn solange dauert das Walk vom Master
    // mindestens.)
    call_out("Walk",1+random( min(QueryProp(MNPC_DELAY)-1,8) ));
    return 1;
  }
  return 0;
}

protected void Stop(int movehome)
{
  if (WALK_MASTER->Registration())
    WALK_MASTER->RemoveWalker();
  else if (find_call_out("Walk")!=-1)
    remove_call_out("Walk");
  if (movehome)
  {
    move(QueryProp(MNPC_HOME), M_TPORT|M_NOCHECK);
    meet_last_player=-1;
  }
  else
  {
    meet_last_player = 0;
  }
}

static int _set_mnpc_flags(int flags)
{
  if (flags & MNPC_WALK)
  {
    if (!QueryProp(MNPC_HOME))
      raise_error("unknown MNPC_HOME\n");
    // RegisterWalk prueft, ob der MNPC schon angemeldet ist.
    RegisterWalk();
  }
  // else nicht von Bedeutung, da in Walk() das flag getestet wird
  if (flags & MNPC_FOLLOW_PLAYER)
  {
    if (!QueryProp(MNPC_PURSUER))
    { // wurde dieses Flag neu eingeschaltet?
      if (environment())
      { // Verfolgung aufnehmen...
        object *pursuer = filter(all_inventory(ENV()), #'interactive);
        filter_objects(pursuer, "AddPursuer", ME);
        SetProp(MNPC_PURSUER, pursuer);
      }
      else
        SetProp(MNPC_PURSUER, ({}));
    }
  }
  else if (pointerp(QueryProp(MNPC_PURSUER)))
  { // wird derzeit irgendwer verfolgt?
    // alle Verfolgungen abbrechen...
    filter_objects(QueryProp(MNPC_PURSUER)-({ 0 }), "RemovePursuer", ME);
    SetProp(MNPC_PURSUER, 0); // Speicher freigeben...
  }
  else
    SetProp(MNPC_PURSUER, 0);

  // nur livings koennen command_me nutzen...
  if (!living(ME))
    flags |= MNPC_DIRECT_MOVE;

  return Set(MNPC_FLAGS, flags, F_VALUE);
}

static void mnpc_InsertEnemy(object enemy)
{
  if ( (QueryProp(MNPC_FLAGS) & MNPC_FOLLOW_ENEMY) &&
         (member(QueryProp(MNPC_PURSUER), PL)==-1))
  {
     PL->AddPursuer(ME);
     SetProp(MNPC_PURSUER, QueryProp(MNPC_PURSUER)+({ PL }));
  }
}

static void mnpc_reset()
{
  int flags = QueryProp(MNPC_FLAGS);
  // meet_last_player == -1 zeigt an, dass der MNPC schon zuhause ist.
  if (meet_last_player == -1
      || !(flags & MNPC_WALK)
      || (flags & MNPC_NO_MOVE_HOME))
    return;

  // Lange keinen Spielerkontakt und kein Spieler im Raum: nach Hause gehen.
  if (QueryProp(MNPC_WALK_TIME)+meet_last_player < time()
      && environment() && !sizeof(filter(
      all_inventory(environment()), #'query_once_interactive)))
  {
    // Abschalten und Heimgehen und dort warten.
    Stop(1);
  }
}

static int _query_mnpc_last_meet()
{   return meet_last_player;   }

static void mnpc_init()
{
  if (interactive(PL))
  {
    // Wenn noetig, Wandern wieder aufnehmen.
    if (meet_last_player<=0)
    {
      RegisterWalk();
    }
    if ( ((QueryProp(MNPC_FLAGS) & MNPC_FOLLOW_PLAYER) &&
         (member(QueryProp(MNPC_PURSUER), PL)==-1)) ||
        ((QueryProp(MNPC_FLAGS) & MNPC_FOLLOW_ENEMY) && IsEnemy(PL)))
    {
      PL->AddPursuer(ME);
      SetProp(MNPC_PURSUER, QueryProp(MNPC_PURSUER)+({ PL }));
    }
    meet_last_player=time();
  }
  else
  {
    // Wenn der reinkommende auch ein MNPC_LAST_MEET groesser 0 hat, ist es
    // ein MNPC, der noch laeuft. Wenn wir nicht laufen, laufen wir los.
    // In diesem und auch im anderen Fall uebernehmen wir aber mal seinen
    // letzten Spielerkontakt, denn der ist juenger als unserer.
    int lm = PL->QueryProp(MNPC_LAST_MEET);
    if (meet_last_player<=0 && lm>0)
    {
      RegisterWalk();
      meet_last_player=lm;
    }
    else if (meet_last_player<lm)
       meet_last_player=lm;
  }
}

static void mnpc_move()
{
  if (environment() && (QueryProp(MNPC_FLAGS) & MNPC_FOLLOW_PLAYER))
  {
    object *liv = QueryProp(MNPC_PURSUER) & all_inventory(environment());
    filter_objects(QueryProp(MNPC_PURSUER)-liv-({ 0 }), "RemovePursuer", ME);
    SetProp(MNPC_PURSUER, liv);
  }
  if (QueryProp(MNPC_FUNC))
    call_other(ME, QueryProp(MNPC_FUNC));
}

static int PreventEnter(string file)
// darf der Raum betreten werden?
{
  string *area;

  if (!sizeof(area=QueryProp(MNPC_AREA)))
    return 0; // Raum darf betreten werden
  else
  {
    int i;
    status exactmatch;
    exactmatch=QueryProp(MNPC_FLAGS) & MNPC_EXACT_AREA_MATCH;
    if ((i=strstr(file, "#"))!=-1) file=file[0..i-1];
    for (i=sizeof(area)-1; i>=0; i--)
    {
      if (exactmatch)
      {
        //exakter Vergleich, kein Substringvergleich gewuenscht
        if (file==area[i])
          return 0;  //betreten
      }
      else
      {
        if (strstr(file, area[i])==0)
          return 0; // Raum betreten
      }
    }
    return 1; //  Raum darf nicht betreten werden
  }
}

static int mnpc_PreventFollow(object dest)
{
  if (dest && PreventEnter(object_name(dest)))
    return 2;
  return 0;
}

// Bewegungssimulation (Bewegungsmeldung) fuer bewegende non-livings
protected int direct_move(object|string dest, int method, string direction)
{
   int res, para, tmp;
   string textout, textin, *mout, vc, fn;
   object oldenv;

   if (living(ME))
      return call_other(ME, "move", dest, method);
   else
   {
      oldenv = environment();
      para=QueryProp(P_PARA);
      if ((para>0) && stringp(dest))
      {
         fn=dest+"^"+para;
         if (find_object(fn) || (file_size(fn+".c")>0))
            dest=fn;
         else if (file_size(vc=implode(explode(fn,"/")[0..<2],"/")
                  +"/virtual_compiler.c")>0)
         {
            // wenn ein VC existiert, prüfen ob dieser ParaObjecte unterstuetzt
            // wenn ja, dann testen ob sich Raum laden laesst...
            if ((!catch(tmp=call_other(vc,"NoParaObjects")) && (!tmp)) &&
                (!catch(call_other( fn, "???" ))))
                dest=fn;
         }
      }

      res = call_other(ME, "move", dest, M_NOCHECK);

      if (oldenv==environment() || living(this_object()))
        return res;

      // als erstes die Meldung fuer das Verlassen des Raumes...
      if ( method & M_TPORT )
        textout = QueryProp(P_MMSGOUT) || QueryProp(P_MSGOUT);
      else
      {
        mout = explode( QueryProp(P_MSGOUT) || "", "#" );
        textout = mout[0] || QueryProp(P_MMSGOUT);
      }

      if (stringp(textout))
      {
         if ( !sizeof(direction) )
           direction = 0;

         send_room(oldenv,
                       Name( WER, 2 ) + " " + textout +
                       (direction ? " " + direction : "") +
                       (sizeof(mout) > 1 ? mout[1] : "") + ".",
                       MT_LOOK,
                       MA_MOVE_OUT,
                       0,
                       ({}),
                       this_object());
      }

      // nun die Meldung für das "Betreten" des Raumes...

      if ( method & M_TPORT )
        textin = QueryProp(P_MMSGIN);
      else
        textin = QueryProp(P_MSGIN);

      if (stringp(textin))
      {
         send_room(environment(this_object()),
                       capitalize(name( WER, 0 )) + " " + textin + ".",
                       MT_LOOK,
                       MA_MOVE_IN,
                       0,
                       ({this_object()}),
                       this_object());
      }
  }
  return res;
}

// Sammelt verfuegbare und prinzipiell benutzbare Ausgaenge
// Standardverhalten beruecktsichtigt P_EXITS und ob special Exits benutzt
// werden duerfen.
// Aber ueberladene Varianten koennten voellig andere Quellen fuer Ausgaenge
// (z.B. eigene Props von Magiern) beruecksichtigen.
struct exit_s *PresentExits(mapping exits=environment()->Query(P_EXITS,
                              F_VALUE))
{
    // Aufgrund der MNPC_FLAGS bestimmen ob nur normale Ausgaenge genutzt
    // werden duerfen
    int flags=QueryProp(MNPC_FLAGS);
    flags = !(flags & MNPC_DIRECT_MOVE) && !(flags & MNPC_ONLY_EXITS);
    // flags ist jetzt nur noch ja/nein bzgl. Special Exits

    return m_values(map(exits,
        function struct exit_s (string cmd, <string|closure>* arr)
        {
            if (flags || !closurep(arr[0]))
              return (<exit_s> cmd:cmd, room:arr[0], msg:arr[1],
                      special:closurep(arr[0]));
            return 0;
        }
        )) - ({0}); // 0 aufgrund nicht benutzbarer SEs
}

// Prueft die Ausgaenge auf Benutzbarkeit durch diesen NPC - standardmaessig
// durch Aufruf von PreventEnter() und liefert einen zufaelligen benutzbaren
// Ausgang.
// Kann Ueberladen werden, um eine andere Art der Auswahl zu bewerkstelligen.
struct exit_s SelectExit(struct exit_s *exitlist=PresentExits())
{
    // Bei normalen Ausgaengen per PreventEnter() pruefen, ob der Zielraum OK
    // ist. Special Exits koennen nicht geprueft und sind OK. (In der Liste
    // sind bei originalem PresentExits() nur special exits, wenn fuer den
    // MNPC erlaubt.)
    exitlist = filter(exitlist, function int (struct exit_s ex)
    {
        return ex.special || !PreventEnter(ex.room);
    });

    // Und zufaellig einen aussuchen
    if (sizeof(exitlist))
        return exitlist[random(sizeof(exitlist))];
    return 0;
}

int Walk()
{
  if (!environment())
  {
    // darf eigentlich nicht vorkommen.
    raise_error("MNPC ohne Environment.\n");
  }

  int flags=QueryProp(MNPC_FLAGS);
  if (!(flags & MNPC_WALK))
    return 0;

  // ggf. neuen Callout eintragen, bevor irgendwas anderes gemacht wird.
  if ((QueryProp(MNPC_DELAY)+QueryProp(MNPC_RANDOM))>=MAX_MASTER_TIME)
    call_out("Walk", QueryProp(MNPC_DELAY)+random(QueryProp(MNPC_RANDOM)));

  // Im Kampf ggf. temporaer nicht weitergehen.
  if ((flags & MNPC_NO_WALK_IN_FIGHT) && InFight())
  {
    meet_last_player=time();
    return 1;
  }

  // MNPC anhalten, wenn lange kein Spielerkontakt. Vom WALK_MASTER abmelden.
  if (QueryProp(MNPC_WALK_TIME)+meet_last_player < time()
      && !sizeof(filter(all_inventory(environment()),
                 #'query_once_interactive))
      )
  {
    // anhalten und ggf. auch direkt nach Hause gehen.
    Stop(flags & MNPC_GO_HOME_WHEN_STOPPED);
    return 0;
  }

  // Zielausgang ermitteln
  struct exit_s ex = SelectExit();

  // Im direct mode wird die Bewegung selbst implementiert -> YNMV!
  if (flags & MNPC_DIRECT_MOVE)
  {
    // in diesem Fall sind keine SEs benutzbar, aber die werden von
    // PresentExit() dann auch nicht geliefert.
    if (ex)
    {
      direct_move(ex.room, M_GO, "nach "+capitalize(ex.msg||ex.cmd));
    }
    else
    {
      // Hngl. Ohne Ausgaenge nach Hause gehen... aber nicht anhalten, von
      // dort gehts ja vielleicht weiter.
      direct_move(QueryProp(MNPC_HOME), M_TPORT|M_NOCHECK, 0);
    }
  }
  else if (ex)
  {
    // Dies ist der Normalfall: Ausgang per Kommando benutzen
    command(ex.cmd);
  }
  else
  {
    // Hngl. Ohne Ausgaenge nach Hause gehen... aber nicht anhalten, von
    // dort gehts ja vielleicht weiter.
    move(QueryProp(MNPC_HOME), M_TPORT|M_NOCHECK);
  }

  return 1;
}
