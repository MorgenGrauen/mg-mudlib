#include <moving.h>
#include <defines.h>
#include <rooms.h>
#include <properties.h>
#include "queue.h"
#include "haus.h"

inherit "/std/room";

protected void create()
{
    object q;
    
    ::create();
    
    SetProp( P_INDOORS, 1 );
    SetProp( P_LIGHT, 1 );
    SetProp( P_INT_SHORT, "In der MorgenGrauen-Bank" );
    SetProp( P_INT_LONG,
             break_string( "Dunkle Schatten erfuellen diesen Raum, nur "
                           "schemenhaft kannst Du einige Schlangen sowie einen "
                           "Informationsstand erkennen. Du beginnst zu "
                           "begreifen:\nDu befindest Dich in der "
                           "MorgenGrauen-Bank, der 'Bank in Ihrem Ruecken'.\n"
                           "Wenn Du Seher oder Seherin bist, kannst Du hier "
                           "einen Bausparvertrag fuer ein Seherhaus erwerben, "
                           "Raten fuer einen laufenden Bausparvertrag "
                           "einzahlen und ein abbezahltes Haus abholen. "
                           "Ausserdem kannst Du hier Geldkarten erwerben und "
                           "Dein Konto auffuellen. Dazu musst Du Dich nur "
                           "zu den entsprechenden Schaltern begeben. "
                           "Nach Sueden kannst Du die Bank wieder "
                           "verlassen.\n", 78, 0, BS_LEAVE_MY_LFS ) );
    
    AddDetail( ({ "wand", "waende" }),
               "An den Waenden treiben die Schatten unheimliche "
               "Spielchen.\n" );
    AddDetail( ({ "boden" }),
               "Der Boden ist schwarz wie die Nacht.\n" );
    AddDetail( ({ "decke" }),
               "Schleimige Faeden scheinen von der dunklen Decke "
               "zu haengen.\n" );
    AddDetail( ({ "schatten" }),
               "Die Schatten erfuellen den ganzen Raum und lassen "
               "Dich nur schemenhafte Details\nerblicken.\n" );
    AddDetail( ({ "details", "schemen" }),
               "Ein Informationsstand sowie vier Schlangen von "
               "Leuten sind als Schemen in\nden Schatten erkennbar.\n" );
    AddDetail( ({ "schleim", "faeden", "schleimfaeden" }),
               "Die Schleimfaeden haengen von der Decke und drohen Dir auf "
               "den Kopf zu\ntropfen. Ein Froesteln schuettelt Dich.\n" );
    AddDetail( ({ "schalter" }),
               break_string( "Vor den Schaltern stehen lange Schlangen von "
                             "Leuten, die auch gerne ein Haus haetten. Es "
                             "gibt einen Antragsschalter, einen "
                             "Ratenzahlungsschalter, einen Ausgabeschalter und "
                             "einen Geldkartenschalter. Vor jedem Schalter "
                             "steht eine Schlange von mehr oder weniger "
                             "geduldig wartenden Leuten. Du wirst Dich wohl "
                             "anstellen muessen...", 78 ) );
    AddDetail( ({ "stand", "informationsstand" }),
               "Hier kannst Du fuer den unverschaemten Preis von 10000 "
               "Muenzen die Kommen-\ntare zum Bausparvertrag erwerben. "
               "Das geht mit 'kaufe kommentar', ist aber\n"
               "angesichts des hohen Preises fast nicht anzuraten.\n" );
    AddDetail( ({ "ausgabeschalter", "hausausgabeschalter", "hausausgabe" }),
               "Wenn Du alle Raten fuer Dein Haus eingezahlt hast, kannst "
               "Du es hier ab-\nholen und mitnehmen. Dafuer musst Du Dich "
               "an die Hausausgabeschlange an-\nstellen.\n" );
    AddDetail( ({ "ratenschalter", "ratenzahlungsschalter" }),
               "Hier kannst Du die Raten fuer Dein Seherhaus einzahlen. "
               "Du wirst Dich\naber erst an die Zahlungsschlange anstellen "
               "muessen.\n" );
    AddDetail( ({ "antragsschalter" }),
               "An diesem Schalter kannst Du einen Bausparvertrag beantragen. "
               "Stell Dich\ndazu an die Antragsschlange an.\n" );
    AddDetail( ({ "geldkartenschalter", "kartenschalter" }),
               break_string( "Am Geldkartenschalter kannst Du die beruehmte "
                             "Seer-Card (TM) erwerben und Dein Guthabenkonto "
                             "auffuellen. Allerdings haben das auch noch "
                             "andere Leute vor - Du wirst Dich also brav "
                             "hinten an die Geldkartenschlange anstellen "
                             "muessen.", 78 ) );

    q=AddItem( PATH+"queue", REFRESH_NONE,
               ([ P_SHORT : "Eine Schlange vor dem Geldkartenschalter",
                P_LONG : "Die Seer-Card (TM) muss ja wirklich begehrt sein. "
                "Zumindest stehen mehrere\nLeute an der Schlange zum "
                "Geldkartenschalter an.\n"
                "Syntax: 'stell an kartenschlange an'.\n",
                P_INT_SHORT : "In der Schlange vor dem Geldkartenschalter",
                P_INT_LONG : "Du steht in der langen Schlange vor dem "
                "Geldkartenschalter. Wenigstens geht\nes halbwegs flott "
                "vorwaerts.\n",
                Q_LENGTH : 7,
                Q_CYCLE_TIME : 8,
                Q_SUSPEND : 1,
                Q_DEST : PATH+"sc_schalter"
                ]) );

    q->AddId( ({"geldkartenschlange", "kartenschlange"}) );

    q=AddItem( PATH+"queue", REFRESH_NONE,
               ([ P_SHORT : "Eine Schlange vor dem Hausausgabeschalter",
                P_LONG  : "In dieser Schlange stehen Leute vor dem "
                "Hausausgabeschalter an.\n"
                +"Syntax: 'stell an ausgabeschlange an'.\n",
                P_INT_SHORT : "In der Schlange vor dem Hausausgabeschalter",
                P_INT_LONG  : "Du wartest vor dem Hausausgabeschalter.\n",
                Q_SUSPEND : 1,
                Q_DEST : PATH+"sb_ausgabe"
                ]) );
    
    q->AddId( ({"hausausgabeschlange", "ausgabeschlange" }) );

    q=AddItem( PATH+"queue", REFRESH_NONE,
               ([ P_SHORT : "Eine Schlange vor dem Schalter zur Ratenzahlung",
                P_LONG  : "Wenn Du eine Rate bezahlen willst, solltest Du "
                "Dich an dieser Schlange\nanstellen.\n"
                +"Syntax: 'stell an ratenschlange an'.\n",
                P_INT_SHORT : "In der Schlange vor dem Schalter zur "
                "Ratenzahlung",
                P_INT_LONG  : "Du wunderst Dich ein wenig ueber die "
                "Massen potentieller Ratenzahler.\n",
                Q_LENGTH : 8,
                Q_CYCLE_TIME : 10,
                Q_DEST : PATH+"sb_einzahlung",
                Q_SUSPEND : 1
                ]) );
    
    q->AddId( ({"ratenschlange", "ratenzahlungsschlange",
                    "zahlungsschlange" }) );

    q=AddItem( PATH+"queue", REFRESH_NONE,
               ([ P_SHORT : "Eine Schlange vor dem Antragsschalter",
                P_LONG  : "Die Leute in dieser Schlange wollen alle ein "
                "eigenes Heim.\n"
                +"Syntax: 'stell an antragsschlange an'.\n",
                P_INT_SHORT : "In der Schlange vor dem Antragsschalter",
                P_INT_LONG  : "Du wartest inmitten anderer Antragssteller "
                "darauf, endlich Deinen\nBausparvertrag beantragen zu "
                "duerfen.\n",
                Q_LENGTH : 5,
                Q_CYCLE_TIME : 20,
                Q_SUSPEND : 1,
                Q_DEST : PATH+"sb_antrag"
                ]) );
    
    q->AddId( ({"antragsschlange"}) );

    AddExit( "sueden", "/d/ebene/room/dra_str1" );

    AddCmd( ({ "kauf", "kaufe" }), "kaufen" );
}

static int
kaufen(string str)
{
  object tp;

  if (!str || str != "kommentar") {
    notify_fail( "Was willst Du kaufen? Hier gibt es nur Kommentare!\n" );
    return 0;
  }

  tp = this_player();
  if (tp->QueryMoney()<10000)
    write( "Du hast nicht genug Geld dabei!\n" );
  else {
    clone_object(PATH+"kommentar")->move(tp, M_NOCHECK);
    tp->AddMoney(-10000);
    write( "Du zahlst 10000 Muenzen und erhaeltst dafuer einen Kommentar.\n" );
  }
  return 1;
}
