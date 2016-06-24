// (c) 2003 by Thomas Winheller (padreic@mg.mud.de)
// Bei diesem File handelt es sich um einen universellen
// Kessel zum Brauen von Traenken. Damit er sich in den
// Raum entsprechend einpassen kann, ist er invis.

// Ein Raum, in dem Traenke und Gift gebraut werden koennen
// soll, braucht nichts weiter machen als:
// -  eine Moeglichkeit zu implementieren Wasser in den Kessel
//    zu fuellen. Hierzu muss dann mittels AddWater(3000)
//    insgesamt 3l Wasser eingefuellt werden. AddWater gibt die
//    Menge des tatsaechlich noch eingefuelltn Wassers zurueck.
// Anmerkung: bisher enthaelt /obj/flasche noch keine Moeglichkeit
//            Fluessigkeiten von einem Fluessigkeitencontainer in
//            einen anderen zu schuetten, aber vielleicht aendert
//            das ja irgendwann mal wer - der Kessel kann dann
//            durch alle diese Flaschen befuellt werden!
#pragma strong_types,rtt_checks

inherit "/std/container";

#include <defines.h>
#include <properties.h>
#include <moving.h>
#include <fishing.h>
#include <items/kraeuter/kraeuter.h>
#include <items/flasche.h>

#ifndef BS
#  define BS(x)             break_string(x, 78)
#endif

private int wassermenge;

// Aktueller Nutzer des Kessels in Form der Spieler-UID 
private string current_user;

// Zeitpunkt, wann die Sperre endet. 
private int timeout;

protected void create()
{
  ::create();
  SetProp(P_SHORT, "Ein schwerer Kessel");
  // \n werden in long() eingefuegt
  SetProp(P_LONG,
    "Ein kleiner, aber sehr schwerer Kessel, in dem die verschiedensten "
    "Traenke gebraut werden koennen."); 
  SetProp(P_NAME, "Kessel");
  SetProp(P_NAME_ADJ, "klein");
  SetProp(P_MATERIAL, MAT_BRONCE);
  SetProp(P_NOGET, "Der Kessel ist zu schwer, um ihn einfach mitnehmen "
     "zu koennen.\n");
  SetProp(P_MAX_WEIGHT, 100000); // ein _wirklich_ grosser Kessel ;o)
  SetProp(P_WEIGHT, 50000);
  SetProp(P_VALUE, 25000);
  SetProp(P_MAX_OBJECTS, 9); // max. 8 Kraeuter + Wasser
  SetProp(P_GENDER, MALE);
  SetProp(P_LIQUID, 3000); // in den Kessel passen 3l :o)
  AddId(({"kessel", KESSELID}));
  AddAdjective(({"klein", "kleiner"}));
  AddCmd("leer|leere&@ID&aus", "cmd_leeren",
    "Was moechtest Du leeren?|Moechtest Du den Kessel etwa ausleeren?");
  AddCmd(({"brau", "braue", "koch", "koche"}), "cmd_brauen");
  AddCmd("fuell|fuelle&trank|heiltrank|ergebnis|kesselinhalt|inhalt&in&"
    "@PRESENT", "cmd_fuellen",
    "Was willst Du fuellen?|Willst Du etwas in etwas fuellen?|"
    "Worein willst Du den Kesselinhalt fuellen?");
  AddCmd(({"tauch", "tauche"}), "cmd_tauchen");
}

#define TRANKFERTIG "_lib_p_krauttrankfertig"

private int check_busy(int useronly)
{
  if (useronly)
    return current_user && current_user != getuid(PL);
  // Timeout darf noch nicht abgelaufen sein. Wird beim Entleeren geprueft.
  return current_user && timeout > time() && current_user != getuid(PL);
}

private void clear_kessel()
{
  all_inventory()->remove(1);
  // gespeicherte Daten nullen
  wassermenge = current_user = timeout = 0;
  SetProp(P_WATER, 0);
  SetProp(TRANKFERTIG, 0);
}

int AddWater(int menge)
{
  // Wenn ein User eingetragen ist, dieser nicht PL ist und die Sperre
  // auch noch nicht abgelaufen ist, dann wird das Einfuellen von Wasser
  // verhindert. Nutzer der Funktion muessen die Rueckgabewerte pruefen
  // und entsprechende Meldungen ausgeben.
  if (check_busy(1))
    return -1;

  int old = wassermenge;
  wassermenge = min(wassermenge+menge, QueryProp(P_LIQUID));

  if (wassermenge<=0)
  {
    wassermenge=0; // wasser entnahme
    SetProp(P_WATER, 0);
  }
  else
     SetProp(P_WATER, W_DEAD);
  return wassermenge-old;
}

static int cmd_leeren(string str)
{
  if (!QueryProp(P_WATER) && !sizeof(all_inventory()))
  {
    write(BS("Im Kessel ist bisher noch nichts enthalten, was Du ausleeren "
             "koenntest."));
  }
  // Es gibt einen aktuellen User, dieser ist nicht PL, und der Timeout
  // ist auch noch nicht abgelaufen => Finger weg.
  else if (check_busy(0))
  {
    tell_object(PL, BS(
      "Der Inhalt des Kessels wurde erst kuerzlich von jemand anderem dort "
      "hineingefuellt. Du solltest Dich nicht daran zu schaffen machen."));
  }
  write("Vorsichtig nimmst Du den Kessel und schuettest seinen Inhalt in den Abfluss.\n");
  say(BS(PL->Name(WER)+" nimmt den Kessel und schuettet den Inhalt in den Abfluss."));
  clear_kessel();
  return 1;
}

/*#include "/d/erzmagier/boing/balance/balance.h"
#include <wizlevels.h>
#define TESTER (BTEAM+({"elendil","saray", "huraxprax"}))*/
static int cmd_brauen(string str)
{
  /*if (!IS_ARCH(this_interactive())
      && !member(TESTER, PL->query_real_name()))
    return 0;*/
  
  notify_fail("WAS moechtest Du brauen?\n");
  if (!str) return 0;
/*  if (str=="zaubertrank") {
     write("Ohne passendes Rezept duerfte dies schwierig werden...\n");
     return 1;
  }*/
  if (member(({"trank","zaubertrank","kraeutertrank","tee","kraeutertee"}),
             str)<0) 
    return 0;

  if (check_busy(1)) {
    tell_object(PL, BS(
      "An dem Trank in dem Kessel arbeitet gerade noch "+
      capitalize(current_user)+". Du kannst hoechstens in ein paar "
      "Minuten versuchen, den Inhalt des Kessels auszuleeren. Selbst "
      "Hand anzulegen, wuerde man Dir sicherlich uebelnehmen."));
  }
  else if (!QueryProp(P_WATER)) {
    write("Vielleicht solltest Du zunaechst noch Wasser in den Kessel "
      "fuellen...\n");
  }
  else if (wassermenge<QueryProp(P_LIQUID)) {
    write("Vielleicht solltest Du zunaechst noch etwas mehr Wasser in "
      "den Kessel\nfuellen...\n");
  }
  else if (sizeof(all_inventory())<3) {
    write("Derzeit ist Dein Trank noch ein wenig waessrig.\n"
         +"Mindestens drei Zutaten muessen in einen Trank schon hinein.\n");
  }
  else {
    write(BS("Vorsichtig laesst Du den Kessel etwas naeher zur Feuerstelle "
      "runter und wartest unter gelegentlichem Ruehren, bis er kocht. "
      "Dein Trank sollte nun fertig sein und Du kannst ihn nun abfuellen. "
      "Was er wohl fuer eine Wirkung haben wird?"));
    say(BS(PL->Name()+" laesst den Kessel zur Feuerstelle herunter und "
      "ruehrt langsam darin herum. Nach einer Weile kocht die Fluessigkeit "
      "darin, und "+PL->Name(WER)+" stellt das Ruehren wieder ein."));
    SetProp(TRANKFERTIG, 1);
  }
  return 1;
}

static int cmd_fuellen(string str,mixed* params)
{
/*  if (!IS_ARCH(this_interactive())
      && !member(TESTER, this_player()->query_real_name()))
    return 0;*/
 
  if ( !QueryProp(TRANKFERTIG) ) 
  {
    write("Im Kessel befindet sich aber gar kein Trank.\n");
  }
  // Abfuellen ist nur fuer den Spieler moeglich, der die Kraeuter
  // reingetan hat.
  else if (check_busy(1))
  {
    tell_object(PL, BS("Diesen Trank hast Du doch gar nicht selbst "
      "gebraut! Du solltest noch eine Weile warten, ob "+
      capitalize(current_user)+" ihn nicht doch noch selbst abfuellen "
      "will. Wenn nicht, koenntest Du nur noch versuchen, den Kessel "
      "auszuleeren - jedenfalls es erscheint Dir viel zu riskant, das "
      "Gebraeu selbst zu trinken, das "+capitalize(current_user)+
      " da zusammengeruehrt hat."));
  }
  else if (BLUE_NAME(params[2])==TRANKITEM)
  {
    int ret = params[2]->Fill(all_inventory());
    switch( ret ) {
      case -3:
      case -1:
        write(BS("Fehler beim Fuellen der Phiole. Bitte sag einem Magier "
          "Bescheid und nenne den Fehlercode "+ret+"."));
        break;
      case -2:
        write(BS("Die Phiole ist bereits gefuellt."));
        break;
      default:
        write(BS("Du nimmst den Kessel und fuellst seinen konzentrierten "
          "Inhalt in Deine Glasflasche. Hoffentlich ist Dir hier ein "
          "toller Trank gelungen."));
        say(BS(PL->Name(WER)+" nimmt den Kessel und fuellt dessen "
          "konzentrierten Inhalt in eine kleine Glasflasche. Was "+
          PL->QueryPronoun(WER)+" da wohl gebraut hat?"));
        clear_kessel();
        break;
    }
  }
  else {
    write("Darein kannst Du den Trank leider nicht fuellen.\n");
  }
  return 1;
}

varargs string long(int mode)
{
  string inv_desc = make_invlist(PL, all_inventory(ME));
  if (inv_desc=="") {
    if (QueryProp(P_WATER))
      return BS(Query(P_LONG)+" Derzeit ist er lediglich mit Wasser "
        "gefuellt.");
    return BS(Query(P_LONG)+" Er ist im Moment leer.");
  }
  if (QueryProp(P_WATER))
    return BS(Query(P_LONG)+" Er ist mit Wasser gefuellt, und Du siehst "
              +"folgende Kraeuter in ihm schwimmen:")+inv_desc;
  return BS(Query(P_LONG))+"Er enthaelt:\n"+inv_desc;
}

static int _query_invis()
{
  if (member(({"nimm", "nehm", "nehme", "leg", "lege",
               "steck", "stecke"}), query_verb())!=-1) return 0;
  return Query(P_INVIS, F_VALUE);
}
/*
varargs string name(int casus, int demon)
{
  SetProp(P_INVIS, 0);
  string ret=::name(casus, demon);
  SetProp(P_INVIS, 1);
  return ret;
}
*/
varargs int PreventInsert(object ob)
{
  int plantid = ob->QueryPlantId(); 
  int *inv = all_inventory(ME)->QueryPlantId();
  
  // es koennen natuerlich nur echte Kraeuter in den Kessel gelegt werden
  if ( plantid<=0 || !IS_PLANT(ob) )
    return 1;
  
  if (QueryProp(TRANKFERTIG))
  {
    tell_object(PL, break_string(
      "Im Kessel ist ein fertiger Trank. Wenn Du etwas neues machen "
      "willst, leere den Kessel oder fuelle den Trank ab."));
    return 1;
  }
  // Reintun darf nur der aktuelle User, es sei denn, ein anderer Spieler 
  // faengt frisch an, wenn der Kessel gerade unbenutzt ist.
  else if ( check_busy(1) )
  {
    tell_object(PL, BS("Dieser Kessel wurde bis gerade eben noch von "+
      capitalize(current_user)+" genutzt. Warte besser, bis der Kessel "
      "wieder frei ist."));
    return 1;
  }
  else if ( !SECURE("krautmaster")->CanUseIngredient(PL, plantid) ) 
  {
    // mit Kraeutern ueber die man nichts weiss, kann man nicht brauen
    tell_object(PL, BS("Ueber die Wirkungsweise von "+ob->name(WEM)+
      " weisst Du bisher leider wirklich ueberhaupt nichts."));
    return 1;
  }
  else if ( sizeof(inv) >= 8 ) 
  {
    tell_object(PL, BS("Mehr als acht Zutaten sollte man nie zu einem "
      "Trank vereinigen, und es sind schon acht im Kessel."));
    return 1;
  }
  else if (member(inv, plantid)>-1) 
  {
    tell_object(PL, BS("Im Kessel befindet sich bereits "+ob->name(WER)+
      ". Du kannst kein Kraut mehr als einmal verwenden."));
    return 1;
  }
  current_user = getuid(PL);
  timeout = time()+120;
  return ::PreventInsert(ob);
}

int PreventLeave(object ob, mixed dest)
{
  if (QueryProp(P_WATER)) {
    tell_object(PL, BS("Es befindet sich bereits Wasser im Kessel, die "
      "einzelnen Zutaten kannst Du nun leider nicht mehr einzeln "
      "rausholen, ohne den ganzen Kessel auszuleeren."));
      return 1;
  }
  // Rausnehmen ist nur fuer den aktuellen User moeglich. Alle anderen
  // koennen auch nach Ablauf der Zeitsperre nur ausleeren.
  else if ( check_busy(1) ) {
    tell_object(PL, BS("Du hast "+ob->name(WEN,1)+" nicht dort hineingetan, "
      "also kannst Du "+ob->QueryPronoun(WEN)+" auch nicht herausnehmen. "
      "Zumindest vorerst nicht. Sollte "+capitalize(current_user)+
      "nicht innerhalb der naechsten paar Minuten weiterbrauen, kannst "
      "Du den Kesselinhalt zumindest mit einem guten Schluck Wasser "
      "rausspuelen."));
    return 1;
  }
  return ::PreventLeave(ob, dest);
}

