// MorgenGrauen MUDlib
//
// unit.c -- Basisklasse fuer Mengenobjekte
//           (neue Version von Padreic)
//
// $Id: unit.c 9077 2015-01-16 23:26:00Z Zesstra $
#pragma strong_types,save_types,rtt_checks
#pragma range_check
#pragma no_clone
#pragma pedantic

inherit "/std/thing";

#define NEED_PROTOTYPES
#include <unit.h>
#undef NEED_PROTOTYPES

#include <properties.h>
#include <thing/properties.h>
#include <defines.h>
#include <language.h>
#include <moving.h>
#include <wizlevels.h>
#include <debug_info.h>

// zum debuggen und extra public
public string debugmode;
public string __set_debug(string recv) {return debugmode=recv;}
#include <living/comm.h>
#define ZDEBUG(x) if (stringp(debugmode) && find_player(debugmode)) \
  find_player(debugmode)->ReceiveMsg(x,MT_DEBUG,0,object_name()+": ",ME)
//#define ZDEBUG(x)

private void _call_DoDecay(object *klone);

private nosave string lastverb;
private nosave int lastevalnumber;

protected void create()
{
  ::create();
  SetProp(U_IDS,({({}),({})}));
  SetProp(P_GENDER, 1);
  SetProp(P_AMOUNT, 1);
  // Props sollten nicht direkt manipuliert werden, nur ueber Set-Methoden
  Set(P_UNIT_DECAY_INTERVAL, PROTECTED, F_MODE_AS);
  Set(P_UNIT_DECAY_QUOTA, PROTECTED, F_MODE_AS);
} 

protected void create_super() {
  set_next_reset(-1);
}

//// Query und Set Funktionen fuer die Propertys

static void check_leave()
{  if (Query(P_AMOUNT,F_VALUE)<1) remove(1);  }

static int _query_invis()
{
  if (QueryProp(P_AMOUNT) < 1)
    return 1;
  return Query(P_INVIS, F_VALUE);
}

static int _set_amount(int am)
{
  if (am<1)
    call_out("check_leave",1);
  SetProp(U_REQ, am);
  return Set(P_AMOUNT,am, F_VALUE);
}

static int _set_u_req(int ureq)
{
  lastverb = query_verb();
  lastevalnumber = debug_info(DINFO_EVAL_NUMBER);
  return Set(U_REQ, ureq, F_VALUE);
}

static int _query_u_req()
{
  // Zwei Dinge benutzten, um zu entscheiden, ob U_REQ noch gueltig ist oder
  // besser auf P_AMOUNT zurueckgesetzt werden sollte: die DINFO_EVAL_NUMBER
  // und das Kommandoverb. Wenn eines von beiden sich geaendert hat, wird
  // U_REQ als ungueltig betrachtet. (Und dies bleibt ein uebler Hack.)
  if (lastevalnumber != debug_info(DINFO_EVAL_NUMBER)
      || lastverb != query_verb())
  {
    return SetProp(U_REQ, Query(P_AMOUNT, F_VALUE));
  }
  return Query(U_REQ, F_VALUE);
}

// Gesamtwert der gerade mit U_REQ ausgewaehlten unitobjekte
static int _query_value()
{
  mixed cpu;
  cpu=Query(U_CPU);
  if (intp(cpu)) return QueryProp(U_REQ) * cpu;
  if (!pointerp(cpu)) return 0;
  return (QueryProp(U_REQ) * cpu[0])/cpu[1];
}

static int _set_value(int num) {
// Setzt den Wert fuer die derzeitige Anzahl
  SetCoinsPerUnits(num, QueryProp(U_REQ));
  return QueryProp(P_VALUE);
}

static int _query_weight()
{
  mixed gpu, req;
  if ((req=QueryProp(U_REQ))<1) return 0;
  gpu=Query(U_GPU);
  if (intp(gpu)) return req * gpu;
  if (!pointerp(gpu)) return 0;
  return MAX(1,(req*gpu[0])/gpu[1]);
}

static int _set_weight(int num) {
// Setzt das Gewicht fuer die derzeitige Anzahl
    SetGramsPerUnits(num, QueryProp(U_REQ));
    return QueryProp(P_WEIGHT);
}

static int _query_total_weight()
{
  mixed gpu, amount;
  if ((amount=Query(P_AMOUNT))<1) return 0;
  gpu=Query(U_GPU);
  if (intp(gpu))
    return amount*(int)gpu;
  if (!pointerp(gpu)) return 0;
  return MAX(1,(amount*gpu[0])/gpu[1]);
}

static int _query_plural()
{
  int i;
  i=QueryProp(U_REQ); // wichtig _nicht_ QueryProp
  return (i<=1 ? 0 : i);
}

// gibt string | string* zurueck.
static string|string* _query_name()
{
  if (QueryProp(U_REQ)==1)
    return "%s%s"+((string *)Query(P_NAME))[0];
  return "%d %s"+((string *)Query(P_NAME))[1];
}

// gibt string | string* zurueck.
static string|string*  _set_name(mixed names)
{
  if(!names)
    return Set(P_NAME,({"",""}));
  if(stringp(names))
    return Set(P_NAME,({names,names}));
  if(pointerp(names))
    switch(sizeof(names)) {
      case 0:  return Set(P_NAME,({"",""}));
      case 1:  if(!stringp(names[0]))
                 names[0]="";
               return Set(P_NAME,({names[0],names[0]}));
      default: if(!stringp(names[0]))
                 names[0]="";
               if(!stringp(names[1]))
                 names[1]="";
               return Set(P_NAME,names[0..1]);
    }
  return QueryProp(P_NAME);
}

// die Funktionen

void AddSingularId(mixed str)
{
  string *ids;
  if (!pointerp(str))
    str=({str});
  ids=Query(U_IDS);
  Set(U_IDS,({ids[0]+str,ids[1]}));
}

void RemoveSingularId(mixed str)
{ 
  if(stringp(str))
    str=({str});
  if(pointerp(str))
    Set(U_IDS,({Query(U_IDS)[0]-str,Query(U_IDS)[1]}));
}
 
void AddPluralId(mixed str)
{
  string *ids;
  
  if (!pointerp(str))
    str=({str});
  ids=Query(U_IDS);
  Set(U_IDS,({ids[0],ids[1]+str}));
}

void RemovePluralId(mixed str)
{ 
  if(stringp(str))
    str=({str});
  if(pointerp(str))
    Set(U_IDS,({Query(U_IDS)[0],Query(U_IDS)[1]-str}));
}

void SetCoinsPerUnits(int coins,int units)
{
  if (coins<0||units<=0) return;
  if (units==1)
    Set(U_CPU, coins);
  else Set(U_CPU,({coins,units}));
}

void SetGramsPerUnits(int grams, int units)
{
  if (grams<0||units<=0) return;
  if (units==1)
    Set(U_GPU, grams);
  else Set(U_GPU,({grams,units}));
}

int *QueryCoinsPerUnits()
{
  mixed cpu;
  cpu=Query(U_CPU);
  if (intp(cpu)) return ({ cpu, 1 });
  return cpu;
}

int *QueryGramsPerUnits()
{
  mixed gpu;
  gpu=Query(U_GPU);
  if (intp(gpu)) return ({ gpu, 1 });
  return gpu;
}

void AddAmount(int am) {
  SetProp(P_AMOUNT, am+Query(P_AMOUNT));
}

int IsUnit() {
  return 1;
}

int IsEqual(object ob)
// haben ob und dieses Objekte die gleiche Blueprint?
{
  if (ob==this_object()) return 0; // das ist ZU gleich :)
  return (BLUE_NAME(ME)==BLUE_NAME(ob));
}

// ueberschreiben von Standardmethoden

varargs string name(int fall, int demo)
{
  int req;
  mixed n_adj;
  string adj;

  if ((req=QueryProp(U_REQ))<1) return 0;
  if (fall!=RAW && 
      pointerp(n_adj=QueryProp(P_NAME_ADJ)) && sizeof(n_adj) )
    adj = implode(map(n_adj, #'DeclAdj, fall, demo && req==1), "");
  else
    adj = "";

  if (req==1)
    return sprintf(QueryProp(P_NAME),
                   (fall==RAW || !QueryProp(P_ARTICLE) 
                    ? "" : QueryArticle(fall,demo,1)), adj) /* +korrektur */;

  if (fall!=WEM)
    return sprintf(QueryProp(P_NAME), req, adj);
  else {
     int last;
     last=Query(P_NAME)[1][<1];
     return sprintf(QueryProp(P_NAME), req, adj)
                  +(last!='s' && last!='n' ? "n" : "");
  }
}

varargs string QueryPronoun(int casus)
{
  if (QueryProp(U_REQ)==1)
    return ::QueryPronoun(casus);
  switch(casus) {
    case WESSEN: return "ihrer";
    case WEM:    return "ihnen";
    //default:     return "sie";
  }
  return("sie"); //fall-through
}

string short()
{
  if (QueryProp(U_REQ)<1 || QueryProp(P_INVIS)) return 0;
  return capitalize(name(WER,0))+".\n";
  /*
  if (req>1) return sprintf(QueryProp(P_NAME), req)+".\n";
  return capitalize(sprintf(QueryProp(P_NAME), QueryArticle(WER,0,1)))+".\n";
  */
}

varargs string long()
{
  return (QueryProp(P_LONG) ? process_string(QueryProp(P_LONG)) : "")+
    sprintf("Du siehst %s.\n",name(WEN));
}

varargs int id(string str,int lvl)
{

  string s1,s2,*ids;
  int i;

  if (!str) return 0;
  
  // Wenn kein pos. Amount, hat dieses Objekt keine IDs mehr, damit es nicht
  // mehr ansprechbar ist.
  int amount=QueryProp(P_AMOUNT);
  if (amount < 1)
    return 0;

  if (::id(str)) {
    SetProp(U_REQ, Query(P_AMOUNT,F_VALUE));
    return 1;
  }

 ids=Query(U_IDS);
  if (!ids)
    return 0;
 
  //die casts auf 'mixed' sind absicht. Sonst geht der Driver offenbar davon
  //aus, dass ids[1] ein String ist. Es ist aber aber ein Array von Strings
  //und genau das will auch match_item() haben. ;-) Zesstra
  if (match_item(str,(mixed)ids[1] )) {
    SetProp(U_REQ, amount);
    return 1;
  }
  if (match_item(str,(mixed)ids[0] )) {
    SetProp(U_REQ,1);
    return 1;
  }
  if (sscanf(str,"%s %s",s1,s2) && s1[0..3]=="alle" && 
    match_item(s2,(mixed)ids[1] )) {
    SetProp(U_REQ, amount);
    return 1;
  }
  if (sscanf(str,"%d %s",i,s1)==2 && i==1 && match_item(s1,(mixed)ids[0] )) {
    SetProp(U_REQ,1);
    return 1;
  }
  if (sscanf(str,"%d %s",i,s1)==2 && match_item(s1,(mixed)ids[1] ) && 
      i<=amount && i>0) {
    SetProp(U_REQ,i);
    return 1;
  }
  return 0;
}

// Status fuer move... *seufz*
int _orig_amount, _move_requested;

varargs int move(object|string dest,int method)
{
  _orig_amount = QueryProp(P_AMOUNT);
  // Wenn M_MOVE_ALL gesetzt ist, wird IMMER ALLES bewegt.
  if (method & M_MOVE_ALL)
    _move_requested = _orig_amount;
  else
    _move_requested = QueryProp(U_REQ);

  ZDEBUG(sprintf("move from %O to %O: amount %d, req %d\n",environment(),dest,_orig_amount,_move_requested));
  // Wenn nicht alles bewegt werden soll, wird die Menge dieses Objektes
  // erstmal reduziert und bewegt. Erst nach der Bewegung wird der Rest dann
  // im alten Environment neu erzeugt.
  if ( _orig_amount > _move_requested )
  {
     // wenn in einem alten Paket das Gewicht noch nicht neu berechnet
     // wurde muss dies geschehn solange die Muenzen im Paket noch das
     // volle Gewicht haben...
     if ( environment() )
         environment()->query_weight_contents();
     // ab jetzt nur auf <_move_requested> Einheiten arbeiten
     Set( P_AMOUNT, _move_requested, F_VALUE);
     ZDEBUG(sprintf("move: amount set to %d\n",_move_requested));
  }

  int res = ::move( dest, method );

  if ( res != MOVE_OK )
  {
      // Fehlgeschlagene Bewegung ist doof.
      // ggf. verfruehte Aenderung (P_AMOUNT oben auf <U_REQ> Einheiten
      // reduziert) rueckgaengig machen
      Set( P_AMOUNT, _orig_amount, F_VALUE );
      ZDEBUG(sprintf("move: not OK, amount restored to %d\n",_orig_amount));
      return res;
  }

  // TODO: eigentlich sollte es nicht passieren, dass die Menge jetzt negagtiv
  // ist. Aber bei Geld kann es durch vereinigen mit Geldboerse/Geldkarten
  // passieren und das ist kein Fehler. Koennte man evtl. mal differenziert
  // pruefen.
  int finalamount= QueryProp(P_AMOUNT);
  /*if ( finalamount < 1 )
  {
    ZDEBUG(sprintf("move: zerstoerung, amount %d\n",finalamount));
    remove(1);
    return ME_WAS_DESTRUCTED;
  }
  */
  ZDEBUG(sprintf("move: OK, amount %d\n",finalamount));
  return res;
}


protected void NotifyMove(object dest, object oldenv, int method)
{
  // Erst einen evtl. nicht zu bewegenden Rest im alten Environment wieder erzeugen.
  if (oldenv && _orig_amount > _move_requested)
  {
      if ( oldenv == dest )
      {
          // Objekt wurde nur an den Anfang des inventory bewegt, sonst nichts.
          // ggf. verfruehte Aenderung (oben auf <req> Einheiten reduziert)
          // rueckgaengig machen
          ZDEBUG(sprintf("move: same env, amount restored to %d\n",_orig_amount));
          Set( P_AMOUNT, _orig_amount, F_VALUE );
      }
      else
      {
        // verbleibende Einheiten in einem neuen Objekte wieder zurueck ins
        // alte Environment zurueck bewgen
          object temp;
          temp = clone_object(BLUE_NAME(ME));
          temp->SetProp( P_AMOUNT, _orig_amount-_move_requested );
          temp->move( oldenv, M_NOCHECK );
          ZDEBUG(sprintf("move: Restmenge im alten Env erzeugt, amount %d\n",_orig_amount-_move_requested));
      }
  }

  // Und jetzt ggf. mit den anderen Units gleichen Typs im neuen Environment
  // vereinigen.
  foreach(object item: all_inventory(environment()))
  {
      if ( IS_EQUAL(item) )
      {
        // Es ist ein Feature, dass auch mit Objekten mit negativen Amount
        // vereinigt wird.
        ZDEBUG(sprintf("move: vereinige mit %O (%d)\n",
               item,item->QueryProp(P_AMOUNT)));
        int mergedamount = QueryProp(P_AMOUNT) + item->QueryProp(P_AMOUNT);
        item->Set( P_AMOUNT, 0, F_VALUE);
        item->remove(1);

        SetProp( P_AMOUNT, mergedamount);
        // U_REQ ist jetzt auch zurueckgesetzt.

        ZDEBUG(sprintf("move: nach vereinigung, amount %d\n",mergedamount));
      }
  }

  // wenn man in frisch geclonten Units die noch kein environment haben
  // per Hand U_REQ auf einen Wert != P_AMOUNT setzt, so gehen die
  // restlichen Einheiten  verloren (es gibt kein Environment, in das man die
  // Resteinheiten zurueck bewegen koennte).

  // Und jetzt mal Decaykrams machen...

  // wenn NO_DECAY_UNTIL_MOVE an ist und dieses ein Move ist, was von einem
  // Env in ein anderes Env geht, loeschen. Nicht geloescht wird
  // hingegen, wenn das move() in das allererste Env erfolgt (nach dem Clonen)
  if ( (QueryProp(P_UNIT_DECAY_FLAGS) & NO_DECAY_UNTIL_MOVE)
      && objectp(dest) && objectp(oldenv) && dest != oldenv)
    SetProp(P_UNIT_DECAY_FLAGS, 
        QueryProp(P_UNIT_DECAY_FLAGS) & ~NO_DECAY_UNTIL_MOVE );
  ::NotifyMove(dest, oldenv, method);
}


void reset() {
  if (!clonep(ME)) {
    // Blueprint ist Master fuer zerfallende Unitobjekte
    // das Decay sollte besser nicht durch manuellen Aufruf in der BP
    // ausgeloest werden, daher Pruefung hier (PO == ME, falls der Reset vom
    // Driver kommt).
    if (previous_object() && previous_object() != ME)
      return; 
    int zeit = QueryProp(P_UNIT_DECAY_INTERVAL);
    if (zeit > 0)
    {
      set_next_reset(zeit);
      // Das Callout muss auf jeden Fall gemacht werden, wenn ein Decay
      // Intervall gesetzt ist, damit die Blueprint auch den naechsten Reset
      // kriegt, auch wenn es jetzt keine Clone gibt. Wenn es kein Quota gibt,
      // kann der Callout wegfallen, beim Setzen eines Quota wird ja eine
      // Funktion an diesem Objekt gerufen.
      if (QueryProp(P_UNIT_DECAY_QUOTA) > 0)
        call_out(#'_call_DoDecay, 1, 0);
    }
  }
  else {
    // Clones
    if (Query(P_AMOUNT,F_VALUE)<1) remove(1);
    else ::reset();
  }
}

varargs int remove(int silent)
{
  int amount = QueryProp(P_AMOUNT);
  int req = QueryProp(U_REQ);
  if (amount > req)
  {
    ZDEBUG(sprintf("remove: reduziere amount %d um %d -> %d\n",
           amount, req, amount-req ));
    SetProp(P_AMOUNT, amount - req);
    return 1;
  }
  ZDEBUG(sprintf("remove: restlos weg.\n"));
  return ::remove(silent);
}

// sollte nicht von aussen gerufen werden.
private void _call_DoDecay(object *klone) {
  ZDEBUG(sprintf("call_DoDecay() in %O\n",ME));
  // Clone oder kein Decay konfiguriert?
  if (clonep()
      || QueryProp(P_UNIT_DECAY_FLAGS) & NO_DECAY)
    return;

  if (!pointerp(klone))
    klone = clones(2);
  // Klone da?
  int i=sizeof(klone);
  if (!i)
    return;

  // in allen Klonen DoDecay rufen
  while(get_eval_cost() > __MAX_EVAL_COST__/2 && i--) {
    if (objectp(klone[i]))
      catch(call_other(klone[i],"DoDecay", 0);publish);
  }
  // das i ist schon abgearbeitet.
  i--;
  // noch was uebrig?
  if (i >= 0)
    call_out(#'_call_DoDecay, 4, klone[0..i]);
}

// zerstoert quota Prozent der vorhandenen Units, liefert die Restmenge
// zurueck. Ruft ggf. DoDecayMessage()
public int DoDecay(int silent) {
  // nur fuer Clones
  if (!clonep())
    return 0;
  ZDEBUG(sprintf("DoDecay() in %O\n",ME));

  // wenn das Objekt NO_DECAY oder NO_DECAY_UNTIL_MOVE in den Flags
  // zerfaellt nix.
  int flags = QueryProp(P_UNIT_DECAY_FLAGS);
  if ((flags & NO_DECAY) || (flags & NO_DECAY_UNTIL_MOVE) )
    return QueryProp(P_AMOUNT);

  int quota = QueryProp(P_UNIT_DECAY_QUOTA);

  // Zugewinn ist nicht. Ausserdem kann nicht mehr zerfallen, als da ist...
  // Also ohne gueltiges Quota raus. Nur bei abs. Zerfall sind Zahlen > 10000
  // erlaubt, bei relativen Zerfall waere das sinnlos.
  if (quota <= 0 || 
      (quota > 10000 && !(flags & ABSOLUTE_DECAY) ) ) 
    return QueryProp(P_AMOUNT);
  
  int amount = QueryProp(P_AMOUNT);
  int minimum = QueryProp(P_UNIT_DECAY_MIN);
  // Zerfall nur, wenn man mehr als das Minimum hat.
  if (amount > minimum) {
    int zerfall;
    if (flags & ABSOLUTE_DECAY) {
      // Decay Quota soll einfach als abs. Zahl aufgefasst werden:
      zerfall = quota;
    }
    else if (flags & INACCURATE_DECAY) {
      // Rundungsfehler ignorieren und dafuer immer min. eine Einheit
      // zerfallen lassein
      zerfall = (amount * quota) / 10000;
      if (zerfall <= 0) zerfall = 1;
    }
    else {
      // Standardfall:
      // nicht-ganzzahligen Rest als Wahrscheinlichkeit fuer den Zerfall einer
      // Einheit auffassen
      float tmp = amount * quota / 10000.0;
      zerfall = to_int(tmp);
      tmp -= zerfall; // Rest in tmp
      // tmp*n ist eine Zahl zwischen 0 und n, wenn tmp*n > random(n)+1,
      // dann zerfaellt eine Einheit extra. Da das Random fuer grosse Zahlen
      // besser verteilt ist, nehm ich n = __INT_MAX__
      if (ceil(tmp * __INT_MAX__) > random(__INT_MAX__) + 1)
        zerfall++;
    }

    // nicht unter Minimum zerfallen
    if (amount - zerfall < minimum)
      zerfall = amount - minimum;

    // erst ggf. Meldung ausgeben.
    if (!silent && zerfall > 0)
      DoDecayMessage(amount, zerfall);
    // dann runtersetzen.
    SetProp(P_AMOUNT, amount - zerfall);
    ZDEBUG(sprintf("%O decayed by %d\n",ME, zerfall));
    return amount-zerfall;
  }
  else if (amount < 0)
    return 0; // neg. Anzahl gilt als "0 uebrig".
  
  // sonst ists wohl zwischen 0 und minimum, nix aendert sich.
  return amount;
}

// gibt die Zerfallsmeldung aus. Wenn man mit der Standardmeldung nicht zufrieden
// ist (duerfte der Normalfall sein), sollte man diese Funktion
// ueberschreiben. Bekommt bisherige und jetzt gerade zerfallene Menge
// uebergeben. Wichtig: zu diesem Zeitpunkt hat sich an der Unit noch nix (!)
// geaendert!
protected void DoDecayMessage(int oldamount, int zerfall) {
  string msg;
  if (oldamount == zerfall) {
    if (living(environment())) {
      tell_object(environment(), break_string(sprintf(
          "Auf einmal %s%s zu Staub!",
          (oldamount>1?"zerfallen Deine ":"zerfaellt D"), name(WER)),78));
    }
    // das tell_room() muss auf jeden fall sein, weil es "lebende" Raeume
    // gibt. Liegt der KRam in einem solchen wuerden die Livings in diesem Raum
    // sonst keine meldung kriegen.
    tell_room(environment(), break_string(sprintf(
          "Auf einmal %s %s zu Staub!",
          (oldamount>1?"zerfallen":"zerfaellt"), name(WER)),78),
        ({environment()}));
  }
  else {
     if (living(environment())) {
      tell_object(environment(), break_string(sprintf(
          "Auf einmal %s %d Deiner %s zu Staub!",
          (zerfall>1?"zerfallen":"zerfaellt"),
          zerfall, Query(P_NAME)[1]),78));
    }
    // das tell_room() muss auf jeden fall sein, weil es "lebende" Raeume
    // gibt. Liegt der KRam in einem solchen wuerden die Livings in diesem Raum
    // sonst keine meldung kriegen.
    tell_room(environment(), break_string(sprintf(
          "Auf einmal %s %d %s zu Staub!",
          (zerfall>1?"zerfallen":"zerfaellt"),
          zerfall, Query(P_NAME)[1]),78), ({environment()}) ); 
  }
}

static int _query_unit_decay_interval() {
  if (clonep()) {
    // fuer clones den Wert aus der BP liefern, der Wert in Clones ist eh
    // uninteressant.
    if (objectp(blueprint()))
      return blueprint()->QueryProp(P_UNIT_DECAY_INTERVAL);
    else
      return load_object(load_name())->QueryProp(P_UNIT_DECAY_INTERVAL);
  }
  return Query(P_UNIT_DECAY_INTERVAL);
}

static int _set_unit_decay_interval(int zeit) {
  // fuer Clones ist die Prop sinnlos und kann nicht gesetzt werden.
  if (clonep()) {
    if (objectp(blueprint()))
      return blueprint()->QueryProp(P_UNIT_DECAY_INTERVAL);
    else
      return load_object(load_name())->QueryProp(P_UNIT_DECAY_INTERVAL);
  }
  // ueber diese Prop liesse sich der Reset relativ beliebig veraendern,
  // verhindern, etc. Daher darf sie nur von diesem Objekt gesetzt werden, von
  // Weisen+ oder eines Objektes, das die gleiche UID hat wie dieses hier oder
  // des Programmieres dieses Objekts oder eines Magiers, der berechtigt fuer
  // die jeweilige UID ist. Letztere beiden werden ueber den Ruf von
  // QueryUIDsForWizard() ermittelt.
  // erstes fremdes Objekt finden und pruefen.
  foreach(object po: caller_stack(1)) {
    if (po != ME) {
      // fremden Aufrufer gefunden
      if (getuid(po) != getuid(ME) 
          && member(master()->QueryUIDsForWizard(getuid(po)),getuid(ME)) == -1
          && (!this_interactive() || !IS_ELDER(this_interactive()) ) )
        // unberechtigt, Abbruch
        return QueryProp(P_UNIT_DECAY_INTERVAL);
      else
        // nur das erste ext. Objekt in der Callkette wird geprueft, raus.
        break;
    }
  }
  set_next_reset(zeit);
  // falls dies aus dem create() gesetzt wird, muss zeitverzoegert eine
  // Funktion von aussen am Objekt gerufen werden, da nach einem create() das
  // Objekt als resettet gilt und sonst keinen Reset kriegt.
  call_out("_query_unit_decay_interval",1);
  return Set(P_UNIT_DECAY_INTERVAL, zeit, F_VALUE);
}

static int _query_unit_decay_quota() {
  if (clonep()) {
    // hat dieses Objekt einen eigenen wert gespeichert?
    int quota=Query(P_UNIT_DECAY_QUOTA, F_VALUE);
    if (quota)
      return quota;
    // sonst den Wert aus der Blueprint
    if (objectp(blueprint()))
      return blueprint()->QueryProp(P_UNIT_DECAY_QUOTA);
    else
      return load_object(load_name())->QueryProp(P_UNIT_DECAY_QUOTA);
  }
  return Query(P_UNIT_DECAY_QUOTA, F_VALUE);
}

static int _set_unit_decay_quota(int quota) { 
  // momentan nur quota zwischen 0 und 10000
  if (quota >= 0 && quota <=10000)
    return Set(P_UNIT_DECAY_QUOTA, quota, F_VALUE);
  else
    return Query(P_UNIT_DECAY_QUOTA, F_VALUE);
}

static int _query_unit_decay_min() {
  if (clonep()) {
    // hat dieses Objekt einen eigenen wert gespeichert?
    int minimum=Query(P_UNIT_DECAY_MIN, F_VALUE);
    if (minimum)
      return minimum;
    // sonst den Wert aus der Blueprint
    if (objectp(blueprint()))
      return blueprint()->QueryProp(P_UNIT_DECAY_MIN);
    else
      return load_object(load_name())->QueryProp(P_UNIT_DECAY_MIN);
  }
  return Query(P_UNIT_DECAY_MIN, F_VALUE);
}

static int _query_unit_decay_flags() {
  if (clonep()) {
    // hat dieses Objekt einen eigenen wert gespeichert?
    int flags=Query(P_UNIT_DECAY_FLAGS, F_VALUE);
    if (flags)
      return flags;
    // sonst den Wert aus der Blueprint
    if (objectp(blueprint()))
      return blueprint()->QueryProp(P_UNIT_DECAY_FLAGS);
    else
      return load_object(load_name())->QueryProp(P_UNIT_DECAY_FLAGS);
  }
  // Die BP liefert den Wert auf jeden Fall ohne NO_DECAY_UNTIL_MOVE zurueck,
  // weil dieses Flag im Falle der BP nicht zurueckgesetzt wird und bei Clients 
  // ohne eigene Flags der Zerfall stoppen wuerde.
  return Query(P_UNIT_DECAY_FLAGS, F_VALUE) & ~NO_DECAY_UNTIL_MOVE;
}

