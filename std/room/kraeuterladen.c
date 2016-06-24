// (c) 2003 by Padreic (padreic@mg.mud.de)
// 
// Es kann bestimmte Laeden zum Handeln von Kraeutern geben.
// Zunaechst einmal gibt es einen in der Dunkelelfengilde.
// Hier koennen Kraeuter ge- und verkauft werden.
// Grundsaetzlich kann es beliebig viele kraeuterkundige
// Haendler geben, eine kurze Absprache waere jedoch von Vorteil.

// Der Laden erweitert die room/shop Funktionen.
// Zur Verwendung muss dieser noch mit einem std/room kombiniert
// werden. Dies erlaubt die Verwendung von eigenen Standardraeumen oder
// Kombination mit Kneipen.

inherit "/std/room/shop";

#define NEED_PROTOTYPES

#include <items/kraeuter/kraeuter.h>
#include <properties.h>

// Flag, das im reset() das Speichern ausloest, wenn es gesetzt ist.
static  int need_save;  
static  int basisvalue=400; // gibt den _Durchschnittswert_ eines Krauts an
static  int maxvalue=10000; // bestimmt den max. Wert aller Kraeuter

// Enthaelt fuer jede PlantID einen Zaehler, der angibt, wieviel von dem
// jeweiligen Kraut verkauft wurde.
// private int *count=({});

// Summe ueber alle Eintraege in count
// private int sum;

protected void create()
{
  if (object_name(this_object()) == __FILE__[0..<3])
  {
    set_next_reset(-1);
    return;
  }
  ::create();

  SetProp(P_BUY_ONLY_PLANTS,1);
  /*
  seteuid(getuid()); // wird zum speichern benötigt
  int si=sizeof(count);
  // fuer jede PlantID einen Eintrag anlegen, d.h. wenn in count noch
  // nicht genug vorhanden sind, werden fehlende Eintraege ergaenzt.
  if (si<PLANTCOUNT) 
    count=count+allocate(PLANTCOUNT-si);
  */
}

protected void create_super()
{
  set_next_reset(-1);
}

static string sell_obj_only_plants(object ob, int short)
{
   if (!IS_PLANT(ob))
      return "Tut mir leid, damit kann ich nichts anfangen.";
   // es werden nur Kraeuter angekauft, die legal geclont wurden,
   // d.h. im entsprechenden Master ordentlich eingetragen wurden!
   if (ob->QueryPlantId()<=0)
      return ob->Name(WER, 2)+" ist illegal auf die Welt gekommen. Ich darf "
            +ob->QueryPronoun(WEN)+" leider nicht annehmen.";
   return ::sell_obj(ob, short);
}

static string sell_obj(object ob, int short)
{
   // es werden nur Kraeuter angekauft, die legal geclont wurden,
   // d.h. im entsprechenden Master ordentlich eingetragen wurden!
  if (IS_PLANT(ob) && ob->QueryPlantId()<=0)
    return "Hm, "+ob->Name(WER, 2)+" stammt aber aus einer sehr dubiosen "
      "Quelle. Ich kann "+ob->QueryPronoun(WEN)+" leider nicht annehmen.";
  if (QueryProp(P_BUY_ONLY_PLANTS))
    return sell_obj_only_plants(ob, short);
  return ::sell_obj(ob,short);
}

/*
void reset()
{
  ::reset();
  // Daten sind nicht sooo wichtig, als das bei jeder Aenderung
  // gesavet werden muesste. Daher nun im reset speichern.
  if (need_save) {
    // basisvalue/PLANTCOUNT: Durchschnittswert eines einzelnen Krauts
    // sum + PLANTCOUNT: Gesamtzahl verkaufter plus Zahl existierender
    //                   Kraeuter
    // Wenn also der Wert all dieser Kraeuter > 10k ist, wird jeder
    // Einzelzaehler auf 90% gestutzt, damit die Werte nicht ins Uferlose
    // steigen.
    if (((sum+PLANTCOUNT)*basisvalue/PLANTCOUNT)>maxvalue) {
      int i, newsum;
      for (i=sizeof(count)-1; i>=0; i--) {
        count[i] = count[i]*9/10;
        newsum+=count[i];
      }
      sum=newsum;
    }
    need_save=0;
  }
}

// Aktualisiert den Datenbestand beim Kaufen oder Verkaufen eines Objekts
void UpdateCounter(object ob, int num)
{
   int id=ob->QueryPlantId();
   if (id>0 && is_plant(ob)) {
      // Kauf oder Verkauf von Kraeutern, veraendert den Wert der
      // Kraeuter
      // Zaehler in der Liste hochzaehlen
      count[id]+=num;
      if (count[id]<0) count[id]=0; // darf aber ansich nich passieren
      // Gesamtsumme aktualisieren
      sum+=num;
      need_save=1;
   }
   ::UpdateCounter(ob, num);
}

// Die Preise die hier im Labor fuer Kraeuter gezahlt und verlangt
// werden, sind nicht fix sondern haengen von Angebot und Nachfrage ab.
// Hier weiss man ueber den wahren Wert der einzelnen Kraeuter bescheid.
static int realValue(object ob, object player)
{
   // Preise fuer normale Gueter ganz normal...
   if (!is_plant(ob))
       return ob->QueryProp(P_VALUE);

   // jede Kraeuterkategorie bekommt den gleichen Wert zugewiesen.   
   // val entspricht dem aktuellen "Durchschnittswert" eines Krautes
   int val=(sum+PLANTCOUNT)*basisvalue/PLANTCOUNT;

   // aber dieser Wert verteilt sich auf unterschiedlich viele Kraeuter
   // (AN: Dieser Kommentar erschliesst sich mir nicht.)
   int id=ob->QueryPlantId();
   if (id<=0) return 0; // illegal geclont -> wertlos
   // ggf. die Zaehlerliste um die fehlenden Eintraege erweitern.
   if ((id-1)>sizeof(count)) 
     count=count+allocate(id-1-sizeof(count));
   
   // "mittleren Wert" des abgefragten Krautes errechnen (Division durch
   // dessen umgesetzte Anzahl)
   val=val/(count[id]+1);
   
   // Wert runden auf naechstniedrigeren glatt durch 10 teilbaren Wert.
   return val-val%10; 
}

// gibt den Preis zurück, der zum Ankauf des Objektes verwendet werden soll
static varargs int QueryValue(object ob, int value, object player)
{
   return ::QueryValue(ob, realValue(ob, player), player);
}

// gibt den Preis an, zu dem das Objekt verkauft werden soll.
static varargs int QueryBuyValue(mixed ob, object player)
{
   if (objectp(ob))
      return (realValue(ob, player)*QueryBuyFact(player) + 50)/100;
   // fixed Objekte...
   return ::QueryBuyValue(ob, player);
}
*/
