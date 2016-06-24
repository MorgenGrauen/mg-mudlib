// (c) by Thomas Winheller (Padreic@mg.mud.de)

#include "../files.h"
#include <moving.h>

inherit ROOM("stdroom");
inherit "/std/room/kraeuter";

void maybe_replace_program()
// dieser Standardraum, darf _nicht_ replaced werden...
{ }

int tuempel_ex(string exit, mapping map_ldfied)
{   return map_ldfied[exit]==ROOM("tuempel");  }

static int plant;

void create()
{
   int i;
   string tuempel, dir;
   mixed tmp;

   if (!clonep(ME)) return;
   ::create();
   plant=0;
   SetProp(P_INDOORS, 0);
   SetProp(P_LIGHT, 1);
   SetProp(P_INT_SHORT, "Am Rande der Zauberlichtung");
   SetProp(EXTRA_LONG, "");
   if (PO) dir=(PO->CustomizeObject());
   if (!dir) return;
   dir=(["o":"ost", "w":"west", "n":"nord", "s":"sued",
         "no":"nordost", "nw":"nordwest",
         "so":"suedost", "sw":"suedwest"])[dir[8..]];
   tuempel=filter(m_indices(QueryProp(P_EXITS)||([])), "tuempel_ex", ME, QueryProp(P_EXITS)||([]))[0][0..<3];
   dir=capitalize(dir); tuempel=capitalize(tuempel);
   SetProp(P_INT_LONG, BS(
     "Du stehst am "+dir+"rand der grossen Lichtung inmitten des Zauberwalds. "
    +tuempel+"lich von hier, befindet sich genau in der Mitte der "
    +"Lichtung ein kleiner Tuempel in dessen Wasser sich das Sonnenlicht in "
    +"allen Farben des Regenbogens spiegelt. Hier und da stehen um den Teich "
    +"herum vereinzelt einige Eichen die sich irgendwie zu bewegen "
    +"scheinen und ab und zu siehst Du wie sich einige kleine Pixies aus "
    +"dem Wald heraus ans Wasser trauen. Die Eichen jedoch, halten immer "
    +"einen deutlichen Abstand zum Wasser."+QueryProp(EXTRA_LONG)));
   AddDetail(({"raum", lower_case(dir)+"rand", "zauberwald"}), QueryProp(P_INT_LONG));
   AddDetail(({"pixies", "zauberwesen", "unfug"}),
     "Pixies sind kleine verspielte Zauberwesen, die hier im Zauberwald wohnen. Um\n"
    +"einem von ihnen zu begegnen wirst Du hier im Wald sicher nich lange suchen\n" 
    +"muessen, doch vorsicht, sie haben nichts als Unfug im Kopf...\n");
   AddDetail(({"farben", "regenbogen", "computer"}),
     "Noch nie einen Regenbogen gesehn? Hmm.. dann sitzt Du wirklich eindeutig\n"
    +"zuviel vor Deinem Computer...\n");
   AddDetail("abstand",
     "Wieso sie diesen Abstand halten kannst Du nicht nachvollziehn, aber mehr als\n"
    +"ein paar Meter gehen sie nie an den alten Weiher dran.\n");
   AddDetail(({"sonne", "sonnenstrahlen", "sonnenlicht", "himmel", "herz"}),
     "Zahlreiche warme Sonnenstrahlen erwaermen Dein Herz und Du bist richtig\n"
    +"gluecklich. Solch ein Idyllisches Plaetzchen willst Du am liebsten nie\n"
    +"wieder verlassen. Fehlt eigentlich nur noch ein Partner, mit dem man\n"
    +"hier gemeinsam in der Sonne liegen und das rauschen des Wassers geniessen\n"
    +"kann.\n");
   AddDetail("partner", "Den musst Du Dir schon selbst mitbringen...\n");
   AddDetail(({"wald", "zauberwald"}),
     "Du befindest Dich quasi mittem ihn ihm. Rund herum um die Lichtung schliesst\n"
    +"sich ein dichter Wald an.\n");
   AddDetail(({"eiche", "eichen"}),
     "Hier befindet sich gerade keine, so dass Du sie nich naeher ansehn kannst.\n");
   AddDetail(({"lichtung", "rand", "plaetzchen"}), QueryProp(P_INT_LONG));
   AddDetail(({"wasser", "tuempel", "teich", lower_case(tuempel)+"en", "mitte"}),
     "Wenn Du Dir den Tuempel in der Mitte der Lichtung etwas naeher ansehen\n"
    +"moechtest, solltest Du vielleicht einfach noch ein bisschen naeher rangehn.\n");
   AddDetail(({"waldweg", "weg"}),
     "Ueber den Waldweg kannst Du den Wald wieder verlassen wenn Du moechtest.\n");
   AddDetail(({"boden"}),
     "Der Boden ist hier nirgends nackt sichtbar, sondern ist mit einem schoenen\n"
    +"gruenen Rasenteppich bedeckt. Nicht zu hoch und nicht zu tief, offensichtlich\n"
    +"bedarf diese Grasart keinerlei Pflege oder aber irgendjemand pflegt diesen\n"
    +"Rasen hier seeehhhhrrr gruendlich.\n");
   AddDetail(({"grasart", "rasen", "art", "gras", "rasenteppich", "halme"}),
     "Du schaust Dir den Rasen noch einmal gruendlich an und streichst mit Deiner\n"
    +"Hand durch die Halme. Was auch immer das fuer eine Art ist, solch einen Rasen\n"
    +"hast Du noch nicht gesehn.\n");
   AddDetail("hand",
     "Sei lieber vorsichtig, sonst ist die Hand schneller ab als Du denkst...\n");
   AddDetail("pflege",
     "Ob jemand diesen Rasen pflegt, bzw. _wer_ kannst Du nicht entdecken...\n");
   AddSounds(({"rauschen", "wasser"}), "Leise hoerst Du das rauschen des Wassers....\n");
}

#define VERB (["iss": "essen", "esse": "essen", "pflueck": "pfluecken", "pfluecke": "pfluecken"])

static int cmd_pilze(string str)
{
   notify_fail("Was moechtest Du "+VERB[query_verb()]+"?\n");
   if (str!="pilz" && str!="pilze") return 0;
   if (plant>time()) {
      write("Du solltest die Pilze erstmal in Ruhe wieder etwas nachwachsen lassen.\n");
      return 1;
   }
   if (PL->QueryProp(ZAUBERWALD)<=time()) {
      write("Du bist nur Gast hier und die Bewohner des Zauberwalds, sehen es nicht gerne\n"
           +"wenn man hier einfach so die Pilze pflueckt.\n");
      return 1;
   }
   plant=time()+300;
   write("Du pflueckst Dir einige Pilze und isst sie. Anschliessend geht es Dir\n"
        +"bedeutend besser.\n");
   PL->reduce_hit_points( negate(100+random(100)) );
   return 1;
}

static int cmd_farne(string str)
{
   int food;
   notify_fail("Was moechtest Du "+VERB[query_verb()]+"?\n");
   if (str!="farn" && str!="farne") return 0;
   if (PL->QueryProp(ZAUBERWALD)<=time()) {
      write("Du bist nur Gast hier und die Bewohner des Zauberwalds, sehen es nicht gerne\n"
           +"wenn man hier einfach so den Farn abreisst.\n");
      return 1;
   }
   // das tanken am Wasser ist umsonst, es heilt dabei wie ein
   // mittleres Kneipenessen das genau 1 Muenze kostet. Geheilt werden nur HP
   // Ein Missbrauch ist im Wald ausgeschlossen, bzw. man muesste den
   // gesamten Wald leermetzeln und kann dann hier tanken bis zum naechsten
   // reset. Bei Kosten von einer Muenze pro Heilung ist es sehr fraglich
   // ob sich das lohnt.... :)
   food=(PL->QueryProp(P_MAX_FOOD)) - (PL->QueryProp(P_FOOD));
   if (food>10) food=10; // nie mehr als fuer 10 food tanken...
   if (food <= 0 || !PL->eat_food(food, 1)) {
      write("Du bist so voll, Du kannst leider wirklich nichts mehr essen...\n");
      return 1;
   }
   write("Du pflueckst ein wenig von dem Farn und isst ihn. Sogleich spuerst Du, wie es\n"
        +"Dir allmaehlich wieder besser geht...\n");
   if (PL->eat_food(food)) { // food gutschreiben erfolgreich?
      PL->buffer_hp(food*6, 8);
      PL->SetProp(ZAUBERWALD, time()+AGGRESSIVE_TIME);
   }
   return 1;
}

static int cmd_sueden()
{
   if (PL->QueryProp(ZAUBERWALD)>time()) {
      write("Du versuchst in den Wald zu fluechten, doch die Farne und Buesche bilden\n"
           +"eine pflanzliche Barriere und lassen Dich nicht hindurch.\n");
      return 1;
   }
   write("Du machst einen Schritt hin nach Sueden und sofort biegen sich auf magische\n"
        +"Weise die Farne und Buesche zur Seite und geben einen Trampelpfad nach Sueden\n"
        +"frei.\n");
   PL->move(ROOM("huette"), M_GO, "durch die Buesche nach Sueden", "zwaengt sich");
   return 1;
}
