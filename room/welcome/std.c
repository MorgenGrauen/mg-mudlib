#include <properties.h>
#include <moving.h>
#include <defines.h>

inherit  "/std/room";

#pragma strong_types
#pragma rtt_checks
#pragma no_shadow
#pragma no_inherit
#pragma pedantic
#pragma range_check

#define BS(x) break_string(x, 78, 0, BS_LEAVE_MY_LFS|BS_SINGLE_SPACE)
#define TUTSTART "/d/anfaenger/arathorn/minitut/room/huette_"

void create() {
  ::create();

  SetProp(P_LIGHT,1);
  SetProp(P_LIGHT_TYPE, LT_STARS|LT_MOON);
  SetProp(P_MAP_RESTRICTIONS, MR_NOUID|MR_NOINFO);
  SetProp(P_INDOORS,0);
  SetProp(P_NO_TPORT, NO_TPORT);

  SetProp(P_INT_SHORT, "Irgendwo im Nirgendwo");
  SetProp(P_INT_LONG, BS(
    "Du schwebst im Nirgendwo. Sterne umgeben Dich, und Du glaubst "
    "zu fallen, obwohl Du Dich vollkommen schwerelos fuehlst. Ein hauch"
    "duenner Schleier aus Staub umfaengt Dich, als Du langsam aber sicher "
    "Deines Koerpers gewahr wirst."));
  AddSpecialExit("welt", function int () {
    if ( PL->QueryGuest() ) 
      return PL->move("/gilden/abenteurer", M_GO);
    else return 0; 
  });
  Set(P_HIDE_EXITS, function mixed () {
    return !(PL && PL->QueryGuest());
  }, F_QUERY_METHOD);
}

void init() {
  set_next_reset(300);
  if ( objectp(PL) )
    call_out("Sequenz", 0, 0);
  return ::init();
}

// Nix resetten, aber Raum entsorgen, wenn kein Spieler drin.
// Notfalls gibt es auch noch nen cleanup() aus dem Standardraum...
protected void reset() {
  if (!sizeof(filter(all_inventory(this_object()), #'query_once_interactive))
      )
    remove(1);
}

//TODO: Delays anpassen bzw. ausklammern, wenn immer =5 ausser am Ende.
void Sequenz(int count) {
  int delay;

  if ( !objectp(PL) || environment(PL) != ME )
    return;

  switch(count) {
    case 0:
      tell_object(PL, BS(QueryProp(P_INT_LONG)));
      delay = 4;
      break;
    case 1:
      tell_object(PL, BS(
        "\nDu schaust Dich erstaunt um. Du befindest Dich anscheinend im "
        "Weltall, aber wie bist Du hierhergekommen? Ist das schon das "
        "MorgenGrauen, oder bist Du wohl erst noch auf dem Weg dorthin?"));
      delay = 8;
      break;
    case 2:
      tell_object(PL, BS(
        "\nMit einem Blick auf Deine Umgebung bemerkst Du, dass der Staub, "
        "der Dich umgibt, sich langsam auf Dich zuzubewegen scheint. "
        "Du willst neugierig danach greifen, aber es will Dir nicht "
        "gelingen. Verwundert stellst Du fest, dass Du keine Haende "
        "besitzt, und bei naeherer Betrachtung wird Dir klar, dass Du "
        "auch sonst noch ziemlich koerperlos bist! "));
      delay = 12;
      break;
    case 3:
      tell_object(PL, BS(
        "\nDas aendert sich aber bald, als der glitzernde Staub allmaehlich "
        "um Dich herum eine durchscheinende Gestalt zu bilden beginnt, "
        "die sich zuegig verdichtet."));
      delay = 8;
      break;
    case 4:
      int g = PL->QueryProp(P_GENDER);
      string desc = PL->QueryProp(P_RACESTRING)[0];
      string msg = (g==MALE?"ein richtiger ":"eine richtige ")+desc;
      tell_object(PL, BS(
        "\nSchon kurz darauf hat sich ein eleganter Koerper geformt, der "
        "nun vollkommen Dir gehoert. Dass Du noch nackt bist, stoert Dich "
        "nicht im geringsten, denn endlich, endlich fuehlst Du Dich "
        "wie "+msg+"!"));
      delay = 10;
      break;
    case 5:
      tell_object(PL, BS(
        "\nDeine Begeisterung ueber Deinen makellosen Koerper wird jaeh "
        "unterbrochen, als ploetzlich die Sterne verblassen und sich mit "
        "unfassbarem Tempo von Dir zu entfernen scheinen. Dann wird es "
        "kurz schwarz um Dich herum und kurz darauf wieder hell."));
      delay = 8;
      break;
    case 6:
      tell_object(PL, "\n"+break_string(
        "Herzlich Willkommen! Uebrigens, wenn Du auf der "
        "Ebene Anfaenger mit anderen reden willst, folgender Tip: Probiere "
        "mal '-anf Hallo Morgengrauen' (natuerlich ohne die ' mit "
        "einzugeben) aus.", 78, "Merlin teilt Dir mit: "));
      delay = 6;
      break;
    case 7:
      if (PL->QueryGuest() ) {
        PL->move("/gilden/abenteurer", M_GO);
        return;
      }
      else
        tell_object(PL, "\n"+break_string(
          "Ich werde Dich jetzt in das Tutorial fuer Anfaenger bewegen, wo "
          "Du Deine ersten Schritte machen kannst...",
          78, "Merlin teilt Dir mit: ")+"\n");
      delay = 4;
      break;
    case 8:
      PL->move(TUTSTART+getuid(PL), M_GO);
      return; // call_out nicht wieder anwerfen.
   }
   ++count;
   call_out("Sequenz", delay, count);
}

// Bei Disconnect Sequenz abbrechen, bei Re-Login wieder starten mit
// count == 5 d.h. Spieler fliegt also quasi "im Schlaf" weiter...
void BecomesNetDead(object pl) {
  while(remove_call_out("Sequenz")!=-1)
   ;
}

void BecomesNetAlive(object pl) {
  if (interactive(pl))
    call_out("Sequenz", 6, 5);
}

