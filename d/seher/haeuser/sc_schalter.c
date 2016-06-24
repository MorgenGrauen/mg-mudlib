#include <properties.h>
#include <wizlevels.h>
#include <defines.h>
#include <moving.h>
#include <bank.h>
#include "haus.h"
#include <money.h>

inherit "std/room";

#define KONTO "seercard_kontostand"

// Prototypes
public void BecomesNetDead( object pl );
static int _guthaben();
static int _einzahlen( string str );
static int _aufladen( string str );
static int _kaufen( string str );


protected void create()
{
    ::create();

    SetProp( P_INDOORS, 1 );
    SetProp( P_LIGHT, 1 );
    SetProp( P_LOG_FILE, "report/tiamak.rep" );
    SetProp( P_INT_SHORT, "Am Geldkartenschalter" );
    SetProp( P_INT_LONG,
             break_string( "Das ist also der Geldkartenschalter. Hinter einer "
                           "kaum durchsichtigen Rauchglasscheibe scheint ein "
                           "Beamter (oder zumindest irgendein Lebewesen) "
                           "geduldig zu warten. Neben dem Schalter haengt "
                           "eine Tafel.", 78 ) );

    AddDetail( ({ "boden" }),
               "Der tiefschwarze Boden gibt Dir das Gefuehl, im absoluten "
               "Nichts zu stehen.\n" );
    AddDetail( "gefuehl", "Kein schoenes Gefuehl.\n" );
    AddDetail( ({ "schwarz", "schwaerze" }), "Tiefschwarz.\n" );
    AddDetail( ({ "wand", "waende" }),
               "Dunkle Schatten tanzen ueber die Waende.\n" );
    AddDetail( ({ "decke" }),
               "Sie scheint Dir in dieser unheimlichen Umgebung auf den "
               "Kopf fallen zu\nwollen.\n" );
    AddDetail( "umgebung", "Schau Dich doch um.\n" );
    AddDetail( ({ "kopf" }), "Das ist der Gegenstand auf Deinem Hals.\n" );
    AddDetail( "gegenstand", "Ich meine Deinen Kopf.\n" );
    AddDetail( "hals", "Wenn Du Nachhilfe in Anatomie brauchst, such Dir "
               "jemand anderen.\n" );
    AddDetail( ({ "jemand" }), "Keiner da.\n" );
    AddDetail( "nachhilfe", "Dazu musst Du wohl woanders suchen.\n" );
    AddDetail( "anatomie", "Schlag es in einem Lexikon nach.\n" );
    AddDetail( "lexikon", "Soetwas hast Du anscheinend noch nie gesehen.\n" );
    AddDetail( ({ "nichts" }),
               "Du bist blind!\n" );
    AddDetail( ({ "schatten" }),
               "Die Schatten sind nur dazu da, um Dich zu irritieren. "
               "Und das machen sie gut.\n" );
    AddDetail( ({ "beamte", "beamter", "beamten", "schalterbeamte",
                      "schalterbeamten", "schalterbeamter" }),
               "Du kannst ihn nur schemenhaft hinter der Rauchglasscheibe "
               "des Schalters er-\nkennen. Die Umrisse scheinen allerdings "
               "zu keiner Lebensform zu gehoeren,\nderer Du Dich erinnern "
               "koenntest.\n" );
    AddDetail( ({ "scheibe", "glasscheibe", "rauchglas", "rauchglasscheibe" }),
               "Ein schemenhafter Beamter sitzt dahinter.\n" );
    AddDetail( ({ "umrisse" }),
               "Sie sind aeusserst beunruhigend.\n" );
    AddDetail( ({ "lebensform", "schemen", "lebewesen" }),
               "Du moechtest gar nicht wissen, um was es sich dabei "
               "handelt.\n" );
    AddDetail( ({ "schalter", "geldkartenschalter" }),
               "An diesem Schalter kannst Du die beruehmte"
               " Seer-Card (TM) erwerben sowie\nDein Konto aufladen.\n" );
    AddDetail( ({ "konto", "karte", "geldkarte", "card", "seercard",
                      "seer-card" }), "Wie das geht, steht auf der Tafel.\n" );
    AddDetail( "tafel", "Du kannst sie lesen.\n" );

    AddSmells( SENSE_DEFAULT, "Es riecht nach altem Staub.\n" );
    AddSmells( "staub", "Hier lagern sicher viele alte Akten.\n" );
    AddSmells( "akten", "Die riechen dann irgendwann nach Staub.\n" );

    AddSounds( SENSE_DEFAULT, "Direkt vor dem Schalter ist es erstaunlich "
               "ruhig.\nGeradezu ehrfurchtsvoll ruhig.\n" );
    AddSounds( ({ "schalter", "ruhe", "ehrfurcht" }),
               "Ob das am Schalterbeamten liegt?\n" );
    AddSounds( ({ "beamter", "beamten", "schalterbeamter", "schalterbeamten" }),
               "Ist er ueberhaupt noch da?\n" );

    AddReadDetail( ({ "tafel", "preisliste" }),
                   "Preisliste:\n"
                   "================================================="
                   "=====================\n\n"
                   "Neue Geldkarte .................................."
                   "...... 10000 Muenzen\n"
                   "Syntax: kaufe geldkarte\n\n"
                   "Einzahlungen auf das Geldkarten-Konto ..........."
                   "...... 10% Gebuehren\n"
                   "Syntax: zahle <n> muenzen ein\n"
                   "oder    lade karte auf\n\n" );

    AddCmd( ({ "zahl", "zahle" }), "_einzahlen" );
    AddCmd( ({ "lad", "lade" }), "_aufladen" );
    AddCmd( ({ "kaufe", "kauf" }), "_kaufen" );
    
    AddExit( "raus", PATH+"seherbank" );
}


public void BecomesNetDead( object pl )
{
    pl->move( PATH+"seherbank", M_GO|M_NOCHECK, 0, "wird rausgeworfen",
              "schlafwandelt herein" );
}


static int _guthaben()
{
    mixed wert;

    if ( !(this_player()->Query(KONTO, F_MODE) & SAVE) )
        this_player()->Set( KONTO, SAVE, F_MODE_AS );
    
    wert = this_player()->Query( KONTO, F_VALUE );

    if ( !intp(wert) || wert < 0 || wert > 100000 )
        return this_player()->Set( KONTO, 0, F_VALUE );

    return wert;
}


static int _einzahlen( string str )
{
    int n, guthaben, gebuehr;
    string dummy;
    object ob;

    if ( !this_player() || this_player() != this_interactive() )
        return 0;
    
    if ( !str || !stringp(str) || sscanf( str, "%d %s", n, dummy ) != 2 ||
         dummy != "muenzen ein" )
        return notify_fail( "Syntax: zahle <n> muenzen ein\n" ), 0;

    if ( !IS_SEER(this_player()) )
        return notify_fail( break_string("Werd doch erstmal Seher.", 78, "Der "
                                         "Schalterbeamte grinst ueberheblich: "
                                         ) ), 0;

    if ( n < 1000 )
        return notify_fail( break_string("Mit so kleinen Betraegen fangen "
                                         "wir erst gar nicht an. Du musst "
                                         "schon mindestens 1000 "
                                         "Muenzen einzahlen.", 78,
                                         "Der Schalterbeamte grummelt: ") ), 0;

    guthaben = this_player()->QueryMoney();

    // Man kann sein Konto natuerlich nicht mit dem Geld vom Konto aufladen ;-)
    if ( ob = present( "\ngeldkarte", this_player() ) )
        guthaben -= ob->QueryProp( P_AMOUNT );

    if ( guthaben < n )
        return notify_fail( break_string("Du hast ja gar nicht soviel Geld!",
                                         78, "Der Schalterbeamte faucht "
                                         "Dich an: ") ), 0;

    if ( (guthaben = _guthaben()) == 100000 )
        return notify_fail( break_string( "Deine Seer-Card (TM) ist bereits "
                                          "maximal aufgeladen. Mehr als "
                                          "100.000 Muenzen koennen damit aus "
                                          "technischen Gruenden nicht "
                                          "verwaltet werden.", 78,
                                          "Der Schalterbeamte sagt: " ) ), 0;

    gebuehr = to_int(n / 11.0 + 0.5);
    n -= gebuehr;

    if ( guthaben + n > 100000 )
        return notify_fail( break_string("Tut mir leid, aber so hohe Betraege "
                                         "kann die Geldkarte nicht verwalten.\n"
                                         "Du kannst die Seer-Card (TM) mit "
                                         "maximal 100.000 Muenzen aufladen. "
                                         "Ausgehend von Deinem momentanen "
                                         "Guthaben von " +
                                         to_string(guthaben) + " Muenzen "
                                         "wuerde Dich das (inkl. unserer "
                                         "Verwaltungsgebuehren) " +
                                         to_string(to_int((100000-guthaben) *
                                                          1.1 + 0.5)) +
                                         " Muenzen kosten.\n"
                                         "Um Dir die Rechnerei zu ersparen, "
                                         "kannst Du auch einfach \"lade karte "
                                         "auf\" benutzen, um Deine Geldkarte "
                                         "maximal aufzuladen.", 78,
                                         "Der Schalterbeamte sagt: ",
                                         BS_LEAVE_MY_LFS) ), 0;

    if ( objectp(ob) ){
        write( "Du zeigst Deine Geldkarte kurz vor.\n" );
        // notwendig, da sonst das Geld evtl. von der Karte selber abgebucht
        // wird, falls sie als oberstes im Inv steht
        this_player()->Set( KONTO, 0, F_VALUE );
    }

    this_player()->AddMoney( -(n + gebuehr) );
    ZENTRALBANK->PayIn( gebuehr );
    this_player()->Set( KONTO, guthaben + n, F_VALUE );

    write( break_string( "Okay, Deine Geldkarte hat nun ein Guthaben von "
                         "insgesamt " + to_string(_guthaben()) + " Muenzen.",
                         78, "Der Schalterbeamte sagt: " ) );

    return 1;
}


static int _aufladen( string str )
{
    int n, guthaben, gebuehr;
    string dummy;
    object ob;

    if ( !this_player() || this_player() != this_interactive() )
        return 0;

    if ( !stringp(str) || sscanf( str, "%s auf", dummy ) != 1 ||
         dummy != "karte" && dummy != "geldkarte" && dummy != "card" &&
         dummy != "seercard" && dummy != "seer-card" )
        return notify_fail( "Syntax: lade geldkarte auf\n" ), 0;

    if ( !IS_SEER(this_player()) )
        return notify_fail( break_string("Werd doch erstmal Seher.", 78, "Der "
                                         "Schalterbeamte grinst ueberheblich: "
                                         ) ), 0;

    if ( (guthaben = _guthaben()) == 100000 )
        return notify_fail( break_string( "Deine Seer-Card (TM) ist bereits "
                                          "maximal aufgeladen. Mehr als "
                                          "100.000 Muenzen koennen damit aus "
                                          "technischen Gruenden nicht "
                                          "verwaltet werden.", 78,
                                          "Der Schalterbeamte sagt: " ) ), 0;

    n = this_player()->QueryMoney();

    // Man kann sein Konto natuerlich nicht mit dem Geld vom Konto aufladen ;-)
    if ( ob = present_clone( SEHERKARTE, this_player() ) )
        n -= ob->QueryProp( P_AMOUNT );

    if ( (gebuehr = to_int( (100000 - guthaben) * 1.1 + 0.5 )) < n )
        n = gebuehr;

    gebuehr = to_int( n / 11.0 + 0.5 );
    n -= gebuehr;

    if ( n + gebuehr < 1000 )
        return notify_fail( break_string("Mit so kleinen Betraegen fangen "
                                         "wir erst gar nicht an. Du musst "
                                         "schon mindestens 1000 "
                                         "Muenzen einzahlen.", 78,
                                         "Der Schalterbeamte grummelt: ") ), 0;

    if ( objectp(ob) ){
        write( "Du zeigst Deine Geldkarte kurz vor.\n" );
        // notwendig, da sonst das Geld evtl. von der Karte selber abgebucht
        // wird, falls sie als oberstes im Inv steht
        this_player()->Set( KONTO, 0, F_VALUE );
    }

    this_player()->AddMoney( -(n + gebuehr) );
    ZENTRALBANK->PayIn( gebuehr );
    this_player()->Set( KONTO, guthaben + n, F_VALUE );

    write( break_string( "Du zahlst " + to_string(n+gebuehr) + " Muenzen ein "
                         "und erhoehst Dein Guthaben damit auf " +
                         to_string(_guthaben()) + " Muenzen insgesamt.", 78,
                         "Der Schalterbeamte sagt: " ) );
    
    return 1;
}


static int _kaufen( string str )
{
    if ( !this_interactive() || this_player() != this_interactive() )
        return 0;

    if ( str != "geldkarte" && str != "karte" && str != "card" &&
         str != "seercard" && str != "seer-card" )
        return notify_fail( "Syntax: kaufe geldkarte\n" ), 0;

    if ( !IS_SEER(this_player()) )
        return notify_fail( break_string("Werd doch erstmal Seher.", 78, "Der "
                                         "Schalterbeamte grinst ueberheblich: "
                                         ) ), 0;

    if ( this_player()->QueryMoney() < 10000 )
        return notify_fail( break_string("Du hast nicht genug Geld bei!",
                                         78, "Der Schalterbeamte faucht "
                                         "Dich an: ") ), 0;

    object ob = clone_object( SEHERKARTE ); 
    this_player()->AddMoney(-10000);
    ZENTRALBANK->PayIn(10000);
    
    if ( ob->move( this_player(), M_GIVE ) != 1 ){
        write( "Der Schalterbeamte grummelt: Du kannst die Geldkarte nicht "
               "mehr tragen.\nDer Schalterbeamte wirft Dir Deine Seer-Card "
               "(TM) veraechtlich vor die Fuesse.\n" );
        // trotzdem einmal in's Inv des Spielers bewegen, damit die Karte
        // auf seinen Namen registriert wird
        ob->move( this_player(), M_GIVE|M_NOCHECK );
        // dann auf den Boden legen
        ob->move( this_object(), M_PUT );
    }
    else
        write( "Der Beamte haendigt Dir eine Seer-Card (TM) aus.\n" );

    write( break_string( "Der Schalterbeamte macht Dich noch darauf "
      "aufmerksam, dass Deine Seer-Card (TM) immer erst dann als "
      "Zahlungsmittel in Anspruch genommen wird, wenn Deine in Boersen oder "
      "in klingender Muenze mitgefuehrte Barschaft aufgebraucht sein "
      "sollte - egal, was davon sich oben im Inventar befindet.\n"
      "Was immer das auch heissen soll.", 78, 0, BS_LEAVE_MY_LFS ) );

    this_player()->Set( KONTO, SAVE, F_MODE_AS );

    return 1;
}
