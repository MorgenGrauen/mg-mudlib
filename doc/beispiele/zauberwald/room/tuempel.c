// (c) by Thomas Winheller (Padreic@mg.mud.de)

#include "../files.h"
#include <moving.h>

inherit ROOM("stdroom");
inherit "/std/room/kraeuter";

void create()
{
   ::create();
   SetProp(P_LIGHT, 2);
   SetProp(P_INDOORS, 0);
   SetProp(P_INT_SHORT, "An einem verwunschenden Weiher");
   AddSpecialDetail(({"ort", "weiher"}), "_query_int_long");
   AddDetail(({"seerosen", "farben", "farbenvielfalt"}),
     "Auf dem Wasser schwimmen zahlreiche Seerosen in allen Farben des Regenbogens\n"
    +"eine solche Farbenvielfalt wie hier, hast Du bisher noch nirgendwo sonst\n"
    +"gesehn.\n");
   AddDetail("regenbogen", "Derzeit steht keiner am Himmel.\n");
   AddDetail(({"libelle", "libellen", "atmosphaere", "summen"}),
     "Hier und da schwirren ueber dem Weiher einige Libellen herum. Ihr Summen\n"
    +"verbreitet eine richtig idyllische Atmosphaere an diesem Ort.\n");
   AddDetail(({"pixies", "zauberwesen"}),
     "Pixies sind kleine verspielte Zauberwesen, die hier im Zauberwald wohnen. Um\n"
    +"einem von ihnen zu begegnen wirst Du hier im Wald sicher nich lange suchen\n"
    +"muessen, doch vorsicht, sie haben nichts als Unfug im Kopf...\n");
   AddDetail(({"wald", "zauberwald", "punkt"}),
     "Der verwunschene Weiher ist mehr oder weniger der zentrale Punkt hier im\n"
    +"Zauberwald. Er liegt mitten auf einer grossen Lichtung umgeben vom dichten\n"
    +"Wald der Pixies\n");
   AddDetail(({"lichtung", "mitte"}),
     "Der Weiher befindet sich genau in der Mitte der Lichtung, sieh Dich halt\n"
    +"einfach mal um.\n");
   AddDetail(({"boden", "ufer"}),
     "Das Ufer des Weihers besteht aus zahlreichen Steinen die bis in den Weiher\n"
    +"hineinreichen. Auf und zwischen diesen Steinen sind jedoch ueberall zahlreiche\n"
    +"kleine und mittlere Pflanzen gewachsen.\n");
   AddDetail(({"stein", "steine"}), "Ein Stein ist ein Stein ist ein Stein ist ein Stein ist ein Stein.\n");
   AddDetail("pflanzen",
     "Die verschiedensten Pflanzen wachsen zwischen den Steinen empor und begruenen\n"
    +"das Ufer. An einer Stelle kannst Du sogar weissen Wasser-Hahnenfuss wachsen\n"
    +"sehen.\n");
   AddDetail(({"wasser", "schimmer", "schimmern"}),
     "Das Wasser ist irgendwie recht trueb und es geht ein sehr seltsames Schimmern\n"
    +"von ihm aus. Irgendetwas besonderes hat es mit diesem Wasser auf sich, das\n"
    +"spuerst Du genau. Du weisst jedoch nicht, ob es etwas gutes oder etwas boeses\n"
    +"ist.\n");
   AddDetail(({"himmel", "luft", "sonne", "waerme", "strahlen", "sonnenstrahlen"}),
     "Die Sonne steht hoch am Himmel und fuellt diesen Ort mit einer herrlichen\n"
    +"Waerme und ihre Strahlen reflektieren im Wasser in allen erdenklichen Farben.\n");
   AddExit("norden", ROOM("lichtungn"));
   AddExit("nordosten", ROOM("lichtungno"));
   AddExit("nordwesten", ROOM("lichtungnw"));
   AddExit("osten", ROOM("lichtungo"));
   AddExit("westen", ROOM("lichtungw"));
   AddExit("sueden", ROOM("lichtungs"));
   AddExit("suedwesten", ROOM("lichtungsw"));
   AddExit("suedosten", ROOM("lichtungso"));
   AddItem(NPC("ulinia"), REFRESH_REMOVE, 1);
   AddCmd(({"bad", "bade", "schwimm", "schwimme", "tauch", "tauche"}), "cmd_schwimmen");
   AddCmd(({"trink", "trinke"}), "cmd_trinken");
   AddCmd(({"pflueck", "pfluecke"}), "cmd_pfluecken");
   // Detail per Hand hinzufuegen, da GetPlant anstelle von AddPlant
   // verwendet wird.
   AddPlantDetail(OBJ("hahnenfuss"));
}

static int cmd_trinken(string str)
{
   int drink;
   object ob;
   if (member(({"wasser", "wasser aus teich", "wasser aus tuempel",
                "wasser aus see", "aus teich", "aus tuempel", "aus wasser",
                "aus see"}), str)==-1) {
      notify_fail("Was moechtest Du trinken?\n");
      return 0;
   }
   if ((ob=present(WALDID("fee"), ME)) && !ob->IsEnemy(PL)) {
      write(BS("Die Waldfee scheint Deinen Entschluss ein wenig von dem "
         +"Wasser zu kosten, bemerkt zu haben und straft Dich mit einem "
         +"mahnenden Blick ab. Du ueberlegst es Dir also noch einmal und "
         +"laesst von Deinem Vorhaben ab."));
      return 1;
   }
   // das tanken am Wasser ist umsonst, es heilt dabei wie ein
   // Kneipengetraenk das genau 1 Muenze kostet. Geheilt werden nur SP
   // Ein Missbrauch ist im Wald ausgeschlossen, bzw. man muesste den
   // gesamten Wald leermetzeln und kann dann ueben bis zum naechsten
   // reset. Bei Kosten von einer Muenze pro Heilung ist es sehr fraglich
   // ob sich das lohnt.... :)
   drink=(PL->QueryProp(P_MAX_DRINK)) - (PL->QueryProp(P_DRINK));
   if (drink>10) drink=10; // nie mehr als fuer 10 soak tanken...
   if (drink <= 0 || !PL->drink_soft(drink, 1)) {
      write("Du bist so voll, Du kannst leider wirklich nichts mehr trinken...\n");
      return 1;
   }
   write("Du gehst an den Rand des Weihers und nimmst einen kraeftigen Schluck von dem\n"
        +"Wasser. Sogleich spuerst Du, wie Deine mentalen Kraefte langsam gestaerkt\n"
        +"werden.\n");
   if (PL->drink_soft(drink)) { // soak gutschreiben erfolgreich?
      PL->buffer_sp(drink*6, 5);
      PL->SetProp(ZAUBERWALD, time()+AGGRESSIVE_TIME);
   }
   return 1;
}

static int cmd_schwimmen(string str)
{
   string verb;

   // besondere Eigenschaft der schluesselwoerter ausnutzen... :)
   verb=(query_verb()||"");
   if (verb[<1]!='e') verb+="e";
   if (member(({"in teich", "in weiher", "in tuempel", "in see"}), str)==-1) {
      notify_fail("Worin willst Du "+verb+"n?\n");
      return 0;
   }
   str=capitalize(str[3..]); // "in " abschneiden...
   if (present(WALDID("fee"), ME)) {
      write(BS("Die Waldfee scheint Deinen Entschluss im "+str+" "+verb
         +"n zu gehn irgendwie bemerkt zu haben und straft Dich mit einem "
         +"mahnenden Blick ab. Deine Lust zum "+capitalize(verb)
         +"n sinkt sogleich gegen null, denn Du verspuerst keine grosse "
         +"Lust Dich mit der Waldfee anzulegen."));
      return 1;
   }
   write(BS("Mutig gehst Du einen Schritt ins Wasser, da ueberkommt Dich auch "
           +"schon ein seltsames kribbeln am ganzen Koerper. In windeseile "
           +"hast Du den "+str+" auch schon wieder verlassen."));
   return 1;
}

static int cmd_pfluecken(string str)
{
   object ob;
   notify_fail("WAS moechtest Du pfluecken?\n");
   if (member(({"hahnenfuss",
                "wasser-hahnenfuss",
                "weisser hahnenfuss",
                "weisser wasser-hahnenfuss"}), str)<0) return 0;
   if ((ob=present(WALDID("fee"), ME)) && !ob->IsEnemy(PL)) {
      write(BS("Die Waldfee scheint Dein Vorhaben bemerkt zu haben und "
         +"straft Dich mit einem mahnenden Blick ab. Du ueberlegst es Dir "
         +"wieder anders und laesst von Deinem Vorhaben ab, solange die "
         +"Fee hier wacht."));
      return 1;
   }
   ob=GetPlant(OBJ("hahnenfuss"));
   if (objectp(ob)) {
      if (ob->move(PL, M_GET)==1)
          write(BS("Vorsichtig pflueckst Du "+ob->name(WEN, 1)
                  +" und nimmst "+ob->QueryPronoun(WEN)+" an Dich."));
      else write(BS("Vorsichtig pflueckst Du "+ob->name(WEN, 1)+" kannst "
                  +ob->QueryPronoun(WEN)+" aber nicht nehmen."));
   }
   else if (!ob)
      write(BS("Der Hahnenfuss ist noch nicht wieder weit genug "
              +"nachgewachsen um ihn pfluecken zu koennen."));
   return 1;
}

static string _query_int_long()
{
   if (present(WALDID("fee"), ME))
      return
        "Du stehst nun an einem kleinen verwunschenen Weiher, inmitten des Zauberwalds.\n"
       +"Auf dem Wasser schwimmen zahlreiche Seerosen in den verschiedensten Farben\n"
       +"und hier und dort schwirrt eine Libelle in der Luft. Mitten ueber dem Weiher\n"
       +"schwebt eine wunderschoene Waldfee in der Luft und scheint den Weiher zu\n"
       +"bewachen. Irgendetwas geheimnisvolles scheint es mit dem Weiher auf sich\n"
       +"zu haben, nur was?\n";
   return
     "Du stehst nun an einem kleinen verwunschenen Weiher, inmitten des Zauberwalds.\n"
    +"Auf dem Wasser schwimmen zahlreiche Seerosen in den verschiedensten Farben und\n"
    +"hier und dort schwirrt eine Libelle in der Luft. Alles scheint hier sehr\n"
    +"friedlich zu sein, fast ein wenig zu friedlich. Irgendetwas geheimnisvolles\n"
    +"scheint es mit diesem Weiher auf sich zu haben, nur was?\n";
}
