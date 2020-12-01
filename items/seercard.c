// MorgenGrauen MUDlib
//
// obj/seercard.c -- bargeldloser Zahlungsverkehr auf Guthabenbasis
//

#pragma strict_types

inherit "/std/thing";

#include <defines.h>
#include <properties.h>
#include <language.h>
#include <unit.h>
#include <moving.h>
#include <wizlevels.h>
#include <money.h>

#define KONTO "seercard_kontostand"
#define BESITZER_GUELTIG \
    (environment() && query_once_interactive(environment()) && \
    getuid(environment()) == owner)

// zum debuggen und extra public
public string debugmode;
public string __set_debug(string recv) {return debugmode=recv;}
#include <living/comm.h>
#define ZDEBUG(x) if (stringp(debugmode) && find_player(debugmode)) \
  find_player(debugmode)->ReceiveMsg(x,MT_DEBUG,0,object_name()+": ",ME)
//#define ZDEBUG(x)

private string owner;

static int _query_amount()
{
    int res;

    // Wenn die Karte im Inv des Besitzers ist, Kontostand liefern
    if ( BESITZER_GUELTIG && IS_SEER(environment()) ){
        res = ({int})environment()->Query(KONTO);

        // ... allerdings nur, wenn er gueltig ist ;-)
        if ( !intp(res) || res < 0 || res > 100000 )
            return environment()->Set( KONTO, 0 ), 0;
        else
            return res;
    }

    // Fuer Fremde ist die Karte wertlos (bis auf ihren Materialwert)
    return 0;
}

static string _feld()
{
    if ( !BESITZER_GUELTIG )
        return break_string( "Das Feld schimmert aus jeder Richtung anders - "
                             "aber mehr kannst Du nicht erkennen.", 78 );

    return "Du kannst in dem Schimmern ein paar Ziffern erkennen.\n";
}


static string _bild()
{
    if ( this_player() && getuid(this_player()) == owner )
        return break_string( "Du findest, dass das Bild ganz schlecht "
                             "getroffen ist. Dass man selber aber auch immer "
                             "so unvorteilhaft abgebildet wird!", 78 );

    return break_string( "Du findest, dass " + capitalize(owner) + " sehr gut "
                         "getroffen ist. Anscheinend wirst nur Du immer so "
                         "unvorteilhaft abgebildet.", 78 );
}

// Ueber das Argument <silent> kann nun gesteuert werden, ob man die 
// zusaetzliche Meldung ausgeben will oder nicht. Dies ist jetzt nur beim
// Untersuchen der Karte der Fall, der Befehl "guthaben" gibt wie frueher
// nur das Guthaben aus.
private string _guthaben(int silent)
{
    int i;

    if ( !BESITZER_GUELTIG )
        return "Du kannst im Schimmern nichts erkennen.\n";

    if ( !(i = _query_amount()) )
        return "Welches Guthaben? :-)\n";

    string msg =  
      "Dein Guthaben betraegt " + ((i > 1) ? to_string(i) : "eine") +
      " Muenze" + ((i > 1) ? "n" : "") + ".\n";
    if ( !silent ) 
      msg += 
        "Du erinnerst Dich an die Worte des Schalterbeamten, dass Deine "
        "Seer-Card (TM) immer erst dann belastet wird, wenn Du Dein Bargeld "
        "komplett verpulvert hast.";
    return break_string(msg, 78, 0, BS_LEAVE_MY_LFS);
}

// Wrapper fuer das Detail "guthaben", um das neue Argument angeben zu 
// koennen
static string _guthaben_det(string key) {
  return _guthaben(0);
}

protected void create()
{
    if ( !clonep(ME) ){
        set_next_reset(-1);
        return;
    }

    ::create();

    SetProp( P_LOG_FILE, "report/tiamak.rep" );
    SetProp( P_NAME, "Geldkarte" );
    SetProp( P_INFO,
             break_string( "So sehr Du Dich auch anstrengst, Du kommst "
                           "einfach nicht hinter das Geheimnis dieser "
                           "Karte. Du weisst nur eines mit Sicherheit: "
                           "es muss etwas mit Magie zu tun haben.", 78 ) );
    SetProp( P_GENDER, FEMALE );
    SetProp( P_MATERIAL, ([ MAT_PEAR_WOOD: 80, MAT_MISC_MAGIC: 20 ]) );
    SetProp( P_WEIGHT, 50 );
    SetProp( P_VALUE, 1000 );
    SetProp( P_NOBUY, 1 );

    AddSounds( SENSE_DEFAULT,
               break_string( "Du kannst nichts hoeren. Es sind anscheinend "
                             "auch keine losen Teile vorhanden.", 78 ) );

    AddSmells( SENSE_DEFAULT, "Auch bargeldlose Zahlungsmittel stinken "
               "nicht. ;-)\n" );

    AddDetail( "sicherheit", "So etwas bietet diese Karte nicht!\n" );
    AddDetail( "geheimnis", "Es lautet \"guthaben\". Aber verrate es "
               "nicht weiter!\nAngeblich kannst Du das Guthaben sogar direkt "
               "wieder abheben.\n" );
    AddDetail( "magie", "Wenn man sie so einfach untersuchen koennte, "
               "waere sie nicht halb so\ngeheimnisvoll.\n" );
    AddDetail( "rechteck",
               break_string( "An dieser Karte ist alles rechteckig: das Bild, "
                             "das merkwuerdige Feld und die Karte selber.",
               78) );
    AddDetail( ({ "holz", "birnbaumholz" }), "Es handelt sich um das "
               "mindestens genauso seltene wie magische Birnbaumholz.\n" );
    AddDetail( ({ "schriftzug", "schrift" }), "Nur wo \"Seer-Card\" drauf "
               "steht, ist auch wirklich eine Seer-Card drin.\n" );
    AddDetail( ({ "buchstabe", "buchstaben" }), "Sie bilden den "
               "unvermeidlichen Schriftzug \"Seer-Card (TM)\".\n" );
    AddDetail( "drittel", "Na unten halt.\n" );
    AddDetail( ({ "seite", "seiten" }), "Eine von beiden.\n" );
    AddDetail( ({ "rechte seite", "rechts" }), "Das ist die eine.\n" );
    AddDetail( ({ "linke seite", "links" }), "Das ist die andere.\n" );
    AddDetail( "rueckseite", "Die Rueckseite ist total leer und "
               "langweilig.\n" );
    AddDetail( ({ "zahl", "zahlen", "ziffern", "ziffer" }), "Vielleicht sollen "
               "sie Dein Guthaben darstellen?\n" );
    AddDetail( ({ "feld", "schimmern", "richtung" }), #'_feld/*'*/ );
    AddDetail( "bild", #'_bild/*'*/ );
    AddDetail( "guthaben", #'_guthaben_det/*'*/ );
    AddDetail( ({ "muenze", "muenzen" }), "Tipp mal \"guthaben\" ein.\n" );

    AddCmd( "guthaben", "_kontostand" );
    AddCmd( ({ "heb", "hebe" }), "_abheben" );

    AddId( ({ SEHERKARTEID, "geldkarte", "seercard", "seer-card",
                  "karte", "card" }) );
}


static int _kontostand( string str )
{
    if ( !this_player() || environment(this_object()) != this_player() )
        return 0;

    if ( !str || str == "" ){
        write( _guthaben(1) );
        return 1;
    }

    return 0;
}


// Abfrage des Kartenbesitzers von aussen
public string query_owner()
{
    return owner;
}


// Lokale Property-Methoden

static string _query_long()
{
    string str;

    str = "Eigentlich ist die Seer-Card ziemlich schlicht gehalten. Sie ist "
        "als ein etwa daumendickes Rechteck aus dunklem Holz geformt. "
        "Auf der rechten Seite ist ein Bild von " +
        ((this_player() && getuid(this_player()) == owner) ? "Dir" :
         capitalize(owner)) + " aufgemalt. Links "
        "daneben befindet sich ein schmucker Schriftzug in grossen silbernen "
        "Buchstaben. Im unteren Drittel der Karte befindet sich ein dezent "
        "schimmerndes rechteckiges Feld.";

    return break_string( str, 78, 0, BS_LEAVE_MY_LFS );
}


static string _query_short()
{
    if ( !owner || !sizeof(owner) )
        return "Eine Geldkarte";

    if ( this_player() && getuid(this_player()) == owner )
        return "Deine Seer-Card (TM)";

    return "Die Geldkarte von " + capitalize(owner);
}


static string _query_autoloadobj()
{
    // Die Karte ist nur fuer den Eigentuemer autoload - auch, wenn sie in
    // einem Paket o.ae. steckt
    foreach (object env: all_environment())
    {
      if (getuid(env) == owner)
          return owner;
    }
    return 0;
}


static string _set_autoloadobj( string wert )
{
    // Darf nach Personalisierung nicht mehr geaendert werden.
    if ( !owner && stringp(wert) )
    {

        if ( member( ({ 'x', 'z', 's' }), wert[<1] ) != -1 ){
            AddAdjective( wert+"'" );
            AddAdjective( wert );
        }

        if ( wert[<1] != 's' )
            AddAdjective( wert+"s" );

        return owner = wert;
    }
    return 0;
}


static string _query_keep_on_sell()
{
    // Der Besitzer der Karte 'behaelt' sie automatisch
    // Teil 2 der Abfrage, falls die Karte in einem Paket o.ae. steckt
    if ( BESITZER_GUELTIG || this_player() && getuid(this_player()) == owner )
        return owner;

    // Bei den anderen je nach Wunsch
    return Query(P_KEEP_ON_SELL);
}


// Man kann aus dem Guthaben auch wieder klingende Muenzen machen.
// Wenn es denn sein muss.
static int _abheben( string str )
{
    int summe, guthaben;
    string dummy;
    
    if ( !this_player() || environment(this_object()) != this_player() )
        return 0;

    if ( str == "1 muenze ab" )
        summe = 1;
    else
        if ( !stringp(str) ||
             sscanf( str, "%d muenzen ab%s", summe, dummy ) != 2 ||
             summe < 1 || dummy != "" ){
            _notify_fail( "Wieviele Muenzen moechtest Du denn abheben?\n" );
            return 0;
        }

    if ( (guthaben = _query_amount()) < summe ){
        _notify_fail( "So hoch ist Dein Guthaben aber nicht!\n" );
        return 0;
    }

    if ( ({int})this_player()->AddMoney(summe) != 1 ){
        write( "Soviel Geld koenntest Du gar nicht mehr tragen.\n" );
        return 1;
    }

    environment()->Set( KONTO, guthaben - summe );

    write( break_string( "Du schuettelst Deine Seer-Card (TM) vorsichtig "
                         "hin und her und murmelst ein paar geheime Worte.\n"
                         "Ploetzlich macht es *PLOPP* und Du haeltst Geld "
                         "in den Haenden!", 78, 0, BS_LEAVE_MY_LFS ) );
    return 1;
}

protected void NotifyMove(object dest, object oldenv, int method)
{
    ::NotifyMove(dest,oldenv,method);

    // Besitzer der Karte setzen, falls noch keiner existiert
    if ( !owner && environment() &&
         query_once_interactive(environment()) )
    {
         owner = getuid(environment());

        if ( member( ({ 'x', 'z', 's' }), owner[<1] ) != -1 ){
            AddAdjective( owner );
            AddAdjective( owner+"'" );
        }
        if ( owner[<1] != 's' )
            AddAdjective( owner+"s" );
    }
}

varargs int id(string str, int lvl)
{
  if (str==SEHERKARTEID_AKTIV
      && BESITZER_GUELTIG)
      return 1;
  return ::id(str, lvl);
}

public void MergeMoney(object geld)
{
  if (geld && previous_object() == geld
      && load_name(geld) == GELD
      && BESITZER_GUELTIG)
  {
    int fremdamount = ({int})geld->QueryProp(P_AMOUNT);
    int meinamount = _query_amount();
    ZDEBUG(sprintf("MergeMoney: meinamount: %d, fremdamount: %d\n",
          meinamount,fremdamount));
    if (meinamount > 0 && fremdamount < 0)
    {
      int amount_to_merge = min(meinamount, abs(fremdamount));
      environment()->Set( KONTO, meinamount - amount_to_merge, F_VALUE );
      geld->SetProp(P_AMOUNT, fremdamount + amount_to_merge);
    }
    ZDEBUG(sprintf("MergeMoney: Final: meinamount: %d, fremdamount: %d\n",
          _query_amount(),({int})geld->QueryProp(P_AMOUNT)));
  }
}

// Fuer die Zuordnung durch das Hoerrohr etc.
public string GetOwner()
{
    return "Tiamak";
}
