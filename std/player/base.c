// MorgenGrauen MUDlib
//
// player/base.c -- the basic player object
//
// $Id: base.c 9467 2016-02-19 19:48:24Z Zesstra $
#pragma strong_types
#pragma save_types
#pragma range_check
#pragma no_clone

#include <sys_debug.h>
#include <regexp.h>
#include <input_to.h>
#include <configuration.h>
#include <logging.h>
#include <werliste.h>
#include <time.h>
#include <errord.h>
#include <wizlevels.h>
#include <money.h>

inherit "/std/hook_provider";
inherit "/std/player/restrictions";
inherit "/std/player/vitems";
inherit "/std/living/attributes";
inherit "/std/living/put_and_get";
inherit "/std/living/clothing";
inherit "/std/thing/properties";
inherit "/std/player/util";
inherit "/std/thing/language";
inherit "/std/player/travel";
inherit "/std/player/combat";
inherit "/std/player/description";
inherit "/std/player/moving";
inherit "/std/player/life";
inherit "/std/player/comm";
inherit "/std/player/viewcmd";
inherit "/std/player/moneyhandler";
inherit "/std/player/command";
inherit "/std/living/skill_attributes";
inherit "/std/living/light";
inherit "/std/player/skills";
inherit "/std/player/quests";
inherit "/std/player/potion";
inherit "/std/player/soul";
inherit "/std/more";
inherit "/std/user_filter";
inherit "/secure/telnetneg";
inherit "/std/player/guide";
inherit "/std/player/reputation";
inherit "/std/player/protocols/gmcp";
inherit "/std/living/helpers";

#define NEED_PROTOTYPES
#include <player/skills.h>
#include <player/gmcp.h>
#include <player/base.h>
#undef NEED_PROTOTYPES
#include <player.h>
#include <properties.h>
#include <udp.h>
#include <config.h>
#include <ansi.h>
#include <wizlevels.h>
#include <living.h>
#include <attributes.h>
#include <language.h>
#include <moving.h>
#include <defines.h>
#include <terminal.h>
#include <new_skills.h>
#include <pager.h>
#include <combat.h>
#include "/secure/questmaster.h"
#include "/secure/lepmaster.h"
#include <events.h>
#include <player/telnetneg.h>

#undef NAME /* DEFINED BY UDP.H; BAD NAME CLASH :( */
#define NAME(who) capitalize(getuid(who))

mapping autoload;           /* autoload-mapping */
int hc_play;

private nosave mapping autoload_rest;
private nosave string  *autoload_error;
private nosave string realip;

// HB-Zaehler. Wenn 0 erreicht wird, wird  ein Telnet TM Paket als Keep-Alive
// an den Client gesendet und der Counter wieder auf hochgesetzt. 
// Wenn == 0, ist das Keep-Alive abgeschaltet.
private int telnet_tm_counter;

nosave string default_home; /* Where to move us if we dont have a home set */

nosave int ndead_lasttime;
nosave mixed ndead_location;
nosave string ndead_l_filename;
nosave int ndead_currently;
nosave int ndead_next_check;
nosave object *hb_obs;

private nosave string default_pray_room;

nosave mixed env_ndead_info;

static int _set_invis(int a);
static string _set_tty(string str);
static mixed _set_fraternitasdonoarchmagorum(mixed arg);


static string al_to_title(int a);

static void ndead_revive();

static int wegmeldung(string player);

int quit();
void save_me(mixed value_items);
varargs int remove(mixed arg);
mixed RaceDefault(string arg);

/** Setzt Defaultwerte vor dem Laden des Savefiles.
    Nur die Werte bleiben spaeter uebrig, die NICHT aus dem Savefile geladen
    werden und diese hier ersetzen.
    Ausserdem kann man hier nicht mit den Werten aus den Savefiles arbeiten.
    Hierzu muss man updates_after_restore() verwenden.
*/
protected void create()
{
  if(QueryProp(P_LEVEL)) 
  {
    return; // darf nur EINMAL gemacht werden
  }
  call_out("checkConsistency", 0);

  ndead_next_check=NETDEAD_CHECK_TIME;
  ndead_lasttime=0;
  ndead_location=0;
  ndead_l_filename=0;
  ndead_currently=0;
  ndead_next_check=0;
  hc_play=0;
  
  command::create();
  properties::create();
  description::create();
  light::create();
  attributes::create();
  clothing::create();
  combat::create();
  life::create();
  comm::create();
  viewcmd::create();
  quests::create();
  restrictions::create();
  moving::create();
  travel::create();
  skills::create();
  helpers::create();
   
  SetProp(P_LEVEL, -1);
  Set(P_LEVEL, SAVE|SECURED, F_MODE_AS);
  Set(P_GHOST, SAVE, F_MODE_AS);
  SetProp(P_SCREENSIZE, -1);
  Set(P_SCREENSIZE, SAVE,F_MODE_AS);
  Set(P_MORE_FLAGS, SAVE, F_MODE_AS);
  SetProp(P_WEIGHT_PERCENT,100);
  SetProp(P_LONG, 0);
  SetProp(P_TITLE, "der hoffnungsvolle Anfaenger");
  SetProp(P_ALIGN, 0);
  SetProp(P_GENDER, NEUTER);
  Set(P_GENDER, SAVE, F_MODE_AS);
  SetProp(P_TTY, "vt100");
  Set(P_TTY, SAVE, F_MODE_AS);
  SetProp(P_WEIGHT, 75000);
  SetProp(P_MAX_HP,50);
  SetProp(P_MAX_SP,50);
  SetProp(P_MAX_FOOD,100);
  SetProp(P_MAX_DRINK,100);
  SetProp(P_MAX_ALCOHOL,100);
  Set( P_WIMPY, 20, F_VALUE);

  SetProp(P_HANDS, ({" mit blossen Haenden", 30}));
  Set(P_HANDS, SAVE, F_MODE_AS);
  SetProp(P_MAX_HANDS, 2);

  Set(P_MARRIED, SAVE, F_MODE_AS);
  Set(P_EXTRA_LOOK, SAVE, F_MODE_AS);
  Set(P_SHOW_EXITS, SAVE, F_MODE_AS);
  Set(P_SHOW_EXITS, 1);
  Set(P_WANTS_TO_LEARN, SAVE, F_MODE_AS);
  Set(P_CAN_FLAGS, SAVE, F_MODE_AS);
  Set(P_TESTPLAYER, SAVE|PROTECTED, F_MODE_AS);
  Set(P_ALLOWED_SHADOW, SAVE|SECURED, F_MODE_AS);
  Set(P_SECOND, SAVE, F_MODE_AS);
  Set(P_INVIS, SAVE, F_MODE_AS);
  Set(P_READ_NEWS, SAVE, F_MODE_AS);
  Set(P_START_HOME, SAVE, F_MODE_AS);
  Set(P_PRAY_ROOM, SAVE, F_MODE_AS);
  Set(P_MAILADDR, SAVE, F_MODE_AS);
  Set(P_HOMEPAGE, SAVE, F_MODE_AS);
  Set(P_ICQ, SAVE, F_MODE_AS);
  Set(P_MESSENGER, SAVE, F_MODE_AS);
  Set(P_LOCATION, SAVE, F_MODE_AS);

  Set(P_NO_ASCII_ART, SAVE, F_MODE_AS);

  Set(P_VISUALBELL, SAVE, F_MODE_AS);
  Set(P_CARRIED_VALUE, SAVE, F_MODE_AS);

  Set(P_PROMPT, "> ");
  Set(P_PROMPT, SAVE, F_MODE_AS);
  Set(P_CALLED_FROM_IP, SAVE, F_MODE_AS);
  Set(P_INFORMME,SAVE|PROTECTED,F_MODE_AS);
  Set(P_WAITFOR,SAVE|PROTECTED,F_MODE_AS);
  Set(P_WAITFOR_REASON,SAVE|PROTECTED,F_MODE_AS);
  Set(P_DAILY_PLAYTIME,SAVE|PROTECTED,F_MODE_AS);
  Set(P_NETDEAD_ENV, PROTECTED|NOSETMETHOD, F_MODE_AS);
  
  autoload = ([]);
  autoload_rest = ([]);
  autoload_error = ({});
  SetProp(P_ARTICLE,0);
  Set(P_GUILD,SAVE,F_MODE_AS);
  Set(P_GUILD_TITLE,SAVE,F_MODE_AS);
  Set(P_GUILD_LEVEL,SAVE,F_MODE_AS);
  Set(P_GUILD_RATING,SAVE,F_MODE_AS);
  Set(P_NEWSKILLS,SAVE,F_MODE_AS);
  Set(P_NEEDED_QP,REQ_QP);
  Set(P_DEADS,0);
  Set(P_DEADS, NOSETMETHOD,F_SET_METHOD);
  Set(P_DEADS,SAVE|PROTECTED|SECURED,F_MODE_AS);
  Set(P_LAST_LOGIN,-1);
  Set(P_LAST_LOGIN, NOSETMETHOD,F_SET_METHOD);
  Set(P_LAST_LOGIN,SAVE|PROTECTED|SECURED,F_MODE_AS);
  Set(P_LAST_LOGOUT,-1);
  Set(P_LAST_LOGOUT, NOSETMETHOD,F_SET_METHOD);
  Set(P_LAST_LOGOUT,SAVE|PROTECTED|SECURED,F_MODE_AS);
  Set(P_CLOCKMSG,SAVE,F_MODE_AS);
  Set(P_TIMEZONE,SAVE,F_MODE_AS);
  Set(P_SHOWEMAIL,SAVE,F_MODE_AS);
  Set(P_LAST_QUIT,SAVE|PROTECTED|SECURED,F_MODE_AS);

  Set(P_CMSG, 0, F_MODE); // to clean out the old clone messages
  Set(P_DMSG, 0, F_MODE);
  Set(P_CLONE_MSG, SAVE, F_MODE);
  SetProp(P_CLONE_MSG, "zaubert etwas hervor");
  Set(P_DESTRUCT_MSG, SAVE, F_MODE);
  SetProp(P_DESTRUCT_MSG, "verschwindet einfach");

  Set(P_FAO, SAVE|SECURED, F_MODE_AS);
  Set(P_FAO_PORTALS, SAVE, F_MODE_AS);

  SetProp(P_NEWBIE_GUIDE,0);
  Set(P_NEWBIE_GUIDE,SAVE,F_MODE_AS);

  // Daran sollte nicht jeder von aussen rumspielen.
  Set(P_TELNET_CHARSET, PROTECTED|SAVE, F_MODE_AS);

  AddId("Interactive");
}

// ACHTUNG: Falls hier mal sonst noch weitere Resets geerbt werden, muss das
// hier ordentlich definiert werden!
void reset() { 
  comm::reset();
  // momentan kann der Reset jetzt abgeschaltet werden, da er nur die
  // TM-History loescht und ggf. von Netdead() und Disconnect() reaktiviert
  // wird.
  set_next_reset(-1);
}

protected void NotifyMove(object dest, object oldenv, int method)
{
  moving::NotifyMove(dest,oldenv,method);
  // ggf. Daten ueber neues Env per GMCP senden.
  if (dest != oldenv)
    GMCP_Room();
}

public void NotifyInsert(object ob, object oldenv)
{
  restrictions::NotifyInsert(ob,oldenv);
  description::NotifyInsert(ob,oldenv);
}
  
public void NotifyLeave(object ob, object dest)
{
  restrictions::NotifyLeave(ob,dest);
  // Die Inserthooks machen oefter mal Objekte kaputt...
  if (objectp(ob))
    description::NotifyLeave(ob,dest);
}

string Forschung()
{
  return LEPMASTER->QueryForschung();
}

/** Setzt Defaultwerte fuer Rassen - wird von den Shells ueberschrieben.
*/
mixed RaceDefault(string arg)
{
  if (!arg)
    return 0;
  switch(arg)
  {
    case P_HANDS :
      return ({" mit blossen Haenden",30,DT_BLUDGEON});
    case P_BODY :
      return 0;
  }
  return 0;
}

/** Prueft Spielerobjekt auf Konsistenz.
  Ueberprueft das Spielerobjekt nach kompletter Initialisierung (im 
  callout) auf korrekte Werte einiger Props.
*/
void checkConsistency()
{ mixed h;
  int   m;

  if (pointerp(h=RaceDefault(P_HANDS)) && sizeof(h)>1)
    m=h[1];
  else
    m=30;
  if((h=Query(P_HANDS))[1] > m && !IS_LEARNER(this_object())) {
    log_file("inconsistent", sprintf(
      "[%s] %O: HANDS: %d\n", dtime(time()), this_player(), h[1]));
    h[1] = m;
    Set(P_HANDS,h);
  }

  if (Query(P_BODY)!=(m=RaceDefault(P_BODY)))
     Set(P_BODY,m);

  if (!Query(P_SECOND_MARK,F_MODE))
    Set(P_SECOND_MARK,SAVE|PROTECTED,F_MODE_AS);
  if (!Query(P_TTY_SHOW,F_MODE)&SAVE)
  {
    Set(P_TTY_SHOW,0,F_VALUE);
    Set(P_TTY_SHOW,SAVE,F_MODE_AS);
  }
  if (Query(P_TTY_COLS,F_MODE)&SAVE)
  {
    Set(P_TTY_COLS,SAVE,F_MODE_AD);
    Set(P_TTY_ROWS,SAVE,F_MODE_AD);
    Set(P_TTY_TYPE,SAVE,F_MODE_AD);
  }
}

/** Bittet das Spielerobjekt, sich zu zerstoeren.
  \param[in] silent Flag, ob ohne Textausgaben zerstoert werden soll
  \return Erfolg der Selbstzerstoerung
*/
varargs int remove(int silent)
{
  return moving::remove(silent);
}

/** Schaltet in allen Objekten im Inv HBs aus.
  Schaltet im Inv in allen Objekten (rekursiv, deep_inventory)
  die Heartbeats aus. Falls obs uebergeben wird, werden diese Objekte
  statt des Inventars benutzt.
  Speichert die Objekte, die einen HB hatten, in der glob. Var. hb_obs.
  @param[in] obs mixed - Objekte, in denen der HB abgeschaltet werden soll. 
    Container werden rekursiv behandelt.
  @attention Achtung! Niemals zweimal hintereinander rufen, ohne 
    zwischendurch restart_heart_beats() gerufen zu haben!
  @sa restart_heart_beats
*/
varargs static void stop_heart_beats(mixed obs)
{
  if (!obs)
  {
    hb_obs=({});
    obs=deep_inventory(ME);
  } 
  foreach(mixed ob: obs) {
    if (pointerp(ob))
        stop_heart_beats(ob);
    else if (set_object_heart_beat(ob,0))  
        hb_obs+=({ob});
  }
}

/** Schaltet HBs in Objekten im Inv wieder ein.
  Schaltet in allen Objekten in hb_obs den Heartbeat ein.
    In hb_obs (glob. Var.) stehen alle Objekte, die beim letzten Aufruf von
    stop_heart_beats() einen HB hatten.
  @sa stop_heart_beats
*/
static void restart_heart_beats()
{
  if (pointerp(hb_obs))
  {
    foreach(object ob: hb_obs)
      set_object_heart_beat(ob,1);
    hb_obs=0;
  }
}

/** Prueft auf abgelaufene Spielzeit.
  Prueft in Spielerobjekten, ob die taegliche Maximalspielzeit
    abgelaufen ist.
    Wird im heart_beat() gerufen.
  @return int - Flag, ob Spielzeit abgelaufen und Logout erfolgen soll
*/
static int CheckDailyPlaytime() {
  int *spieldauer,d;

  if (!pointerp(spieldauer=Query(P_DAILY_PLAYTIME)))
    return 0;
  // 0:Minuten pro Tag, 1:Nr. des letzen Tages, 2:Nr. des angefangenen Tages,
  // 3:letzte Zeitpruefung, 4:verbleibende Zeit
  d=time()/86400;
  if (spieldauer[1]<=d) { // Ende der zeitbeschraenkten Tage?
    Set(P_DAILY_PLAYTIME,0);
    return 0;
  } else if (spieldauer[2]!=d) { // Neuer Tag?
    spieldauer[4]=spieldauer[0];
    spieldauer[2]=d;
  } else {
    spieldauer[4]-=(time()-spieldauer[3]);
  }
  spieldauer[3]=time();  // Letzte Zeitpruefung
  Set(P_DAILY_PLAYTIME,spieldauer);
  if (spieldauer[4]<0) { // Keine Zeit mehr uebrig fuer heute
    if (!interactive(ME))
      return 1;
    write("Du hast lange genug gemuddet fuer heute.\n");
    say(Name(WER)+" hat fuer heute genug gemuddet.\n");
    remove_interactive(ME);
    return 1;
  }
  return 0;
}

/** Gibt Erwartemeldung mit Grund aus.
  @param[in] who   string - Wer ist hereingekommen?
  @param[in] invis int - Ist der Spieler Invis?
*/
static void Show_WaitFor_Reason(string who, int invis)
{
  mixed list;
  string reason,name;

  if (invis) name="("+who+")";
    else name=who;
  if ((mappingp(list=QueryProp(P_WAITFOR_REASON))) && (reason=list[who]))
    tell_object(ME,sprintf("\nDu erwartest %s wegen:\n%s\n",name,reason));
  else
    tell_object(ME,sprintf("Du erwartest %s aus keinem bestimmten Grund.\n",
      name));
}

/** Gibt Liste der Erwarteten Spieler an this_player() aus.
*/
static void ListAwaited() //Anwesende Erwartete auflisten
{
    string *list;
    mixed mlist;
    object ob;
    int mag;

    mag=IS_LEARNER(ME);

    list=({});
    foreach(string erwartet : QueryProp(P_WAITFOR)) {
        if (objectp(ob=find_player(lower_case(erwartet)))) {
            if (ob->QueryProp(P_INVIS)) {
                if (mag) list+=({ sprintf("(%s)",erwartet) });
            }
            else list+=({erwartet});
        }
    }
    if (sizeof(list))
        printf("Anwesende Erwartete: %s.\n",
            CountUp(sort_array(list,#'>)));

    if ((mappingp(mlist=QueryProp(P_WAITFOR_REASON))) && (sizeof(mlist)))
        {
          foreach(string erwartet : mlist) {
                if (!(ob=find_player(lower_case(erwartet))) ||
                    (!mag && ob->QueryProp(P_INVIS)));
                else Show_WaitFor_Reason(erwartet,ob->QueryProp(P_INVIS));
          }
        }
}
/** Teilt den Gilden und anderen Spielern mit, wer reingekommen ist.
  Ausserdem wird ggf. PlayerQuit() im Environment gerufen.
  \param[in] rein int - wahr, wenn der Spieler einloggt.
*/
protected void call_notify_player_change(int rein)
{
  string wer = getuid(ME);
  // erst die Gilde informieren
  string gilde = QueryProp(P_GUILD);
  if (stringp(gilde) && (find_object("/gilden/"+gilde)
        || file_size("/gilden/"+gilde+".c")>0))
    catch(("/gilden/"+gilde)->notify_player_change(ME, rein); publish);

  // dann die anderen Spieler
  int mag = IS_LEARNER(ME);
  int invis = QueryProp(P_INVIS);
  object *u = users() - ({ME}); // sich selber nicht melden 
  if (mag && invis) {   // Invismagier nur Magiern melden
    u = filter(u, function int (object o)
        { return query_wiz_level(o) >= LEARNER_LVL; }
        );
  }
  u->notify_player_change(capitalize(wer),rein,invis);

  // und beim Ausloggen noch das Env informieren.
  if (!rein) {
    if(environment()) catch(environment()->PlayerQuit(ME);publish);
  }
}

/** Ruft im uebergebenen Objekt ein init() auf, sofern notwendig.
  Ruft in ob ein init() auf, falls das Objekt nach dem
    letzten Ausloggen geschaffen wurde.
  \param[in] ob object - Objekt, in dem init() gerufen wird.
  \param[in] logout int - Letzter Logout
  \return 1, falls init() gerufen wurde. 0 sonst.
*/
static int call_init( object ob, int logout )
{
    if ( objectp(ob) && object_time(ob) > logout )
        return catch(ob->init(); publish), 1;
    return(0);
}

/** Holt seit dem letzten Ausloggen ausgefallene Inits nach.
  Ruft in den uebergebenen Objekten call_init(), was einen init()
  ausloest, falls das Objekt seit dem letzten Ausloggen erstellt wurde.
  \param[in] logout Zeitpunkt des letzten Logouts
  \param[in] obs Array von Objekten
  \sa call_init()
*/
static void inits_nachholen( int logout, object *obs )
{
    filter( obs, "call_init", ME, logout );
}

// Zeichensatz konfigurieren.
// Wenn es einen manuell konfigurierten Zeichensatz gibt (der jetzt ja
// eingelesen ist), wird der erstmal eingestellt.
private void set_manual_encoding()
{
  string enc = QueryProp(P_TELNET_CHARSET);
  if (enc)
  {
    if (stringp(enc) &&
        !catch(configure_interactive(ME, IC_ENCODING, enc); publish))
    {
      // hat geklappt fertig
      return;
    }
    // wenn kein string oder nicht erfolgreich -> Prop zuruecksetzen
    tell_object(ME, sprintf(
          "Der von Dir eingestellte Zeichensatz \'%s\' konnte nicht "
          "eingestellt werden. Die Voreinstellung \'%s\' wurde wieder "
          "eingestellt.", enc,
          interactive_info(ME, IC_ENCODING)));
     SetProp(P_TELNET_CHARSET, 0);
  }
}

/** Belebt einen Netztoten wieder.
  Wird im Login gerufen, wenn der Spieler netztot war. Aequivalent zu
  start_player()
  @param[in] silent Wenn Flag gesetzt, werden keine Meldung an den Raum
  ausgegeben.
  @param[in] ip Textuelle Repraesentation der IP-Adresse, von der der Spieler
  kommt.
  @see start_player()
*/
varargs void Reconnect( int silent )
{
    int num;
    string called_from_ip;
    object *inv;

    if ( query_once_interactive(ME) )
    {
        // Erstmal - sofern vorhanden - den manuell konfigurierten Zeichensatz
        // einstellen. Im folgenden wird dann versucht, die TELOP CHARSET
        // auszuhandeln, die aendert das evtl. nochmal.
        set_manual_encoding();
        // perform the telnet negotiations. (all that are available)
        "*"::startup_telnet_negs();
        Set( P_LAST_LOGIN, time() );
    }

    enable_commands();
    set_living_name( getuid() );
    _remove_netdead();
    set_heart_beat(1);
    // Hunttimes aktualisieren und ggf. Feinde vergessen.
    update_hunt_times((time()-QueryProp(P_LAST_LOGOUT)) /__HEART_BEAT_INTERVAL__);
    // Heartbeats in Objekten im Inv reaktiveren.
    restart_heart_beats();
    // einige geerbte Module wollen ggf. was aufraeumen, neu initialisieren...
    "*"::reconnect();

    log_file( "syslog/shell/REENTER", sprintf( "%-11s %s, %-15s (%s).\n",
                                  capitalize(getuid(ME)), ctime(time())[4..15],
                                  query_ip_number(ME)||"Unknown",
                                  query_ip_name(ME)||"Unknown" ),
              200000 );

    if ( ndead_currently )
        ndead_revive();

    if ( !silent && interactive(ME) )
        call_notify_player_change(1);

    if ( query_once_interactive(ME) )
        modify_prompt();

    // Login-event ausloesen
    EVENTD->TriggerEvent(EVT_LIB_LOGIN, ([
          E_OBJECT: ME,
          E_PLNAME: getuid(ME),
          E_ENVIRONMENT: environment() ]) );

    catch( num = "secure/mailer"->FingerMail(geteuid());publish );

    if ( num )
        write( "Du hast " + num + " neue" + (num == 1 ? "n Brief" : " Briefe")
               + " im Postamt liegen.\n" );

    if ( QueryProp(P_AWAY) )
        write( break_string( "Du bist als abwesend gekennzeichnet: " +
                             QueryProp(P_AWAY) + ".", 78 ) );

    catch( RegisterChannels(); publish );

    if ( (called_from_ip = Query(P_CALLED_FROM_IP)) &&
         query_ip_number(ME) != called_from_ip ) {
        string tmp;

        if ( stringp(tmp = query_ip_name(called_from_ip)) &&
             tmp != called_from_ip )
            tmp = " [" + tmp + "]";
        else
            tmp = "";

        write( "Das letzte Mal kamst Du von " + called_from_ip + tmp + ".\n" );
    }

    Set( P_CALLED_FROM_IP, query_ip_number(ME) );

    // falls Gegenstaende mit 'upd -ar' upgedated wurden, muessen die
    // "verloren gegangenen" init()'s nachgeholt werden
    if ( query_once_interactive(ME) && !IS_LEARNER(ME) )
        call_out( "inits_nachholen", 0, Query(P_LAST_LOGOUT),
                  all_inventory(ME) );

    // noch nicht geclonte Autoloader "nach"clonen
    while ( remove_call_out("load_auto_objects") != -1 )
        /* do nothing */;

    if ( sizeof(autoload_rest) )
        call_out( "load_auto_objects", 0, autoload_rest );

    if (ndead_location) {
      catch( ndead_location->BecomesNetAlive(ME);publish );
      inv = all_inventory(ndead_location);
      ndead_location = 0;
    }
    else
      inv = ({});

    inv += deep_inventory(ME);

    //ZZ foreach statt call_other(), damit nen bug in BNA nicht die anderen
    //BNA verhindert.
    foreach(object ob: inv) {
        //es ist nicht auszuschliessen, dass Items durch BecomesNetAlive()
        //eines anderen zerstoert werden.
        if (objectp(ob))
          catch( call_other(ob, "BecomesNetAlive", ME);publish );
    }

    // Erst an dieser Stelle, weil der Spieler u.U. durch ein BecomesNetAlive()
    // noch bewegt wurde.
    if ( !silent && environment() && object_name(environment()) != NETDEAD_ROOM )
    {
        if(query_hc_play()<=1)
          tell_room(environment(),QueryProp(P_NAME) + " weilt wieder unter den Lebenden.\n",({ME}) );
        else 
          tell_room(environment(),QueryProp(P_NAME) + " weilt wieder unter den Verstorbenen.\n",({ME}) );
    }

    NewbieIntroMsg();

    if ( query_once_interactive(ME) )
        ListAwaited();
}

/** Loggt einen Spieler aus und macht ihn netztot.
  Bewegt einen Spieler in den Netztotenraum, deaktiviert Heartbeats im
  Inventar, ruft BecomesNetDead(), loest Erwartemeldungen aus, triggert
  Ausloggevent.
*/
void NetDead()
{
  object *inv;

  catch(RemoveChannels();publish);

  if(query_hc_play()>1)
    say("Ploetzlich weicht alle spirituelle Energie aus "+QueryProp(P_NAME)+".\n");
  else
    say("Ploetzlich weicht alles Leben aus "+QueryProp(P_NAME)+".\n");

  _set_netdead();
  remove_call_out("quit");
  remove_living_name();
  // Wird zwar im save_me() gemacht, aber die Zeitunterschiede beim
  // fingern direkt nach dem Ausloggen fuehren immer wieder zu Verwirrungen
  if(query_once_interactive(ME) && !QueryProp(P_INVIS))
      Set(P_LAST_LOGOUT,time());
  if (ME)
    ndead_location = environment();

  if (query_once_interactive(ME))
    call_notify_player_change(0);

  // Logout-event ausloesen
  EVENTD->TriggerEvent(EVT_LIB_LOGOUT, ([
          E_OBJECT: ME,
          E_PLNAME: getuid(ME),
          E_ENVIRONMENT: environment() ]) );

  set_next_reset(900);
  /* Bei Nicht-Magier-Shells wird comm::reset() aufgerufen, das prueft, ob
     der Spieler immer noch netztot ist, und falls ja, die tmhist loescht.
     Die Methode wird von /std/shells/magier.c ueberschrieben, netztote
     Magier (die eigentlich schon anderweitig beseitigt worden sein sollten)
     werden remove()d und destruct()ed. --Amynthor 05.05.2008 */

  if (environment()) {
    catch(environment()->BecomesNetDead(ME);publish);
    inv = deep_inventory(ME)+all_inventory(environment());
  }
  else inv=deep_inventory(ME);
  foreach(object ob: inv) {
      if (objectp(ob)) //man weiss nie was BND() macht...
          catch( call_other(ob, "BecomesNetDead", ME);publish );
  }
}


/** Sendet ggf. Telnet Timing Marks als Keep-Alive Pakete an den Client.
  * Wird in heart_beat() gerufen.
  * @return 1, falls der Spieler Keep-Alive Paket wuenscht, sonst 0. 
  * @see heart_beat()
*/
protected int CheckTelnetKeepAlive(int delay) {
  if (telnet_tm_counter > 0) {
    // Spieler hat offenbar ein Keep-Alive konfiguriert ...
    if (!(--telnet_tm_counter)) {
      // und das Intervall ist gerade abgelaufen.
      // Prop offenbar gesetzt, FEature ist wirklich gewuenscht,
      // Telnet Timing Mark senden 
      send_telnet_timing_mark();
      // alle 120 HBs (240s, 4min).
      // sollte eigentlich 240 / __HEART_BEAT_INTERVAL__ sein. Aber spart
      // eine Operation im HB. ;-)
      telnet_tm_counter = delay || 120;
    }
    return 1; // Keep-Alive ist eingeschaltet
  }
  return 0;
}

static void ndead_move_me();

/** Heartbeat des Spielerobjektes.
  Prueft taegliche Spielzeit, speichert regelmaessig den Spieler,
  bewegt Netztote in den Netztodenraum, ruft die HBs aus living/combat und
  player/life.
*/
protected void heart_beat() {
  if (!ME)
    return;
  if (ndead_currently)
  {
    if (interactive(ME))
    {
      ndead_revive();
      ndead_location=0;
    }
    else return;
  }
  else
    if (!(ndead_next_check--))
    {
      ndead_next_check=NETDEAD_CHECK_TIME;
      if (!interactive(ME))
        if (ndead_lasttime)
        {
          save_me(1);
          if (IS_LEARNER(ME))
          {
            quit();
            if (ME)
              remove();
            if (ME)
              destruct(ME);
            return;
          }
          ndead_move_me();
          // Zumindest bei Gaesten ist das Objekt jetzt zerstoert
          if (!objectp(this_object()))
            return;
        }
        else
          ndead_lasttime=1;
    }
  if (ME && ndead_lasttime && interactive(ME))
    ndead_lasttime=0;
  if (CheckDailyPlaytime())
    return;

  CheckTelnetKeepAlive(QueryProp(P_TELNET_KEEPALIVE_DELAY));

  life::heart_beat();
  combat::heart_beat();
  skills::heart_beat();
}

/** ID-Funktion fuer Spielerobjekte.
 * id() fuer Spieler. Besondere Behandlung fuer Froesche und Geister,
 * sowie Invis-Status.
 * Gibt immer 0 zurueck, wenn P_INVIS && lvl < P_LEVEL
 * @param[in] str angefragter ID-String
 * @param[in] lvl Level des Anfragenden
 * @return 1, falls str auf den Spieler zutrifft, 0 sonst.
 */
varargs int id(string str, int lvl)
{
  if (::id(str))
    return 1;
  if (Query(P_INVIS) && lvl < QueryProp(P_LEVEL))
    return 0;
  if (QueryProp(P_GHOST)&& str == "geist von "+ lower_case(QueryProp(P_NAME)))
    return 1;
  if (QueryProp(P_FROG) && str == "frosch") return 1;
  return(0);
}

/** Setzt Spielerhomepage (Spielerkommando).
  * @param[in] str Spielereingabe
  * @return 1 bei Erfolg, 0 sonst.
  */
static int set_homepage(string str)
{
  mixed tmp;
  if (!(str=_unparsed_args())) {
    if (!QueryProp(P_HOMEPAGE))
      write("Du hast keine URL-Adresse gesetzt!\n");
    else
      write("Deine offizielle URL-Adresse lautet: " + QueryProp(P_HOMEPAGE)
            +"\n");
    return 1;
  }
  write("Deine offizielle URL-Adresse wurde geaendert.\n");
  if (str=="keine") 
    SetProp(P_HOMEPAGE, 0);
  else {
    tmp = filter(regexplode(str, "[<][^>]*[>]"), function int (string e) {
              return (e[0] != '<');
            });
    write("Sie lautet jetzt: "+(str = implode(tmp, ""))+"\n");
    SetProp(P_HOMEPAGE, str);
  }
  return 1;
}

/** Setzt Spieler-Wohnort (Spielerkommando).
  * \param[in] str Spielereingabe
  * \return 1 bei Erfolg, 0 sonst.
  */
static int set_location( string str )
{
    if ( str == "0" || str == "loeschen" ){
        Set( P_LOCATION, 0 );
        write( "Du loescht Deine Ortsangabe.\n" );
        return 1;
    }

    if ( stringp(str = _unparsed_args()) && str != "" ){
        Set( P_LOCATION, capitalize(str) );
        printf( "Du aenderst Deine Ortsangabe auf \"%s\".\n",
                Query(P_LOCATION) );
    }
    else if ( stringp(Query(P_LOCATION)) )
        printf( "Deine Ortsangabe lautet \"%s\".\n", Query(P_LOCATION) );
    else{
        Set( P_LOCATION, 0, F_VALUE );
        write( "Du hast keine Ortsangabe gesetzt.\n" );
    }

    return 1;
}

/** Setzt ICQ-UIN des Spielers (Spielerkommando).
  * \param[in] str Spielereingabe
  * \return 1 bei Erfolg, 0 sonst.
  */
static int set_icq(string str) {
  int num;

  if (!str || str=="") {
    if (!num=QueryProp(P_ICQ))
      write("Du hast keine ICQ-Nummer gesetzt!\n");
    else
      printf("Deine ICQ-Nummer lautet: %d\n",num);
    return 1;
  }
  if (sscanf(str,"%d",num)!=1 || !num) {
    write("Deine ICQ-Nummer wurde geloescht.\n");
    SetProp(P_ICQ, 0);
  } else {
    write("Deine ICQ-Nummer wurde geaendert.\n");
    printf("Sie lautet jetzt: %d\n",num);
    SetProp(P_ICQ, num);
  }
  return 1;
}

/** Setzt Instant Messanger vom Spieler (Spielerkommando).
  * \param[in] str Spielereingabe
  * \return 1 bei Erfolg, 0 sonst.
  */
static int set_messenger(string str) {
  int num;
  string s;

  if (!str || str=="") {
    if (!s=QueryProp(P_MESSENGER))
      if (!num=QueryProp(P_ICQ)) 
        write("Du hast keine Messenger-ID gesetzt.\n");
      else 
        printf("Du hast keine Messenger-ID gesetzt, aber eine ICQ-Nummer: %d\n", num);
    else
      printf("Deine Messenger-ID lautet: %s\n", s);
    return 1;
  }
  if (str=="loeschen" || str=="keine") {
    write("Deine Messenger-ID wurde geloescht.\n");
    SetProp(P_MESSENGER, 0);
  } else {
    s = _unparsed_args();
    printf("Deine Messenger-ID lautet nun: %s\n", s);
    SetProp(P_MESSENGER, s);
  }
  return 1;
}


// Prueft, ob der String vermutlich eine eMail-Adresse ist.
// dies ist nicht narrensicher, wird aber die meisten eMail-Adressen zulassen
// und viel Schrott ablehnen.
private string check_email(string str) {
  if (!stringp(str)) return 0;
  return regmatch(lower_case(str),
      "[a-z0-9._%+-]+@(?:[a-z0-9-]+\.)+[a-z]{2,4}",RE_PCRE);
}

/** Setzt Email-Adresse des Spielers (Spielerkommando).
  * \param[in] str Spielereingabe
  * \return 1 bei Erfolg, 0 sonst.
  */
static int set_email(string str)
{
  if (!(str=_unparsed_args())) {
    write("Deine offizielle Email-Adresse lautet: " + QueryProp(P_MAILADDR)
          +"\n");
    return 1;
  }
  str = check_email(str);
  if (!str) {
    notify_fail("Deine Eingabe scheint keine gueltige EMail-Adresse "
        "zu sein.\n");
    return 0;
  }
  write("Deine EMail-Adresse wurde geaendert zu:\n"
      +str+"\n");
  SetProp(P_MAILADDR, str);
  return 1;
}

/** Spielerkommando 'selbstloeschung'.
  * Gibt Meldung aus und fragt nach einer Bestaetigung.
  * \return 1 bei Erfolg, 0 sonst.
  */
static int self_delete()
{
  string msg = sprintf("%s\n"
    "Wenn Du Dich selbstloeschen willst, ist Dein Charakter UNWIDERRUFLICH "
    "verloren. Es gibt KEINE Moeglichkeit, ihn wiederzuerschaffen. Solltest "
    "Du nur zeitweilig vom "MUDNAME" wegbleiben wollen, so benutze bitte "
    "den Befehl 'spielpause'.\n"+
    "Fallst Du %s immer noch selbstloeschen willst, gib Dein Password "
    "ein.\n\n",
    (QueryProp(P_NO_ASCII_ART) ? "Bist Du Dir wirklich sicher?\n"
     : "     B I S T  D U  D I R  W I R K L I C H  S I C H E R ???????\n"),
    query_real_name());
  write(break_string(msg,78,0,BS_LEAVE_MY_LFS));
  input_to("self_delete2",INPUT_PROMPT|INPUT_NOECHO, "Bitte das Password angeben: ");
  return 1;
}

/** Spielerkommando 'selbstloeschung'.
  * Empfaengt Bestaetigung des Spielers und ruft Loeschfunktion in 
  * /secure/master auf.
  * \param[in] str Spielereingabe
  * \return 1 bei Erfolg, 0 sonst.
  */
int self_delete2(string str)
{
  int ret;
  ret=({int})"secure/master"->delete_player(str, getuid(PL));
  if (!ret)
  {
    write("Das hat nicht hingehauen (Jof sei Dank ....)\n");
    return 1;
  }
  if (QueryProp(P_GUILD)&&file_size(GUILD_DIR+QueryProp(P_GUILD)+".c")>-1)
    catch(call_other(GUILD_DIR+QueryProp(P_GUILD), "austreten");publish);

  if (QueryProp(P_DEADS) < 5) {
    write("Adios! Man sieht sich.\n");
    say(name(WER,1)+" hat sich gerade selbst zerstoert.\n");
  }
  else {
    write(
      "\nTod kommt auf seinem weissen Pferd angeritten.\n"
     +"Er steigt ab, baut sich drohend vor Dir auf und mustert Dich schadenfroh.\n"
     +"\nTod sagt: ENDLICH! NUN KANN DIR AUCH LARS NICHT MEHR HELFEN!\n"
     +"\nTod holt weit mit seiner Sense aus. Mit grossem Schwung laesst er sie auf\n"
     +"Dich zusausen und dann...\n");
    say(name(WER,1)+" schied gerade endgueltig von uns.\n");
  }

  // Event ausloesen. ;-)
  EVENTD->TriggerEvent(EVT_LIB_PLAYER_DELETION, ([
        E_PLNAME: getuid(ME),
        E_ENVIRONMENT: environment(),
        E_GUILDNAME: QueryProp(P_GUILD) ]) );

  remove(1);
  return 1;
}

/** Setzt neue taegliche Spieldauer (Spielerkommando).
  * \param[in] str Spielereingabe
  * \return 1 bei Erfolg, 0 sonst.
  */
static int spieldauer(string str) {
  int min,day;

  notify_fail("  spieldauer <x> minuten fuer %d tage\noder\n"+
              "  spieldauer <x> stunden fuer %d tage\n");
  if (!str)
    return 0;
  if (sscanf(str,"%d stunde%~s fuer %d tag%~s",min,day)==4)
    min*=60;
  else if (sscanf(str,"%d minute%~s fuer %d tag%~s",min,day)!=4)
    return 0;
  if (min<5)
    min=5;
  if (min>=1440)
    return notify_fail("Witzbold.\n"),0;

  Set(P_DAILY_PLAYTIME,
      ({min*60,time()/86400+day,time()/86400,time(),min*60}));
  // 0:Minuten pro Tag, 1:Nr. des letzen Tages, 2:Nr. des angefangenen Tages,
  // 3:letzte Zeitpruefung, 4:verbleibende Zeit
  printf("Du darfst die naechsten %d Tag(e) nur noch\n"+
   "%d Minuten am Tag mudden.\n",day,min);
  return 1;
}

/** Interpretiert Angabe des Spielers fuer Spielpause.
  * \param[in] a Zeitangabe fuer Spielpause.
  * \return Zeitpunkt, Ende der Spielpause.
  */
private int InterpretTime(string a){
  // akzeptiert folgende Formate:
  //   dd.mm.jj     (Rueckgabe: 0:00 des entsprechenden Tages)

  int *ts = allocate(9);
  int i,j,k,nrargs;

  if ((nrargs=sscanf(a,"%d.%d.%d",i,j,k))==3 ||
      (nrargs=sscanf(a,"%d.%d.",i,j))==2) {
    // wenn kein jahr angegeben ist, das aktuelle nehmen.
    if (nrargs == 2)
       ts[TM_YEAR] = localtime()[TM_YEAR];
    else {
       // Zwei-Ziffern-Angabe des Jahres...
       if (k<100)
           k += 2000;
       ts[TM_YEAR] = k;
    }
    ts[TM_MDAY] = i;
    ts[TM_MON] = j - 1;

    int zeit = mktime(ts);

    // negative und vergangene Zeiten pruefen.
    if (zeit <= time()) {
      write("Dieser Zeitpunkt liegt in der Vergangenheit.\n");
      return 0;
    }
    return zeit;
  }
  return 0;
}

/** Setzt neue Spielpause (Spielerkommando).
  * Fragt vorher nach Bestaetigung durch den Spieler.
  * \param[in] str Spielereingabe
  * \return 1 bei Erfolg, 0 sonst.
  * \sa spielpause2()
  */
static int spielpause(string str)
{
  int days,endezeit;
  string foo;

  notify_fail("spielpause <x> tage          oder\n"+
              "spielpause bis tt.mm[.jj]\n");
  if (!str) return 0;
  if(sscanf(_unparsed_args(),"bis %s",foo)==1) {
    endezeit = InterpretTime(foo);
    if (endezeit == 0) 
        return 0;
    days = ((endezeit - time()) / 86400) + 1;
  }
  else if(sscanf(str, "%d tag%s", days, foo) == 2) {
    if (days < 0)
        days = -1;
    else
        endezeit = (time()/86400) * 86400 + days * 86400;
  }
  else return 0;

  if (days > 0)
    write(strftime("Du wirst Dich erst wieder am %d.%m.%Y einloggen koennen!\n",
          endezeit));
  else if (days < 0)
    write( "Du wirst Dich auf unbestimmte Zeit nicht mehr einloggen koennen.\n"
          +"Wenn Du wieder spielen willst, musst Du Dich an einen Gott oder\n"
          +"Erzmagier wenden (mit einem Gast oder Mail von aussen).\n" );
  else {
    write( "Die Spielpause ist aufgehoben.\n" );
    master()->TBanishName(getuid(this_object()), 0);
    return 1;
  }
  write( "Wenn Du das wirklich willst, gib jetzt 'ja' ein.\n" );
  input_to( "spielpause2", INPUT_PROMPT, "]", days);
  return 1;
}

/** Setzt neue taegliche Spieldauer, wird ueber spielpause() gerufen.
  * \param[in] str Spielereingabe, Bestaetigung
  * \param[in] days Dauer der Spielpause (in Tagen).
  * \sa spielpause()
  */
static void spielpause2(string str, int days)
{
  if (str && (str == "ja" || str == "Ja" || str == "JA")) {
    master()->TBanishName(getuid(this_object()), days);
    write(
      "Ok, die Spielpause wird mit dem naechsten Ausloggen wirksam.\n"
     +"Solltest Du es Dir bis dahin noch einmal ueberlegt haben, so kannst\n"
     +"Du den Vorgang mit 'spielpause 0 tage' wieder rueckgaengig machen.\n" );
    return;
  }
  write("Vorgang wurde abgebrochen.\n" );
}

/** Setzt neues Passwort (Spielerkommando).
  * Fragt nach altem Passwort und ruft change_password2().
  * \return 1 bei Erfolg, 0 sonst.
  * \sa change_password2(), change_password3(), change_password4()
  */
static int change_password() {
  string verb;
  verb=query_verb();
  if (verb!="passwd"&&verb!="password"&&verb!="passwort")
    return 0;
  input_to("change_password2",INPUT_NOECHO|INPUT_PROMPT,
      "Bitte das ALTE Passwort angeben: ");
  return 1;
}

/** Setzt neues Passwort (Spielerkommando).
  * Prueft altes Passwort, fragt nach neuem Passwort und ruft 
  *   change_password3().
  * \param[in] str Spielereingabe des alten Passwortes
  * \return 1 bei Erfolg, 0 sonst.
  * \sa change_password(), change_password3(), change_password4()
  */
static int change_password2(string str) {
  write("\n");
  if (!str)
    str="";
  if (MASTER->update_password(str,str) == 0) {
    write("Falsches Passwort!\n");
    return 1;
  }
  input_to("change_password3",INPUT_NOECHO|INPUT_PROMPT,
      "Bitte das NEUE Passwort eingeben: ", str);
  return 1;
}

/** Setzt neues Passwort (Spielerkommando).
  * Prueft neues Passwort, fragt nach Bestaetigung des neues
  * Passwortes und ruft change_password4().
  * \param[in] str Spielereingabe des neuen Passwortes
  * \return 1 bei Erfolg, 0 sonst.
  * \sa change_password(), change_password2(), change_password4()
  */
static int change_password3( string str, string passwold )
{
    write( "\n" );

    if ( !str || str == "" ){
        write( "Abgebrochen !\n" );
        return 1;
    }

    if ( passwold == str ){
        write( "Das war Dein altes Passwort.\n" );
        input_to( "change_password3", INPUT_NOECHO|INPUT_PROMPT,
            "Bitte das NEUE Passwort eingeben (zum Abbruch Return "
            "druecken): ", passwold);
        return 1;
    }

    if ( !MASTER->good_password( str, getuid(ME) ) ){
        input_to( "change_password3", INPUT_NOECHO|INPUT_PROMPT,
            "Bitte das NEUE Passwort eingeben: ", passwold);
        return 1;
    }

    input_to( "change_password4", INPUT_NOECHO|INPUT_PROMPT,
        "Bitte nochmal: ", passwold, str);
    return 1;
}

/** Setzt neues Passwort (Spielerkommando).
  * Prueft neues Passwort und setzt neues Passwort.
  * \param[in] str Spielereingabe des neuen Passwortes
  * \return 1 bei Erfolg, 0 sonst.
  * \sa change_password(), change_password2(), change_password3()
  */
static int change_password4( string str, string passwold, string passwnew )
{
    write( "\n" );

    if ( !str || str != passwnew ){
        write( "Das war verschieden! Passwort NICHT geaendert.\n" );
        return 1;
    }

    if ( MASTER->update_password( passwold, passwnew ) )
        write( "Passwort geaendert.\n" );
    else
        write( "Hat nicht geklappt!\n" );

    return 1;
}


/*
 *-----------------------------------------------------------------
 * Rueckmeldungen von Spielern an Magier
 *-----------------------------------------------------------------
 */
static int fehlerhilfe(string str) {
  ReceiveMsg("Welche Art von Fehler moechtest Du denn melden?\n"
      "Fehlfunktionen    ->  bug\n"
      "Ideen/Anregungen  ->  idee\n"
      "Tippfehler/Typos  ->  typo\n"
      "fehlende Details  ->  detail\n"
      "Syntax-Probleme   ->  syntaxhinweis",
      MT_NOTIFICATION|MSG_BS_LEAVE_LFS);

  return 1;
}

static varargs int ReportError2(string player_input, string error_type)
{
  if ( stringp(player_input) && sizeof(player_input) &&
       player_input != "~q" && player_input != ".." )
  {
    object obj;
    // Eingabe am : aufsplitten, da dieser als Trennzeichen verwendet wird,
    // wenn sich die Eingabe auf ein bestimmtes Objekt beziehen soll. Das
    // erste Element im Ergebnisarray wird dann als dessen ID aufgefasst,
    // nach der weiter unten gesucht wird.
    // Zusaetzlich Whitespace drumherum wegschneiden.
    string *input_segments = regexplode(player_input,
      "[[:blank:]]*:[[:blank:]]*", RE_OMIT_DELIM | RE_PCRE);

    // Wenn mind. 2 Segmente erzeugt wurden, soll offenbar ein bestimmtes
    // Objekt angesprochen werden, so dass dieses nun gesucht werden kann.
    if ( sizeof(input_segments) > 1 )
    {
      // Findet sich hinter dem ersten : nur ein Leerstring, wird davon
      // ausgegangen, dass da keine Meldung kam.
      // Fuer seltene Sonderfaelle wie mehrfache ":" wird noch geprueft, ob
      // von der rekonstruierten Meldung abzueglich der ":" ein Leerstring
      // uebrigbleibt. Wenn ja, war es wohl wirklich eine unbrauchbare
      // Meldung, und es wird eine neue angefordert.
      if ( input_segments[1] == "" &&
           implode(input_segments[1..],"") == "" )
      {
        _notify("Du hast hinter dem : nix eingegeben, bitte nochmal "
          "versuchen.\n", MA_UNKNOWN);
        // Eine neue Eingabe wird aber nur angefordert, wenn der aktuelle
        // Input ohnehin schon aus einem input_to()-Durchlauf stammte.
        // In diesem Fall ist extern_call() wahr.
        if ( extern_call() )
          input_to("ReportError2", INPUT_PROMPT, "]", error_type);
        return 1;
      }

      // ID kleinschreiben, denn nur eine solche kann von Spielern eingegeben
      // werden, um ein Objekt anzusprechen, wir haben aber die Eingabe
      // per _unparsed_args(0) geholt, d.h. ggf. mit Grossbuchstaben.
      string obnam = lower_case(input_segments[0]);
      if (obnam == "hier" || obnam == "raum")
        obj = environment(this_player());
      else
      {
        // Und das Objekt suchen, dabei zuerst im env() schauen. Nur present()
        // ginge auch, wuerde aber zuerst im Inv suchen, was aber nicht
        // gewuenscht ist.
        obj = present(obnam, environment(this_object())) || present(obnam);
      }

      // Wenn das Objekt gefunden wird, wird nur der Teil hinter dem ersten
      // : rekonstruiert und als Fehlermeldung abgesetzt.
      // Gibt es ein solches Objekt nicht, wird der Raum als Fallback
      // verwendet. Die Eingabe vor dem : gehoerte dann offenbar zur Meldung
      // dazu, und es wird dann spaeter die gesamte Spielereingabe im Original
      // als Fehlermeldung abgesetzt.
      if ( objectp(obj) )
        player_input = implode(input_segments[1..],":");
      else
        obj = environment(this_object());
    }
    else
    {
      // Hat der Spieler keinen : verwendet, verwenden wir das Bezugsobjekt
      // oder den aktuellen Raum.
      obj = QueryProp(P_REFERENCE_OBJECT);
      if (!objectp(obj) || !present(obj))
        obj = environment(this_interactive());
    }
    smart_log(error_type, player_input, obj);
  }
  else
    _notify("Eingabe abgebrochen.\n", MA_UNKNOWN);
  return 1;
}

static int ReportError(string player_input)
{
  // ungeparstes Kommando einlesen, um die Meldung unmodifiziert zu erhalten
  player_input = _unparsed_args(0);
  string pl_msg, error_type;

  // Anhand des eingegebenen Kommandoverbs wird der Typ der Fehlermeldung
  // ermittelt.
  switch(query_verb())
  {
    case "idee":
      pl_msg = "Was fuer eine Idee hast Du denn?\n";
      error_type = "IDEA";
      break;
    case "md":
    case "detail":
      pl_msg = "Fuer welches Detail fehlt denn die Beschreibung?\n";
      error_type = "DETAILS";
      break;
    case "typo":
      pl_msg = "Wo ist denn der Tippfehler?\n";
      error_type = "TYPO";
      break;
    case "bug":
      pl_msg = "Wie sieht der Fehler denn aus?\n";
      error_type = "BUGS";
      break;
    case "syntaxhinweis":
      pl_msg = "Mit welcher Syntax gibt es denn was fuer Probleme?\n";
      error_type = "SYNTAX";
      break;
  }

  // Hat der Spieler etwas eingegeben, wird die Eingabe direkt an die Hilfs-
  // funktion weitergereicht. Ansonsten wird eine Meldung ausgegeben und die
  // Eingabe manuell per input_to() abgefragt.
  if ( stringp(player_input) && sizeof(player_input) )
  {
    return ReportError2(player_input, error_type);
  }
  ReceiveMsg(pl_msg, MT_NOTIFICATION);
  input_to("ReportError2", INPUT_PROMPT, "]", error_type);
  return 1;
}

static void confirm_error(string answer, object obj, string str,
                          string myname, string desc, mapping err)
{
  if (answer != "j" && answer != "ja")
  {
    _notify("Eingabe abgebrochen.\n", MA_UNKNOWN);
    return;
  }
  if (!obj)
  {
    _notify(sprintf("Leider existiert das Objekt nicht mehr, an dem Du "
            "D%s melden wolltest.", desc), MA_UNKNOWN);
    return;
  }

  // ggf. will das Objekte selber loggen, dann wird nicht zentral geloggt.
  if (obj->SmartLog(0, myname, str, strftime("%d. %b %Y")))
  {
    _notify(sprintf(
       "Du hast an %s erfolgreich %s abgesetzt.\n"
       "Hinweis: Das Objekt selber hat die Meldung protokolliert.",
       (obj->IsRoom() ? "diesem Raum" : obj->name(WEM,1)),desc),
       MA_UNKNOWN);
  }
  else
  {
    // Eintragung in die Fehler-DB
    string hashkey = ({string})ERRORD->LogReportedError(err);
    _notify(sprintf(
          "Ein kleiner Fehlerteufel hat D%s an %s unter der ID %s "
          "notiert.", desc,
          (obj->IsRoom() ? "diesem Raum" : obj->name(WEM,1)),
          hashkey || "N/A"),
        MA_UNKNOWN);
  }
  _notify("Vielen Dank fuer die Hilfe.\n", MA_UNKNOWN);
}

/** Loggt eine Spielermeldung an Magier.
  * Loggt die Spielermeldung in das passende File unter /log/report/ oder im
  * vom Magier gewuenschten File. Hierbei werden Fehler, Ideen, MDs und Typos
  * in getrennte Files sortiert.
  * \param[in] str Spielermeldung
  * \param[in] myname Art der Spielermeldung (DETAILS, BUG, TYPO, MD)
  * @see md(string), idea(string), bug(string), typo(string)
  */
protected void smart_log(string myname, string str, object obj)
{
  mapping err = ([ F_PROG: "unbekannt",
           F_LINE: 0,
           F_MSG: str,
           F_OBJ: obj
         ]);

  string desc="etwas unbekanntes";
  switch(myname) {
    case "BUGS":
      desc="einen Fehler";
      err[F_TYPE]=T_REPORTED_ERR;
      break;
    case "DETAILS":
      desc="ein fehlendes Detail";
      err[F_TYPE]=T_REPORTED_MD;
      break;
    case "IDEA":
      desc="eine Idee";
      err[F_TYPE]=T_REPORTED_IDEA;
      break;
    case "TYPO":
      desc="einen Typo";
      err[F_TYPE]=T_REPORTED_TYPO;
      break;
    case "SYNTAX":
      desc="einen Syntaxhinweis";
      err[F_TYPE]=T_REPORTED_SYNTAX;
      break;
  }
  _notify(sprintf(
          "Du hast %s an %s mit der Beschreibung \"%s\" eingegeben. "
          "Moechtest Du dieses speichern?", desc,
          (obj->IsRoom() ? "diesem Raum" : obj->name(WEM,1)),
          str), MA_UNKNOWN);
  input_to("confirm_error", INPUT_PROMPT, "]", obj, str, myname, desc, err);
}

/** Speichert den Spieler und loggt ihn aus (Spielerkommando 'ende').
  * Der Spieler wird vollstaendig ausgeloggt, d.h. das Spielerobjekt
  * zerstoert.
  * \return 1 bei Erfolg, 0 sonst.
  * @see disconnect()
  */
int quit()
{
  SetProp(P_LAST_QUIT,time());
  catch(RemoveChannels();publish);
  if(!QueryGuest())
  {
    save_me(0);
    tell_object(ME,"Speichere "+QueryProp(P_NAME)+".\n");
  }

  if (interactive(ME))
    call_notify_player_change(0);

  remove_living_name();
  // EVT_LIB_LOGOUT wird in remove() getriggert.
  if(catch(remove();publish)) destruct(ME);
  return 1;
}

/** Wrapper im quit() herum, verhindert 'ende', falls Spieler kaempft.
  * \return 0 oder Rueckgabewert von quit()
  * @see quit()
  */
static int new_quit() {
  notify_fail("Du bist in Gedanken noch bei Deinem letzten Kampf.\n"+
              "Warte noch etwas bevor Du das Spiel verlaesst,\n"+
              "damit Du so nicht in RL weitermachst...\n");
  if (time()-Query(P_LAST_COMBAT_TIME)<120 && !IS_LEARNING(ME))
    return 0;
  return quit();
}

/** Gibt die Infos ueber den Char an den Spieler aus (Spielerkommando 'info').
  * \param[in] arg Wenn arg=="short", wird eine Kurzuebersicht ausgegeben.
  * \return 1
  * @see short_score()
  */
static int score(string arg) {
  int val;
  mixed ind;
  object *enem1, *enem2, *inv;

  if (QueryProp(P_GHOST)) {
    write("Im ewigen Leben gibt es keine Punkte.\n");
    return 1;
  }

  int plev = LEPMASTER->QueryLevel();
  <string|int> tmp = QueryProp(P_GENDER);
  string gender;
  switch(tmp) {
  case MALE: gender = "maennlich"; break;
  case FEMALE: gender = "weiblich"; break;
  case NEUTER: gender = "neutral"; break;
  default: gender = "unbekannt";
  }

  ind = m_indices(QueryProp(P_ATTRIBUTES));
  tmp = "";
  foreach(string index: ind) {
    string aname;
    switch (index) {
      case "int": aname = "Intelligenz"; break;
      case "con": aname = "Ausdauer"; break;
      case "dex": aname = "Geschicklichkeit"; break;
      case "str": aname = "Kraft"; break;
      default:
        if(stringp(index)) aname = capitalize(index);
        else aname = "Unbekannt";
    }
    aname = sprintf("%-18'.'s %2.2d", aname+" ", QueryRealAttribute(index));
    if((val = QueryAttributeOffset(index)))
      aname += sprintf(" (%s%d)", (val>=0?"+":""), val);
    tmp += aname + "\n";
  }

  printf("- %-'-'68s\n",
         TeamPrefix()+capitalize(implode(explode(short()||"","\n"),""))+" ");
  if(arg!="short") {
    printf("Rasse ............ %-' '18s Abenteuer ........ %d %s\n",
           QueryProp(P_RACE), QueryProp(P_QP),
           (val = QM->QueryTotalQP()) == QueryProp(P_QP) ? "" : "("+val+")");
    printf("Geschlecht ....... %-' '18s Groesse .......... %d cm\n",
           gender, QueryProp(P_SIZE));
    printf("Stufe ............ %-3.3d %-' '14s Gewicht .......... %d kg\n",
           QueryProp(P_LEVEL), (QueryProp(P_LEVEL) < plev ? "("+plev+")" : ""),
           QueryProp(P_WEIGHT) / 1000);
    printf("Gilde ............ %-' '18s Gildenstufe ...... %d\n",
         capitalize(QueryProp(P_GUILD)), QueryProp(P_GUILD_LEVEL));
  }
  printf("Erfahrung ........ %-' '18s Charakter ........ %-s\n\n",
         QueryProp(P_XP)+ " Punkte", al_to_title(QueryProp(P_ALIGN)));
  printf("%#-76.2s\n\n", tmp);
  printf("Gesundheit ....... %-3.3d %-' '14s Gift ............. %s\n",
         QueryProp(P_HP),
         (QueryProp(P_HP) == (val = QueryProp(P_MAX_HP)) ? "" : "("+val+")"),
         ((val = QueryProp(P_POISON)) ?
          (val < 4 ? "leicht" : "gefaehrlich") : "gesund"));
  printf("Konzentration .... %-3.3d %-' '14s Vorsicht ......... %s\n",
         QueryProp(P_SP),
         (QueryProp(P_SP) == (val = QueryProp(P_MAX_SP)) ? "" : "("+val+")"),
         ((ind = QueryProp(P_WIMPY)) ? ""+ind : "mutig"));
  printf("Todesfolgen....... %-' '18s %s\n",
         ((val = death_suffering()) ? ""+((val+9)/10) : "kein Malus"),
         (QueryProp(P_WIMPY) && ind=QueryProp(P_WIMPY_DIRECTION))
         ? sprintf("Fluchtrichtung ... %O", ind) : "");
  printf("%s",
         (time()-Query(P_LAST_COMBAT_TIME)<120 && !IS_LEARNING(ME)) ?
         "Spiel verlassen .. nicht moeglich\n" : ""
         );

  if(arg!="short") {
    write(break_string(Forschung(), 70));
    if(ind=QueryProp(P_AWAY))
      printf("Du bist nicht ansprechbar: %O\n",ind);
  }

  if(sizeof(enem1=(QueryEnemies())[0])) {
    enem2=({});
    inv=all_inventory(environment(ME));
    foreach(object en: enem1) {
      if (member(inv,en)==-1) // Ist unser Feind und ist nicht hier
        enem2+=({en});
    }
    if(sizeof(enem2))
    {
      write(break_string(
            "Du verfolgst " + CountUp(map_objects(enem2, "name", WEN))+".",
            78));
    }
  }
  if(arg!="short") show_age();
  printf("%-'-'70s\n", "");
  return 1;
}

/** Gibt eine kuerzere Info ueber den Char aus (Spielerkommando punkte|score).
    Ruft score("short").
  * \param[in] arg UNUSED
  * \return 1 bei Erfolg, 0 sonst.
  * @see score(string), very_short_score()
  */
static int short_score(string arg) {
  return score("short");
}

/** Gibt eine Miniinfo ueber LP / KP aus (Spielerkommando: kurzinfo)
  * \return 1
  * @see score(string), short_score(string)
  */
static int very_short_score(string arg) {
  int    lp,mlp,xlp,kp,mkp,xkp;
  string bar;

  lp=QueryProp(P_HP); mlp=QueryProp(P_MAX_HP);
  kp=QueryProp(P_SP); mkp=QueryProp(P_MAX_SP);
  if (mlp)
    xlp=(lp*40/mlp);
  if (mkp)
    xkp=(kp*40/mkp);
  bar="  .    .    .    .    .    .    .    .  ";
  if (QueryProp(P_NO_ASCII_ART) || arg == "-k")
    printf("Gesundheit: %3.3d (%3.3d), Konzentration: %3.3d (%3.3d)\n",
           lp, mlp, kp, mkp);
  else
    printf("Gesundheit:    0 |%'#'40.40s| %3.3d%s\n"+
           "Konzentration: 0 |%'#'40.40s| %3.3d%s\n",
           (xlp<0?bar:bar[xlp..]),lp,(lp==mlp?"":sprintf(" (%d)",mlp)),
           (xkp<0?bar:bar[xkp..]),kp,(kp==mkp?"":sprintf(" (%d)",mkp))
           );
  return 1;
}

/** Gibt eine Manpage/Hilfeseite an den Spieler aus.
    Beruecksichtigt hierbei die Synonymliste aus dir/.synonym, um die richtige
    Manpage auszugeben.
  * \param[in] dir Verzeichnis der gewuenschten Manpage
  * \param[in] page Name der gewuenschten Manpage
  * \return String der gewuenschten Manpage
  */
static string getmanpage(string dir, string page)
{
  string text, *syn;
  int i;

  if (dir[<1] != '/')
    dir += "/";

  if ((text=read_file(dir+page)) && sizeof(text))
    return text;

  if (text = read_file(dir+".synonym")) {
    syn = regexplode(text, "([ \t][ \t]*|\n)");
    if ((i=member(syn, page))!=-1)
      return read_file(dir+syn[i+2]);
  }
  return 0;
}

/** Gibt eine Hilfeseite an den Spieler aus (Spielerkommando hilfe|man).
  * Die Hilfeseite wird in div. Verzeichnissen gesucht (je nach Gilde,
  * Magier-/Spielerstatus).
  * \param[in] str Name der gewuenschten Hilfeseite.
  * \return 1
  */
static int help(string str) {
  string verb, rest, text, gilde;
  mixed found;

  found=0;
  text = "";
  if (str) {
    str = implode( explode(str, ".." ), "");

    if ( sscanf( str, "gilde %s %s", gilde, rest)==2)
      str=rest;
    else
      gilde=QueryProp(P_GUILD);
    if (!gilde) gilde="abenteurer";

    if ( sscanf( str, "%s %s",verb,rest )==2 ) str = verb;

    if ((IS_LEARNER(PL)) ) {
      if (rest = getmanpage("/doc/wiz/",str)) {
        found = 1;
        text += rest;
      }
      else if (rest = getmanpage("/doc/mcmd/", str)) {
        found = 1;
        text += rest;
      }
    }

    if ((IS_SEER(PL)) /*&& !found*/ ) {
      if (rest = getmanpage("/doc/scmd/",str)) {
        if (found)
          text += "\n--------------------\n";
        found = 1;
        text += rest;
      }
    }

    if (rest = getmanpage("/doc/g."+gilde+"/",str)) {
      if (found)
        text += "\n--------------------\n";
      found = 1;
      text += rest;
    } else {
      if (rest = getmanpage("/doc/help/",str)) {
        if (found)
          text += "\n--------------------\n";
        found = 1;
        text += rest;
      }
      else if (rest = getmanpage("/doc/pcmd/",str)) {
        if (found)
          text += "\n--------------------\n";
        found = 1;
        text += rest;
      }
      else if (rest = getmanpage("/doc/REGELN/",str)) {
        if (found)
          text += "\n--------------------\n";
        found = 1;
        text += rest;
      }
    }

    if (!found)
      text = "Dazu ist keine Hilfe verfuegbar.\n";

    More(text,0);
    return 1;
  }
  if (IS_LEARNER(PL))
    text = read_file("/doc/hilfe.magier");
  else if (IS_SEER(PL))
    text = read_file("/doc/hilfe.seher");

  More(text + read_file("/doc/hilfe.spieler"), 0);
  return 1;
}

/** Ermittelt angebene Optionen fuer das Spielerkommando 'wer'.
  * \param[in] str vom Spieler spezifizierter String von Filteroptionen: -k,
  * -v, -a, -s (+ Langformen).
  * \return Array von Spieleroptionen als veroderte Int-Flags und Rest der
  * Spielereingabe ohne die Optionen.
  */
static mixed filter_who_options(string str)
{
  string* opt;
  int i,len,res;

  opt = explode(str," "); len=sizeof(opt);
  res = 0;
  for(i=0;i<len;i++)
    switch(opt[i]){
      case "-k":
      case "-kurz":
      res |= WHO_SHORT; break;
      case "-v":
      case "-vertikal":
      res |= WHO_VERTICAL; break;
      case "-alphabetisch":
      case "-a":
      case "-alpha":
      res |= WHO_ALPHA; break;
      case "-s":
      case "-spieler":
      res |= WHO_PLAYER_VIEW; break;
      default:
      return ({ res, implode(opt[i..]," ") });
    }
  return ({ res, 0 });

}

/** Spielerkommando 'wer', fragt /obj/werliste ab.
  * \param[in] str Spielereingabe mit Optionen fuer wer.
  * \return 1
  */
static int who(string str) {
  int i,shrt;
  string ret;
  mixed ans;

  if ((str=_unparsed_args())&&str[0..0]!="-") {
    ans = filter_who_options(str);
    shrt = ans[0];
    str = ans[1];
    if (!shrt) {
      if (ret=INETD->_send_udp(str,
                              ([ REQUEST: "who", SENDER: getuid(ME) ]), 1 ))
        write(ret);
      else
        write("Anfrage abgeschickt.\n");
      return 1;
    }
  }
  if (str) i=(member(str,'o')>0); else i=0;
  if (sizeof(str)>1 && str[0] == '-') str = str[1..1];
  More(implode( "/obj/werliste"->QueryWhoListe(
    IS_LEARNER(ME) && QueryProp(P_WANTS_TO_LEARN),shrt,0,str,i),"\n"),0);
  return 1;
}

/** Spielerkommando 'kwer', fragt /obj/werliste ab.
  * \param[in] str Spielereingabe mit Optionen fuer wer.
  * \return 1
  */
static int kwho(string str)
{
  int shrt;
  mixed res;

  if(str) {
    res = filter_who_options(str);
    shrt = res[0];
    str = res[1];
  }
  More(implode( "/obj/werliste"->QueryWhoListe(
      IS_LEARNER(ME) && QueryProp(P_WANTS_TO_LEARN), shrt|WHO_SHORT ,0,str),
    "\n")+"\n\n",0);
  return 1;
}

/** Spielerkommando 'kkwer', gibt eine einfache Liste der Anwesenden aus.
    Filtert unsichtbare Spieler aus, falls SPielerobjekt kein Magier ist.
  * \param[in] str Spielereingabe mit Optionen fuer wer.
  * \return 1
  */
static varargs int kkwho(string str) {
  object *obs;
  string *namen;

  obs=filter_users(str);
  namen=({});
  if (IS_LEARNER(this_player())) {
    foreach(object ob: obs) {
      if (environment(ob))
        namen+=({capitalize(geteuid(ob))});
    }
  } 
  else {
    foreach(object ob: obs) {
      if (!ob->QueryProp(P_INVIS) && environment(ob))
        namen+=({capitalize(geteuid(ob))});
    }
  }
  if (sizeof(namen))
    write(break_string(CountUp(sort_array(namen,#'>),", ", ", ")+".",75));
  else
    write("Keine passenden Spieler gefunden.\n");
  
  return 1;
}

/** Spielerkommando 'toete'.
  * Prueft auf Geist, Gast, toten HC-Spieler, Waffe/freie Hand. Versucht einen
  * Gegner zu finden und ruft dann Kill(string).
  * \param[in] str Spielereingabe
  * \return 1 oder 0, falls kein potentieller Gegner bei 'toete alle' gefunden
  * wird.
  */
static int kill(string str) {
  object eob;

  if (QueryProp(P_GHOST))
  {
    write("Das kannst Du in Deinem immateriellen Zustand nicht.\n");
    return 1;
  }
  
  if(hc_play>1)
  {
    write("DAS HAST DU HINTER DIR.\n");
    return 1;
  }
  
  if (QueryGuest())
  {
    write("Du bist doch nur Gast hier.\n");
    return 1;
  }
  if (!str || str == "") {
    write("WEN willst Du toeten?\n");
    return 1;
  }
  if( !QueryProp(P_WEAPON) && QueryProp(P_FREE_HANDS)==0 ) {
    write(
      "Dazu solltest Du eine Waffe gezueckt oder eine Hand frei haben.\n");
    return 1;
  }
  str=lower_case(str);
  if (str=="alle") {
    object* livs = filter(all_inventory(environment(PL)),
        function int (object ob) {
            if (living(ob) && !query_once_interactive(ob)
                  && !ob->QueryProp(P_INVIS)
                  && !ob->QueryProp(P_NO_GLOBAL_ATTACK)
                  && !ob->QueryProp(P_FRIEND))
            {
                Kill(ob);
                return 1;
            }
            return 0;
        } );
    // wenn Gegner gefunden, raus, ansonsten kommt die Fehlermeldung unten.
    if (sizeof(livs)) return 1;
  }
  else {
    int i=1;
    while(objectp(eob = present(str,i++,environment(PL)))) {
      if (living(eob) && !eob->QueryProp(P_INVIS))
        break;
      else
        eob=0;
    }
  }
  if (!objectp(eob)) {
    // per write und return 1 ist hier mal ok, weil dies Kommando im Spieler
    // eh zuletzt in der Kommandokette ausgefuehrt wird und per return 0 eh
    // kein anderes mehr zum Zug kommt.
    write("Du siehst hier kein derartiges Wesen!\n");
    return 1;
  }
  else if (eob == PL) {
    write("Selbstmord ist keine Loesung!\n");
    return 1;
  }

  /* Kill him */
  Kill(eob);
  return 1;
}

/** Spielerkommando 'stop'.
  * Loescht die Gegnerliste, sofern man nicht InFight() ist.
  * \param[in] str Spielereingabe, wird ignoriert.
  * \return 1
  */
static int stop( string str )
{
    if ( InFight() ){
        write( "Das geht nicht mitten im Kampf.\n" );
        return 1;
    }

    if ( !str ){
        StopHuntingMode();
        write( "Ok.\n" );
        return 1;
    }

    if ( !StopHuntID(str) )
        write( "So jemanden verfolgst Du nicht!\n" );

    return 1;
}

/** Spielerkommando fuers emoten ':'.
  * \param[in] str Spielereingabe
  * \param[in] genitiv Genetivflag
  * \return 1 oder 0, falls Spieler nicht emoten kann (kein CAN_EMOTE Flag in
  * P_CAN_FLAGS).
  */
int emote(string str,int genitiv)
{
  string *commands,message,verb;
  object living;
  int size;

  if (!(Query(P_CAN_FLAGS)&CAN_EMOTE)) return 0;
  if (query_verb()[0]==';') genitiv=1;
  if (query_verb()[0]==':'||query_verb()[0]==';')
    verb=query_verb()[1..]+" ";
  else
    verb="";
  str=this_player()->_unparsed_args();
  commands=explode(verb+(str||""),"#");
  message=break_string((IS_SEER(ME) ? "" : ">")
                       +capitalize(genitiv ? name(WESSEN) :
                                   name())
                       +" "+commands[0],78);
  size=sizeof(commands);
  if(size>=3)
  {
    living=find_living(lower_case(commands[1]));
    if(!living || environment(living)!=environment() ||
       (living->QueryProp(P_INVIS)) && !IS_LEARNER(ME))
    {
      write(capitalize(commands[1])+" sehe ich hier nicht!\n");
      return 1;
    }
    if(living!=this_object())
      tell_object(living,break_string((IS_SEER(this_player()) ? "" : ">")
                                    +capitalize(genitiv ?
                                                this_player()->name(WESSEN) :
                                                this_player()->name())
                                    +" "+commands[2],78));
  }
  if(size>=4)
    write(break_string(commands[3],78));
  else
    write(message);
  tell_room(environment(),message,({this_object(),living}));
  return 1;
}

/** Spielerkommando fuers remoten 'r:'.
  * \param[in] str Spielereingabe
  * \param[in] flag Genetivflag
  * \return 1 oder 0, falls Spieler nicht emoten kann (kein CAN_REMOTE Flag in
  * P_CAN_FLAGS).
  */
static int remote(string str, int flag)
{
  int m;
  string tmp, dest;
  string *exstr;
  object destpl;

  if ( !(Query(P_CAN_FLAGS) & CAN_REMOTE) )
      return 0;

  if ( !(str=_unparsed_args()) ||
       sizeof( (exstr=explode(str," ")) - ({""}) ) <= 1 ){
      write("Was willst Du zu wem `emoten`?\n");
      return 1;
  }

  dest = lower_case(exstr[0]);

  if( !(destpl=find_player( dest ) ) ||
      (destpl->QueryProp(P_INVIS) && !IS_LEARNER(ME)) ){
    write("Einen solchen Spieler gibt es derzeit nicht.\n");
    return 1;
  }

  tmp = implode( exstr[1..], " " );

  tmp = regreplace( tmp, "(^|[^#])#($|[^#])", "\\1aus der Ferne\\2", 1 );
  tmp = regreplace( tmp, "(^|[^\\^])\\^($|[^\\^])", "\\1in der Ferne\\2", 1 );
  tmp = regreplace( tmp, "##", "#", 1 );
  tmp = regreplace( tmp, "\\^\\^", "^", 1 );

  if ( strstr( tmp, "aus der Ferne" ) == -1
       && strstr( tmp, "in der Ferne" ) == -1 )
      tmp += " aus der Ferne";

  if ( QueryProp(P_INVIS) && IS_LEARNER(destpl) ){
      str = "(" + capitalize(getuid(ME));
      if ( flag )
          str += member( "sxz", str[<1] ) == -1 ? "s" : "'";
      str += ")";
  }
  else
      str = (flag ? capitalize(name(WESSEN)) : capitalize(name(WER)));

  str += " " + tmp + (member( ".?!", tmp[<1] ) == -1 ? "." : "") + "\n";

  m = destpl->ReceiveMsg(str, MT_COMM|MT_FAR, MA_EMOTE,0, ME);
  switch(m)
  {
    case MSG_DELIVERED:
      // Als origin muss der Empfaenger uebergeben werden, sonst kann die
      // Meldung in der tmhist nicht richtig zugeordnet werden.
      ReceiveMsg(capitalize(destpl->name()) + "->" + str, MT_COMM|MT_FAR,
                 MA_EMOTE, 0, destpl);
      break;
    case MSG_BUFFERED:
      write( capitalize(destpl->name(WER) + " ist gerade beschaeftigt.\n") );
      break;
    case MSG_IGNORED:
      write( capitalize(destpl->name(WER) + " ignoriert Dich.\n") );
      break;
    case MSG_VERB_IGN:
    case MSG_MUD_IGN:
      write( capitalize(destpl->name(WER) + " ignoriert Deine Meldung.\n") );
      break;
    default:
      write( capitalize(destpl->name(WER) + " kann Dich gerade nicht "
            "wahrnehmen.\n") );
  }
  return 1;
}

/** Spielerkommando fuers emoten im Genitiv ';'.
  * Ruft emote(string,int) auf.
  */
static int gemote(string str)
{
  return emote(str, 1);
}

/** Spielerkommando fuers remoten im Genitiv 'r;'.
  * Ruft remote(string, int) auf.
  */
static int gremote(string str)
{
  return remote(str, 1);
}

static void load_auto_objects(mapping map_ldfied);

private void InitPlayer();
private void InitPlayer2();
private void InitPlayer3();

/** Gibt eine Zufallszahl um P_AVERAGE_SIZE herum zurueck.
  * \return Zufaellige Groesse.
  */
private int RandomSize()
{
  return (100+random(13)-random(13)+random(13)-random(13))*
    (QueryProp(P_AVERAGE_SIZE)||170)/100;
}

/** Setzt bestimmte Props im Spieler, falls diese nicht gesetzt sind oder
  * loescht obsolete Props. Repariert bestimmte Datenstrukturen im Spieler.
  * Wird von start_player() nach Laden des Savefiles gerufen.
  * Momentan wird z.B. die Groesse gesetzt, falls sie bisher 0 ist und die
  * Skills des Spielers initialisiert bzw. repariert oder auf die aktuellste
  * Version des Skillsystems
  * Ruft ggf. InitSkills() und FixSkills().
  * @param[in] newflag Gibt an, ob es ein neuerstellter Spieler ist.
  * @sa start_player(), InitSkills(), FixSkills()
  */
private void updates_after_restore(int newflag) {
  // Seher duerfen die Fluchtrichtung uebermitteln lassen.
  // Eigentlich koennte es Merlin machen. Dummerweise gibt es ja auch alte
  // Seher und dann kann es gleiche fuer alle hier gemacht werden. (Ob der
  // Code jemals rauskann?)
  //TODO: Irgendwann alle Seher korrigieren und Code nach Merlin schieben...
  if (IS_SEER(ME))
    SetProp(P_CAN_FLAGS,QueryProp(P_CAN_FLAGS) | CAN_REPORT_WIMPY_DIR);

  // ggf. Invis-Eigenschaft aus dem Loginobjekt abrufen (Invislogin), koennte
  // ja anders als aus Savefile sein. Gesetztes P_INVIS aus diesem aber
  // beibehalten.
  if (IS_LEARNER(ME) && !QueryProp(P_INVIS)
//      && load_name(previous_object()) == "/secure/login"
      )
  {
    SetProp(P_INVIS, previous_object()->query_invis());
    if (QueryProp(P_INVIS))
      tell_object(ME, "DU BIST UNSICHTBAR!\n" );
  }
  "*"::updates_after_restore(newflag);

  attributes::UpdateAttributes();

  int size=Query(P_SIZE);
  if (!size) size=RandomSize();
  while(size==QueryProp(P_AVERAGE_SIZE))
    size=RandomSize();
  Set(P_SIZE,size);

  // Props werden nicht mehr genutzt. TODO: irgendwann entfernen, wenn
  // sinnvoll oder Savefiles extern bereinigt.
  Set(P_SECOND_LIST, SAVE, F_MODE_AD);
  Set(P_SECOND_LIST, 0, F_VALUE);
  Set("creation_date", SAVE|SECURED|PROTECTED, F_MODE_AD);
}


/** Setzt den HC-Modus.
  */
varargs nomask void set_hc_play(string str,int val)
{
   string str1;

    str1 = explode( object_name(previous_object()), "#" )[0];
    
    if ( str1 != "/secure/login" &&
         previous_object()!=this_object() &&
         extern_call() &&
         (!geteuid(ME) || geteuid(ME) != getuid(ME) ||
          capitalize(geteuid(ME)) != str ||
          geteuid(ME) != geteuid(previous_object())) ){
        write( "DIESER VERSUCH WAR ILLEGAL !!\n" );
        return;
    }
    
    hc_play=val;
}

/** gibt den HC-Modus zurueck.
  */
nomask int query_hc_play()
{
  return hc_play;
}

/** Initialisiert und aktiviert das Spielerobjekt.
  * Kann nur von /secure/login oder /secure/master gerufen werden.
  * Startet Telnet Negotiation, laedt Savefile, setzt einige Props. 
  * Ruft updates_after_restore(), um div. Daten zu aktualisieren/reparieren.
  * Ruft create() aus /std/player/potion.c.
  * Bei neuem Spieler wird der entsprechende Event ausgeloest die Attribute
  * auf die Startwerte gesetzt.
  * Prueft Zweitiemarkierungen.
  * Ruft InitPlayer().
  * @param[in] str Name des Charakters, der geladen werden soll.
  * @param[in] ip textuelle Repraesentation der IP-Adresse des Spielers.
  * @return 1 bei Erfolg, 0 sonst.
  * @todo Div. Reparaturen/Updates nach updates_after_restore() auslagern.
  */
varargs nomask int start_player( string str, string ip )
{
    mixed second;
    int newflag;  /* could player be restored? */
    string str1;

    call_out( "disconnect", 600 );

    str1 = explode( object_name(previous_object()), "#" )[0];

    if ( str1 != "/secure/login" &&
         str1 != "/secure/master" &&
         (!geteuid(ME) || geteuid(ME) != getuid(ME) ||
          capitalize(geteuid(ME)) != str ||
          geteuid(ME) != geteuid(previous_object())) ){
        write( "DIESER VERSUCH WAR ILLEGAL !!\n" );
        destruct(ME);
        return 0;
    }

    /* try to restore player. If it doesn't exist, set the new flag */
    newflag = !restore_object( SAVEPATH + lower_case(str)[0..0] + "/"
                               +lower_case(str) );

    updates_after_restore(newflag);

    if ( query_once_interactive(ME) )
    {
      // Erstmal - sofern vorhanden - den manuell konfigurierten Zeichensatz
      // einstellen. Im folgenden wird dann versucht, die TELOP CHARSET
      // auszuhandeln, die aendert das evtl. nochmal.
      set_manual_encoding();
      // Telnet-Negotiations durchfuehren, aber nur die grundlegenden aus
      // telnetneg. Alle anderen sollten erst spaeter, nach vollstaendiger
      // Initialisierung gemacht werden.
      telnetneg::startup_telnet_negs();
      modify_prompt();
      Set( P_LAST_LOGIN, time(), F_VALUE );
    }

    Set( P_WANTS_TO_LEARN, 1 ); // 1 sollte der default sein !!!
    Set( P_WANTS_TO_LEARN, PROTECTED, F_MODE_AS );
    // Eingefuegt 18.11.99, kann nach einem Jahr wieder raus:
    Set( P_TESTPLAYER, PROTECTED, F_MODE_AS );

    if ( IS_LEARNER(ME) )
        SetProp( P_CAN_FLAGS, QueryProp(P_CAN_FLAGS)|CAN_REMOTE );

    Set( P_NAME, str );
    Set( P_NAME, SECURED, F_MODE_AS );

    if ( !QueryProp(P_NEEDED_QP) )
        SetProp( P_NEEDED_QP, REQ_QP );

    Set( P_NEEDED_QP, NOSETMETHOD, F_SET_METHOD );
    Set( P_NEEDED_QP, SAVE|SECURED, F_MODE_AS );

    /* autosave the player after 500 heartbeats */
    time_to_save = age + 500;
    potion::create(); /* DO IT HERE AFTER THE RESTORE !! */

    AddId( getuid() );
    SetProp( P_AC, 0 );
    SetProp( P_WEAPON, 0 );

    /* Set some things which wont be set when all is OK */
    SetProp( P_MAX_HP, (QueryAttribute(A_CON) * 8 + 42 ));
    SetProp( P_MAX_SP, (QueryAttribute(A_INT) * 8 + 42 ));

    catch( bb = "/secure/bbmaster"->query_bb() );

    /* If this is a new character, we call the adventurers guild to get
     * our first title !
     */
    if ( newflag ) {
        if ( QueryGuest())
            SetProp( P_TITLE, "ueberkommt das "MUDNAME" ..." );

        Set( P_LEVEL, -1 );
        SetProp( P_ATTRIBUTES, ([ A_STR:1, A_CON:1, A_INT:1, A_DEX:1 ]) );
        SetProp( P_HP, QueryProp(P_MAX_HP) );

        // Event ausloesen
        EVENTD->TriggerEvent(EVT_LIB_PLAYER_CREATION, ([
              E_OBJECT: ME,
              E_PLNAME: getuid(ME) ]) );
    }
    
    InitPlayer();

    // Padreic 01.02.1999
    if ( !IS_LEARNER(ME) && (second = QueryProp(P_SECOND)) )
    {
        if ( stringp(second) && lower_case(second)[0..3] == "von " )
        {
            second = lower_case(second[4..]);
            SetProp( P_SECOND, second );
        }

        if ( !stringp(second ) ||
             !master()->find_userinfo(second))
        {
            // Wenns nur an Gross-/Kleinschreibung liegt, wird automatisch
            // korrigiert.
            if ( stringp(second) &&
                 master()->find_userinfo(lower_case(second)))
            {
                SetProp( P_SECOND, lower_case(second) );
                log_file( "WRONG_SECOND",
                          sprintf( "%s: %s: P_SECOND = %O -> Automatisch "
                                   "korrigiert,\n",
                                   dtime(time()), object_name(), second ) );
            }
            else {
                tell_object( ME,
                             "*\n*\n* Deine Zweitiemarkierung ist ungueltig, "
                             "bitte aendere diese und sprich im\n* Zweifel "
                             "bitte einen Erzmagier an.\n*\n*\n" );

                log_file( "WRONG_SECOND",
                          sprintf( "%s: %s: P_SECOND = %O\n",
                                   dtime(time()), object_name(), second ) );
                // ein bisschen deutlicher auffordern.. Padreic 08.04.1999
                move( "/d/gebirge/room/zwafflad", M_GO );
            }
        }
    }
    return(0);
}

/** Letzte Phase der Spielerinitialisierung beim Laden des Charakters.
  * Ruft enable_commands(), aktiviert den Heartbeat und aktiviert die
  * Kommandos aus den geerbten command.c, put_and_get.c, team.c, soul.c,
  * guide.c, setzt den Living Name.
  * Registriert UseSpell() als Catchall-Kommando.
  * Laesst den Spieler ggf. einen Level per /std/gilde aufsteigen, ruft
  * call_notify_player_change(), loest Login-Event aus.
  * Gibt Willkommenstexte, News und neue Mails aus.
  * Findet den Startraum und bewegt den Spieler dorthin.
  * Ruft FinalSetup() (aus den Rassenshells).
  * Begrenzt Geldmenge im Spieler (wegen Bankzweities) nach Tragkraft und
  * erstattet Geld bei Reboot.
  * Ruft set_is_wizard().
  * Startet Clonen der Autoloader.
  * @sa InitPlayer3(),InitPlayer2(), InitPlayer(), start_player().
  */
private void InitPlayer4()
{
    int num, str, ski;
    string err, called_from_ip;
    mixed start_place;
    object mon;

    enable_commands();
    set_heart_beat(1);
    command::initialize();
    add_put_and_get_commands();
    add_team_commands();
    add_soul_commands();
    add_guide_commands();
    add_action( "UseSpell", "", 1 );
    set_living_name( getuid() );
    while ( remove_call_out("disconnect") != -1 )
        ;

    if ( QueryProp(P_LEVEL) == -1 )
    {
        catch( "/std/gilde"->try_player_advance(this_object()) ;publish );
    }

    if ( interactive(ME) )
        call_notify_player_change(1);

    if ( interactive(this_object()) ) {
        cat( "/etc/NEWS" );

        NewbieIntroMsg();

        if ( QueryProp(P_INVIS) && !IS_WIZARD(ME) )
            SetProp( P_INVIS, 0 );

        catch( num = "secure/mailer"->FingerMail(getuid());publish );

        if ( num )
            write( "Du hast " + num + " neue" +
                   (num == 1 ? "n Brief" : " Briefe")+" im Postamt liegen.\n" );

        catch( RegisterChannels();publish );

        if ( (called_from_ip = Query(P_CALLED_FROM_IP)) &&
             query_ip_number(ME) != called_from_ip ){
            string tmp;

            if ( stringp(tmp = query_ip_name(called_from_ip)) &&
                 tmp != called_from_ip )
                tmp = " [" + tmp + "]";
            else
                tmp = "";

            write( "Das letzte Mal kamst Du von " + called_from_ip
                   + tmp + ".\n" );
        }

        Set( P_CALLED_FROM_IP, query_ip_number(ME) );
    }

    if ( !stringp(default_home) || default_home == "" )
        default_home = "/gilden/abenteurer";

    if ( IS_SEER(ME) && !IS_LEARNER(ME) )
        catch( start_place = HAUSVERWALTER->FindeHaus(getuid(ME));publish );
    // wenn der Spieler noch ganz frisch ist und noch wenig  Stufenpunkte
    // gekriegt hat und das Tutorial noch nicht gemacht hat, startet er im
    // Tutorial.
    else if (QueryProp(P_LEP) <= 120
             && QM->HasMiniQuest(this_object(),
               "/d/anfaenger/ennox/tutorial/npcs/verkaeufer") != 1)
        start_place = "/room/welcome/"+ getuid(this_object());
    else
        start_place = 0;

    if ( !start_place )
        start_place = QueryProp(P_START_HOME);

    if( objectp(start_place) || (stringp(start_place) && start_place != "" ) ){
        if ((err = catch(move( start_place, M_GO|M_SILENT|M_NO_SHOW );publish)) 
            || !environment() )
            err = catch(move( default_home, M_GO|M_SILENT|M_NO_SHOW );publish);
    }
    else
        err = catch(move( default_home, M_GO|M_SILENT|M_NO_SHOW );publish);

    if ( err )
        catch(move( "/gilden/abenteurer", M_GO|M_SILENT|M_NO_SHOW );publish);

    // Die Shell muss FinalSetup() nicht implementieren. Daher Callother
    catch( ME->FinalSetup();publish );

    // Login-event ausloesen
    EVENTD->TriggerEvent(EVT_LIB_LOGIN, ([
          E_OBJECT: ME,
          E_PLNAME: getuid(ME),
          E_ENVIRONMENT: environment() ]) );

    // erst jetzt GMCP freigeben und zu verhandeln.
    gmcp::startup_telnet_negs();

    // Schonmal sichern, falls ein Bug (Evalcost...) dazwischen kommen sollte.
    autoload_rest = autoload;

    // Um Geld-Xties mit illegal viel Geld zu vermeiden, kommt ein Check:
    if ( !IS_LEARNER(ME) && !Query(P_TESTPLAYER) &&
         mon = present( "\ngeld", ME ) ){
        // maximale Kraft, die der Spieler haette haben koennen
        str = QueryAttribute(A_STR) + 4;
        if ( Query(P_FROG) )
            str += 30;
        if ( str < 1 )
            str = QueryRealAttribute(A_STR) + 4;
        if ( str > 30 )
            str = 30;

        // Trageskill beachten
        ski = to_int(UseSkill( SK_CARRY, ([SI_SKILLARG : str ]) ));
        if ( !intp(ski) )
            ski = 0;

        // Wieviel konnte der Spieler insgesamt maximal tragen?
        num = 9200 + str * 800 + ski;
        if ( num < 3000 )
            num = 3000;

        // Verdoppeln fuer einen guten Container und nochmal 20% draufschlagen
        // zur Sicherheit. Das Ganze danach *4, um die maximale Anzahl Muenzen
        // zu erhalten.
        num = (int) (num * 8.8);

        // Geld, das zuviel ist, auf den Maximalwert kuerzen.
        // Zur Sicherheit wird mitgeloggt. Falls ein Spieler sich (zu recht)
        // beschwert, kann man immer noch wieder die Summe korrigieren ;-)
        if ( (str = mon->QueryProp(P_AMOUNT)) > num ){
            mon->SetProp( P_AMOUNT, num );
            log_file( "ZUVIEL_GELD", sprintf( "%s: %s hatte %d Muenzen bei "
                                              "sich. Korrigiert auf %d.\n ",
                                              ctime(time())[4..],
                                              capitalize(getuid(ME)),
                                              str, num ) );
        }
    }
    int entschaedigung = QueryProp(P_CARRIED_VALUE);
    if ( entschaedigung > 0 )
    {
        write( "Du findest " + entschaedigung +
               " Muenzen, die Du beim letzten Mal verloren hast.\n" );

        if ( MayAddWeight( entschaedigung / 4 ) ){
            write( "Weil Du nicht mehr soviel tragen kannst, spendest Du den "+
                   "Rest der Zentralbank.\n" );

            num = (QueryProp(P_MAX_WEIGHT) - query_weight_contents()) * 4;
            entschaedigung = (num < 0) ? 0 : num;
        }

        AddMoney( entschaedigung );
        SetProp(P_CARRIED_VALUE,0);
    }

    if ( !QueryProp(P_INVIS) )
        say( capitalize(name(WER)) + " betritt diese Welt.\n" );
    else
        write( "DU BIST UNSICHTBAR!\n\n" );
#if __EFUN_DEFINED__(set_is_wizard)
    if ( IS_WIZARD(getuid(ME)) )
        set_is_wizard( ME, 1 );
    else
        set_is_wizard( ME, 0 );
#endif
    if ( query_once_interactive(ME) )
        ListAwaited();

    // Autoloader werden ganz zum Schluss geclont, da das bis zum bitteren
    // (Evalcost-)Ende geschieht und danach u.U. keine Rechenzeit fuer
    // andere Aktionen mehr ueber ist
    load_auto_objects( autoload );
}

/** Setzt die Spielerinitialisierung nach start_player() fort.
  * Prueft den Wert der vom Spieler getragenen Sachen (fuer Entschaedigung
  * nach Reboot).
  * Setzt div. Properties des "Spielerkoerpers", die eigentlich gespeichert
  * werden, nach einem Reboot zurueck.
  * Fragt ggf. nach eMail-Adresse und uebergibt per input_to an
  * InitPlayer2().
  * @sa InitPlayer3(),InitPlayer2(), InitPlayer(), start_player().
*/
private void InitPlayer()
{
    string mailaddr;

    // wenn es einen Crash gab, sollen Spieler nicht noch extra bestraft werden
    if ( file_time( "/save/" + getuid()[0..0] + "/" + getuid() + ".o" )
         < last_reboot_time() ){
        SetProp( P_FOOD, 0 );
        SetProp( P_DRINK, 0 );
        SetProp( P_ALCOHOL, 0 );
        SetProp( P_BLIND, 0 );
        SetProp( P_DEAF, 0 );
        SetProp( P_POISON, 0 );
        SetProp( P_GHOST, 0 );
        SetProp( P_FROG, 0 );
        SetProp( P_HP, QueryProp(P_MAX_HP) );
        SetProp( P_SP, QueryProp(P_MAX_SP) );
    }

    if ( QueryGuest() )
        Set( P_MAILADDR, "none" );
    else if ( !(mailaddr = Query(P_MAILADDR)) || mailaddr == "" ) {
        write(break_string(
          "Eine gueltige EMail-Adresse erleichtert es erheblich, Dir "
          "ein neues Passwort setzen zu lassen, falls Du einmal Dein "
          "Passwort vergisst.",78)); 
        input_to( "getmailaddr",INPUT_PROMPT,
            "Gib bitte Deine EMail-Adresse an: " );
        return;
    }
    InitPlayer2();
}

/** liest eMail-Adresse vom Spieler ein und speichert sie.
  * Uebergibt anschliessend an InitPlayer2()
  * @param[in] maddr Spielereingabe der Emailadresse.
  * @sa InitPlayer2().
  */
static void getmailaddr( string maddr )
{
    maddr = check_email(maddr);

    if ( !stringp(maddr)) {
      write("Deine Eingabe scheint keine gueltige EMail-Adresse gewesen "
          "zu sein.\n");
      input_to( "getmailaddr", INPUT_PROMPT,
          "Gib bitte Deine EMail-Adresse an: " );
      return;
    }
    Set( P_MAILADDR, maddr );
    InitPlayer2();
}


/** Prueft Geschlecht des Spielers und fragt ggf. beim Spieler nach.
  * Uebergibt an InitPlayer3() oder get_gender().
  * @sa InitPlayer3(), get_gender().
  */
private void InitPlayer2()
{
    if( member(({ MALE, FEMALE }), QueryProp(P_GENDER) ) == -1 ) {
        input_to( "getgender", INPUT_PROMPT,
            "Bist Du maennlich oder weiblich: ");
        return;
    }

    InitPlayer3();
}

/** Liest Spielerantwort auf die Frage nach dem Geschlecht des Chars ein.
  * Wird gerufen von input_to().
  * Uebergibt an InitPlayer3().
  * @sa InitPlayer3().
  */
static void getgender( string gender_string )
{
    gender_string = lower_case( gender_string );

    if ( sizeof(gender_string)==1 && gender_string[0] == 'm' ){
        write( "Willkommen, mein Herr!\n" );
        SetProp( P_GENDER, MALE );
    }
    else if ( sizeof(gender_string)==1 && gender_string[0] == 'w' ){    
        write( "Willkommen, gnae' Frau!\n" );
        SetProp( P_GENDER, FEMALE );    
    }    
    else {
        write( "Wie? Was? Verstehe ich nicht!\n" );    
        input_to( "getgender", INPUT_PROMPT,
            "Bist Du maennlich oder weiblich? (tippe m oder w): ");    
        return;
    }

    InitPlayer3();
}


/** Prueft Terminaltyp des Spielers und fragt ggf. beim Spieler nach.
  * Uebergibt an InitPlayer4() oder gettty().
  * @sa InitPlayer4(), gettty().
  */
private void InitPlayer3()
{
    if ( !QueryProp(P_TTY) || QueryProp(P_TTY) == "none" )
    {
        write( "Waehle einen Terminaltyp (kann spaeter mit <stty> geaendert "
               "werden)\n");
        input_to( "gettty", INPUT_PROMPT, "vt100, ansi, dumb (Standard: dumb): " );
        return;
    }
    InitPlayer4();
}

/** Liest Spielerantwort auf die Frage nach dem Terminaltyp ein.
  * Wird gerufen von input_to().
  * Uebergibt an InitPlayer4().
  * @sa InitPlayer4().
  */
static void gettty( string ttystr )
{
    if ( !ttystr || ttystr == "" )
        ttystr = "dumb";

    ttystr = lower_case(ttystr);

    if ( ttystr == "vt100" ){
        write( "Dies sollte " + ANSI_BOLD + "fett" + ANSI_NORMAL + " sein.\n" );
        SetProp( P_TTY, ttystr );
    }
    else
        if ( ttystr == "ansi" ){
            write( "Dies sollte " + ANSI_RED + "rot" + ANSI_NORMAL +
                   " sein.\n" );
            SetProp( P_TTY, "ansi" );
        }
        else if ( ttystr == "dumb" ){
            write( "Ohje, oede! Besorg Dir ein besseres Terminal!\n" );
            SetProp( P_TTY, "dumb" );
        }
        else {
            write( "Dieser Terminaltyp wird nicht unterstuetzt. Nimm bitte "
                   "einen aus:\nvt100, ansi or dumb (Standard ist dumb).\n" );
            input_to( "gettty", INPUT_PROMPT,
                "vt100, ansi or dumb (Standard ist dumb): ");
            return;
        }

    InitPlayer4();
}


/** Liefert die UID des Charakters zurueck, also den Charakternamen.
  * @return UID des Objekts (Charaktername).
  */
nomask string query_real_name() {
  /* ACHTUNG !! DIES LFUN DARF NICHT ENTFERNT WERDEN !!! */
  /* Sie wird vom Gamedriver (zB bei F_ED) aufgerufen !! */
  /* Ich bin da zwar nicht so ueberzeugt von, dass der Driver die heutzutage
   * noch ruft, aber die halbe Mudlib ruft sie. ;-) (Zesstra, 27.4.08)
   */
  return getuid();
}

/*
 * the wizard command review: show player moving messages
 */
int review() {
  string *msg;
  write(short());
  write("Deine Bewegungen werden wie folgt gemeldet:\n"+
        "mout:  "+name(WER)+" "+(msg=explode(QueryProp(P_MSGOUT),"#"))[0]
       +" <Richtung>"+(sizeof(msg)>1 ? msg[1] : "")+".\n"+
        "min:   "+name(WER)+" "+QueryProp(P_MSGIN)+".\n"+
        "mmout: "+name(WER)+" "+QueryProp(P_MMSGOUT)+".\n"+
        "mmin:  "+name(WER)+" "+QueryProp(P_MMSGIN)+".\n"+
        (IS_LEARNER(ME) ?
         "cmsg:  "+name(WER)+" "+QueryProp(P_CLONE_MSG)+".\n"+
         "dmsg:  <Irgendetwas> "+QueryProp(P_DESTRUCT_MSG)+".\n"
         : "")+
        "Wenn Du jemanden angreifst, sieht das so aus:\n"+
        name(WER)+" greift Dich"+QueryProp(P_HANDS)[0]+" an.\n");
  return 1;
}

/*
 * set player moving messages
 */

static int setmin(string str)
{
  SetProp(P_MSGIN, _unparsed_args()||"kommt an");
  write("Ok.\n");
  return 1;
}

static int setmout(string str)
{
  string *msg;

  if(sizeof(msg=explode((_unparsed_args()||"geht"),"#"))>2)
  {
    write("Du darfst nur einmal '#' fuer die Richtung angeben.\n");
    return 1;
  }
  if(sizeof(msg)>1)
  {
    if (msg[0]!="" && msg[0][<1]==' ') msg[0]=msg[0][0..<2];
    SetProp(P_MSGOUT, msg[0]+"#"+msg[1]);
  }
  else
    SetProp(P_MSGOUT, _unparsed_args()||"geht");
  write("Ok.\n");
  return 1;
}

static int setmmin(string str)
{
  SetProp(P_MMSGIN, _unparsed_args()||"erscheint");
  write("Ok.\n");
  return 1;
}

static int setmmout(string str)
{
  SetProp(P_MMSGOUT, _unparsed_args()||"verschwindet");
  write("Ok.\n");
  return 1;
}

static int setcmsg(string str)
{
  SetProp(P_CLONE_MSG, _unparsed_args()||"zaubert etwas aus "
          + QueryPossPronoun(MALE,WEM) + " Aermel hervor");
  write("Ok.\n");
  return 1;
}

static int setdmsg(string str)
{
  SetProp(P_DESTRUCT_MSG, _unparsed_args()||"wird von " + name(WER,1)
          + " zerstaeubt");
  write("Ok.\n");
  return 1;
}

static int set_title(string str)
{
  string bonus;
  
  if(!IS_SEER(this_object()) && !FAO_HAS_TITLE_GIFT(this_object()) )
  {
    return 0;
  }
  
  bonus = "";
  if (!(str=_unparsed_args()))
    str = "";
  else if( str[0..2]=="\\b," || str[0..2]=="\\b'" )
  {
    bonus = "\b"; // ein backspace fuer ein (hoch)komma ist ok! :-)
    str = str[2..];
  }
  if(str=="0")  // damit der Gildentitel zum Vorschein kommen kann
    SetProp(P_TITLE, 0);
  else
    SetProp(P_TITLE, bonus+str);
  write("Ok.\n");
  return 1;
}

static int extra_input(string str, string look)
{
  if (str=="**")
  {
    if (look=="")
      SetProp(P_EXTRA_LOOK,0);
    else
      SetProp(P_EXTRA_LOOK,look);
    write("Ok.\n");
    return 1;
  }
  input_to("extra_input",INPUT_PROMPT, "]" ,look+str+"\n");
  return 1;
}

static int extralook(mixed str)
{
  if( str=="?" )
  {
    write( "Dein Extralook ist : "+QueryProp(P_EXTRA_LOOK) + "\n");
    return 1;
  }
  write("Bitte gib Deinen Extra-Look ein. Beenden mit **:\n");
  input_to("extra_input",INPUT_PROMPT,"]","");
  return 1;
}

static void calculate_value()
{
  int carried_value, value;

  carried_value=0;
  foreach(object ob: deep_inventory(ME)) {
    if (!ob->QueryProp(P_AUTOLOADOBJ))
      carried_value+=((value=({int})ob->QueryProp(P_VALUE)) > 1000 ? 1000 : value);
  }
  SetProp(P_CARRIED_VALUE, carried_value);
}

void save_me(mixed value_items)
{
  // Gaeste werden nicht gespeichert
  if( getuid()[0..3]=="gast" )
    return;
 
  // Autoloader identifizieren und speichern
  autoload=([]);
  foreach(object ob: deep_inventory(ME)) {
    mixed val = ob->QueryProp( P_AUTOLOADOBJ );
    if (val && clonep(ob))
    {
      string obname=load_name(ob);
      if (obname == GELD)
        autoload[obname] += val;
      else
        autoload += ([obname:val]);
    }
  }
  // An noch nicht geclonte Autoloader denken!
  autoload += autoload_rest;
  
  // Im Bedarfsfall Wert des Inventory bestimmen
  if (value_items)
    calculate_value();
  else
    SetProp(P_CARRIED_VALUE, 0);

  // Logout-Zeit speichern
  if(query_once_interactive(ME) && !QueryProp(P_INVIS))
    Set(P_LAST_LOGOUT,time());

  // Funktion zur Bestimmung des Gildenrating aufrufen
  string gilde=GUILD_DIR+QueryProp(P_GUILD);
  if (find_object(gilde) || file_size(gilde+".c")>-1)
    catch(call_other(gilde,"GuildRating",this_object());publish);

  // Speichern des Spielers
  save_object(SAVEPATH+getuid()[0..0]+"/" + getuid());
}

static varargs void log_autoload( string file, string reason, mixed data, string error )
{
  if (member(autoload_error,file)!=-1) return;
  log_file(SHELLLOG("NO_AUTO_FILE"),sprintf("%s: %s: %s\nreason: cannot %s file\ndata: %O\n%s\n",
           ctime()[4..15],capitalize(getuid()),file,reason,data,
           (error?"Fehlermeldung: "+error+"\n":"")));
  autoload_error+=({file});
}

// tics, die fuer autoloader reichen sollten:
#define SAFE_FOR_AUTOLOADER __MAX_EVAL_COST__/4

private void load_auto_object( string file, mixed data )
{
    object ob;
    string error;
   
    if( get_eval_cost() < SAFE_FOR_AUTOLOADER ) return;
    m_delete( autoload_rest, file );
    autoload_error-=({file});

    if ( file == "/obj/money" )
      file = "/items/money";
    if ( file == "/obj/seercard" )
      file = "/items/seercard";
    
    ob = find_object(file);
    
    if (!ob)
    {
        if (file_size(file+".c")<0&&
           file_size(implode(explode(file,"/")[0..<2],"/")+
                     "/virtual_compiler.c")<0)
        {
           log_autoload(file,"find",data,0);
           return;
        }
        if (error = catch(load_object(file); publish))
        {
           log_autoload(file,"load",data,error);
           return;
        }
    }
    if ( error = catch(ob = clone_object(file); publish) )
    {
        log_autoload(file,"clone",data, error);
        return;
    }

    if ( error = catch(ob->SetProp( P_AUTOLOADOBJ, data ); publish) )
    {
        log_autoload(file,"SetProp",data, error);
        ob->remove(1);
        if (ob) destruct(ob);
        return;
    }

    if ( error = catch(ob->move( ME, M_NOCHECK );publish) ) {
        log_autoload(file,"move",data, error);
        ob->remove(1);
        if(ob) destruct(ob);
        return;
    }
}

static void load_auto_objects( mapping map_ldfied )
{
    if ( (!mappingp(map_ldfied) || !sizeof(map_ldfied)) && !sizeof(autoload_rest) )
        return;

    // Mit Netztoten Spielern rechnen manche Autoloader nicht. Also
    // das Clonen unterbrechen und in Reconnect() wieder anwerfen.
    if ( !interactive() )
        return;

    // Kleiner Hack: autoload_rest ist eine globale Variable, die beim
    // Clonen der einzelnen Autoloader direkt veraendert wird.
    // So lange das Mapping noch Eintraege hat, muessen wir noch fehlende
    // Autoloader clonen.
    if ( !sizeof(autoload_rest) )
        autoload_rest = map_ldfied;

    // Schon hier einen call_out() zum "Nach"clonen von noch nicht geclonten
    // Autoloadern starten, da spaeter u.U. keine Rechenzeit mehr dafuer da ist.
    while ( remove_call_out("load_auto_objects") != -1 )
        /* do nothing */;
    
    // Mit Parameter '0' aufrufen, da das globale Mapping benutzt wird.
    // Verzoegerung 0 in rekursiven Callouts ist bloed, also 1s Delay
    call_out( "load_auto_objects", 2, 0 );
    
    // Mit catch() gegen die Evalcost-Falle!
    // Mit Absicht das walk_mapping() aus der "alten" Version erhalten und
    // nicht durch eine (einfachere) Schleife inkl. get_eval_cost() ersetzt,
    // da eine Schleife gegenueber der walk_mapping()-Loesung den Aufbau
    // der previous_object()-Kette veraendern wuerde; darauf testen aber
    // manche Objekte.
    catch( walk_mapping( autoload_rest, #'load_auto_object/*'*/ ) );
}

/*
 * al_to_title: Make the numeric alignment value into a string
 */
static string al_to_title(int a)
{
  if (a >= KILL_NEUTRAL_ALIGNMENT * 100)
    return "heilig";
  if (a > KILL_NEUTRAL_ALIGNMENT * 20)
    return "gut";
  if (a > KILL_NEUTRAL_ALIGNMENT * 4)
    return "nett";
  if (a > - KILL_NEUTRAL_ALIGNMENT * 4)
    return "neutral";
  if (a > - KILL_NEUTRAL_ALIGNMENT * 20)
    return "frech";
  if (a > - KILL_NEUTRAL_ALIGNMENT * 100)
    return "boese";
  return "satanisch";
}

static int toggle_whimpy_dir(string str) {
  SetProp(P_WIMPY_DIRECTION,str=_unparsed_args()||str);
  if (str)
    printf("Ok, Fluchtrichtung %O.\n",str);
  else
    printf("Ok, bevorzugte Fluchtrichtung deaktiviert.\n");
  return 1;
}

static int toggle_whimpy(string str)
{
  int i;

  if(!str || str=="" || (sscanf(str,"%d",i)<0))
  {
    write("vorsicht <hp>, 0<=hp<"+QueryProp(P_MAX_HP)+"\n");
    return 1;
  }
  if(i>=QueryProp(P_MAX_HP) || i<0)
  {
    write("Der Wert ist nicht erlaubt.\n");
    return 1;
  }
  if(!i) write("Prinz Eisenherz-Modus.\n");
  else write("Vorsicht-Modus ("+i+")\n");
  SetProp(P_WIMPY,i);
  return 1;
}

/** Bestimmt, ob das Spielerobjekt beschattet werden darf.
  * Beschatten ist nur fuer Objekte erlaubt, die in /std/player/shadows
  * abgelegt sind.
  * Ausnahme: Testspieler mit gesetztem P_ALLOWED_SHADOW
  * @param[in] obj Objekt, was beschatten moechte.
  * @return 0, wenn Beschatten erlaubt, 1 sonst.
  */
varargs nomask int query_prevent_shadow(object obj)
{
  string what, allowed_shadow;

//  if ( Query(P_TESTPLAYER) )
//      return 0;
  
  if (obj){
    what=object_name(obj);
    if (what && what != "" &&
        sscanf(what,"/std/player/shadows/%s#%~d",what)==2)
      return 0;
    
    // Einem Testspieler kann man P_ALLOWED_SHADOW auf einen zu testenden
    // Shadow setzen.
    if ( Query(P_TESTPLAYER) && 
         stringp(what) && 
         stringp(allowed_shadow=Query(P_ALLOWED_SHADOW)) &&
         strstr(what, allowed_shadow)==0)
            return 0;
  }
  return 1;
}

static int uhrzeit()
{
  write(dtime(time()+QueryProp(P_TIMEZONE)*3600)+".\n");
  return 1;
}

protected string SetDefaultHome(string str)
{
  return default_home=str;
}

string QueryDefaultHome()
{
  return default_home;
}

protected string SetDefaultPrayRoom(string str)
{
  if(hc_play>1)
  {
    default_pray_room="/room/nirvana";
  }
  else
    default_pray_room=str;
  
  return default_pray_room;
}

string QueryPrayRoom()
{
  if(hc_play>1)
  {
    return "/room/nirvana";
  } 
  string room = QueryProp(P_PRAY_ROOM);
  if (stringp(room))
    return room;
  else if (default_pray_room)
    return default_pray_room;
  // hoffentlich ist das wenigstens gesetzt. 
  return default_home;
}

void _restart_beat()
{
  tell_object(ME,
      "Der GameDriver teilt Dir mit: Dein Herzschlag hat wieder eingesetzt.\n");
  set_heart_beat(1);
}

static int weg(string str)
{
  if (!(str=_unparsed_args()))
  {
    printf("Du bist nicht%s als abwesend gekennzeichnet.\n",
           QueryProp(P_AWAY) ? " mehr" : "");
    SetProp(P_AWAY, 0);
    return 1;
  }
  write("Du bist jetzt als abwesend gekennzeichnet.\n");
  SetProp(P_AWAY, str);
  return 1;
}

/* Ein Befehl zum anschauen der Wegmeldung anderer Spieler */
static int wegmeldung(string player)
{

  object player_ob;
  string weg;

  if ( !player || player=="" || 
       player==lowerstring(this_player()->QueryProp(P_NAME)))
  {
    weg=this_player()->QueryProp(P_AWAY);
    write ("Du bist "+(weg?"":"nicht ")+"als abwesend gekennzeichnet.\n");
    if (weg)
      write(break_string(weg, 78,"Grund: ",BS_INDENT_ONCE));
    return 1;  
  }

  // Welcher Spieler ist gemeint?
  player_ob=find_player(player);

  // Spieler nicht da oder Invis und Anfrager is kein Magier
  if (!player_ob || 
      (player_ob->QueryProp(P_INVIS) && !IS_LEARNER(this_player()))) 
  {
    write(capitalize(player)+" ist gerade nicht im Spiel.\n");
    return 1;
  }

  weg=player_ob->QueryProp(P_AWAY);

        // player_ob->Name() gibt bei invis-Magiern "Jemand" zurueck
  write (player_ob->QueryProp(P_NAME)+" ist "+
         (weg?"":"nicht ")+"als abwesend gekennzeichnet.\n");

  if (weg)
    write(break_string(weg, 78,"Grund: ",BS_INDENT_ONCE));

  return 1;
}

static string timediff(int time)
{
  string ret;

  ret="";
  if(time>=86400) {
    ret+=time/86400+"d ";
    time%=86400;
  }
  if(time<36000) ret+="0";
  ret+=time/3600+":";
  time%=3600;
  if(time<600) ret+="0";
  ret+=time/60+":";
  time%=60;
  if(time<10) ret+="0";
  ret+=time+"";
  return ret;
}


/* Ein Befehl zum anschauen der Idlezeit anderer Spieler */
static int idlezeit(string player)
{

  object player_ob;
  int idle;

  if ( !player || player=="" || 
       player==lowerstring(this_player()->QueryProp(P_NAME)))
  {
    write ("Du bist selber natuerlich gerade nicht idle.\n");
    return 1;  
  }

  // Welcher Spieler ist gemeint?
  player_ob=find_player(player);

  // Spieler nicht da oder Invis und Anfrager is kein Magier
  if (!player_ob || 
      (player_ob->QueryProp(P_INVIS) && !IS_LEARNER(this_player()))) 
  {
    write(capitalize(player)+" ist gerade nicht im Spiel.\n");
    return 1;
  }

  idle=query_idle(player_ob);

        // player_ob->Name() gibt bei invis-Magiern "Jemand" zurueck
  write (player_ob->QueryProp(P_NAME)+" ist "+
         (idle>=60?timediff(idle):"nicht")+" passiv.\n");

  return 1;
}


/** Belebt einen netztoten Spieler wieder.
  * Reaktiviert Heartbeats, bewegt den Spieler zurueck an den Ort, der eim
  * Einschlafen zum Aufwachen ermittelt wurde (im einfachsten Fall der Raum,
  * wo man eingeschlafen ist).
  */
static void ndead_revive()
{
    string fname;
    int ret;

    set_heart_beat(1);
    ndead_next_check = NETDEAD_CHECK_TIME;
    ndead_currently = 0;
    ndead_lasttime = 0;

    if ( !objectp(ndead_location) && 
         stringp(ndead_l_filename) && sizeof(ndead_l_filename)) {
        
        if ( member( ndead_l_filename, '#' ) == -1 ){
            catch(load_object( ndead_l_filename); publish);
            ndead_location = find_object(ndead_l_filename);
        }
        else {
            ndead_location = find_object(ndead_l_filename);
            if ( !ndead_location && env_ndead_info ){
                fname = explode( ndead_l_filename, "#" )[0];
                catch(ndead_location = 
                    (load_object(fname)->SetProp(P_NETDEAD_INFO, env_ndead_info));
                    publish);
                if ( !objectp(ndead_location) ){
                    catch(load_object( ndead_location);publish);
                    ndead_location = find_object(ndead_location);
                }
            }
        }
    }

    if ( !objectp(ndead_location)
        || catch(ret = move( ndead_location, M_GO|M_SILENT );publish) 
        || ret != 1 ) {
        move( "gilden/abenteurer", M_GO|M_SILENT );
        ndead_location = environment();
    }

    //  ndead_location=0;
    ndead_l_filename = 0;
    env_ndead_info = 0;
}

/** Bewegt einen netztoten Spieler in den Netztotenraum
  * Gerufen von heartbeat().
  * Zerstoert Gaeste, verlaesst ggf. das Team, ermittelt, ob der Spieler beim
  * Aufwachen in das alte Environment bewegt werden soll oder in einen anderen
  * Raum, hierzu wird im Environment P_NETDEAD_INFO abgefragt.
  * Deaktiviert die Kommandos per disable_commands().
  */
static void ndead_move_me() {
  object team;

  set_heart_beat(0);
  stop_heart_beats();
  if (QueryGuest()) {
    quit();
    if (ME)
      remove();
    if (ME)
      destruct(ME);
    return;
  }
  ndead_next_check=NETDEAD_CHECK_TIME;
  ndead_currently=1;
  ndead_lasttime=0;
  ndead_location=environment();
  if (objectp(ndead_location))
    ndead_l_filename=object_name(ndead_location);
  
  if (objectp(environment())
      && sizeof(explode(object_name(environment()),"#")) > 1)
    env_ndead_info=environment()->QueryProp(P_NETDEAD_INFO);
  else
    env_ndead_info=0;
  
  if ( objectp(team=Query(P_TEAM)) )
      // Der Test auf assoziierte Teammitglieder (== FolgeNPCs)
      // verhindert, dass Spieler nach "schlafe ein" aus dem
      // Team ausgetragen werden. -- 29.01.2002 Tiamak
      //      && !objectp(amem=Query(P_TEAM_ASSOC_MEMBERS))
      //      && !(pointerp(amem) && sizeof(amem)))
    team->RemoveMember(ME);
  
  disable_commands();
  move(NETDEAD_ROOM,M_GO|M_NO_ATTACK|M_NOCHECK,"ins Reich der Netztoten");
}

/** Ist dieser Character ein Gast?
  * @return 1, wenn Gast, 0 sonst.
  */
int QueryGuest()
{
  return sscanf(getuid(),"gast%~d");
}

/** Spielerkommando 'schlafe ein'.
  * Ruft remove_interactive() bei Spielern, bei Magiern wird quit() gerufen,
  * um das Magierobjekt zu zerstoeren.
  * @sa quit()
  */
int disconnect(string str)
{ 
  string verb;
  string desc = break_string(
      "\"schlafe ein\" beendet Deine Verbindung mit "MUDNAME". Du behaeltst "
     "Deine Sachen.\nFalls "MUDNAME" jedoch abstuerzt oder neu gestartet "
     "wird, waehrend Du weg bist, verlierst Du die meisten allerdings "
     "(genauso, als wenn Du Deine Verbindung mit \"ende\" beendet haettest). "
     "In diesem Fall bekommst Du dann eine kleine Entschaedigung."
     ,78,0,BS_LEAVE_MY_LFS);

  verb=query_verb();
  if (!verb)
    verb="AUTOCALL";
  if (verb[0..5]=="schlaf" && str!="ein")
  {
    notify_fail(desc);
    return 0;
  }
  if (IS_LEARNER(this_object()))
    return quit();
  
  tell_object(this_object(), desc);

  if (clonep(environment()) && !environment()->QueryProp(P_NETDEAD_INFO))
    tell_object(this_object(),break_string(
        "\nACHTUNG: Wenn Du hier laengere Zeit schlaefst, "
        "kommst Du vermutlich nicht an diesen Ort zurueck!",78));

  say(capitalize(name(WER))+" hat gerade die Verbindung zu "MUDNAME" gekappt.\n");
  remove_interactive(ME);
  call_out(#'clear_tell_history,4);
  return 1;
}

static int finger (string str)
{
  string ret;
  mixed xname;

  if (!str || str==""
      || sizeof(explode(str," ")-({"-n","-p","-s","-v","-a"}))>1)
  {
    write("finger <spielername> oder finger <spielername@mudname>\n"+
      "Bitte nur den reinen Spielernamen verwenden, keine Namensvorsaetze oder Titel\n");
    return 1;
  }
  xname=explode(str,"@");
  if(sizeof(xname)==2)
  {
    if (xname[0]=="-n " || xname[0]=="-p " || xname[0]=="-s ")  {
      write("finger <spielername>@<mudname> - der Spielername fehlt.\n");
      return 1;
    }
    if (ret=INETD->_send_udp(xname[1],([
                   REQUEST: "finger",
                   SENDER: getuid(ME),
                   DATA: (explode(xname[0]," ")-({"-n","-p","-s"}))[0]
                   ]), 1))
            write(ret);
        else
      write("Anfrage abgeschickt.\n");
    return 1;
  }
  "/p/daemon/finger"->finger_single(str,1);
  return 1;
}

string lalign(string str, int wid)
{
  return (str+"                                                   "+
      "                                                   ")[0..wid-1];
}

#define MUDS_BAR "\
-------------------------------------------------------------------------------"

private void format(mixed mud, mixed hosts, string output)
{
  output += lalign(hosts[mud][HOST_NAME], 20) + "  " +
        (hosts[mud][HOST_STATUS] ?
           hosts[mud][HOST_STATUS] > 0 ?
             "UP       " + ctime(hosts[mud][HOST_STATUS])[4..15] :
             "DOWN     " + ctime(-hosts[mud][HOST_STATUS])[4..15]
         : "UNKNOWN  Never accessed.") + "\n";
}

static int muds() {
  mapping hosts;
  mixed muds, output;

  output = lalign("Mudname", 20) + "  Status   Last access";
  output += "\n" + MUDS_BAR[0..sizeof(output)] + "\n";
  muds = sort_array(m_indices(hosts = INETD->query("hosts")),#'>);
  map(muds, #'format, hosts, &output);
  More(output);
  return 1;
}

// **** local property methods
static int _set_level(int i)
{
  if (!intp(i)) return -1;
  if (i<1) return -1;
  Set(P_LEVEL, i);
  GMCP_Char( ([P_LEVEL: i]) );
  return i;
}

static int _set_invis(int a)
{
  return Set(P_INVIS, intp(a) ? a : !Query(P_INVIS));
}

/* sets the terminal type */
/* note: support vt100 (b/w), ansi (color), dumb (none) */
static string _set_tty(string str) {
  if(str != "dumb" && str != "vt100" && str != "ansi")
    return Query(P_TTY);
  return Set(P_TTY, str);
}

static int stty(string str)
{
  if(str!="dumb"&&str!="vt100"&&str!="ansi"&&str!="reset")
  {
    write("Kommando: stty dumb|vt100|ansi oder reset\n");
  }
  if(str == "reset") {
      printf("[30;47m[0mDieser Text sollte lesbar sein!\n");
      return 1;
  }

  write("TTY steht jetzt auf "+SetProp(P_TTY,str)+".\n");
  if(str == "ansi" || str == "vt100") {
      printf("Terminal Test:\n");
      printf("VT100: [1mfett[0m [4munterstrichen[0m "+
             "[5mblinkend[0m [7minvers[0m\n");
      if(str == "ansi") {
          printf("ANSI Farben und VT100 Attribute:\n");
          foreach(int fg: 30 .. 37) {
              foreach(int bg: 40 .. 47) {
                  printf("[%d;%dm[1m@[0m", fg, bg);
                  printf("[%d;%dm[4m@[0m", fg, bg);
                  printf("[%d;%dm[5m@[0m", fg, bg);
                  printf("[%d;%dm[7m@[0m", fg, bg);
              }
              printf("\n");
          }
          printf("Sollte dieser Text hier nicht richtig lesbar\nsein, "+
                 "benutze das Kommando 'stty reset'.\n");
      }

  }
  return 1;
}

int set_ascii_art(string str)
{
  if (str!="ein"&&str!="aus")
  {
     printf("Du moechtest 'Grafik' "+(QueryProp(P_NO_ASCII_ART)?"NICHT ":"")+
                     "sehen.\n");
  }

  if (str=="ein") {
    SetProp(P_NO_ASCII_ART, 0);
    printf("Zukuenftig moechtest Du 'Grafik' sehen.\n");
  }

  if (str=="aus") {
    SetProp(P_NO_ASCII_ART, 1);
    printf("Zukuenftig moechtest Du KEINE 'Grafik' mehr sehen.\n");
  }


  return 1;
}

int _set_shell_version(int arg)
{
  if (!intp(arg))
    return -1;
  Set(P_SHELL_VERSION,({QueryProp(P_RACE),arg}));
  return 1;
}

int _query_shell_version()
{   mixed sv;

    if (!(sv=Query(P_SHELL_VERSION)) || !pointerp(sv) || sizeof(sv)!=2 ||
        sv[0]!=QueryProp(P_RACE) || !intp(sv[1]))
      return 0;
    return sv[1];
}

// XxXxXxXxXx

int more(string str)
{
  if(!str)
  {
    notify_fail("Usage: more <file>\n");
    return 0;
  }
  if (file_size(str) <= 0) {
    notify_fail(str+": No such file\n");
    return 0;
  }
  More(str, 1);
  return 1;
}

static int set_visualbell(string str)
{
  if(!str)
  {
    write("Derzeitige Einstellung fuer Tonausgabe: "+
         (QueryProp(P_VISUALBELL)?"AUS":"EIN")+".\n");
    return 1;
  }
  if (str=="ein")
  {
    if(!QueryProp(P_VISUALBELL))
      write("Die Tonausgabe stand schon auf EIN.\n");
    else
      {
  SetProp(P_VISUALBELL,0);
        write("OK, Tonausgabe auf EIN gestellt.\n");
      }
  }
  else
    if (str=="aus")
    {
      if(QueryProp(P_VISUALBELL))
        write("Die Tonausgabe stand schon auf AUS.\n");
      else
        {
          SetProp(P_VISUALBELL,1);
    write("OK, Tonausgabe auf AUS gestellt.\n");
  }
    }
    else
      write("Syntax: ton [ein|aus]\n");
  return 1;
}

static int set_screensize(string str)
{
  int size;

  if (str && (str[0..2] == "abs" || str[0..2]=="rel")) {
    size = QueryProp(P_MORE_FLAGS);
    if (str[0..2] == "abs") {
      size |= E_ABS;
      write("Es wird beim Prompt die Zeilenzahl des Textes angegeben.\n");
    }
    else {
      size &= ~E_ABS;
      write("Es wird beim Prompt der prozentuale Anteil des Textes angegeben.\n");
    }
    SetProp(P_MORE_FLAGS, size);
    return 1;
  }

  if ( str && (str=="auto" || sscanf( str, "auto %d", size )) ){
      if ( size > 0 ){
          write("Ungueltiger Wert! "
                "In Verbindung mit 'auto' sind nur negative Werte erlaubt.\n");
          return 1;
      }

      SetProp( P_SCREENSIZE, size-1 );

      write("Ok, Deine Zeilenzahl wird nun automatisch ermittelt (derzeit "+
            QueryProp(P_SCREENSIZE)+").\n"+
            break_string("Bitte beachte, dass dies nur einwandfrei "
                         "funktioniert, wenn Dein Client Telnetnegotiations "
                         "unterstuetzt (siehe auch \"hilfe telnegs\").") );
    return 1;
  }

  if ( !str || str=="" || !sscanf( str, "%d", size ) || size < 0 || size > 100){
      write(break_string(
            sprintf("Mit dem Befehl 'zeilen <groesse>' kannst Du einstellen, "
            "wieviele Zeilen bei mehrseitigen Texten auf einmal ausgegeben "
            "werden. Die angegebene Groesse muss zwischen 0 und 100 liegen. "
            "Bei Groesse 0 wird einfach alles ausgegeben (ohne Pause). Mit "
            "der Einstellung 'auto' wird die Groesse automatisch ueber "
            "die Telnetnegotiations ermittelt (siehe auch 'hilfe telnegs'). "
            "Um nach einer Seite Text noch etwas Platz zu haben, kann man z.B. "
            "'zeilen auto -3' einstellen.\n"
            "Die Voreinstellung ist 20 Zeilen.\n"
            "Mit 'zeilen abs[olut]' und 'zeilen rel[ativ]' kannst Du fest"
            "legen, ob im Prompt bei langen Texten die aktuelle Zeilennummer "
            "oder eine prozentuale Angabe ausgegeben wird.\n"
            "Deine aktuelle Einstellung ist %d%s Zeilen (%s).",
            QueryProp(P_SCREENSIZE),
            Query(P_SCREENSIZE) < 0 ? " 'automatische'" : "",
            QueryProp(P_MORE_FLAGS) & E_ABS ? "absolut" : "relativ"),78,0,1));
    return 1;
  }

  SetProp( P_SCREENSIZE, size );

  printf( "Okay, Deine Zeilenzahl steht nun auf %d.\n", size );
  return 1;
}

static int _query_screensize()
{
    int sz,rows;

    if ( (sz=Query(P_SCREENSIZE)) >= 0 )
        return sz;

    if ( !rows=QueryProp(P_TTY_ROWS) )
        return 0;

    return (rows+=sz) >= 5 ? rows : 5;
}

static int presay(string str)
{
  if (!str=_unparsed_args())
    write("Dein Presay ist jetzt geloescht.\n");
  else
    printf("Dein Presay lautet jetzt: \"%s\".\n",str=capitalize(str));
  SetProp(P_PRESAY,str);
  return 1;
}

static int sethands(string str)
{
  mixed *hands;

  if (!(str=_unparsed_args()))
  {
    write("sethands <message>\n");
    return 1;
  }
  if (str=="0")
      hands=RaceDefault(P_HANDS);
  if (!hands || !pointerp(hands))
      hands=Query(P_HANDS);
  hands[0]=" "+str;
  Set(P_HANDS,hands);
  write("Ok.\n");
  return 1;
}

static int inform(string str)
{
  switch (str) {
    case "on":
    case "ein":
    case "an":
      if (Query(P_INFORMME))
    write("Das hattest Du schon so eingestellt.\n");
      else
      {
    write("Kuenftig wirst Du informiert, wenn jemand das "MUDNAME" verlaesst/betritt.\n");
    Set(P_INFORMME,1);
      }
      return 1;
    case "aus":
    case "off":
      if (!Query(P_INFORMME))
    write("Das hattest Du schon so eingestellt.\n");
      else
      {
    write("Ok.\n");
    Set(P_INFORMME,0);
      }
      return 1;
    case 0:
      write("Inform-Mode ist "+(Query(P_INFORMME)?"an":"aus")+"geschaltet.\n");
      return 1;
    }
  write("inform an oder inform aus, bitte.\n");
  return 1;
}

void delayed_write(mixed *what)
{
  if (!pointerp(what)||!sizeof(what)||!pointerp(what[0]))
    return;
  tell_object(ME,what[0][0]);
  if (sizeof(what)>1&&sizeof(what[0])>1)
    call_out("delayed_write",what[0][1],what[1..]);
}

void notify_player_change(string who, int rein, int invis)
{
  string *list,name;
  mixed mlist;

  if (invis) name="("+who+")";
    else name=who;

  if (Query(P_INFORMME))
  {
      if (rein)
        tell_object(ME,name+" ist gerade ins "MUDNAME" gekommen.\n");
      else
        tell_object(ME,name+" hat gerade das "MUDNAME" verlassen.\n");
  }

  if(Query(P_WAITFOR_FLAGS) & (0x01))return ;

  if(pointerp(list=Query(P_WAITFOR)) && sizeof(list) && member(list,who)!=-1)
  {
    if (!QueryProp(P_VISUALBELL))
        name+=sprintf("%c",7); // Char fuer Pieps an den String anhaengen.
    // Moechte der Spieler keine ASCII-Grafik sehen, wird diese Meldung ohne
    // Leerzeichen formatiert, so dass sie von Screenreadern vorgelesen wird.
    // Anderenfalls wuerde sie einzeln buchstabiert.
    if ( QueryProp(P_NO_ASCII_ART) )
    {
      delayed_write( ({ ({ sprintf("%s IST JETZT %sDA !!!\n", 
                           name, (rein?"":"NICHT MEHR ")) }) }) );
    }
    else 
    {
      delayed_write( ({ ({ sprintf("%s   I S T   J E T Z T   %sD A !!!\n",
                           name, (rein?"":"N I C H T   M E H R   ")) }) }) );
    }
  }

  if (rein && (sizeof(mlist=QueryProp(P_WAITFOR_REASON))) &&
     (mappingp(mlist)) && (mlist[who]))
        Show_WaitFor_Reason(who,invis);
}

static int erwarte(string str)
{
  string *list,*str1;
  mixed mlist;

  if (!mappingp(mlist=QueryProp(P_WAITFOR_REASON)))
     mlist=([]);
  if (!pointerp(list=Query(P_WAITFOR)))
     list=({});

  if (!str || str=="-u")
  {
     if(Query(P_WAITFOR_FLAGS)&0x01)
       write("Du hast 'erwarte' temporaer deaktiviert.\n");
     write("Du erwartest jetzt");
     if (!sizeof(list))
        write(" niemanden mehr.\n");
     else
     {
        write(":\n");
        if (!str) list=sort_array(list,#'>);
        More(break_string(CountUp(list),78));
     }
     return 1;
  }
  if(str=="aus"){
    Set(P_WAITFOR_FLAGS,Query(P_WAITFOR_FLAGS)|0x01);
    write("Erwarte ist jetzt deaktiviert.\n");
    return 1;
  }
  if(str=="an" || str=="ein"){
    Set(P_WAITFOR_FLAGS,Query(P_WAITFOR_FLAGS)&0xFE);
    write("Erwarte ist jetzt aktiv.\n");
    return 1;
  }

  str1=explode(_unparsed_args()||""," ");
  if (sizeof(str1)==1)
  {
     if (str1[0]!="wegen")
     {
        str=capitalize(lower_case(str));
        if (member(list,str)!=-1)
        {
           SetProp(P_WAITFOR_REASON,m_copy_delete(mlist,str));
           list-=({str});
           write(str+" aus der Liste entfernt.\n");
        } else
        {
           if (sizeof(list)>1000)
           {
             write("Du erwartest schon genuegend!\n");
             return 1;
           }
           list+=({str});
           write(str+" an die Liste angehaengt.\n");
        }
        Set(P_WAITFOR,list);
     }
     else
     {
        if (sizeof(mlist))
        {
           write("Du erwartest aus einem bestimmten Grund:\n");
           write(break_string(CountUp(sort_array(m_indices(mlist),
                                                 #'>))+".",78));
        }
        else write("Du erwartest niemanden aus einem bestimmten Grund.\n");
     }
     return 1;
  }
  notify_fail("Falsche Syntax, siehe 'hilfe erwarte'!\n");
  if (str1[1]!="wegen") return 0;
  if (sizeof(str1)==2)
     Show_WaitFor_Reason(capitalize(lower_case(str1[0])),0);
  else {
     string s=capitalize(lower_case(str1[0]));
     if (sizeof(str1)==3 && (str1[2]=="nichts" || str1[2]=="loeschen"))
        if (!mlist[s])
           write("Du hast "+s+" aus keinem bestimmten Grund erwartet!\n");
        else
        {
           SetProp(P_WAITFOR_REASON,m_copy_delete(mlist,s));
           write("Du erwartest "+s+" aus keinem bestimmten Grund mehr!\n");
        }
     else
     {
        // Menge an erwarte-wegen Eintraegen begrenzen.
        int lim;
        if (IS_ARCH(ME)) lim=120;
        else if (IS_LEARNER(ME)) lim=80;
        else if (IS_SEER(ME)) lim=60;
        else lim=30;
        if (!mlist[s] && sizeof(mlist)>=lim)
           write("Sorry, aber Du erwartest schon genuegend Leute!\n");
        else
        {
           // Meldung wieder zusammensetzen
           string meldung = implode(str1[2..], " ");
           // und Laenge auf 78 Zeichen abschneiden.
           meldung = sprintf("%.78s", meldung);
           m_add(mlist, s, meldung);
           SetProp(P_WAITFOR_REASON, mlist);
           Show_WaitFor_Reason(s,0);
        }
     }
  }
  return 1;
}

static int uhrmeldung(string str)
{
  if (!(str=_unparsed_args()))
  {
    str=QueryProp(P_CLOCKMSG);
    if (!str)
    {
      write("Du hast die Standard-Uhrmeldung.\n");
      return 1;
    }
        if( !stringp(str) ) str = sprintf("%O\n",str);
    printf("Deine Uhrmeldung ist:\n%s\n",str[0..<2]);
    return 1;
  }
  if (str=="0")
  {
    SetProp(P_CLOCKMSG,0);
    write("Ok, Du hast jetzt wieder die Standard-Meldung.\n");
    return 1;
  }
  if (sizeof(explode(str,"%d"))>2)
  {
    write("Fehler, es darf nur ein %d in der Meldung vorkommen.\n");
    return 1;
  }
  /* Mehrere %-Parameter verursachen das Abschalten der Uhr zur vollen Stunde.
   */
  if (sizeof(explode(str,"%"))>2)
  {
    write("Fehler: Zuviele %-Parameter in der Meldung.\n");
    return 1;
  }
  /* Nur ein %-Parameter, aber der falsche: nicht sinnvoll. */
  else
  {
    int i = strstr(str,"%",0);
    if ( i>-1 && ( i==sizeof(str)-1 || str[i+1]!='d'))
    {
      write("Fehler: Falscher %-Parameter in der Meldung.\n");
      return 1;
    }
  }
  str+="\n";
  SetProp(P_CLOCKMSG,str);
  write("Ok.\n");
  return 1;
}

static int zeitzone(string str)
{
  int zt;
  if(!str || str==""){
    if(!(zt=QueryProp(P_TIMEZONE)))
      write("Du hast derzeit die gleiche Zeitzone wie das "MUDNAME" "+
            "eingestellt.\n");
    else if(zt>0)
      printf("Deine Zeitzone ist auf %d Stunden vor (oestlich) von Berlin "+
             "eingestellt.\n",zt);
    else
      printf("Deine Zeitzone ist auf %d Stunden nach (westlich) von "+
             "Berlin eingestellt.\n",-zt);
    return 1;
  }
  if(sscanf(str,"utc %d",zt)==1)  zt=(zt-1)%24;
  else zt=to_int(str)%24;

  SetProp(P_TIMEZONE,zt);

  if(!zt)
    write("Du hast derzeit die gleiche Zeitzone wie das "MUDNAME" "+
          "eingestellt.\n");
  else if(zt>0)
    printf("Deine Zeitzone ist auf %d Stunden vor (oestlich) von Berlin "+
           "eingestellt.\n",zt);
  else
    printf("Deine Zeitzone ist auf %d Stunden nach (westlich) von "+
           "Berlin eingestellt.\n",-zt);
  return 1;
}

static int emailanzeige(string str){
  notify_fail("Syntax: emailanzeige [alle|freunde|niemand]\n");
  if(!str || str==""){
    if(!(str=QueryProp(P_SHOWEMAIL)))str="Niemandem";
    else if(str=="alle")str="allen";
    else if(str=="freunde")str="Deinen Freunden";
    else if(str=="niemand")str="niemandem";
    else{
      SetProp(P_SHOWEMAIL,0);
      str="Niemandem";
    }
    write("Deine Email wird "+str+" angezeigt.\n");
    return 1;
  }
  else if(member(({"alle","freunde","niemand"}),str)==-1)return 0;

  SetProp(P_SHOWEMAIL,str);

  if(str=="alle")str="allen";
  else if(str=="freunde")str="Deinen Freunden";
  else str="niemandem";
  write("Deine Email wird "+str+" angezeigt.\n");
  return 1;
}

static int zaubertraenke()
{
  More("/room/orakel"->TipListe());
  return 1;
}

varargs static int angriffsmeldung(string arg) {
  if (arg=="ein" || arg=="an")
    SetProp(P_SHOW_ATTACK_MSG,1);
  else if (arg=="aus")
    SetProp(P_SHOW_ATTACK_MSG,0);
  if (QueryProp(P_SHOW_ATTACK_MSG))
    write("Du siehst saemtliche Angriffsmeldungen von Dir.\n");
  else
    write("Du siehst nur neue Angriffsmeldungen von Dir.\n");
  return 1;
}

static mixed _query_localcmds()
{
  return ({({"zeilen","set_screensize",0,0}),
           ({"email","set_email",0,0}),
           ({"url","set_homepage",0,0}),
           ({"icq","set_icq",0,0}),
           ({"messenger", "set_messenger", 0, 0}), 
           ({"ort","set_location",0,0}),
           ({"punkte","short_score",0,0}),
           ({"score","short_score",0,0}),
           ({"info","score",0,0}),
           ({"kurzinfo","very_short_score",0,0}),
           ({"quit","new_quit",0,0}),
           ({"ende","new_quit",0,0}),
           ({"disconnect","disconnect",0,0}),
           ({"schlaf","disconnect",1,0}),
           ({"toete","kill",0,0}),
           ({"angriffsmeldung","angriffsmeldung",0,0}),
           ({"passw","change_password",1,0}),
           ({"hilfe","help",1,0}),
           ({"selbstloeschung","self_delete",0,0}),
           ({"spielpause","spielpause",0,0}),
           ({"spieldauer","spieldauer",0,0}),
           ({"idee","ReportError",0,0}),
           ({"typo","ReportError",0,0}),
           ({"bug","ReportError",0,0}),
           ({"fehler","fehlerhilfe",0,0}),
           ({"md","ReportError",0,0}),
           ({"detail","ReportError",0,0}),
           ({"syntaxhinweis","ReportError",0,0}),
           ({"vorsicht","toggle_whimpy",0,0}),
           ({"stop","stop",0,0}),
           ({"kwho","kwho",0,0}),
           ({"kwer","kwho",0,0}),
           ({"kkwer","kkwho",0,0}),
           ({"kkwho","kkwho",0,0}),
           ({"who","who",0,0}),
           ({"wer","who",0,0}),
           ({"zeit","uhrzeit",0,0}),
           ({"uhrzeit","uhrzeit",0,0}),
           ({"weg","weg",0,0}),
           ({"wegmeldung", "wegmeldung", 0, 0}),
           ({"idlezeit", "idlezeit", 0, 0}),
           ({"finger","finger",0,0}),
           ({"muds","muds",0,0}),
           ({"emote","emote",0,0}),
           ({":","emote",1,0}),
           ({";","emote",1,0}),
           ({"remote","remote",0,SEER_LVL}),
           ({"r:","remote",1,0}),
           ({"r;","gremote",1,0}),
           ({"titel","set_title",0,0}),
           ({"review","review",0,SEER_LVL}),
           ({"setmin","setmin",0,SEER_LVL}),
           ({"setmout","setmout",0,SEER_LVL}),
           ({"setmmin","setmmin",0,SEER_LVL}),
           ({"setmmout","setmmout",0,SEER_LVL}),
           ({"sethands","sethands",0,SEER_LVL}),
           ({"presay","presay",0,SEER_LVL}),
           ({"extralook","extralook",0,SEER_LVL}),
           ({"fluchtrichtung","toggle_whimpy_dir",0,SEER_LVL}),
           ({"inform","inform",0,0}),
           ({"erwarte","erwarte",0,0}),
           ({"stty","stty",0,0}),
           ({"grafik", "set_ascii_art", 0, 0}), 
           ({"uhrmeldung","uhrmeldung",0,0}),
           ({"zeitzone","zeitzone",0,0}),
           ({"behalte","behalte",0,0}),
           ({"zweitiemarkierung","zweitiemarkierung",0,0}),
           ({"emailanzeige","emailanzeige",0,0}),
           ({"topliste","topliste",0,0}),
           ({"ton","set_visualbell",0,0}),
           ({"telnegs","show_telnegs",0,0}),
           ({"spotte", "spotte", 0, 0}),
           ({"reise","reise",0,0}),
           ({"zaubertraenke","zaubertraenke",0,0}),
           ({"telnet","telnet_cmd",0,0}),
     })+
     command::_query_localcmds()+
     viewcmd::_query_localcmds()+
     comm::_query_localcmds()+
     skills::_query_localcmds()+
     description::_query_localcmds();
}

static int _check_keep(object ob)
{
  return (ob->QueryProp(P_KEEP_ON_SELL))==geteuid(ME);
}

static mixed _set_testplayer(mixed arg) {
  mixed res;
  object setob;

  setob=this_player();
  if (!objectp(setob) || !query_once_interactive(setob))
    setob=this_interactive();
  if (!objectp(setob))
    setob=previous_object();
  if (setob && !IS_DEPUTY(setob)) {
    arg=geteuid(setob);
    if (!arg || arg=="NOBODY")
      arg=getuid(setob);
    arg=capitalize(arg);
  }
  res=Set(P_TESTPLAYER,arg);
  Set(P_TESTPLAYER,PROTECTED,F_MODE_AS);
  return res;
}

int zweitiemarkierung(string arg)
{
  if (!QueryProp(P_SECOND))
    return _notify_fail("Aber Du bist doch gar kein Zweiti.\n"),0;
  notify_fail("Syntax: zweitiemarkierung [unsichtbar|sichtbar|name]\n");
  if (!arg)
    return 0;
  switch (arg)
  {
    case "unsichtbar" :
      SetProp(P_SECOND_MARK,-1);
      write("Jetzt sieht kein Spieler mehr, dass Du ein Zweiti bist.\n");
      return 1;
    case "sichtbar" :
      SetProp(P_SECOND_MARK,0);
      write("Jetzt sieht kein Spieler mehr, wessen Zweiti Du bist.\n");
      return 1;
    case "name" :
      SetProp(P_SECOND_MARK,1);
      write("Jetzt koennen alle sehen, wessen Zweiti Du bist.\n");
      return 1;
  }
  return 0;
}

int topliste(string arg)
{
    if (!arg)
    {
        printf("Du hast Dich fuer die Topliste %s.\n",
            (QueryProp(P_NO_TOPLIST) ? "gesperrt" : "freigegeben"));
        return 1;
    }
    else if (member(({"j","ja","n","nein"}),arg)==-1)
        return _notify_fail("Syntax: topliste [ja|nein]\n"),0;
    if (arg[0]=='j')
    {
        SetProp(P_NO_TOPLIST,0);
        write("Du kannst jetzt (theoretisch) in der Topliste auftauchen.\n");
    }
    else
    {
        SetProp(P_NO_TOPLIST,1);
        "/secure/topliste"->DeletePlayer();
        write("Du wirst jetzt nicht (mehr) in den Toplisten auftauchen.\n");
    }
    Set(P_NO_TOPLIST,SAVE|PROTECTED,F_MODE_AS);
    return 1;
}

int show_telnegs(string arg)
{
    if (!arg)
    {
        write("Du bekommst Aenderungen Deiner Fenstergroesse "+
              (QueryProp(P_TTY_SHOW)?"":"nicht ")+"angezeigt.\n");
        return 1;
    }
    if (member(({"ein","an","aus"}),arg)==-1)
    {
        write("Syntax: telnegs [ein|aus]\n");
        return 1;
    }
    if (arg=="ein" || arg=="an")
    {
        write("Du bekommst "+(QueryProp(P_TTY_SHOW)?"":"nun ")+
              "Aenderungen Deiner Fenstergroesse angezeigt.\n");
        Set(P_TTY_SHOW,1);
        return 1;
    }
    write("Du bekommst "+(QueryProp(P_TTY_SHOW)?"nun ":"")+
          "Aenderungen Deiner Fenstergroesse nicht "+
          (QueryProp(P_TTY_SHOW)?"mehr ":"")+"angezeigt.\n");
    Set(P_TTY_SHOW,0);
    return 1;
}

private int set_keep_alive(string str) {
  if (str == "ein") {
    telnet_tm_counter = QueryProp(P_TELNET_KEEPALIVE_DELAY) || (240 / __HEART_BEAT_INTERVAL__);
    tell_object(this_object(), break_string( sprintf(
        "An Deinen Client werden jetzt alle %i Sekunden unsichtbare Daten "
        "geschickt, um zu verhindern, dass Deine Verbindung zum "MUDNAME
        " beendet wird.",
        telnet_tm_counter*__HEART_BEAT_INTERVAL__), 78));
    // Bei Magiern ist der HB evtl. ausgeschaltet und muss eingeschaltet
    // werden.
    if (!object_info(this_object(), OC_HEART_BEAT))
      configure_object(this_object(), OC_HEART_BEAT, 1);
  }
  else if (str == "aus") {
    telnet_tm_counter = 0;
    tell_object(this_object(),break_string(
        "Du hast das Senden von unsichtbaren Daten (Keep-Alive-Pakete) an "
        "Deinen Client ausgeschaltet.",78));
  }
  else {
    if (!telnet_tm_counter)
      tell_object(this_object(), break_string(
        "An Deinen Client werden keine Keep-Alive-Pakete geschickt.",78));
    else
      tell_object(this_object(), break_string(
        "An Deinen Client werden alle 4 Minuten " 
        "unsichtbare Daten geschickt, damit Deine Verbindung "
        "zum "MUDNAME" nicht beendet wird.",78));
  }
  return 1;
}

private int print_telnet_rttime() {
  int rtt = QueryProp(P_TELNET_RTTIME);
  if (rtt>0)
    tell_object(ME, break_string(
      "Die letzte gemessene 'round-trip' Zeit vom MG zu Deinem Client "
      "und zurueck betrug " + rtt + " us.",78));
  else
    tell_object(ME, break_string(
      "Bislang wurde die 'round-trip' Zeit vom MG zu Deinem Client "
      "noch nicht gemessen oder Dein Client unterstuetzt dieses "
      "nicht.",78));
  return 1;
}

// Falls es eine per telnet vom Client ausgehandelte Einstellung fuer CHARSET
// gibt, hat die manuelle Einstellung von Spielern hier geringere Prioritaet
// und bildet nur den Fallback.
private int set_telnet_charset(string enc) {
  struct telopt_s tdata = query_telnet_neg()[TELOPT_CHARSET];
  if (!sizeof(enc))
  {
    if (!tdata->data || !tdata->data["accepted_charset"])
    {
      tell_object(ME, break_string(sprintf(
          "Zur Zeit ist der Zeichensatz \'%s\' aktiv. "
          "Alle Ausgaben an Dich werden in diesem Zeichensatz gesendet "
          "und wir erwarten alle Eingaben von Dir in diesem Zeichensatz. ",
          interactive_info(ME, IC_ENCODING)), 78));
    }
    else
    {
      tell_object(ME, break_string(sprintf(
          "Zur Zeit ist der Zeichensatz \'%s\' aktiv. "
          "Alle Ausgaben an Dich werden in diesem Zeichensatz gesendet "
          "und wir erwarten alle Eingaben von Dir in diesem Zeichensatz. "
          "Dieser Zeichensatz wurde von Deinem Client ausgehandelt.",
          interactive_info(ME, IC_ENCODING)), 78));
      if (QueryProp(P_TELNET_CHARSET))
        tell_object(ME, break_string(sprintf(
          "Dein manuell eingestellter Zeichensatz ist \'%s\', welcher "
          "aber nur genutzt wird, wenn Dein Client keinen Zeichensatz "
          "aushandelt.", QueryProp(P_TELNET_CHARSET)),78));

    }
  }
  // Wenn es "loeschen" ist, wird die Prop genullt und wir stellen den Default
  // ein. Allerdings nur, wenn nix per telnet ausgehandelt wurde, dann wird
  // das beibehalten.
  else if (lower_case(enc) == "loeschen")
  {
    SetProp(P_TELNET_CHARSET, 0);
    // wurde was per telnet option charset ausgehandelt? dann wird (weiterhin)
    // das genommen und nicht umgestellt.
    if (!tdata->data || !tdata->data["accepted_charset"])
    {
      configure_interactive(ME, IC_ENCODING, interactive_info(0,IC_ENCODING));
      tell_object(ME, break_string(sprintf(
          "Der Default \'%s\' wurde wieder hergestellt. "
          "Alle Ausgaben an Dich werden in diesem Zeichensatz gesendet "
          "und wir erwarten alle Eingaben von Dir in diesem Zeichensatz. "
          "Sollte Dein Client die Telnet-Option CHARSET unterstuetzen, kann "
          "dieser allerdings direkt einen Zeichensatz aushandeln oder "
          "ausgehandelt haben, der dann stattdessen gilt.",
          interactive_info(ME, IC_ENCODING)), 78));
    }
    else
    {
      tell_object(ME, break_string(sprintf(
          "Der Default \'%s\' wurde wieder hergestellt. Allerdings hat "
          "Dein Client mit dem MG den Zeichensatz \'%s\' ausgehandelt, "
          "welcher immer noch aktiv ist.",
          interactive_info(0, IC_ENCODING),
          interactive_info(ME, IC_ENCODING)), 78));
    }
  }
  else
  {
    // Wenn der Zeichensatz keine //-Variante ist, machen wir den zu
    // einer. Das verhindert letztlich eine Menge Laufzeitfehler, wenn ein
    // Zeichen mal nicht darstellbar ist.
    if (strstr(enc, "//") == -1)
      enc += "//TRANSLIT";
    if (catch(configure_interactive(ME, IC_ENCODING, enc); nolog))
    {
      tell_object(ME, break_string(sprintf(
            "Der Zeichensatz \'%s\' ist nicht gueltig oder zumindest auf "
            "diesem System nicht verwendbar.", enc),78));
    }
    else
    {
      SetProp(P_TELNET_CHARSET, interactive_info(ME, IC_ENCODING));
      if (!tdata->data || !tdata->data["accepted_charset"])
      {
        tell_object(ME, break_string(sprintf(
            "Der Zeichensatz \'%s\' wurde eingestellt. Alle Ausgaben an "
            "Dich werden in diesem Zeichensatz gesendet und wir erwarten "
            "alle Eingaben von Dir in diesem Zeichensatz. Sollte Dein "
            "Client die Telnet-Option CHARSET unterstuetzen, kann "
            "dieser allerdings direkt einen Zeichensatz aushandeln, der "
            "dann stattdessen gilt.",
            interactive_info(ME, IC_ENCODING)),78));
      }
      else
      {
        // Der via telnet ausgehandelte Charset muss wieder hergestellt
        // werden.
        configure_interactive(ME, IC_ENCODING,
                              tdata->data["accepted_charset"]);
        tell_object(ME, break_string(sprintf(
          "Der Zeichensatz \'%s\' wurde gespeichert. Allerdings hat "
          "Dein Client mit dem MG den Zeichensatz \'%s\' ausgehandelt, "
          "welcher immer noch aktiv ist.",
          QueryProp(P_TELNET_CHARSET),
          interactive_info(ME, IC_ENCODING)), 78));
      }
    }

  }
  return 1;
}

int telnet_cmd(string str) {
  if (!str) return 0;
  string *args = explode(str, " ");
  string newargs;
  if (sizeof(args) > 1)
    newargs = implode(args[1..], " ");
  else
    newargs = "";

  switch(args[0])
  {
    case "keepalive":
      return set_keep_alive(newargs);
    case "rttime":
      return print_telnet_rttime();
    case "charset":
      return set_telnet_charset(newargs);
    case "tls":
      if (tls_query_connection_state(ME) > 0)
        tell_object(ME,
            "Deine Verbindung zum Morgengrauen ist TLS-verschluesselt.\n");
      else
        tell_object(ME,
            "Deine Verbindung zum Morgengrauen ist nicht verschluesselt.\n");
      return 1;
    case "client-gui":
      GMCP_offer_clientgui(newargs);
      return 1;
  }
  return 0;
}

int spotte( string str )
{
    _notify_fail( "Hier ist nichts, was Du verspotten koenntest!\n" );
    return 0;
}

int behalte(string str)
{
  object ob,*obs;
  string s;

  if (str)
  {
    if (str=="alles") {
      filter_objects(all_inventory(), "SetProp", P_KEEP_ON_SELL, getuid());
      write("Ok!\n");
      return 1;
    }
    if (str=="nichts") {
      filter_objects(all_inventory(), "SetProp", P_KEEP_ON_SELL, 0);
      write("Ok!\n");
      return 1;
    }
    if (!sizeof(obs=find_obs(str,PUT_GET_DROP)))
    {
      _notify_fail("Aber sowas hast Du nicht dabei!\n");
      return 0;
    }
    else ob=obs[0];

    if (ob->QueryProp(P_KEEP_ON_SELL)==geteuid(ME))
        ob->SetProp(P_KEEP_ON_SELL,0);
    else
        ob->SetProp(P_KEEP_ON_SELL,geteuid(ME));

    // erneut abfragen, da sich der Wert nicht geaendert haben muss
    if (ob->QueryProp(P_KEEP_ON_SELL)==geteuid(ME))
        write(break_string(sprintf("Ok, Du wirst %s jetzt bei 'verkaufe alles' "
                                   "behalten.\n",ob->name(WEN)),78));
    else
        write(break_string(sprintf("Ok, Du wirst %s beim naechsten 'verkaufe "
                                   "alles' mitverkaufen!\n",ob->name(WEN)),78));

    return 1;
  }
  s=make_invlist(ME,filter(all_inventory(ME),#'_check_keep)); //'));
  More(s);
  return 1;
}

static int _query_lep()
{
  int val;
  val = LEPMASTER->QueryLEP();
  Set( P_LEP, val );
  return val;
}

static mixed _set_fraternitasdonoarchmagorum(mixed arg)
{
  if (!intp(arg)) return -1;

  if ((!previous_object(1)||object_name(previous_object(1))!=FAO_MASTER) && 
      (!this_interactive() || !IS_ARCH(this_interactive())))
    return -1;

  if (!intp(arg)) return -1;

  log_file("fao/P_FAO",sprintf("%s - %s P_FAO gesetzt auf %O\n",
        dtime(time()),query_real_name(),arg) );
  return Set(P_FAO,arg);
}

nomask public string set_realip(string str)
{
  if(previous_object()
     && strstr(object_name(previous_object()),"/secure")==0)
  {
    realip=str;
  }
  return realip;
}

nomask public string query_realip()
{
  return realip ? realip : 0;
}

mixed _query_netdead_env() {
        return ndead_location || ndead_l_filename;
}
