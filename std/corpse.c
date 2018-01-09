// MorgenGrauen MUDlib               
//
// corpse.c -- corpse standard object
//
// $Id: corpse.c 9391 2015-12-07 22:28:53Z Arathorn $

// A corpse.
// 
// Ekeltexte by Boing
//
// This is a decaying corpse. It is created automatically
// when a player or monster dies.
//
// Erweiterung Raschaua:
// - Kopieren von P_CLASS
// - Teilweises aufessen
#pragma strict_types
#pragma save_types
#pragma range_check
#pragma pedantic

//#define NEED_PROTOTYPES

inherit "std/container";

#include <properties.h>
#include <language.h>
#include <defines.h>
#include <moving.h>
#include <wizlevels.h>
#include <living/life.h>
#include "/secure/scoremaster.h"

#define CORPSE_OBJ "/std/corpse.c"
#define PILE_OBJ "/std/pile.c"

nosave int _decay = 4;
nosave string _name = "Unbekannt";
nosave object moerder;
private nosave int gespottet = 0;
nosave int nahrung_gesamt = 1;
nosave int nahrung_aktuell = 1;
// Spielerleiche?
private nosave status spielerleiche;

void Identify( object ob );
void do_decay();
int QueryDecay();
static int mampf( string str );
static int spott(string str);
void ChannelMessageJeer( mixed sender, string text, int flag );
object _channel( object ob );
void transform_into_pile();

void create()
{
  if (object_name(ME) == __FILE__[0..<3]) {
    set_next_reset(-1);
  }
  ::create();
  if( clonep(this_object()) )
  {
    AddId(({ "leiche","\nleiche","rest","ueberrest"}));
    SetProp( P_GENDER, FEMALE );
    SetProp( P_NAME, "Leiche" );
    SetProp( P_MATERIAL, ([ MAT_MISC_DEAD: 100 ]) );
    SetProp( P_CORPSE_DECAY_TIME, 30 );
    SetProp(P_NO_SCORE,1);
    AddCmd(({"iss","verspeise"}),"mampf");
    AddCmd("spotte","spott");
    SetProp(P_ARTICLE, 1);
  }
  else
    SetProp( P_ARTICLE, 0 );
}

/* Damit die Leiche nicht voll wird... */
int MayAddWeight( int weight ) { return 0; }
int MayAddObject( object ob ) { return 1; }


/* Humni, 2004-06-16: Entstatict */
string _query_leichen_name()
{   return _name;   }

/* Uebernehme den Namen von ob */
void Identify( object ob )
{
    closure cl;
    int i;
    string info;

    spielerleiche = query_once_interactive(ob);
    cl=symbol_function("QueryProp", ob);
    _name = (string) ob->name(WESSEN,0);
    SetProp( P_SHORT, "Die Leiche "+ _name );
    SetProp( P_LONG, "Du siehst die sterblichen Ueberreste "+ _name + ".\n" );
    _decay = 4;
    SetProp( P_ORIG_NAME, to_string(ob->name(RAW))      );
    SetProp( P_PILE_NAME, to_string(ob->name(WEM))      );
    AddId( "\nleiche "+ QueryProp(P_PILE_NAME)          );
    SetProp( P_KILLER,        (object)   funcall(cl, P_KILLER)        );
    SetProp( P_ATTRIBUTES,    (mapping)  funcall(cl, P_ATTRIBUTES)    );
    SetProp( P_LAST_DAMTYPES, (string *) funcall(cl, P_LAST_DAMTYPES) );
    SetProp( P_XP,     to_int(funcall(cl, P_XP))        );
    SetProp( P_SIZE,   to_int(funcall(cl, P_SIZE))      );
    SetProp( P_LEVEL,  to_int(funcall(cl, P_LEVEL))     );
    SetProp( P_MAX_HP, to_int(funcall(cl, P_MAX_HP))    );
    SetProp( P_RACE,   to_string(funcall(cl, P_RACE))   );
    SetProp( P_CLASS,  (string *)(funcall(cl, P_CLASS)) );
    SetProp( P_HANDS,  (mixed *) funcall(cl, P_HANDS)   );
    SetProp( P_WEIGHT, (mixed *) funcall(cl, P_WEIGHT)  );
    SetProp( P_ALIGN,  to_int(funcall(cl, P_ALIGN))     );
    SetProp( P_ORIG_FILE_NAME, object_name(ob)          );
    SetProp( P_HEAL,   to_int(funcall(cl, P_HEAL))      );
    
    // Saettigung der Leiche berechnen
    // P_WEIGHT und P_SIZE werden verwendet, Mittel bilden
    // Ein Saettigungspunkt fuer (konservative Rechnung):
    //           5 kg
    //          12 cm
    // Beispiele:
    //          Mensch, 1.80 m gross, 75 kg schwer:
    //          180/12 + 75/5 = 15 + 15 = 30
    //          Kuh, 1.50 m gross, 500 kg schwer:
    //          150/12 + 500/5 = 12 + 100 = 112
    //          Drache, 5 m gross, 1500 kg schwer:
    //          500/12 + 1500/5 = 41 + 300 = 341
    nahrung_gesamt = QueryProp(P_WEIGHT)/5000 + QueryProp(P_SIZE)/12;
    if (nahrung_gesamt <= 0)
      nahrung_gesamt = 1;
    nahrung_aktuell = nahrung_gesamt;

    if ( intp(i = funcall(cl, P_CORPSE_DECAY_TIME)) && i > 2 )
        SetProp( P_CORPSE_DECAY_TIME, i );

    call_out( "do_decay", QueryProp(P_CORPSE_DECAY_TIME) );

    if ( !query_once_interactive(ob) )
        moerder = (object) "/std/corpse"->_channel(ob);
}

public int IsPlayerCorpse() { return spielerleiche; }

public string QueryHealInfo()
{
  string info = "Solltest Du auf die aberwitzige Idee verfallen, diese "
    "Leiche zu essen, ";
  switch( QueryProp(P_HEAL) )
  {
    case -__INT_MAX__..-100:
      info += "wird Dir das erheblichen Schaden zufuegen.";
      break;
    case -99..-11:
      info += "ist das ausgesprochen ungesund fuer Dich.";
      break;
    case -10..-1:
      info += "wird Dir sicher ein wenig uebel werden.";
      break;
    case 0..5:
      info += "wird Dir nichts schlimmes passieren.";
      break;
    case 6..__INT_MAX__:
      info += "koenntest Du sogar ein wenig geheilt werden.";
      break;
  }
  return info;
}

private string knabber_text()
{
  string txt;
  switch (nahrung_aktuell * 4 / nahrung_gesamt) {
  case 4:
    // Leiche noch komplett
    txt = "";
    break;
  case 3:
    // noch maximal 99%
    txt = " mit Knabberspuren uebersaete";
    break;
  case 2:
    // noch maximal 74%
    txt = " angefressene";
    break;
  case 1:
    // noch maximal 49%
    txt = " schon halb aufgefutterte";
    break;
  default:
    // noch maximal 24%
    txt = " total abgenagte";
    break;
  }
  return txt;
}

static string _query_short()
{
  string txt;
  switch(_decay) {
  case 4:
    txt = "Die"+knabber_text()+" Leiche ";
    break;
  case 3:
    txt = "Die bereits ziemlich stinkende"+knabber_text()+" Leiche ";
    break;
  case 2:
    txt = "Die schimmelnde und halbverweste"+knabber_text()+" Leiche ";
    break;
  case 1:
    txt = "Die verfaulten"+(sizeof(knabber_text()) ?
                            "n":"")+" Einzelteile ";
    break;
  }
  txt += _name;
  return txt;
}

void do_decay()
{
    _decay -= 1;
    
    if ( _decay > 0 ) 
        {
          if ( (_decay == 2) && (QueryProp( P_HEAL ) >= -4) )
            SetProp( P_HEAL, -4 );
            call_out( "do_decay", QueryProp(P_CORPSE_DECAY_TIME) );
            return;
        }
   
    transform_into_pile();
    remove();
}

/* Haeh? Das move ins env macht der Master im prepare_destruct() doch eh? */
/*
varargs int remove()
{
    // units bekommen eine Chance zur Vereinigung...
    filter_objects( all_inventory(ME), "move", environment(ME), M_SILENT );
    return ::remove();
}
*/

int QueryDecay() { return _decay; }

static int * _query_nahrung()
{   return ({nahrung_aktuell, nahrung_gesamt});   }

private string mampf_heilung(int wieviel)
{
  int heal;
  string msg;
  heal = QueryProp(P_HEAL);
  // Anteil an Gesamtheilung ausrechnen
  heal = heal * wieviel / nahrung_gesamt;
  if ( ( heal ) < 0 ) {
    // 
    this_player()->do_damage( random(-heal), this_object() );
    msg = "Buah, diese Leiche war zweifellos nicht besonders gesund.\n";
  } else {
    this_player()->heal_self( random(heal) );
    msg = "Hmmm, lecker!\n";
  }
  return msg;
}

static int mampf( string str )
{
    int gegessen;

    _notify_fail("Was moechtest Du essen?\n");
    
    if ( !str || !id(str) )
        return 0;

    if (this_player()->QueryProp(P_GHOST))
    {
	_notify_fail("Das wuerde durch Dich hindurch fallen.\n");
	return 0;
    }

    // Das folgende ist nicht ganz sauber, da die Staerke bei eat_food nicht
    // unbedingt dem Saettigungswert entsprechen muss (wegen FOOD_VALUE).
    gegessen = (int) this_player()->QueryProp(P_MAX_FOOD) -
      (int) this_player()->QueryProp(P_FOOD);
    if (gegessen <= 0) {
      // Spieler ist proppenvoll, Meldung ausgeben
      gegessen = 0;
      write("Du bekommst von der Leiche nicht einen Bissen mehr runter.\n");
    } else if (gegessen >= nahrung_aktuell) {
      // spieler kann die gesamte Leiche essen, also entfernen.
      this_player()->eat_food(nahrung_aktuell);
      // Verdammt. eat_food() kann TP umgebracht haben und im Falle eines NPC
      // ist der dann weg.
      if (objectp(this_player())) {
	write(mampf_heilung(nahrung_aktuell));
	say( sprintf("%s wird von %s voll Hingebung verspeist.\n",
                   capitalize(name(WER)),
                   (string) this_player()->name(WEM)) );
      }
      transform_into_pile();
      remove();
    } else {
      // Auch teilweise Verspeisung ist moeglich, nahrung_aktuell anpassen
      this_player()->eat_food(gegessen);
      if (objectp(this_player())) {
	write(mampf_heilung(gegessen)+"Leider bist Du nicht in der Lage,"
               " alles aufzuessen.\n");
      say( sprintf("%s knabbert an %s herum.\n",
                   (string) this_player()->name(),
                   capitalize(name(WEM)) ) );
      }
      nahrung_aktuell -= gegessen;
    }
    return 1;
}


static int spott( string str )
{
    if ( !str )
        return _notify_fail( "Syntax: spotte <text>\n" ), 0;
    
    if ( !objectp(moerder) )
        return _notify_fail( "Dieses Opfer ist den Spott nicht wert.\n" ), 0;
    
    if ( PL != moerder )
        return _notify_fail( "Du kannst nur Deine eigenen Opfer "
                             +"verspotten!\n" ), 0;

    if ( gespottet )
        return _notify_fail( "Du kannst Dein Opfer nur einmal verspotten!\n" )
            , 0;

    if ( _decay < 4 )
        return _notify_fail( "Da solltest Du schon etwas spontaner sein.\n" )
            , 0;

    str = (string) PL->_unparsed_args();
    
    switch ( str[0] )
        {
        case ':':
            "/std/corpse"->ChannelMessageJeer( PL, str[1..], MSG_EMOTE );
            break;
          
        case ';':
            "/std/corpse"->ChannelMessageJeer( PL, str[1..], MSG_GEMOTE );
            break;
          
        default:
            "/std/corpse"->ChannelMessageJeer( PL, str, MSG_SAY );
            break;
        }
  
  gespottet = 1;
  write( "Du verspottest Dein totes Opfer.\n" );
  
  return 1;
}


void ChannelMessageJeer( mixed sender, string text, int flag )
{
  if (member(inherit_list(previous_object()),CORPSE_OBJ)>-1)
      CHMASTER->send( "Moerder", sender, text, flag );
}


// _channel() --
#define KILL_MESSAGES \
"Jetzt hat mich doch glatt %s erwischt :(", \
"Wie soll das noch enden, %s?", \
"Unglaublich, wie hat %s das geschafft?", \
"RACHE! Bringt %s um!", \
"%s ist staerker als ich dachte.", \
"Wenn ich Dich erwische, %s!", \
"%s hat mich ermordet!", \
"Welche Schmach! %s hat mich besiegt.", \
"Huetet Euch vor %s!", \
"Warum hat mir niemand gegen %s geholfen?", \
"%s, Du Monsterschaender!", \
"Monster aller Regionen, vereinigt Euch gegen %s!", \
"Danke, %s, Du hast mich von dieser schrecklichen, " \
"sterblichen Huelle befreit.", \
"%s? Ich bin dann jetzt weg.", \
"Macht Euch keine Muehe, %s hat mich mal kurz zu " \
"Gevatter Tod geschickt.", \
"Oh nein, %s hat mich schon wieder kaltgemacht!", \
"Sheriff! Verhafte %s!", \
"Hilfe! Diebe! Moerder! Halsabschneider! %s!", \
"Wieder mal an %s gestorben, find ich albern!", \
"%s, probier das bei Anne!", \
"Und wieder ein Drittel Erfahrung weg wegen %s...", \
"Oh waer ich doch ein Spieler, dann duerfte mich "\
"%s nicht mehr toeten!", \
"Wenn Du das noch oefter machst, %s, dann spiele "\
"ich hier nicht mehr!", \
"%s, wieso redet der Kerl hier in Grossbuchstaben "\
"zu mir? CAPS-LOCK kaputt?", \
"%s ist schuld!",\
"So schaff ich's wohl nie, Seher zu werden, %s :(",\
"Musste das WIRKLICH sein, %s?",\
"Wuenscht mir ne kurze Todessequenz, ich hab sie "\
"%s zu verdanken...",\
"Naechstes Mal bring mir lieber Post statt mich "\
"zu ermorden, %s!",\
"%s, wieso sagt der Typ: KOMM MIT MIR, STERBLICHER?",\
"Ich glaub, jetzt kenne ich ALLE Todessequenzen, %s!",\
"Was, %s hat mir WIE VIELE Lebenspunkte abgezogen???",\
"Verdammt, %s wollte nicht sterben...",\
"Tod den Monsterschaendern! Straft %s!",\
"Naechstes Mal werde ICH siegen, %s!",\
"Findest Du das fair, %s?",\
"Das war ein gemeiner Tiefschlag, %s!",\
"Wenn ich mal gross bin, zahl ich Dir das alles "\
"heim, %s!",\
"ALAAAAAARM! %s IST AUSGEBROCHEN!",\
"Ein Koenigreich fuer den, der mir den Kopf von "\
"%s bringt!",\
"Suche Killer, zahle gut, Zielperson %s.",\
"Das sag ich alles Boing, %s!",\
"Das sag ich alles Rumata, %s!",\
"Das sag ich alles Jof, %s!",\
"Das sag ich alles dem Sheriff, %s!",\
"Komm Du mir nur wieder, %s!",\
"Ich fordere Revanche, %s!",\
"Sheriff! Unternimm was gegen %s!",\
"%s, wer hat DICH denn eingeladen?",\
"NAECHSTES MAL NEHM ICH DICH AUCH MIT, %s!",\
"Haetten wir das nicht ausdiskutieren koennen, %s?",\
"Dich merk ich mir, %s!",\
"Na prima, schon wieder dieser dunkle Typ! Und "\
"alles nur wegen %s.",\
"Hallo? Hat jemand meinen Koerper gesehen? Muesste "\
"irgendwo bei %s liegen!",\
"%s, waer jetzt nicht eigentlich Vorlesung?",\
"Lern lieber, statt unschuldige Monster zu er"\
"morden, %s!",\
"Ohne dieses Lag haette mich %s nicht umgebracht!",\
"In spaetestens einer Stunde komme ich wieder, %s!",\
"Na warte %s, beim naechsten Mal trete ich Dir "\
"kraeftig in den Gedan-Bereich!",\
"SCHIEBUNG, der letzte Schlag von %s war nicht "\
"regelgerecht!",\
"Lieber eine H2O-Vergiftung, als von %s ermordet "\
"zu werden!",\
"Die Ehre werd ich %s nicht antun, und hier "\
"herumposaunen, wer mich umgenietet hat!",\
"Is mir doch egal %s, ich glaube an "\
"die Wiedergeburt!",\
"Aber vorsicht, %s isch kuuul maen!",\
"Ok, %s, Du wirst von meiner Erwarteliste gestrichen!",\
"Das naechste Mal sieht man unsere beiden Namen auf [Tod:], %s!",\
"Magier zu mir, hier ist ein Bug: %s ist immer noch nicht tot!",\
"Morgen frueh um halb sechs in der Arena, %s! Ich hab die Wahl der Waffen.",\
"%s ist schlimmer als das Mensaessen. Absolut toedlich.",\
"Mist, meine Vorsicht hat versagt! Und das bei %s!",\
"%s, die Hydra ist mir lieber...",\
"So, meine Ignoriereliste ist schon wieder um einen Namen laenger, %s."

#define KILL_MESSAGES2 \
"Feierabend! Danke, %s!",\
"Toll! GENAU das hab ich jetzt gebraucht! Super, %s! :(",\
"Schon wieder ein Tag versaut wegen %s!",\
"Ihr sagt, ICH sei ein Monster? Ihr kennt %s nicht!!!",\
"Sperrt %s mal in's Polargebiet! Gaebe ein gutes Untier ab...",\
"Freiheit fuer Nicht-Spieler! Vernichtet %s!",\
"Merlin, mach %s zum Magier!",\
"%s ist NICHT zu meiner Beerdigung eingeladen!",\
"Ich hab keine Angst vor Dir, %s!",\
"%s ist doof und stinkt!",\
"Moegest Du im Lag verrotten, %s!",\
"%s wird gleich das Spiel verlassen, um meiner Rache zu entgehen!",\
"Der Todestag von %s wird fuer mich ein Feiertag sein!",\
"%s ist ein Metzelskript, ich hab's genau gesehen!", \
"Waer ich Tanjian, haettest Du das nicht geschafft, %s!", \
"Das naechste Mal komme ich als Kaempfer wieder, %s!", \
"Ich lass' mich aufwerten und werde Zauberer, %s!", \
"Gildenbalance! %s ist viel zu stark!", \
"Jetzt ist mir das Kleingeld zum Tanken ausgegangen, %s...", \
"%s hat ein Zap-Tool!", \
"%s, Du <zensiert>!", \
"%s, das ist doch Hullepanz. (c) Catweazle", \
"Bald bin ich SuperSeher, %s!", \
"Mit Dir 'piel ich nich' mehr, %s, Du hast in mein Foermssen depinkelt!", \
"Verdammt, %s muss mir den Feenring geklaut haben...", \
"Nie ist ein Kleriker in der Naehe, wenn man ihn braucht, %s...", \
"Menno, %s, wo war hier noch mal die Heilstelle?", \
"Hoffentlich werde ich naechstes Mal als Kuh wiedergeboren, %s...", \
"Wenn Du sonst noch Probleme hast, %s, geh' zu Marvin und red' " \
"mit ihm darueber.", \
"Du, %s, Gewalt ist keine Loesung, lass uns darueber diskutieren.", \
"Das naechste Mal zeig' ich Dir meine Mateteesammlung, %s.", \
"%s eunt domum!", \
"%s, ist es so kalt hier oder bin ich das?", \
"Ich war schon wieder zur falschen Zeit am falschen Ort, %s...", \
"Danke, %s. Im Namen des Mondes werde ich Dich bestrafen!", \
"Das war mal wieder ein Schuss in den Ofen, %s...", \
"Kannst Du das mit Deinem Gewissen vereinbaren, %s?", \
"Und Kandri sprach: Mein ist die Rache, %s!", \
"%s, Du haettest meine Leiche nicht auch noch schaenden muessen...", \
"%s, ich kann so nicht arbeiten!", \
"%s, ich geb' Dir mal 2 Muenzen, dann kannst Du Deine Sorgen " \
"jemand anderem erzaehlen...", \
"Manchmal verspeist Du den Baer, %s, manchmal verspeist " \
"der Baer Dich...", \
"Das naechste Mal betrittst _Du_ die Welt des Schmerzes, %s!", \
"Wenn ich wiederkomme, %s, werde ich erst Dich toeten, dann " \
"Deinen Partner, dann Deine Brueder und Schwestern und jeden " \
"der Dich kennt!", \
"Oh nein, %s! Schon wieder diese ewig lange Todessequenz mit der " \
"Drachenschule!", \
"%s, wo bekomme ich jetzt einen neuen Koerper her?", \
"Lass Dir meine Leiche schmecken, %s.", \
"Haeh?", \
"Ja, aber wieso das denn? Och menno, %s!", \
"So ein Mist, %s, ich hatte 'nen Disconnect...", \
"*winkewinke*, %s!", \
"Heute ist nicht alle Tage, ich komm' wieder, keine Frage.", \
"Lass das, %s!", \
"Niemand! Niemand nennt mich eine feige Sau, %s!", \
"Da hab' ich jetzt wohl verloren, %s...", \
"%s, ich kann nicht metzeln, ich kann nur sterben...", \
"Hattest Du ueberhaupt einen Termin, %s?", \
"Du warst gar nicht an der Reihe, %s!", \
"Aetsch, %s, der Kill zaehlt nicht!", \
"Jetzt noch mal mit Gefuehl, %s.", \
"Tschuess erstmal, %s.", \
"Ich weiss nicht ob Du es gemerkt hast, %s, aber " \
"ich bin schon tot...", \
"%s, ich haette nicht gedacht, dass ich noch mal sterbe " \
"bevor Taramis angeschlossen wird...", \
"Ich wollte sowieso grade gehen, %s.", \
"Shit happens, %s.", \
"Dazu faellt mir jetzt nix mehr ein, %s."

#define KILL_MESSAGES3 \
"Was heisst denn \"Dein Teddy knuddelt Dich ein letztes Mal\", %s?", \
"Ich bin beeindruckt, %s.", \
"Ok, Dein Gesicht merke ich mir, %s!", \
"Kaum da, haut mich %s schon wieder tot... *grummel*", \
"Ich werde Dich verfolgen, %s! Ich werde in Deine Traeume eindringen " \
"und Dich in den Wahnsinn treiben! Du wirst diesen Tag noch verfluchen!", \
"Ich geh ja schon, %s.", \
"Auf den ersten Blick sieht %s gar nicht so gefaehrlich aus.", \
"Von %s besiegt - wie peinlich.", \
"Mist. Einmal nicht aufgepasst, schon wird man von %s ermordet.", \
"Das war ein Glueckstreffer, %s!", \
"Hah, das ist doch nur eine Fleischwunde! Gibst Du etwa schon auf, %s?", \
"%s macht gerade eine destruktive Phase durch.", \
"Zum Glueck ist das ja nur ein Spiel, %s. Sonst waere ich jetzt wirklich " \
"boese.", \
"Begrabe mich wenigstens anstaendig, %s.", \
"Hm, die Flatrate brauch' ich dann wohl doch nicht, %s.", \
"Na warte, %s! Zur Strafe werde ich gleich das Mud crashen!", \
"Ich sollte nicht mehr soviel mudden. Dann muesste ich mich auch nicht mehr " \
"ueber %s aergern.", \
"Auf am Boden Liegende einzuschlagen! Schaem Dich, %s!", \
"Einigen wir uns auf unentschieden, %s?", \
"Ich sagte doch \"Ich ergebe mich!\", %s.", \
"Aeh, %s, koennte das unter uns bleiben? Ich habe schliesslich einen " \
"Ruf zu verlieren...", \
"Du bist vielleicht staerker, %s, aber dafuer bin ich schoener.", \
"Die Rechnung fuer die Behandlung meiner Minderwertigkeitskomplexe geht " \
"an %s.", \
"Ab und zu muss man %s ja gewinnen lassen, sonst gibt es wieder Traenen.", \
"Bis eben war mir %s noch sympathisch.", \
"Geht's Dir jetzt besser, %s?", \
"Macht kaputt, was mich kaputt macht! Toetet %s!", \
"Beim naechsten Mal mach' ich es Dir nicht mehr so einfach, %s!", \
"Weisst Du, %s, ich habe nur verloren, weil Du... aeh, weil ich... " \
"aeh... weil Du gewonnen hast.", \
"Jo, immer feste druff, %s. Ist ja nur ein dummer NPC. *grummel*", \
"Ich muss sagen, %s kann feste schlagen.", \
"Also %s! Kommst hier rein, haust mich um und tust dann so, als sei nichts " \
"gewesen...", \
"In welcher Gilde konnte man nochmal \"befriede\" lernen, %s?", \
"Tja, %s, aus diesem Kampf bist Du wohl nur als Vorletzter hervorgegangen" \
"...", \
"Vorsicht, %s greift immer von hinten an!", \
"Was meint %s mit \"Sparringspartner\"?", \
"Ich habe mir bei Dir nur deshalb keine Muehe gegeben, %s, weil Du eh nicht " \
"stupst.", \
"Huch, ich bin ja schon wieder auf -Moerder!", \
"Ich spuck' Dir in's Auge und blende Dich, %s!", \
"Huch, wo seid ihr alle hin?", \
"Wer war das?", \
"Moegest Du in interessanten Zeiten leben, %s.", \
"Ich sehe was, was Du nicht siehst, %s, und es ist... schwarz.", \
"Hey Maedels! Ratet, wen ich gerade getroffen habe!", \
"%s?", \
"Noch einmal sowas, %s, und ich lasse Deine Gilde abwerten. Ich kann das!", \
"Was bedeutet 'Jemand beschwoert Dein Bild herauf.'? Warst Du das, %s?"

private string *moerder_msgs = ({KILL_MESSAGES, KILL_MESSAGES2, KILL_MESSAGES3,
"Sterben ist schoen, Sterben ist toll - Ach waer ich wie ein Kaempfer voll!",
"Lauschige Nacht - ich geh mir die Sterne anschauen. Bis spaeter!",
"Echt %s, das war 0815, keine Kreativitaet beim Toeten!",
"%s, Du kannst ja gar ni... Verdammt!",
"Kaempfer, Zauberer, Tanjian, alles Luschen! Ich werde Abenteurer, da ist "
    "wenigstens Thrill!", 
"Toeten, Toeten, Toeten. Ist das alles, was Du kannst, %s?",
"Ach geh doch Bluemchen pfluecken %s!", 
"Mein Tod war KEIN FP %s... HA!",
"Lars gibt mir nen Whisky, wenn ich komme. Und Dir?",
"Geh lernen, Depp!",
"Auf der naechsten Mud-Party haut Dich mein Magier zurueck!",
"Lass gut sein, tat eh nicht weh!", 
"Ich lass Dich von Zook abhaengen, %s!",
"Koennen wir naechstes Mal nicht lieber einen trinken gehen, %s?",
"Verdammt, meine neue Gilde ist ja noch gar nicht angeschlossen!",
"Yohoho und ne Buddel voll Rum!",
"Beim Kartenspiel haettest Du keine Chance gehabt %s!",
"%s, Du hast echt keine Ahnung von stilvollem Metzeln...",
"Was muss ich noch tun fuer Deinen Aufstieg, %s? *seufz*",
"EK-Poser!",
"Leistung ist was anderes, %s!",
"Lass mich leeeeeeb... Arrrrggggh!",
"Hast Du sonst nix in der Drachenschule gelernt, %s?",
"Und wer kuemmert sich jetzt um meine Haustiere, %s?",
"Ich musste sterben, Du willst mich nur beerben. Ich gehe "
    "mit Stil, Du findest nicht viel.",
"Ich habe nun mehr Tode als Du, %s. Topp mich, wenn Du kannst!",
"Ok, dann geh ich halt WOW spielen.",
"Toll, freut sich der Naechste. Wieder ne Todesfolge, %s.",
"Du gucktest so traurig, %s, ich machte Dir meinen Tod zum Geschenk. "
    "Sollst ja auch mal 'n Erfolgserlebnis haben!",
"Nun reiche ich den letzten Becher, trinke den Wein, erkenne Dein Sein.",
"%s hat nicht einmal getroffen. Ich habe mich wortwoertlich totgelacht.",
"Lass mich nachdenken, %s. Ich starb fuer nen EK?",
"Ich kann nicht glauben, dass das gerade passiert ist!",
"Du Suender. Du sollst _nicht_ toeten!",
"%s, mein Testament steckt im rechten Schuh!",
"Sterben und sterben lassen, mein Freund. Wir sehen uns wieder!",
"Und ich sag noch, %s... Das ist KEINE Schwertscheide... Aber Du musst "
  "ja alles selbst ausprobieren.",
"Soll das jetzt alles gewesen sein, %s?",
"In meinem Code ist ein Fehler und gleich gibt's 'nen Crash.",
"Ach, auch egal, %s. Ist eh gleich Schichtwechsel.",
"Jofverdammt... warst Du gut, %s!",
"Duelle von Killern enden meist fuer einen toedlich und das wirst "
  "Du s... Argh!",
"Bist Du sicher, dass Du den EK bekommen hast, %s?",
"Danke %s! Weisst Du, ich hatte mit Zook gewettet, wer mich "
  "toetet. Und ich habe eine Wiedergeburt gewonnen!",
"Nun bin ich sicher vor Dir, %s!",
"Ich mache jetzt Urlaub und habe Zook gebeten, meinen Reset "
  "um einen Monat zu verschieben. Gluecklich, %s?",
});


int _query_kma() { return sizeof(moerder_msgs); }


object _channel( object ob )
{
  int m_FMM, m_HP, m_WC, m_AC, s_HP, s_WC, s_AC;
  string msg;
  object rueck;

  if (member(inherit_list(previous_object()),CORPSE_OBJ)>-1)
      {
          string killer;
          int i, x, y, z, nr;
          closure m_q, s_q;
    
          while( previous_object(i) &&
                 !query_once_interactive(previous_object(i)) )
              i++;
          
          if( !previous_object(i) || IS_LEARNER(previous_object(i)) )
              return rueck;

          killer = (string) previous_object(i)->name();
          
          if ( lower_case(killer) != getuid(previous_object(i)) )
              killer = capitalize(getuid(previous_object(i)));
          
          m_q = symbol_function( "QueryProp", ob ); // Monster
          s_q = symbol_function( "QueryProp", previous_object(i) ); // Spieler
          
          if ( (m_FMM = (int) funcall( m_q, P_FORCE_MURDER_MSG )) >= 0 )
              if ( (object_name(ob) == "/obj/shut") ||
                   (m_FMM > 0) ||
                   (nr = (random(100) >= 99) ? 1 : 0 ) || 
                   (nr = ((x = (m_HP = (int) funcall( m_q, P_MAX_HP )) * 
                           ((m_WC = (int) funcall( m_q, P_TOTAL_WC )) +
                            (m_AC = (int) funcall( m_q, P_TOTAL_AC ))))
                          > 200000) ? 2 : 0) ||
                   (nr = (((y = m_HP * (m_WC + m_AC)) >
                           (z = 5 * (s_HP = (int) funcall( s_q, P_MAX_HP )) *
                            ((s_WC = (int) funcall( s_q, P_TOTAL_WC )) +
                             (s_AC = (int) funcall( s_q, P_TOTAL_AC )))))
                          ? 3 : 0)))
                  {
                      SetProp( P_NAME, "Geist "+(string) ob->name(WESSEN, 0) );

                      if( !(msg = (string) ob->QueryProp(P_MURDER_MSG)) )
                          msg = moerder_msgs[random(sizeof(moerder_msgs))]; 

		      if ( stringp(msg) )
			  msg = sprintf( msg, killer || "Moerder" );

                      CHMASTER->send( "Moerder", this_object(), funcall(msg) );

                      rueck = previous_object(i);
                  }
          
          log_file( "moerder.log",
                    sprintf( "MON(%O) COND(%d) NPC(%d), DIFF(%d,%d)\n",
                             ob, nr, x, y, z) );
      }

  return rueck;
}

void transform_into_pile() {
	if( environment()->QueryProp(P_PREVENT_PILE) ) return;
	object* inv = all_inventory();
	if( sizeof(inv)<2 ) return;
	object p = clone_object(PILE_OBJ);
	filter_objects( inv, "move", p, M_SILENT | M_NOCHECK );
	p->move( environment(), M_SILENT | M_NOCHECK );
}

// Verhindert die Zerstoerung im reset() von Containern, die mit
// remove_multiple() nach identischen Objekten suchen, um Muell zu
// reduzieren. Das ist aber nur dann sinnvoll, wenn nach dem Zerstoeren
// nicht mehr Muell rumliegt als vorher, daher geben wir 0 zurueck, wenn
// die Leiche mehr als ein Objekt enthaelt. Weiterhin raeumt sich die Leiche
// im Lauf der Zeit selber auf/weg.
// Objekte, die 0 zurueckgeben, werden in remove_multiple() uebersprungen.
public string description_id()
{
  if ( sizeof(all_inventory(this_object())) > 1 )
  {
    return 0;
  }
  return ::description_id();
}
