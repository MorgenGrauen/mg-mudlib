#pragma strong_types
#pragma save_types
#pragma pedantic
#pragma range_check
#pragma no_clone

inherit "/std/trading_price";

#define NEED_PROTOTYPES
#include <thing/commands.h>
#include <thing/description.h>
#undef NEED_PROTOTYPES

#include <defines.h>
#include <properties.h>
#include <rooms.h>
#include <language.h>
#include <moving.h>
#include <routingd.h>
#include <bank.h>
#include <combat.h>
#include <input_to.h>
#include <unit.h>
#include <money.h>

// TODO: pruefen, um die Variablen private sein koennen.

// allgemein benoetige Variablen
nosave string storage; // Filename des Stores in dem die Objekte lagern...
nosave mapping ob_anz; // wie oft ist ein Objekt im Laden vorhanden?

// Jetzt Variablen fuer staendig verfuegbare Objekte:
nosave string *fixed_obj; // Liste der staendig verfuegbaren Objekte
nosave mapping fixed_value; // Preise bei Sonderangeboten
nosave mapping fixed_ids;   // Ids

varargs void AddFixedObject(string str, int val, string|string* ids)
{
  int i;

  // Achtung, bei solchen Objekten muss die Blueprint initialisiert werden!
  if (!str) return;
  if (!val) val=str->QueryProp(P_VALUE);
  if (!ids)
  {
    if (str->QueryProp("u_ids")) // units haben keine P_IDS
      ids=str->QueryProp("u_ids")[0];
    else
      ids=str->QueryProp(P_IDS);
  }
  if (!pointerp(ids))
  {
    if (stringp(ids))
      ids=({ids});
    else
      ids=({});
  }

  fixed_obj += ({str});
  fixed_value[str] = val;
  // Alle IDs entfernen, die Sonderzeichen enthalten. Die koennte ein Spieler
  // bei "kaufe" ohnehin nicht eingeben.
  ids -= regexp(ids, "[\b\n\r\t]");
  foreach(string id : ids)
  {
    // Nur IDs aufnehmen, die keine Grossbuchstaben enthalten, da Spieler
    // diese ebenfalls nicht eingeben koennte.
    if ( lowerstring(id) == id )
      fixed_ids[id]=str;
  }
}

void RemoveFixedObject(string filename)
{
  if( !stringp(filename) || !sizeof(fixed_obj))
    return;
  if( member(fixed_obj, filename)==-1 )
    return;

  fixed_obj -= ({ filename });
  m_delete(fixed_value, filename);

  foreach(string id, string file : fixed_ids)
  {
    if ( file == filename )
      m_delete(fixed_ids, id);
  }
}

static string SetStorageRoom(string str)
{
  if (str && stringp(str)) return storage=str;
  return 0;
}

string QueryStorageRoom()
{   return storage;   }

protected void create()
{
  object router;
  
  if (object_name(this_object()) == __FILE__[0..<3])
  {
    set_next_reset(-1);
    return;
  }

  trading_price::create();

  SetProp( P_NAME, "Haendler" );
  SetProp( P_GENDER, MALE );
  SetProp( P_ROOM_TYPE, QueryProp(P_ROOM_TYPE) | RT_SHOP );

  AddCmd("zeige","list");
  AddCmd(({"kauf","kaufe","bestell","bestelle"}),"buy");
  AddCmd(({"verkauf","verkaufe","verk"}),"sell");
  AddCmd(({"versetz","versetze"}),"force_sell");
  AddCmd(({"schaetz","schaetze"}),"evaluate");
  AddCmd(({"untersuche","unt"}), "show_obj");

  SetTradingData(50000,300,3);

  ob_anz=([]);
  fixed_obj=({});fixed_value=([]);fixed_ids=([]);

  AddFixedObject(BOERSE, 80,({ "boerse","geldboerse"}));

  if (!clonep(ME) && objectp(router=find_object(ROUTER)))
    router->RegisterTarget(TARGET_SHOP,object_name(ME));
}

protected void create_super() {
  set_next_reset(-1);
}

// Legacy-Version von GetShopItems() fuer die erbenden Laeden, die auf
// die Funktion in dieser Form angewiesen sind.
static mixed *GetList()
{
  object store = load_object(storage);
  store->_register_shop(ME);

  mixed *output=({});
  if (!objectp(store))
    return output;

  mixed tmp = map(fixed_obj, #'load_object)+all_inventory(store);
  mapping tmp2 = ([]);
  mixed str;
  string comp;
  int i;
  int s=1;

  for (i=sizeof(tmp)-1 ; i>=0 ; i--)
  {
    str = ({ ({ sprintf("%-25.25s%7.7d",
                        (tmp[i]->short()||"???")[0..<3],
                        QueryBuyValue(tmp[i], PL)),
                tmp[i] }) });
    comp=str[0][0][0..25];
    if (!tmp2[comp])
    {
      tmp2[comp] = s++;
      output += str;
    }
    else output[tmp2[comp]-1][0] = str[0][0];
  }
  return output;
}

// Legacy-Version von PrintList() fuer die erbenden Laeden, die auf
// die Funktion in dieser Form angewiesen sind.
static int DoList(string query_fun)
{
  mixed* output=GetList();
  int si = sizeof(output);
  if (!si)
  {
    write("Im Moment sind wir leider VOELLIG ausverkauft!\n");
    return 1;
  }
  string out="";
  int i;
  int indent;
  for (i=0; i<si; i++)
  {
    if (call_other(ME, query_fun, output[i][1]))
    {
      indent = !indent;
      out += sprintf("%3d. %s", i+1, output[i][0]);
      if (!indent)
        out += "\n";
      else out += " | ";
    }
  }
  if (indent)
    out+="\n";
  PL->More(out);
  return 1;
}

// Liefert eine Liste der Objekte zurueck, die gerade im Storage liegen,
// pro Blueprint jeweils eins.
protected object* GetShopItems()
{
  object store = load_object(storage);
  store->_register_shop(ME);
  object* output = ({});
  object* all_items = all_inventory(store);

  // Wir brauchen eine Liste, die von jeder Blueprint nur einen Clone
  // enthaelt. Daher werden die Ladenamen der Objekte als Keys im Mapping
  // <items> verwendet und jeweils der aktuelle Clone als Value zugeordnet.
  mapping items = m_allocate(sizeof(all_items));
  foreach(object ob: all_items)
  {
    items[load_name(ob)] = ob;
  }
  // Die Fixed Objects werden ans Ende angehaengt, damit in dem Fall, dass
  // ein Clone eines solchen Objektes im Lager liegt, dieser zuerst verkauft
  // wird und nicht immer wieder ein neuer erstellt wird.
  return m_values(items) + map(fixed_obj, #'load_object);
}

// Kuemmert sich um die Listenausgabe fuer den Befehl "zeige"
varargs protected int PrintList(string filter_fun, int liststyle)
{
  // Alle Items im Lager holen. Wenn keine vorhanden, tut uns das leid.
  object *items_in_store = GetShopItems();
  if ( !sizeof(items_in_store) ) {
    write("Im Moment sind wir leider VOELLIG ausverkauft!\n");
    return 1;
  }

  // Das Listenformat ist von der Spielereingabe abhaengig. Wurde "lang"
  // angefordert, geben wir einspaltig aus mit groesserer Spaltenbreite.
  // Die Spaltenbreite wird dabei von dem Item mit dem laengsten Namen im
  // gesamten Shop-Inventar bestimmt, damit nicht bei jeder Teilliste
  // (Waffen, Ruestungen, Verschiedenes) unterschiedliche Breiten rauskommen.
  //
  // Der erste Parameter enthaelt die Katalognummer des Items, der zweite
  // die Kurzbeschreibung, der dritte den Preis.
  string listformat = "%3d. %-25.25s %6.6d";
  if ( liststyle == LIST_LONG )
  {
    string* names = sort_array(
                      items_in_store->short(),
                      function int (string s1, string s2) {
                        return (sizeof(s1) < sizeof(s2));
                      });
    // Wenn einspaltig ausgegeben wird, soll die Liste nicht beliebig breit
    // werden. Daher wird die Short auf 65 Zeichen limitiert.
    int len = 65;
    if ( sizeof(names) )
      len = min(len, sizeof(names[0]));
    listformat = "%3d. %-"+len+"."+len+"s %6.6d";
  }

  string out="";
  // Variablen, die innerhalb der Schleife benoetigt werden.
  string kurz;
  int indent, preis;
  object item;
  // Ueber die Liste laufen. <i> wird benoetigt, um die ausgegebene Liste
  // konsistent numerieren zu koennen, damit kaufe <nr> funktioniert.
  foreach(int i : sizeof(items_in_store))
  {
    item = items_in_store[i];
    if ( call_other(ME, filter_fun, item) )
    {
      // Kurzbeschreibung und Preis ermitteln. Items ohne Short werden
      // als "?" angezeigt.
      kurz = (item->short() || "???")[0..<3];
      preis = QueryBuyValue(item, PL);
      // Beschreibung des Items anfuegen.
      out += sprintf(listformat, i+1, kurz, preis);
      indent = !indent;
      // Wenn indent gesetzt ist, handelt es sich um das linke Item in der
      // zweispaltigen Liste, dann fuegen wir einen Spaltentrenner an,
      // ansonsten ist es das rechte, dann brechen wir um.
      // Gilt natuerlich nur fuer kurze Listen.
      out += ((indent && liststyle==LIST_SHORT)? " | " : "\n");
    }
  }
  // Wenn die Liste eine ungerade Zahl Items enthaelt, ist in der letzten
  // Zeile links ein Item aufgefuehrt, daher muss danach umbrochen werden.
  // Gilt natuerlich nur fuer kurze Listen
  if (indent && liststyle==LIST_SHORT)
    out+="\n";

  // Vor den Listen wird eine Info-Zeile ausgegeben, um gefilterte Listen
  // kenntlich zu machen. Wird nach der Filterung des Inventars erzeugt,
  // um eine leere Meldung ausgeben zu koennen, wenn nach Filterung nichts
  // mehr uebrigbleibt.
  string was;
  switch(filter_fun)
  {
    case "IsArmour": was = "Ruestungen"; break;
    case "IsWeapon": was = "Waffen"; break;
    case "NoWeaponNoArmour":
         was = (out==""?"sonstigen Waren":"sonstige Waren"); break;
    default: was = "Waren"; break;
  }
  // <out> ist ein Leerstring, wenn keine Waren da sind, die dem Filterkri-
  // terium entsprechen. Dann gibt's eine entsprechende Meldung.
  if ( out == "" )
    out = sprintf("Leider sind momentan keine %s im Angebot.\n", was);
  else
    out = sprintf("Folgende %s kannst Du hier kaufen:\n",was) + out;

  PL->More(out);
  return 1;
}

// Hilfsfunktionen zum Filtern des Ladeninventars fuer den "zeige"-Befehl
static int AlwaysTrue(object ob)
{   return 1;   }

static string IsWeapon(object ob)
{  return ob->QueryProp(P_WEAPON_TYPE);  }

static string IsArmour(object ob)
{  return ob->QueryProp(P_ARMOUR_TYPE);  }

static int NoWeaponNoArmour(object ob)
{ return (!ob->QueryProp(P_WEAPON_TYPE) && !ob->QueryProp(P_ARMOUR_TYPE)); }


// Diese Funktion ist oeffentlich, falls Magier abfragen wollen, ob ein laden
// ein Objekt zerstoeren wuerde. Aber: Benutzung auf eigenes Risiko! Es wird
// nicht garantiert, dass diese Funktion bzw. ihr Interface sich nicht
// aendert.
public int CheckForDestruct(object ob) // Pruefen, ob zerstoert werden soll
{
  string str;
  /*
   * P_NOBUY - Objekte auf jeden Fall zerstoeren
   */
  if(ob->QueryProp(P_NOBUY)) return 1;
  /*
   * Beschaedigte Objekte werden ebenfalls zerstoert
   */
  if(ob->QueryProp(P_DAMAGED)) return 1;
  /*
   * Ruestungen wenn sie a) eine DefendFunc definiert haben oder
   *                     b) ueber der in KEEP_ARMOUR_CLASS definierten AC
   *                        liegen (siehe /sys/combat.h)
   */
  if(str = IsArmour(ob))
  {
    if(objectp(ob->QueryProp(P_DEFEND_FUNC))) return 1;
    if(ob->QueryProp(P_AC) >= KEEP_ARMOUR_CLASS[str]) return 1;
    return 0;
  }
  /*
   * Waffen wenn sie a) 1-haendig sind und eine WC > 120 besitzen oder
   *                 b) 2-haendig sind und eine WC > 150 besitzen oder aber
   *                 c) eine HitFunc definiert haben
   */
  if(str = IsWeapon(ob))
  {
    if(ob->QueryProp(P_NR_HANDS) > 1 && ob->QueryProp(P_WC) > 150) return 1;
    if(ob->QueryProp(P_NR_HANDS) < 2 && ob->QueryProp(P_WC) > 120) return 1;
    if(objectp(ob->QueryProp(P_HIT_FUNC))) return 1;
    return 0;
  }
  return 0;
}

static int list(string str)
{
  _notify_fail(
    "Bitte 'zeige', 'zeige waffen', 'zeige ruestungen' oder\n"
    "'zeige verschiedenes' eingeben. Wenn Du das Schluesselwort 'lang'\n"
    "oder '-1' anhaengst, wird die Liste einspaltig ausgegeben.\n");

  if (!stringp(str) || !sizeof(str) )
    return PrintList("AlwaysTrue");
  if ( str == "lang" || str == "-1" )
    return PrintList("AlwaysTrue", LIST_LONG);

  string *params = explode(str," ");
  if (sizeof(params[0])<3)
    return 0;

  int liststyle = LIST_SHORT;
  if ( sizeof(params)>1 && params[1] == "lang" )
    liststyle = LIST_LONG;

  str=params[0][0..2];
  if (str=="waf")
    return PrintList("IsWeapon", liststyle);
  if (str=="ver")
    return PrintList("NoWeaponNoArmour", liststyle);
  if (str=="rue")
    return PrintList("IsArmour", liststyle);
  return 0;
}
/*
static varargs int QueryBuyValue(mixed ob, object client)
{
  if (objectp(ob))
    return trading_price::QueryBuyValue(ob, client);
  return (fixed_value[ob]*QueryBuyFact(client))/100;
}
*/

static varargs int QueryBuyValue(object ob, object client)
{
  int fprice = fixed_value[load_name(ob)];

  return (fprice>0) ? 
         fprice*QueryBuyFact(client)/100 : 
         trading_price::QueryBuyValue(ob, client);
}

static void UpdateCounter(object ob, int num)
{
  string tmp;

  if (!num || !objectp(ob)) return;
  tmp=BLUE_NAME(ob);
  if (tmp[0..2]!="/d/" && tmp[0..8]!="/players/")
    tmp=ob->short()+tmp;
  ob_anz[tmp] += num;
  if (ob_anz[tmp] <= 0)
    m_delete(ob_anz,tmp);
}

protected object FindInStore(string|int x)
{
  object* list = GetShopItems();
  if ( intp(x) && x>0 && x<=sizeof(list) ) {
    return list[x-1];
  }
  if (stringp(x))
  {
    if ( fixed_ids[x] )
      return load_object(fixed_ids[x]);
    list = filter_objects(list, "id", x);
    if ( sizeof(list) )
      return list[0];
    // Wenn nix im Store gefunden (das schliesst eigentlicht schon die BPs der
    // fixen Objekte ein, aber nicht, wenn diese nicht konfiguriert sind. D.h.
    // diese Pruefung ist fuer nicht-konfigurierte BPs), Liste der
    // FixedObjects pruefen unde so die eventuell manuell in
    // AddFixedObject() angegebene Liste von IDs beruecksichtigen.
    else if ( fixed_ids[x] )
      return load_object(fixed_ids[x]);
  }
  return 0;
}

static string buy_obj(mixed ob, int short)
{ return 0; }

private void really_buy(int val, object pl, object ob, int u_req)
{
  // Staendig verfuegbare Objekte (fixed_obj) sind daran erkennbar, dass sie
  // nicht im Lager liegen. Daher hier einen Clone erstellen, der dann 
  // stattdessen rausgegeben wird.
  if ( !present(ob, find_object(storage)) )
    ob = clone_object(ob);

  // In Unitobjekten U_REQ (wieder) setzen (wegen input_to (bei dem sich das
  // Kommandoverb aendert und deswegen U_REQ geloescht wird), und wegen
  // Kaufens von Fixed-Objekt-Unitobjekten (bei diesen muss U_REQ _nach_ dem
  // Clonen im Clone gesetzt werden, nicht automagisch in der BP durch den
  // Aufruf von id() weiter vorher).
  if (u_req>0)
  {
    // Das QueryProp() ist nicht unnoetig. Bei der Abfrage von U_REQ wird
    // U_REQ genullt, wenn das aktuelle query_verb() != dem letzten ist.
    // Bei der ersten Abfrage wuerde als das hier gesetzt U_REQ wieder
    // geloescht. Daher muss das jetzt hier als erstes einmal abgefragt
    // werden...
      ob->QueryProp(U_REQ);
      ob->SetProp(U_REQ, u_req);
  }

  pl->AddMoney(-val);
  _add_money(val);

  if (ob->move(pl,M_GET) != MOVE_OK) // Kann der Spieler das Objekt tragen?
  {
    write(break_string("Du kannst "+ob->name(WEN,1)+" nicht mehr tragen. "
     "Ich lege "+ob->QueryPronoun(WEN)+" hier auf den Boden.",78,
     Name(WER)+" sagt: "));
    ob->move(ME,M_NOCHECK);                         // Nein :-)
  }
  else
  {
    // Falls das Objekt sich vereinigt hat, muss jetzt wieder U_REQ
    // restauriert werden.
    ob->SetProp(U_REQ, u_req);
    write(break_string("Du kaufst "+ob->name(WEN,1)+".", 78));
  }

  say(break_string(PL->Name(WER)+" kauft "+ob->name(WEN)+".",78), ({PL}));
  UpdateCounter(ob,-1);
}

static void ask_buy(string str, int val, object pl, object ob, int u_req)
{
  _notify_fail(break_string("Gut, Du kannst es Dir ja jederzeit "
   "noch anders ueberlegen.",78,Name(WER)+" sagt: "));

  if(!str || !stringp(str) || str == "nein" || str == "n")
  {
    return;
  }
  if(str != "ja" && str != "j")
  {
    return;
  }
  really_buy(val, pl, ob, u_req);
}

static int buy(string str)
{
  int i, val, par, dex;
  mixed ob, res;
  string dummy;

  if (!str) {
    write("Was willst Du kaufen?\n");
    return 1;
  }

  if (stringp(QueryProp(P_KEEPER)) && !present(QueryProp(P_KEEPER), ME)) {
    write("Es ist niemand da, der Dich bedienen koennte.\n");
    return 1;
  }

  _notify_fail(break_string("Das kann ich in meinem Lager nicht finden.",78,
   Name(WER)+" sagt: "));

  // Um auch Teile von Unit-Stacks kaufen zu koennen, z.B. "kaufe 5 phiolen",
  // darf hier zusaetzlich <dummy> nur ein Leerstring sein, sonst verzweigt
  // die Syntaxpruefung hierhin und es wird das 5. Item der Liste gekauft.
  if (sscanf(str,"%d%s",i,dummy)>0 && i>0 && !sizeof(dummy)) {
    ob=FindInStore(i);
  }
  else ob=FindInStore(str);

  if (!ob) return 0;

  if (str = buy_obj(ob,0)) 
  {
    write(break_string(str,78,Name(WER)+" sagt: "));
    return 1;
  }

  val = QueryBuyValue(ob,PL);

  if (PL->QueryMoney() < val)
  {
    write(break_string(capitalize(ob->QueryPronoun(WER))+" wuerde "+val+
     " Muenzen kosten, und Du hast nur "+PL->QueryMoney()+".",78,
     Name(WER)+" bedauert: "));
    return 1;
  }

  // Anzahl der angeforderten Einheiten vor dem Bewegen zwischenspeichern.
  // Weil dabei im Fall von Units eine Vereinigung mit bereits im Inventar
  // befindlichen Einheiten stattfindet, muss das ggf. nach Bewegen
  // zurueckgesetzt werden.
  int u_req = ob->QueryProp(U_REQ);

  if ((res = ob->QueryProp(P_RESTRICTIONS)) && mappingp(res) &&
      (res = call_other("/std/restriction_checker",
                        "check_restrictions",PL,res)) &&
      stringp(res))
  {
    _notify_fail(break_string("Du koenntest "+ob->name(WEN,2)+" nicht "
     "verwenden. Grund: "+res+"Moechtest Du "+ob->QueryPronoun(WEN)+
     " dennoch kaufen?",78,Name(WER)+" sagt: "));

    input_to("ask_buy",INPUT_PROMPT, "(ja/nein) ", val,PL,ob,u_req);
    return 0;
  }

  par = ob->QueryProp(P_PARRY);
  dex = PL->QueryAttribute(A_DEX);

  if ((((par < PARRY_ONLY) && ((dex + 8) * 10) < ob->QueryProp(P_WC)) ||
       ((par > PARRY_NOT)  && ((dex + 5) *  2) < ob->QueryProp(P_AC))) &&
      VALID_WEAPON_TYPE(ob))
  {
    _notify_fail(break_string("Du koenntest "+ob->name(WEN,2)+" nicht "
     "zuecken, da Deine Geschicklichkeit dafuer nicht ausreicht. Moechtest "
     "Du "+ob->QueryPronoun(WEN)+" dennoch kaufen?",78,
     Name(WER)+" sagt: "));

    input_to("ask_buy",INPUT_PROMPT, "(ja/nein) ", val,PL,ob,u_req);
    return 0;
  }

  really_buy(val, PL, ob, u_req);

  return 1;
}

private void give_money(int value)
// Geld gutschreiben...
{
  if (!value) return;
  write(break_string(Name(WER, 1)+" zahlt Dir "+value+" Goldstueck"
                    +(value==1?".":"e."), 78));
  if ((PL->AddMoney(value))<=0) {
     object mon;

     write("Du kannst das Geld nicht mehr tragen!\n");
     mon=clone_object(GELD);
     mon->SetProp(P_AMOUNT,value);
     mon->move(ME,M_MOVE_ALL|M_NOCHECK);
  }
}

static int make_to_money(object ob, int value)
// Interne Funktion, die ob versucht in das Lager zu ueberfuehren und das
// Geld das dabei fuer den Spieler abfaellt zurueckliefert.
{
  int ret;

  if (!objectp(ob) || environment(ob)==find_object(storage)) {
    write(break_string(Name(WER, 1)+" wundert sich ueber Dich.", 78));
    return 0;
  }
  if (value>QueryProp(P_CURRENT_MONEY)) {
    write(break_string("Ich hab das Geld leider nicht mehr.", 78,
                       Name(WER, 1)+" sagt: "));
    return 0;
  }
  // U_REQ merken, falls sich Objekte vereinigen. Sonst stimmt nicht nur der
  // Name, sondern es werden ggf. auch die falsche Anzahl zerstoert.
  //TOOO: Oder Units entsorgen und als Feature deklarieren?
  int req = ob->QueryProp(U_REQ);
  if (CheckForDestruct(ob) > 0)  // soll ob zerstoert werden?
  {
    ret = ob->move(storage,M_PUT|M_GET);
    // Falls das Objekt sich vereinigt hat (Units), muessen die gewuenschten
    // Einheiten restauriert werden.
    // Problem: falls das verkaufte Objekt Units hat, beschaedigt ist und sich
    // vereinigt hat, sind jetzt leider alle Einheiten im Lager beschaedigt.
    // Das ist unschoen - aber mir jetzt zuviel AUfwand, das korrekt zu bauen,
    // weil es nur sehr selten vorkommt. (Hint: separater Muellraum)
    ob->SetProp(U_REQ, req);
    if (ret > 0) // Sonst werden auch Sachen zerstoert, die man nicht
    {                 // weglegen kann
      say(break_string(PL->Name()+" verkauft "+ob->name(WEN)+".", 78));
      if(ob->QueryProp(P_DAMAGED))  // Andere Meldung bei beschaedigten
      {                             // Objekten ...
        write(break_string(Name(WER,1)+" findet zwar keinen Gefallen an "
         +ob->name(WEM,1)+", nimmt "+ob->QueryPronoun(WEN)+" Dir zuliebe "
         "aber trotzdem.",78));
      }
      else
      {
        write(break_string(Name(WER, 1)+" findet Gefallen an "
           +ob->name(WEM, 1) + " und legt "+ob->QueryPronoun(WEN)
           +" zu "+(QueryProp(P_GENDER)==FEMALE?"ihren":"seinen")
           +" Privatsachen.", 78));
      }
      /* Er zahlt Dir "+value+" Muenze"+(value==1?"":"n")+" dafuer.", 78)); */
      _add_money(-value);
      _add_money(value*QueryProp(P_SHOP_PERCENT_LEFT)/100); // Wegen Zerstoerung des Objektes
      UpdateCounter(ob,1);
      ob->remove(1);
      return value;
    }
    else if (ret == ME_CANT_BE_DROPPED)
    {
      string|int nodrop = ob->QueryProp(P_NODROP);
      if (stringp(nodrop) && sizeof(nodrop))
        write(nodrop);
      else
        write(break_string("Du kannst "+ob->name(WEN,1)+" nicht verkaufen!",
          78));
      return 0;
    }
    else
      write(break_string(ob->Name(WER)+" interessiert mich nicht.", 78,
               Name(WER, 1)+" sagt: "));
  }
  else if ((ret=(ob->move(storage,M_PUT|M_GET)))>0)
  {
    // Falls das Objekt sich vereinigt hat (Units), muessen die gewuenschten
    // Einheiten restauriert werden.
    ob->SetProp(U_REQ, req);
    say(break_string(PL->Name(WER)+" verkauft "+ob->name(WEN)+".", 78));
    _add_money(-value);
    UpdateCounter(ob,1);
    return value;
  }
  else if (ret == ME_CANT_BE_DROPPED)
  {
    string|int nodrop = ob->QueryProp(P_NODROP);
    if (stringp(nodrop) && sizeof(nodrop))
      write(nodrop);
    else
      write(break_string("Du kannst "+ob->name(WEN,1)+" nicht verkaufen!",
        78));
    return 0;
  }
  else
    write(break_string("Du kannst "+ob->name(WEN,1)+" nicht verkaufen!",
      78));
  return 0;
}

static void ask_sell(string str, object ob, int val, int u_req)
// Wenn ein einzelnen Stueck unter Wert verkauft werden soll, wird nachgefragt
// u_req ist bei Unitobjekten die Anzahl an zu verkaufenden Einheiten. Bei
// normalen Objekten ist u_req 0.
{
  str=lower_case(str||"");
  if (str=="ja"||str=="j")
  {
     // In Unitobjekten U_REQ (wieder) setzen.
     if (u_req>0)
     {
       // Das QueryProp() ist nicht unnoetig. Bei der Abfrage von U_REQ wird
       // U_REQ genullt, wenn das aktuelle query_verb() != dem letzten ist.
       // Bei der ersten Abfrage wuerde als das hier gesetzt U_REQ wieder
       // geloescht. Daher muss das jetzt hier als erstes einmal abgefragt
       // werden...
         ob->QueryProp(U_REQ);
         ob->SetProp(U_REQ, u_req);
     }
     give_money(make_to_money(ob,val));
  }
  else
     write(break_string("Ok, dann behalts!", 78,
             Name(WER, 1)+" sagt: "));
}

static string sell_obj(object ob, int short)
// Ist der Haendler bereit ob zu kaufen? wenn nein, Rueckgabe einer Meldung,
// die der Haendler sagen soll.
{  mixed nosell;

   if (BLUE_NAME(ob)==GELD)
      return "Das waere ja wohl Unsinn, oder ...?";
   if (nosell=ob->QueryProp(P_NOSELL))
   {
     if (stringp(nosell))
       return nosell;
     return ("Du kannst "+ob->name(WEN,1)+" nicht verkaufen!");
   }
   if (ob->QueryProp(P_CURSED))
     return ob->Name(WER,1)
         +" ist mir irgendwie ungeheuer! Das kannst Du nicht verkaufen!";
   // man sollte keine COntainer mit Inhalt verkaufen koennen, ggf. kauft sie
   // dann ein anderer Spieler.
   if (first_inventory(ob))
   {
     return ob->Name(WER, 1) + " ist nicht leer!";
   }
   return 0;
}

static varargs int sell(string str, int f)
{
  int i, val, oval, tmp;
  object *obs;

  if (stringp(QueryProp(P_KEEPER)) && !present(QueryProp(P_KEEPER),ME)) {
     write("Es ist niemand da, der Dich bedienen koennte.\n");
     return 1;
  }
  
  if (!str) {
     notify_fail("Was moechtest Du denn verkaufen?\n");
     return 0;
  }
  
  /* Ergebnis von find_obs() sollte unifiziert werden, damit ein mehrfach
     gefundenes Objekt nicht mehrfach versucht wird zu verkaufen.
     Beispiel: Objekt hat P_NOBUY gesetzt und mehrere IDs gesetzt. Wenn
     ein Spieler es mit "verkaufe ID1 und ID2" versucht zu verkaufen,
     wuerde das einen Bug ausloesen. Derselbe Bug entsteht, wenn man mit
     "verkaufe ID1 und ID1" verkauft. */
  obs = PL->find_obs(str, PUT_GET_DROP);
  /* Erst im Inventar schauen, dann im Environment. find_obs() ohne 2.
     Parameter macht das standardmaessig andersherum.
     TODO: Aenderung ueberpruefen, sobald das neue put_and_get.c 
     eingebaut wurde. */
  if ( !sizeof(obs) )
    obs = PL->find_obs(str, PUT_GET_TAKE) || ({});
  obs = m_indices(mkmapping(obs));
  if (!i=sizeof(obs)) {
     notify_fail("Was moechtest Du denn verkaufen?\n");
     return 0;
  }
  call_other(storage, "_register_shop", ME);
  if (i==1) {
     if (str=sell_obj(obs[0], 0)) {
        write(break_string(str, 78, Name(WER, 1)+" sagt: "));
        return 1;
     }
     if ((oval=obs[0]->QueryProp(P_VALUE))<=0) {
        write(break_string(obs[0]->Name(WER)
              +(obs[0]->QueryProp(P_PLURAL) ? " haben" : " hat")
              +" keinen materiellen Wert, tut mir leid.", 78,
              Name(WER, 1)+" sagt: "));
        return 1;
     }
     val=QuerySellValue(obs[0], PL);
     if (!val) {
        write(break_string("Ich bin absolut pleite. Tut mir aufrichtig leid.",
              78, Name(WER, 1)+" sagt: "));
        return 1;
     }
     if (val==oval || f) {
        give_money(make_to_money(obs[0], val));
        return 1;
     }
     if (str=obs[0]->QueryProp(P_NODROP)) {
        if (stringp(str))
           write(str);
        else write(break_string("Du kannst "+obs[0]->name(WEN,1)
                               +" nicht verkaufen!", 78));
        return 1;
     }

     if (obs[0]->QueryProp(P_DAMAGED))  // Bei beschaedigten Objekten gibt
     {                                  // es auch hier eine andere Meldung
       write(break_string("Da "+obs[0]->name(WER)+" beschaedigt "
        +(obs[0]->QueryProp(P_PLURAL)?"sind":"ist")+", kann ich Dir "
        "nur "+val+" Muenze"+(val == 1?"":"n")+" dafuer bieten. Und "
        "damit mache ich noch Verlust! Nimmst Du mein Angebot an? "
        "(ja/nein)",78,Name(WER,1)+" sagt: "));
     }
     else                              // Default
     {
       write(break_string(Name(WER, 1)+" sagt: "
          +"Nach der aktuellen Marktlage kann ich Dir dafuer nur "
          +val+" Muenze"+(val==1?"":"n")+" bieten, obwohl "
          +obs[0]->name(WER)+" eigentlich "+oval+" Muenze"
          +(oval==1?"":"n")+" wert waere. Willst Du "
          +(QueryProp(P_PLURAL) ? "sie" : "es")
          +" mir dafuer ueberlassen?", 78));
     }
     // in ask_sell() gibt es das query_verb() nicht mehr, weswegen U_REQ in
     // Unitobjekten zurueckgesetzt wird. Damit geht die info verloren,
     // wieviele Objekte der Spieler angegeben hat. Diese muss gerettet und
     // via ask_sell() in make_to_money() ankommen. In normalen Objekten ist
     // U_REQ 0.
     input_to("ask_sell",INPUT_PROMPT, "(ja/nein) ",obs[0], val,
              (obs[0])->QueryProp(U_REQ) );
     return 1;
  }
  for (--i; i>=0 && get_eval_cost()>50000; i--) {
     if (oval=obs[i]->QueryProp(P_VALUE)) {
        if (obs[i]->QueryProp(P_KEEP_ON_SELL)==getuid(PL)
            || obs[i]->QueryProp(P_WORN) || obs[i]->QueryProp(P_WIELDED))
           write(break_string(obs[i]->Name(WER)+": Du behaeltst "
                +obs[i]->name(WEN)+".", 78));
        else if (str=sell_obj(obs[i], 1))
           write(break_string(obs[i]->Name(WER)+": "+str, 78));
        else {
           tmp=QuerySellValue(obs[i], PL);
           if (!tmp) {
              write(break_string(
                    "Ich bin absolut pleite. Tut mir aufrichtig leid.", 78,
                    Name(WER, 1)+" sagt: "));
              break;
           }
           else if (!f && tmp*10<oval)
              write(break_string(obs[i]->Name(WER)+": "+Name(WER, 1)
                    +" bietet Dir aber nur "+tmp+" Goldstueck"
                    +(tmp>1 ? "e" : "")+" dafuer.", 78));
           else {
              str=(obs[i]->Name(WER));
              if (tmp=make_to_money(obs[i], tmp)) {
                 write(break_string(str+": "+Name(WER, 1)
                      +" gibt Dir dafuer "+tmp+" Goldstueck"
                      +(tmp==1?".":"e."), 78));
                 val+=tmp;
              }
           }
        }
     }
  }
  if (!val)
     write(break_string("Hmmm, Du hast aber nicht besonders viel zu bieten...",
                        78, Name(WER)+" sagt: "));
  else give_money(val);
  return 1;
}

static int force_sell(string str)
{  return sell(str, 1);  }

static int evaluate(string str)
{
  object ob;
  int val,rval;

  if (!str) return 0;
  if(stringp(QueryProp(P_KEEPER)) && !present(QueryProp(P_KEEPER), ME)) {
    write("Es ist niemand da, der Dich bedienen koennte.\n");
    return 1;
  }

  ob=present(str, ME);
  if (!ob) ob=deep_present(str,PL);
  if (!ob) {
    write("Hm? "+capitalize(str)+"? Wovon redest Du?\n");
    return 1;
  }
  if (living(ob)) {
    _notify_fail("Nanu, seit wann werden hier Lebewesen verkauft?\n");
    return 0;
  }
  if (str=sell_obj(ob, 0)) {
    write(break_string(str, 78, Name(WER)+" sagt: "));
    return 1;
  }
  rval=ob->QueryProp(P_VALUE);
  if (rval) {
    val=QuerySellValue(ob, PL);
    if (rval==val) {
      tell_object(this_player(),break_string(
         "Naja, ich denke, " +val+ " Muenze"
         + (val==1 ? "" : "n")
         + " waere"+(ob->QueryProp(P_AMOUNT)>1?"n ":" ")
         + (ob->QueryPronoun(WER))+" schon wert.\n",78));
    }
    else if (val) {
        tell_object(this_player(),break_string(
          "Oh, nach der aktuellen Marktlage kann ich nur "+val+" Muenze"+
          (val==1?"":"n")+" bezahlen, obwohl "
          + (QueryProp(P_PLURAL) ? "sie" : "es")
          + " eigentlich "+rval
          + " Muenze"+(rval==1?"":"n")+" wert ist.\n",78));
    }
    else write("Ich bin vollkommen pleite. Tut mir leid.\n");
  }
  else write("Das ist voellig wertlos.\n");
  return 1;
}

static int show_obj(string str)
{
  int i;
  string was;
  mixed ob;

  if (!str) return 0;
  if (sscanf(str,"%s im laden",was)>0 || sscanf(str,"%s in laden",was)>0) {
    _notify_fail("Das kann ich im Lager nicht finden.\n");
    ob=FindInStore(was);
  } else if (sscanf(str,"%d",i)>0 && i>0) {
    _notify_fail("Das kann ich im Lager nicht finden.\n");
    ob=FindInStore(i);
  }
  if (!ob) return 0;
  write(ob->Name(WER)+":\n"+ob->long()+capitalize(ob->QueryPronoun())
       +" kostet "+QueryBuyValue(ob,PL)+" Muenzen.\n");
  return 1;
}

// benutzt von trading_price::QueryValue(object, int, object)
static int ObjectCount(object ob)
{
  string tmp;

  if (!objectp(ob)) return 0;
  tmp = BLUE_NAME(ob);
  if (tmp[0..2]!="/d/" && tmp[0..8]!="/players/") tmp=ob->short()+tmp;
  return ob_anz[tmp];
}

// benutzt von trading_price::QuerySellValue(object, object)
static varargs int QueryValue(object ob, int value, object client)
{
  int new_value, mymoney;

  if (!objectp(ob)) return 0;
  if (Query("laden::compat")) {
    new_value=(value>1000?1000:value);
    mymoney = QueryProp(P_CURRENT_MONEY);
    if (new_value>mymoney)
      return (mymoney>0?mymoney:0);
    else return new_value;
  }
  return ::QueryValue(ob, value, client);
}

void reset()
{
  mixed *keys;
  int i;

  trading_price::reset();

  if (!mappingp(ob_anz))
    return;
  keys=m_indices(ob_anz);
  for (i=sizeof(keys)-1;i>=0;i--) {
    ob_anz[keys[i]]=ob_anz[keys[i]]*7/8;
    if (!ob_anz[keys[i]])
       m_delete(ob_anz,keys[i]);
  }
}

varargs int CheckFindRestrictions(object ob, mixed restr, closure qp) {
  return 0;
}

int EvalWeapon(object ob, closure qp) {
  int wc,val;

  wc=funcall(qp,P_WC);
  val=funcall(qp,P_EFFECTIVE_WC);
  if (val>wc) wc=val;
  return wc;
}

varargs object FindBestWeapon(mixed type, int maxmon, int maxw, int hands,
                              int bestwc, mixed restr) {
  object bestob,ob;
  string otype;
  int wc,bestval,val,w,bestw;
  closure qp;

  if (!stringp(storage) || !objectp(ob=find_object(storage))) return 0;
  if (!maxmon) maxmon=100000;
  if (!maxw) maxw=75000;
  if (!hands) hands=2;
  if (val=QueryBuyFact()) maxmon=(maxmon*100)/val;
  if (type && !pointerp(type) && !mappingp(type)) type=({type});

  for (ob=first_inventory(ob);ob;ob=next_inventory(ob)) {
    qp=symbol_function("QueryProp",ob);
    if (!otype=funcall(qp,P_WEAPON_TYPE)) continue;
    if (type && member(type,otype)<0) continue;
    wc=EvalWeapon(ob,qp);
    if (wc<bestwc) continue;
    if (funcall(qp,P_NR_HANDS)>hands) continue;
    w=funcall(qp,P_WEIGHT);
    if (w>maxw) continue;
    val=funcall(qp,P_VALUE);
    if (val>maxmon) continue;
    if (bestob && wc<=bestwc) {
      if (val>bestval) continue;
      else if (val==bestval && w>bestw) continue;
    }
    if (val>bestval && bestob && wc<=bestwc) continue;
    if (CheckFindRestrictions(ob,restr,qp)) continue;
    bestob=ob;
    bestwc=wc;
    bestval=val;
    bestw=w;
  }
  return bestob;
}

int EvalArmour(object ob,closure qp) {
  int ac,val;

  ac=funcall(qp,P_AC);
  val=funcall(qp,P_EFFECTIVE_AC);
  if (val>ac) ac=val;
  return ac;
}

varargs mapping FindBestArmoursT(mixed type, int maxmon, int maxw,
                                 mapping bestac, mixed restr) {
  object ob;
  string otype;
  int ac,val,sum,w,wsum;
  mapping bestob,bestval,bestw;
  closure qp;

  if (!stringp(storage) || !objectp(ob=find_object(storage))) return ([]);
  if (!maxmon) maxmon=100000;
  if (!maxw) maxw=75000;
  if (val=QueryBuyFact()) maxmon=(maxmon*100)/val;
  if (type && !pointerp(type) && !mappingp(type)) type=({type});
  if (!mappingp(bestac)) bestac=([]);
  bestob=([]);bestval=([]);bestw=([]);

  for (ob=first_inventory(ob);ob;ob=next_inventory(ob)) {
    qp=symbol_function("QueryProp",ob);
    if (!otype=funcall(qp,P_ARMOUR_TYPE)) continue;
    if (type && member(type,otype)<0) continue;
    ac=EvalArmour(ob,qp);
    if (ac<bestac[otype]) continue;
    w=funcall(qp,P_WEIGHT);
    if (wsum-bestw[otype]+w>maxw) continue;
    val=funcall(qp,P_VALUE);
    if (sum-bestval[otype]+val>maxmon) continue;
    if (bestob[otype] && ac<=bestac[otype]) {
      if (val>bestval[otype]) continue;
      else if (val==bestval[otype] && w>bestw[otype]) continue;
    }
    if (CheckFindRestrictions(ob,restr,qp)) continue;
    sum=sum-bestval[otype]+val;
    wsum=wsum-bestw[otype]+w;
    bestob[otype]=ob;
    bestac[otype]=ac;
    bestval[otype]=val;
    bestw[otype]=w;
  }
  return bestob;
}

varargs object *FindBestArmours(mixed type, int maxmon, int maxw,
                                                mapping bestac, mixed restr) {
  return m_values(FindBestArmoursT(type,maxmon,maxw,bestac,restr));
}
