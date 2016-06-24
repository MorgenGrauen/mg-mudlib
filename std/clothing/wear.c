// MorgenGrauen MUDlib
//
// clothing/wear.c -- Funktionen rund ums Anziehen/Tragen von Kleidung.
//
// $Id: combat.c 6243 2007-03-15 21:10:21Z Zesstra $

#pragma strict_types
#pragma save_types
#pragma no_clone
#pragma pedantic
#pragma range_check

#define NEED_PROTOTYPES

#include <thing/properties.h>
#include <thing/commands.h>
#include <thing/description.h>
#include <living/clothing.h>
#include <clothing.h>
#include <living/combat.h>
#include <language.h>
#include <defines.h>
#include <new_skills.h>
#include <moving.h>

// Globale Variablen
nosave int  flaw, ftime;

void create()
{
  // Einige Properties sollten nicht von aussen gesetzt werden koennen
  Set(P_WORN, PROTECTED, F_MODE);
  Set(P_LAST_USE, PROTECTED, F_MODE_AS);
  Set(P_RESTRICTIONS,([]),F_VALUE);

  // Bekleidung benoetigt Kommandos, mit denen man sie an- und
  // ausziehen kann
  AddCmd( ({"zieh","ziehe"}),"ziehe" );
  AddCmd( ({"trag","trage"}),"do_wear" );
}

// aktuelles Lebewesen, was diese Kleidung (oder auch Ruestung) zur Zeit
// traegt.
public object QueryUser()
{
  return QueryProp(P_WORN);
}


// Ausgabe von Meldungen ueber write() oder _notify_fail(), je nachdem, ob der
// Spieler alles anzieht oder was bestimmtes.
protected void msg(string str, mixed fl) {
  if (!stringp(str)) {
    return;
  }
  if (fl) {
    write(str);
  }
  else {
    _notify_fail(str);
  }
}

/*
 * Ausgabe einer Meldung beim Anziehen geht nur an Spieler, nicht an NPC.
 * Die Umgebung bekommt immer eine Meldung.
 */
varargs void doWearMessage(int all) {
  string *str,s1;
  mixed wearmsg;

  if(wearmsg=QueryProp(P_WEAR_MSG)) { // Ist eine WearMsg gesetzt?
    if(closurep(wearmsg)) { // Evtl. gar als extra Fkt.?

      str = funcall(wearmsg, PL);
      if(interactive(PL)) {
         // Im Falle, dass all gesetzt ist, wird als Indent der Name des
         // angezogenen Objektes gesetzt. (trag alles)
         write(break_string(str[0],78,
                (all?(Name(WER)+": "):0), BS_LEAVE_MY_LFS));
      }
      //(Zesstra): say() ist hier bloed, weil es an das Env vom this_player()
      //ausgibt, sofern der existiert. So koennen Spieler die meldung kriegen,
      //obwohl die laengst woanders sind (z.B. Sequenzen)
      //Daher nun Ausgabe an das Env vom Env (wenn das kein Raum sein sollte,
      //was durchaus sein koennte, macht tell_room() nix). 
      if ( objectp(environment()) && objectp(environment(environment())) )
          tell_room(environment(environment()),
            break_string(str[1], 78, 0, BS_LEAVE_MY_LFS),({PL}) );

      return;
    }
    else if(interactive(PL)) {
      s1 = replace_personal(sprintf(wearmsg[0],"@WEN2"), ({PL,ME}), 1);

      write(break_string(s1,78,(all?(Name(WER)+": "):0), BS_LEAVE_MY_LFS));
    }

    s1 = replace_personal(sprintf(wearmsg[1],"@WER1","@WENU2"),
                            ({PL, ME}), 1);
 
    if ( objectp(environment()) && objectp(environment(environment())) )
        tell_room(environment(environment()),
            break_string(s1, 78, 0, BS_LEAVE_MY_LFS),({ PL }) );

    return;
  }
  /*
   * Keine WearMsg gesetzt. Ausgabe der Default-Meldungen.
   */
  else if(interactive(PL)) {
    write(break_string("Du ziehst " + name(WEN,1) + " an.",78,
     (all?(Name(WER)+": "):0)));
  }
  if ( objectp(environment()) && objectp(environment(environment())) )
      tell_room(environment(environment()),break_string(PL->Name(WER)
            + " zieht " + name(WEN,0) +" an.",78), ({PL}));
}

/*
 * Ausgabe einer Meldung beim Ausziehen geht nur an Spieler, nicht an NPC.
 * Die Umgebung bekommt natuerlich immer eine Meldung.
 */
void doUnwearMessage(object worn_by, int all)
{
  string *str,s1;
  mixed msg;

  if(!objectp(worn_by)) { // Na huch, gar nicht angezogen? Abbruch.
    return;
  }

  if(msg=QueryProp(P_UNWEAR_MSG)) { // Ist eine UnwearMsg gesetzt?
    if(closurep(msg)) { // Oho! Gar gleich als Fkt.?

      str = funcall(msg, worn_by);
      if(interactive(worn_by)) {
         tell_object(worn_by,break_string(str[0], 78,
                                 (all?(Name(WER)+": "):0),BS_LEAVE_MY_LFS));
      }

      //(Zesstra): say() ist hier bloed, weil es an das Env vom this_player()
      //ausgibt, sofern der existiert. So koennen Spieler die meldung kriegen,
      //obwohl die laengst woanders sind (z.B. Sequenzen)
      //Daher nun Ausgabe an das Env vom worn_by (wenn das kein Raum sein sollte,
      //macht tell_room() nix). 
      if ( objectp(environment(worn_by)) )
          tell_room(environment(worn_by),
              break_string(str[1],78, 0, BS_LEAVE_MY_LFS),({ worn_by }) );
      
      return;
    }
    else if(interactive(worn_by)) {
      s1 = replace_personal(sprintf(msg[0],"@WEN2"),
                      ({worn_by,ME}), 1);

      tell_object(worn_by,break_string(s1,78,
                              (all?(Name(WER)+": "):0), BS_LEAVE_MY_LFS));
    }

    s1 = replace_personal(sprintf(msg[1],"@WER1","@WENU2"),
                    ({worn_by, ME }), 1);

    if ( objectp(environment(worn_by)) )
        tell_room(environment(environment()),
            break_string(s1,78, 0, BS_LEAVE_MY_LFS),({ worn_by }) );
    return;
  }
  /*
   * Keine UnwearMsg gesetzt. Ausgabe der Default-Meldungen.
   */
  else if(interactive(worn_by)) {
    tell_object(worn_by,break_string("Du ziehst " + name(WEN,1) + " aus.",78,
     (all?(Name(WER)+": "):0)));
  }
  if ( objectp(environment(worn_by)) )
      tell_room(environment(worn_by), break_string(worn_by->Name(WER)
            + " zieht " + name(WEN,0) + " aus.",78), ({ worn_by }) );
}

// Diese Funktion wird aufgerufen, wenn die Ruestung wirklich angezogen
// wird
protected void InformWear(object pl, int silent, int all) {
    return;
}

// Diese Funktion wird aufgerufen, wenn die Ruestung wirklich ausgezogen
// wird
protected void InformUnwear(object pl, int silent, int all) {
    return;
}

// liefert Werte <0 zurueck, wenn der Spieler die Kleidung _nicht_ 
// anziehen darf.
// Hierbei steht -1 dafuer, dass der Aufrufer return 0 machen sollte,
// <= -2 sollte zur einem return !all fuehren.
protected int _check_wear_restrictions(int silent, int all) {
  mixed   type,res;
  object  *armours;

  // Man kann nur Kram anziehen, die man bei sich traegt
  if (environment()!=PL) {            
    msg(break_string("Du musst "+name(WEN,1)+" erst nehmen!",78,
            (all?(Name(WER)+": "):0)), all);
    return(-1);
  }

  // Eine getragene Ruestung kann man nicht nochmal anziehen
  if (QueryProp(P_WORN)) {
    msg(break_string("Du traegst "+name(WEN,1)+" bereits.",78,
          (all?(Name(WER)+": "):0)), all);
    return(-1);
  }

  // Diese Funktion versucht immer, TP anzuziehen (*args*). Es gibt aber viele
  // Magier, die ohne TP oder mit dem falschen TP anziehen wollen. Daher mal
  // pruefen und ggf. Fehler ausloesen.
  if (!this_player())
      raise_error("Kein this_player() existent beim Anziehen!\n");
  else if (this_player() != environment())
      raise_error("Meine Umgebung beim Anziehen ist nicht this_player()!\n");

  // Ueber P_RESTRICTIONS kann man einige Restriktionen definieren, ohne
  // gleich auf eine WearFunc zurueckgreifen zu muessen.
  // Die Auswertung erfolgt ueber den RestrictionChecker
  if ((res=QueryProp(P_RESTRICTIONS)) && mappingp(res) &&
      (res=(string)"/std/restriction_checker"->check_restrictions(PL,res))
      && stringp(res)) {  
    msg(break_string(res,78,(all?(Name(WER)+": "):0)),all);
    return(-1);
  }

  // Ist eine WearFunc gesetzt, wird diese aufgerufen.
  if (objectp(res=QueryProp(P_WEAR_FUNC)) && 
      !(res->WearFunc(ME, silent, environment()))) {
    // Eine Meldung muss von der WearFunc ausgegeben werden
    return(-2);
  }

  // scheinbar darf man das hier anziehen. ;-)
  return 0;
}

protected void _informwear(int silent, int all) {

  // Eine Meldung ausgeben, wenn das silent-Flag nicht gesetzt ist
  if (!silent) {
    doWearMessage(all);
  }

  // Inform-Funktion aufrufen
  InformWear(PL, silent, all);
}

// Die Funktion, die das eigentliche Anziehen durchfuehrt
varargs int DoWear(int silent, int all) {
  int nh;

  // Bedingungen pruefen, _check_restrictions() gibt die notwendigen Meldungen
  // an den Spieler aus.
  int res = _check_wear_restrictions(silent, all);
  if (res == -1)
    return(0);
  else if (res <= -2)
    return(!all);

  // Der Check auf freie Haende muss nach allen anderen Checks aus Kleidung
  // und Ruestung erfolgen und ist im Prinzip identisch fuer beide. Daher wird
  // der hier in dieser Funktion gemacht.
  // Soll das Objekt Haende "benutzen"? Steht da auch wirklich ein
  // Integer-Wert drin? ich mach da jetzt ein raise_error(), das soll
  // schliesslich gefixt werden. Ausserdem spart es nen Workaround beim
  // Ausziehen.
  if (!intp(nh=QueryProp(P_NR_HANDS))) {
    raise_error(sprintf("Invalid P_NR_HANDS in %O",object_name()));
  }
  // Wenn Haende benutzt werden sollen, muss natuerlich auch getestet
  // werden, ob das ueberhaupt geht
  if (nh>0) {
    if (!(PL->UseHands(ME, nh))) {
      // Schade, nicht genug Haende frei -> Meldung ausgeben
      write(break_string("Du hast keine Hand mehr frei.",78,
            (all?(Name(WER)+": "):0)));
      return(!all);
    }
  }

  // OK, die Ruestung kann angezogen werden.
  // Behinderung beim Wechsel nur fuer Spieler
  if (query_once_interactive(PL))
  // Wenn das Ganze ,,wirklich'' eine Kleidung/Ruestung ist und kein SMS
  // oder aehnliches...
  if (!QueryProp(P_WEAPON_TYPE)) {
    // Aktion noch setzen, Spieler hat ja was angezogen
    PL->SetProp(P_LAST_WEAR_ACTION,({WA_WEAR,time()}));
    // Im Kampf verliert der Spieler durch Kleidungswechsel eine Runde.
    if (PL->InFight()) {
      PL->SetProp(P_ATTACK_BUSY,1);
    }
  }
  // Eintragen in P_CLOTHING/P_ARMOURS
  PL->Wear(this_object());

  PL->SetProp(P_EQUIP_TIME,time());
  SetProp(P_WORN, PL);
  SetProp(P_EQUIP_TIME,time());

  // ggf. andere Objekte informieren etc.
  _informwear(silent, all);

  // Fertig mit dem Anziehen. Vorgang beenden bzw. mit anderen
  // Ruestungen fortfahren
  return !all;
}


// liefert 0 zureck, wenn die Kleidung/Ruestung ausgezogen werden kann
// bei M_NOCHECK ist das Ausziehen immer erlaubt, allerdings wird
// P_REMOVE_FUNC auch dann gerufen (wenn auch ignoriert).
// <0 verbietet das Ausziehen
protected int _check_unwear_restrictions(object worn_by, int silent, 
                                            int all)
{
  // Nicht getragene Ruestungen kann man auch nicht ausziehen
  if (!objectp(worn_by)) {
      return(-2);
  }

  // Ist eine RemoveFunc gesetzt, wird diese aufgerufen
  // Im Falle von M_NOCHECK wird das Ergebnis allerdings ignoriert.
  mixed res=QueryProp(P_REMOVE_FUNC);
  if (objectp(res)
      && !res->RemoveFunc(ME,silent,worn_by)
      && !(silent & M_NOCHECK)
      )
  {
      // Eine Meldung muss von der RemoveFunc ausgegeben werden
      return(-2);
  }

  // generell hebt M_NOCHECK die Restriktionen auf - sonst kommt es zu
  // massiven Inkonsistenzen beim Bewegen mit M_NOCHECK.
  if (silent & M_NOCHECK)
    return 1;

  // Eine verfluchte Ruestung kann man natuerlich nicht ausziehen
  res=QueryProp(P_CURSED);
  if (res ) {
    if (stringp(res)) {
      // Stand in P_CURSED ein String? Dann diesen ausgeben
      tell_object(worn_by,      
          (res[<1]=='\n' ? res : break_string(res,78,
            (all?(Name(WER)+": "):0))));
    }
    else {
      // Sonst eine Standard-Meldung ausgeben
      tell_object(worn_by,break_string(
          "Du kannst " + name(WEN) + " nicht ausziehen, " + QueryPronoun(WER)
          + " ist verflucht worden.\n",78,(all?(Name(WER)+": "):0)));
    }
    return(-2);
  }

  // Ausziehen moeglich
  return(1);
}

protected void _informunwear(object worn_by, int silent, int all) {

  // Inform-Funktion aufrufen
  InformUnwear(worn_by, silent, all);

  // Meldung ausgeben, wenn das silent-Flag nicht gesetzt ist
  if (!(silent&M_SILENT)) {
    doUnwearMessage( worn_by, all );
  }
}

// Die Funktion, die das eigentliche Ausziehen durchfuehrt
// hier steht nur drin, was auf jeden Fall fuer Kleidungen und Ruestungen
// gleich ist, damit man bei Ruestungen diese Funktion nicht ueberschreiben
// muss.
varargs int DoUnwear(int silent, int all) {
  object worn_by;
  int nh;

  // Das Flag "silent" wird in der RemoveFunc() etwas anders behandelt
  // als ueberall anders. Deshalb wird M_SILENT gesetzt, sofern "silent"
  // _irgendeinen_ Wert ausser M_NOCHECK hatte.
  if ( silent & ~M_NOCHECK )
      silent |= M_SILENT;

  // Standard-Notfiyfail setzen.
  if (all)
    notify_fail("Alles ausgezogen, was ging.\n");

  // Hat das Objekt Haende "benutzt"? Steht da auch wirklich ein
  // Integer-Wert drin? Wenn nicht, mach ich nen raise_error(), das duerfte
  // eigentlich gar nicht passieren. Pruefung mal am Anfang machen, bevor
  // irgendwas anderes (RemoveFunc()) passiert ist.
  if (!intp(nh=QueryProp(P_NR_HANDS))) {
    raise_error(sprintf("Invalid P_NR_HANDS in %O",object_name()));
  }

  worn_by=QueryProp(P_WORN);
  // darf ausgezogen werden? Wenn nicht, Ende.
  int res = _check_unwear_restrictions(worn_by,silent,all);
  if (res < 0)
    return(!all);

  // OK, alles klar, die Ruestung wird ausgezogen
  worn_by->Unwear(ME);

  // Benutzte Haende wieder freigeben
  if (nh>0) {
    worn_by->FreeHands(ME);
  }

  worn_by->SetProp(P_EQUIP_TIME, time());
  SetProp(P_WORN, 0);

  // Flag noch setzen, Spieler hat ja was ausgezogen
  // Aber nur wenns auch der Spieler selbst ist.
  // und wenn das wirklich eine Ruestung und kein SMS o.ae. ist.
  if (PL && PL==worn_by && !QueryProp(P_WEAPON_TYPE)) {
    //Behinderung beim Wechsel nur fuer Spieler
    if (query_once_interactive(PL)) {
      PL->SetProp(P_LAST_WEAR_ACTION,({WA_UNWEAR,time()}));
      if (PL->InFight()) { 
        PL->SetProp(P_ATTACK_BUSY,1);
      }
    }
  }

  // ok, nun noch andere Objekte informieren.
  _informunwear(worn_by,silent,all);

  // Fertig mit dem Anziehen. Vorgang beenden bzw. mit anderen
  // Ruestungen fortfahren
  return !all;
}

protected int _do_wear(string str, int silent, int all) {
  int *last;

  // Standard-Notfiy-Fail setzen.
  if (all)
    notify_fail("Alles angezogen, was ging.\n");

  // Ist diese Ruestung ueberhaupt gemeint? Bei "trage alles" ist dies
  // natuerlich immer der Fall
  if (!str || (!all && !id(str))) {
      return 0;
  }

  // Vielleicht darf der Spieler ja gar nix mehr anziehen.
  if ((object)PL->InFight()) {        
    last=(int*)PL->QueryProp(P_LAST_WEAR_ACTION);
      if (pointerp(last) && (last[0]==WA_UNWEAR) && ((time()-last[1])<2)) {
        notify_fail("Du hast doch gerade erst etwas ausgezogen!\n"
            "So schnell bist Du nicht!\n");        
        return 0;        
      }
  }

  // Auf zum eigentlichen Anziehen
  return DoWear(silent, all);

}

// Funktion, die das "trage"/"ziehe * an"-Kommando auswertet
varargs int do_wear(string str, int silent) {  
  int all;  

  // Hat der Spieler "trage alles" eingegeben?
  all=(str=="alles" || str=="alle kleidung" || str=="alle bekleidung");
  
  return(_do_wear(str,silent,all));
}

protected int _do_unwear(string str, int silent, int all) {
  int * last;

  // Ist diese Ruestung ueberhaupt gemeint? Und hat dieser Spieler sie
  // auch wirklich an?
  if (!stringp(str) || (!all && !id(str))) {
    return 0;
  }

  if (!QueryProp(P_WORN)) { 
    if (all) {  
      notify_fail("Alles ausgezogen, was ging.\n");
      return 0;
    }
    if (!Query(P_ARTICLE) || QueryProp(P_PLURAL)) {
      notify_fail( break_string(
            "Du traegst k"+name(WEN,0)+".",78) );
    }
    else {    
      notify_fail( break_string(
           "Du traegst "+name(WEN,1)+" nicht.",78) );  
    }
    return 0;
  }

  // Vielleicht darf der Spieler ja gar nichts mehr ausziehen.
  if ((object)PL->InFight()) {
    last=(int*)PL->QueryProp(P_LAST_WEAR_ACTION);
    if (pointerp(last) && (last[0]==WA_WEAR) && ((time()-last[1])<2)) {
      notify_fail("Du hast doch gerade erst etwas angezogen!\n"              
          "So schnell bist Du nicht!\n");
      return 0;
    }
  }
  // Auf zum eigentlichen Ausziehen
  return DoUnwear(silent, all);
}

// Die Funktion, die das "ziehe * aus"-Kommando auswertet
varargs int do_unwear(string str, int silent) {
  int all;

  all=(str=="alles" || str=="alle kleidung" || str=="alle bekleidung");

  return(_do_unwear(str,silent,all));
}

// Funktion, die das "ziehe"-Kommando auswertet
int ziehe(string str) {   
  string ob;

  // Uebergebenes Argument pruefen
  if (!stringp(str)) {
    return 0;  
  }

  // Ist ANziehen gemeint?
  if (sscanf(str, "%s an", ob)==1) {
      return do_wear(ob );
  }

  // Oder ist AUSziehen gemeint?
  if (sscanf(str, "%s aus", ob)==1 ) { 
      return do_unwear(ob);
  }

  // Ok, es geht wohl weder ums an- noch ums ausziehen
  return 0;
}


// Beschaedigen des Kleidungsstuecks

// Direktes Beschaedigen der Kleidung durch Setzen der Prop gibts nicht. ;-)
// Das geht aus Kompatibilitaetgruenden bei Ruestungen, aber nicht mehr bei
// Kleidung. Punkt.
static mixed _set_item_damaged(mixed arg) {
  return(QueryProp(P_DAMAGED));
}

// Will man eine Kleidung beschaedigen oder reparieren, so macht man das
// am besten ueber die Funktion Damage(argument). Positive Argumente
// bedeuten eine Beschaedigung, negative eine Reparatur. Der Rueckgabewert
// ist die wirklich durchgefuehrte Aenderung des Beschaedigungswertes
int Damage(int new_dam) {
  return 0;
}

// Wird die Kleidung einer Belastung ausgesetzt (bei Ruestungen z.B. bei einem
// Angriff eines Gegners), dann wird TakeFlaw() aufgerufen. Bei Kleidungen
// koennte man ja per P_SENSITIVE arbeiten oder ein Magier ruft bei Aktionen
// TakeFlaw() auf.
varargs void TakeFlaw(mixed dam_types,mapping einfos) {   
  int quality;

  // Ist der Ruestung eine Qualitaet gesetzt worden, so kann es zu einer
  // allmaehlichen Beschaedigung der Ruestung kommen. Im if() flaw gleich
  // hochzaehlen.
  if ((quality=QueryProp(P_QUALITY)) && !((++flaw) % quality)) {      
    Damage(1);
  }

  // Zeitpunkt des ersten Aufrufes festhalten
  if (!ftime)   
    ftime=time();
}

// Die Flaw-Daten koennen natuerlich auch abgerufen werden
mixed *QueryFlaw() {
  return ({flaw,ftime,dtime(ftime)});
}

public status IsClothing() {return 1;}

