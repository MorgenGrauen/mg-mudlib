/* Shutdown-Daemon -- die Messages muessen wohl nochmal ueberarbeitet werden */
/*
 *  04.04.2004   Vanion  die() ueberschrieben, ignorierbar gemacht
 *  15.01.2000   Tiamak  aufgeraeumt, sicherer gemacht 
 *  15.02.1998   Wargon  Armageddon teilt einem die Restzeit mit
 *  07.01.1998         Rumata  IS_ELDER ausgebaut.
 *
 *  23.07.1997         17.00         Yantro
 *    Ich hab mal die shouts Zeitabhaengig gemacht, also sie sollten
 *    heftiger werden mit Annaeherung an den Reboot. Deswegen gibt
 *    es nun 4 arrays mit messages und die Entsprechende Aufrufaenderung
 *    in ArmasShout()
 */

#pragma combine_strings
#pragma strong_types
#pragma strict_types
#pragma no_inherit
#pragma no_clone
#pragma no_shadow

inherit "std/npc";

#define S_PRESAY 1
#define S_IGNORE 2

#include <properties.h>
#include <defines.h>
#include <language.h>
#include <moving.h>
#include "/secure/wizlevels.h"

#define messages1 ({ \
  "Wolken ballen sich am Himmel. Es scheint wohl ein Unwetter aufzuziehen. " \
  "Du hast ein ungutes Gefuehl in Deiner Haut.", \
  "Aus der Ferne hoerst Du ein Donnern ... ob das ein Gewitter ist?", \
  "Wolken ziehen schnell vorbei und verdunkeln die Sonne und die Sterne. " \
  "Dies scheint keines der normalen Unwetter zu sein, die Du schon erlebt " \
  "hast.", \
  "Das ferne Rauschen der Baeche und Fluesse wird lauter. Die Wassermassen " \
  "scheinen nicht mehr kontrolliert zu fliessen und Du bekommst Angst!", \
  "Der Himmel faerbt sich violett. Das Unwetter scheint gewaltiger zu " \
  "werden als sonst. Dir laeuft ein Schauer ueber den Ruecken." \
  })

#define messages2 ({ \
  "Die Wolken verdunkeln den Himmel. Es wird ueberall duester in der Welt " \
  "und Du kannst kaum noch erkennen, was um Dich herum geschieht.", \
  "Ein starker Wind weht durch alle Gegenden und zerrt an allem, was nicht " \
  "fest ist. Hoffentlich wird das nicht noch schlimmer.", \
  "Regen prasselt ueberall nieder und fuellt die Baeche und Seen, die das " \
  "Wasser aber kaum noch halten koennen. Du versucht eine trockene Stelle " \
  "zu finden.", \
  "Der Himmel ist fast schwarz. Der Regen faellt in einer Menge, als haetten " \
  "alle Schleusen der Welt geoeffnet, um sie fuer immer zu ertraenken.", \
  "Die Luft scheint zu knistern! Das Ende des Unwetters ist sicher noch nicht "\
  "in Sicht. Elektrizitaet entlaedt sich im Himmel zwischen den Wolken. " \
  "Was fuer ein Lichtspiel!", \
  "Blitz und Donner hallen durch die Welt. Es scheint keine Stelle zu geben, " \
  "an der man sicher sein kann." \
  })

#define messages3 ({ \
  "Du siehst, wie die Einwohner der Welt versuchen, sich panisch vor dem " \
  "Unwetter zu retten, aber es ist fuer sie zu spaet.", \
  "Blitze nie gesehener Gewalt schlagen rund um Dich ein. Feuer flammen auf " \
  "und scheinen alles verbrennen zu wollen, was um Dich herum existiert.", \
  "Dicke Wolken tuermen sich immer weiter auf und scheinen Dich erdruecken zu "\
  "wollen. Sie sind nicht mehr schwarz, sondern blutrot wie das Wasser, was " \
  "sie von sich geben.", \
  "Die Fluesse und Meere der Welt treten ueber ihre Ufer und zerstoeren alles "\
  "Leben. Das Wasser hat sich zu Blut gewandelt! Um Dich herum wird alles " \
  "rot!", \
  "Der Donner grollt mit roher Gewalt durch die Welt. Die Schallwellen " \
  "schlagen mit aller Kraft gegen die Gebirge und scheinen sie reizen zu " \
  "wollen. Du kriegst Angst!", \
  "Der Regen faerbt sich rot! Das ist kein Wasser mehr, sondern Blut, was da " \
  "auf Dich herabregnet. Dir schwinden kurz die Sinne beim Gedanken an das, " \
  "was noch kommen mag." \
  })

#define messages4 ({ \
  "Das Blut des Regens scheint Dich fortzuschwemmen. Ueberall ist das Blut " \
  "ueber die Ufer getreten und sucht sich seinen toedlichen Weg.", \
  "Die Vulkane der Welt brechen aus und die Lava verbrennt alles, was sich " \
  "ihr in den Weg stellt. Sie bringt das Blut zum Verdampfen und erfuellt " \
  "alles mit ihrer zerstoererischen, grausamen Hitze.", \
  "Ploetzlich ist alles mit Feuer und Hitze erfuellt. Die Meere und " \
  "Fluesse verdampfen und der Dampf treibt heiss ueber die Welt, um alles " \
  "zu verbruehen, was sich nicht schuetzt!", \
  "Von ueberall hoerst Du Schreie und Du erkennst panische Angst in den " \
  "Augen der Eingeborenen. Elendig verenden sie in der Hitze zwischen Feuer " \
  "und Lava. Dir wird uebel.", \
  "Um Dich herum siehst Du nur brennende Truemmer. Die Welt scheint vor " \
  "ihrem Untergang, denn ueberall oeffnet sich der Boden und spuckt Feuer. " \
  "Heisses Magma quillt aus ihm heraus und mischt sich zischend mit dem Blut " \
  "der Meere. Es gibt nun sicher kein Entrinnen mehr.", \
  "Du kannst Dich kaum noch halten. Erdbeben erschuettern alles um Dich " \
  "herum. Ueberall kannst Du nur noch Truemmer erkennen, wo einmal Haeuser " \
  "standen. Alles ist erfuellt vom Geruch des Todes!" \
  })

static int restzeit;
static string *ignorierer = ({});

static int _query_restzeit() { return restzeit + find_call_out("NextStep"); }
static string *_query_ignore() { return ignorierer; }

public varargs void die(int poisondeath,int extern)
{
  object tp;
  object room;

  if (objectp(room=environment(this_object())))
    if (!objectp(tp=this_player()))
      tell_room(room,"Armageddon kratzt sich ratlos am Kopf.\n");
    else 
      tell_room(room,
        "Armageddon sagt: Ich weigere mich, einfach so zu sterben, "+
        (tp->Name())+".\n");
  return;
}

status access_check() {
    //wer darf rebooten?

    //in einem process_string() gar nicht.
    if (process_call()) return(0);

    //Master darf natuerlich. (->slow_shut_down(), vom Driver bei
    //Speicherknappheit gerufen.)
    if (previous_object() && 
        previous_object()==find_object(__MASTER_OBJECT__))
        return(1);

    // rebooten duerfen ansonsten nur [W]+
    if ( ELDER_SECURITY) {
        return(1);
    }

    //andere nicht.
    return(0);
}

public void create()
{    
    ::create();
    
    SetProp( P_NAME, "Armageddon" );
    SetProp( P_GENDER, MALE );
    SetProp( P_SHORT, "Armageddon, der Weltenvernichter" );
    AddId( "armageddon" );
    set_living_name( "armageddon" );
    SetProp( P_ARTICLE, 0 );
    SetProp( P_XP, 0 );
    SetProp( P_NO_ATTACK,
             "Armageddon grinst: Dein Ende kommt noch frueh genug!\n" );

    if (!access_check()) {
        destruct(this_object());
        raise_error("Armageddon darf nur von W+ aktiviert werden!");
    }
}


static varargs string text_time( int sec, int flag )
{
    string s;
    int tage, stunden, minuten, sekunden;

    sekunden = sec;
    tage = sekunden / 86400;
    stunden = (sekunden % 86400) / 3600;
    minuten = (sekunden % 3600) / 60;
    sekunden = sec % 60;
    s = "";
    
    if ( sec >= 86400 )
        s += tage + " Tag" + (tage == 1 ? ", " : (flag ? "en, " : "e, "));
    
    if ( sec >= 3600 )
        s += stunden + " Stunde" + (stunden == 1 ? ", " : "n, ");
    
    if ( sec >= 60 )
        s += minuten + " Minute" + (minuten == 1 ? "" : "n");
    
    s += (sec/60 ? " und " : " ") + sekunden + " Sekunde" +
        (sekunden == 1 ? "" : "n");
    
    return s;
}


public string _query_long()
{
    return break_string( "Dies ist Armageddon, der Weltenvernichter. Als Du "
                         "ihn ansiehst, blickt er auf und fluestert Dir zu: "
                         "Noch " + text_time(_query_restzeit()) +
                         " muessen vergehen, ehe ich die Welt vernichte.", 78 );
}


public string _query_short()
{
    return "Armageddon, der Weltenvernichter";
}


public int shut( mixed minutes )
{
    int a, b, c;

    if (!access_check()) {
        destruct(this_object());
        raise_error("Armageddon darf nur von W+ aktiviert werden!");
        return(-1);   //never reached
    }
    
    a = b = c = 0;
    move( "/gilden/abenteurer", M_GO );
    
    if ( intp(minutes) )
        c = minutes;
    else if ( stringp(minutes) ){
        if ( sscanf( minutes, "%d:%d:%d", a, b, c ) == 3 )
            c += b * 60 + a * 24 * 60;
        else if( sscanf( minutes, "%d:%d", b, c) == 2 )
            c += b * 60;
        else if( sscanf( minutes, "%d", c ) != 1 )
            return -2;
    }
    else {
      destruct(ME);
      raise_error("Illegal argument type to shut()\n");
    }

    while ( remove_call_out("NextStep") >= 0 )
        ;
    
    restzeit = c * 60;
    call_out( "NextStep", 0, restzeit );
    return restzeit;
}

// Schickt die Meldung str an alle Spieler, die Armageddon nicht ignorieren
// Flags: S_IGNORE - Shout kann von Spielern ignoriert werden.
//        S_PRESAY - Es wird ein "Armageddon ruft:" vorangestellt.
static void SHOUT( string str, int flags )
{
   if (flags&S_PRESAY)
     str = break_string(str, 78, "Armageddon ruft: ");
   else
     str = break_string(str, 78);
     
   if (flags&S_IGNORE)
     filter(
       users()-filter_objects(users(),"TestIgnore",({"armageddon"}))
              -map(ignorierer, #'find_player),
       #'tell_object, str);
   else
     filter( users(), #'tell_object, str);
}


static void NextStep( int seconds )
{
    int neu;

    if ( seconds <= 0 ){
        SHOUT( "Ich starte das Spiel jetzt neu !", S_PRESAY);
        shutdown("");
        return;
    }
    
    if ( seconds <= 600 )
      SHOUT( "Teile mir mit, wenn Du in den Laden gebracht werden willst!", 
              S_PRESAY|S_IGNORE);
    
    if ( seconds > 2 * 86400 )
        neu = seconds - 5 * 3600;
    else if ( seconds > 4800 )
        neu = seconds - 1800;
    else
        neu = seconds * 3 / 4 - 10;
    
    restzeit = neu;
    call_out( "NextStep", seconds-neu, neu );
    
    SHOUT( "In " + text_time( seconds, 1 ) + " werde ich die Welt "
           "zerstoeren und neu erschaffen!", S_PRESAY|S_IGNORE);
    
    if ( neu < 900 && find_call_out("ArmasShout") == -1 )
        call_out( "ArmasShout", random(60)+10 );
}


static void ArmasShout()
{
    int dauer;
    
    dauer = _query_restzeit();
    call_out( "ArmasShout", 40+random(40) );
    
    if ( 701 < dauer )
        SHOUT( messages1[ random(sizeof(messages1))], S_IGNORE);
    else if ( 501 < dauer )
        SHOUT( messages2[ random(sizeof(messages2))], S_IGNORE);
    else if ( 251 < dauer )
        SHOUT( messages3[ random(sizeof(messages3))], S_IGNORE);
    else
        SHOUT( messages4[ random(sizeof(messages4))], S_IGNORE);
}


public void catch_tell( string str )
{
    string who, what;
    object ob;

    if ( sscanf( str, "%s teilt Dir mit: %s", who, what ) != 2 ||
         !this_player() )
        return;
    
    // Ein Spieler will fuer dieses Reboot ignoriert werden, oder
    // das Ignoriere soll aufgehoben werden.
    if (lower_case(what)[0..<2]=="ruhe"){
      if (member(ignorierer, getuid(this_player()))>-1)
      {
         ignorierer -= ({ getuid(this_player()) });
         tell_object( this_player(), break_string(
          "Gut, ich notier's mir. Ab nun hast Du wieder am grossen Showdown "
          "teil. Viel Spass dabei.\n", 78, "Armageddon teilt Dir mit: "));
      } else {
         ignorierer += ({ getuid(this_player()) });
         tell_object( this_player(), break_string(
          "Gut, ich notier's mir. Der Weltuntergang soll Dich beim Spielen "
          "nicht weiter stoeren.\n", 78, "Armageddon teilt Dir mit: "));
      }
      return;
    }
   
    // In den Laden
    if ( _query_restzeit() < 600 ){
        this_player()->move( "/d/ebene/room/PortVain/laden", M_TPORT );
        return;
    }
    
    tell_object( this_player(),
                 break_string( "Es ist noch zu frueh! Probier es 10 Minuten "
                               "vor dem Ende nochmal!\nIn " +
                               text_time( _query_restzeit(), 1 ) +
                               " ist es soweit!", 78,
                               "Armageddon teilt Dir mit: ", 1 ) );
    return;
}


public varargs int remove()
{
    write( "Armageddon will nicht removed werden!\n" );
    return 0;
}


public int do_damage( int dam, object enemy )
{
    return 0;
}

// Nicht jeder Magier darf Armageddon destructen.
string NotifyDestruct(object caller) {
    if( (caller!=this_object() && !ELDER_SECURITY) || process_call() ) {
      return "Du darfst Armageddon nicht zerstoeren!\n";
    }
    return 0;
}

