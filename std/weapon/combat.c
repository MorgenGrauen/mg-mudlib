// MorgenGrauen MUDlib
//
// weapon/combat.c -- combat part of the weapon standard object
//
// $Id: combat.c 9425 2016-01-02 18:19:40Z Zesstra $

#include <sys_debug.h>

#pragma strict_types
#pragma save_types
#pragma no_clone
#pragma range_check

#define NEED_PROTOTYPES

#include <thing/properties.h>
#include <thing/commands.h>
#include <thing/description.h>
#include <properties.h>
#include <language.h>
#include <combat.h>
#include <attributes.h>
#include <defines.h>
#include <moving.h>
#include <new_skills.h>

// Globale Variablen
               int     ftime, flaw;
private nosave int     logged;
private nosave closure defend_cl, hit_cl;

void create() 
{
    // Variablen initialisieren
    logged=ftime=flaw=0;
    defend_cl=0;
    hit_cl=0;

    // Einige Grundwerte setzen
    SetProp(P_WEAPON_TYPE, WT_CLUB);
    SetProp(P_DAM_TYPE, ({DT_BLUDGEON}));
    SetProp(P_NR_HANDS, 2);
    SetProp(P_PARRY,PARRY_NOT);
    SetProp(P_AC,0);
    Set(P_RESTRICTIONS,([]),F_VALUE);
    Set(P_LAST_USE,time(),F_VALUE);

    // Einige Properties sollten nicht von aussen gesetzt werden koennen
    Set(P_PARRY,   PROTECTED, F_MODE_AS);
    Set(P_WIELDED, PROTECTED, F_MODE_AS);
    Set(P_LAST_USE,PROTECTED, F_MODE_AS);
    
    // Diese kann von aussen gesetzt werden (noch), aber bitte nur ueber die
    // hier definierte Setmethode.
    Set(P_DAMAGED, PROTECTED, F_MODE_AS);

    // Eine Waffe benoetigt Kommandos, mit denen man sie zuecken 
    // und wegstecken kann
    AddCmd( ({"zueck","zuecke","zieh","ziehe"}), "wield" );
    AddCmd( ({"steck","stecke"}), "unwield" );
}

// aktuelles Lebewesen, was diese Waffe zur Zeit gezueckt hat.
public object QueryUser()
{
  return QueryProp(P_WIELDED);
}

/*
 * Ausgabe einer Meldung beim Zuecken geht nur an Spieler, nicht an NPC.
 * Die Umgebung bekommt natuerlich immer eine Meldung.
 */
void doWieldMessage()
{
  string *str, s1;
  
  if(QueryProp(P_WIELD_MSG))  // Ist eine WieldMsg gesetzt?
  {
    if(closurep(QueryProp(P_WIELD_MSG)))  // Sogar als eigene Fkt.?
    {
      str = funcall(QueryProp(P_WIELD_MSG),this_player());

      if(interactive(this_player()))
      {
        ({int})this_player()->ReceiveMsg(str[0],
            MT_NOTIFICATION|MSG_BS_LEAVE_LFS, MA_WIELD, 0,
            this_player());
      }
    
      if ( objectp(environment()) && objectp(environment(environment())) )
          send_room(environment(environment()),
              str[1],
              MT_LOOK|MSG_BS_LEAVE_LFS,
              MA_WIELD, 0, ({this_player()}), environment());
      return;
    }
    else if(interactive(this_player()))
    {
      s1 = replace_personal(sprintf(QueryProp(P_WIELD_MSG)[0],"@WEN2"),
		      ({this_player(),this_object()}), 1);

      ({int})this_player()->ReceiveMsg(s1,
          MT_NOTIFICATION|MSG_BS_LEAVE_LFS, MA_WIELD, 0,
          this_player()); 
    }

    // Abwaertskompatibel: Das erste %s wird zu WEN1, das zweite zu WEN2
    s1 = replace_personal(sprintf(QueryProp(P_WIELD_MSG)[1],"@WEN1","@WEN2"),
		    ({this_player(), this_object()}), 1);
        
    if ( objectp(environment()) && objectp(environment(environment())) )
        send_room(environment(environment()),
              s1,
              MT_LOOK|MSG_BS_LEAVE_LFS,
              MA_WIELD, 0, ({this_player()}), environment());
    return;
  }
  /*
   * Keine WieldMsg gesetzt, Ausgabe der Default-Meldungen.
   */
  else if(interactive(this_player()))
  {
    ({int})this_player()->ReceiveMsg(
        "Du zueckst "+name(WEN,1)+".",
        MT_NOTIFICATION, MA_WIELD, 0, this_player());
  }
  
  //s.o. ersetzt durch tell_room()
  //say(break_string(this_player()->Name(WER)+" zueckt "
  // +name(WEN,0)+".",78),({ this_player() }));
  if ( objectp(environment()) && objectp(environment(environment())) )
      send_room(environment(environment()),    
          ({string})this_player()->Name(WER)+" zueckt "+name(WEN,0)+".",
          MT_LOOK,
          MA_WIELD, 0, ({this_player()}), environment());
}

/*
 * Ausgabe einer Meldung beim Wegstecken geht nur an Spieler, nicht an NPC.
 * Die Umgebung bekommt immer eine Meldung.
 */
void doUnwieldMessage(object wielded_by)
{
  string *str,s1;

  if(!objectp(wielded_by))  // Hoops! Gar nicht gezueckt? Abbruch!
  {
    return;
  }

  if(QueryProp(P_UNWIELD_MSG))  // Hier gibt es eine UnwieldMsg.
  {
    if(closurep(QueryProp(P_UNWIELD_MSG)))  // Sogar als Closure, wow :-)
    {
      str = funcall(QueryProp(P_UNWIELD_MSG),wielded_by);

      if(interactive(wielded_by))
      { 
        ({int})wielded_by->ReceiveMsg(str[0],
          MT_NOTIFICATION|MSG_BS_LEAVE_LFS, MA_UNWIELD, 0, wielded_by);
      }
      if ( objectp(environment()) && objectp(environment(environment())) )
          send_room(environment(environment()),
              str[1],
              MT_LOOK|MSG_BS_LEAVE_LFS,
              MA_UNWIELD, 0, ({wielded_by}), environment());
      return;
    }
  
    else if(interactive(wielded_by))
    {
      s1 = replace_personal(sprintf(QueryProp(P_UNWIELD_MSG)[0],"@WEN2"),
		      ({this_player(),this_object()}), 1); 
      ({int})wielded_by->ReceiveMsg(s1,
          MT_NOTIFICATION|MSG_BS_LEAVE_LFS, MA_UNWIELD, 0, wielded_by);
    }

    s1 = replace_personal(sprintf(QueryProp(P_UNWIELD_MSG)[1],"@WEN1","@WEN2"),
		    ({wielded_by, this_object()}), 1);
    
    if ( objectp(environment()) && objectp(environment(environment())) )
          send_room(environment(environment()),
              s1,
              MT_LOOK|MSG_BS_LEAVE_LFS,
              MA_UNWIELD, 0, ({wielded_by}), environment());
    return;
  }
  /*
   * Keine UnwieldMsg, also Default-Meldungen ausgeben.
   */
  else if(interactive(wielded_by))
  {
    ({int})wielded_by->ReceiveMsg(
        "Du steckst "+name(WEN,1)+" zurueck.",
        MT_NOTIFICATION, MA_UNWIELD, 0, wielded_by);
  }
  if ( objectp(environment()) && objectp(environment(environment())) )
      send_room(environment(environment()),
              ({string})wielded_by->Name(WER) +" steckt "+name(WEN,0)+" zurueck.",
              MT_LOOK,
              MA_UNWIELD, 0, ({wielded_by}), environment()); 
}

// Diese Funktion wird aufgerufen, wenn die Waffe wirklich gezueckt wird
protected void InformWield(object pl, int silent)
{
    return;
}
     
// Diese Funktion wird aufgerufen, wenn die Waffe wirklich weggesteckt
// wird
protected void InformUnwield(object pl, int silent) 
{
    return;
}


// wield_me soll knallen. 
varargs int wield_me(int silent) 
{
  raise_error("wield_me() ist veraltet. Bitte nutze DoWield()\n");
  return 1;
}

// Die Funktion, die das eigentliche Zuecken durchfuehrt.
varargs int DoWield(int silent) 
{   int     dex, parry;
    closure cl;
    mixed   res;
  
    // Man kann nur Waffen zuecken, die man auch im Inventory hat.
    if (environment()!=PL) 
    {
        _notify_fail( break_string(
            "Du musst "+name(WEN,1)+" schon erst nehmen!",78) );
        return 0;
    }

    // Eine gezueckte Waffe kann man natuerlich nicht nochmal zuecken
    if (QueryProp(P_WIELDED)) 
    {
        notify_fail("Das hast Du schon gemacht!\n");
        return 0;
    }

    // Waffen, die ein oder mehrere Attribut veraendern und gegen
    // das gesetzte Limit verstossen, haben keine Wirkung bezueglich der
    // Attribute.
    if ( mappingp(res=QueryProp(P_M_ATTR_MOD)) && ({int})PL->TestLimitViolation(res) )
    {
        write(break_string(
            "Irgendetwas an Deiner Ausruestung verhindert, dass Du Dich mit "+
            name(WEM,1)+" so richtig wohl fuehlst.",78));
    }

    // Ueber P_RESTRICTIONS kann man einige Restriktionen definieren, ohne
    // gleich auf eine WieldFunc zurueckgreifen zu muessen.
    // Die Auswertung erfolgt ueber den RestrictionChecker
    if ( (res=QueryProp(P_RESTRICTIONS)) && mappingp(res) &&
         (res=call_other("/std/restriction_checker","check_restrictions",
             PL,res)) && stringp(res) ) 
    {
        notify_fail(res);
        return 0;
    }

    parry=QueryProp(P_PARRY);
    dex=({int})PL->QueryAttribute(A_DEX);

    // Testen, ob der Spieler die noetige Geschicklichkeit besitzt, um
    // mit dieser (Parier)Waffe umgehen zu koennen
    if ( ((parry<PARRY_ONLY) && ((dex+8)*10)<QueryProp(P_WC)) || 
         ((parry>PARRY_NOT)  && ((dex+5)*2 )<QueryProp(P_AC)) )
    {
        notify_fail(
            "Du bist nicht geschickt genug, um mit dieser Waffe umzugehen.\n");
        return 0;
    }

    // Eine Gezueckte Waffe muss natuerlich erst mal weggesteckt werden.
    if ( (parry<PARRY_ONLY) && objectp(res=({object})PL->QueryProp(P_WEAPON)) )
    {
        if ( (({int})res->DoUnwield(silent)) && !(({object})PL->QueryProp(P_WEAPON)) )
        {
            // Wenn die alte Waffe weggesteckt werden konnte, nochmal
            // versuchen zu zuecken
            return DoWield(silent);
        }
        else 
        {
            // Sonst Meldung ausgeben
            notify_fail("Das geht nicht, solange Du noch eine andere "+
                 "Waffe gezueckt hast.\n");
            return 0;
        }
    }
    // Das gleiche gilt natuerlich fuer Parierwaffen
    if ( (parry>PARRY_NOT) && objectp(res=({object})PL->QueryProp(P_PARRY_WEAPON)) )
    {
        if ( (({int})res->DoUnwield(silent)) && !(({object})PL->QueryProp(P_PARRY_WEAPON)) )
        {
            // Wenn die alte Parierwaffe weggesteckt werden konnte, nochmal
            // versuchen zu zuecken
            return DoWield(silent);
        }
        else 
        {
            // Sonst Meldung ausgeben
            notify_fail("Das geht nicht, solange Du noch eine andere "+
                 "Parierwaffe gezueckt hast.\n");
            return 0;
        }
    }

    // Ist eine WieldFunc gesetzt, wird diese aufgerufen.
    if (objectp(res=QueryProp(P_WIELD_FUNC)) 
	&& !(({int})res->WieldFunc(ME,silent,environment())))
    {
        // Eine Meldung sollte schon von der WieldFunc ausgegeben werden.
        return 1;
    }

    // Die zulaessigen Hoechstwerte duerfen natuerlich nicht
    // ueberschritten werden.
    if ( (parry<PARRY_ONLY) && (QueryProp(P_WC)>MAX_WEAPON_CLASS) )
    {
        notify_fail(
            "Ungueltige Waffenklasse, bitte Erzmagier verstaendigen.\n");
        return 0;
    }
    if ( (parry>PARRY_NOT) && (QueryProp(P_AC)>MAX_PARRY_CLASS) )
    {
        notify_fail(
            "Ungueltige Parierklasse, bitte Erzmagier verstaendigen.\n");
        return 0;
    }

    // Testen, ob der Zuecker genug Haende frei hat.  
    if (!(({int})PL->UseHands(ME,QueryProp(P_NR_HANDS))))
    {
        notify_fail("Du hast keine Hand mehr frei.\n");
        return 0;
    }

    // Ok, jetzt ist alles klar, die (Parier)Waffe wird gezueckt
    SetProp(P_WIELDED, PL);
    SetProp(P_EQUIP_TIME,time());

    cl=symbol_function("SetProp",PL);

    if (parry<PARRY_ONLY)
    {
        // Dieses Objekt als Waffe setzen
        funcall(cl,P_WEAPON, ME );
    }
    if (parry>PARRY_NOT)
    {
        // Dieses Objekt als Parierwaffe setzen
        funcall(cl,P_PARRY_WEAPON, ME );
    }

    // Waffen koennen Attribute aendern/blockieren. Also muessen diese
    // nach dem Zuecken aktualisiert werden
    ({void})PL->register_modifier(ME);
    ({void})PL->UpdateAttributes();

    // P_TOTAL_AC/P_TOTAL_WC im Spieler aktualisieren. Da dort Attribute
    // eingehen, kann das erst hier gemacht werden.
    if (parry<PARRY_ONLY)
    {
        ({int})PL->QueryProp(P_TOTAL_WC);
    }
    if (parry>PARRY_NOT)
    {
        ({int})PL->QueryProp(P_TOTAL_AC);
    }

    // Zueck-Meldung ausgeben, wenn das silent-Flag nicht gesetzt ist
    if (!silent)
    {
        doWieldMessage();
    }

    // Alle Waffen werden im awmaster registriert, sobald sie von einem
    // Spieler gezueckt werden
    if (!logged && query_once_interactive(PL)) 
    {
        call_other("/secure/awmaster","RegisterWeapon",ME);
        logged=1;
    }

    // Inform-Funktion aufrufen
    InformWield(PL,silent);

    // Fertig mit dem Zuecken
    return 1;
}

// Die Funktion, die das eigentliche Wegstecken durchfuehrt.
varargs int DoUnwield(int silent) 
{   closure cl;
    int     parry;
    mixed   res;
    object  wielded_by;

    // Waffen, die nicht gezueckt sind, kann man natuerlich nicht
    // wegstecken
    if (!objectp(wielded_by=QueryProp(P_WIELDED)))
    {
        return 0;
    }

    // Ist eine UnwieldFunc gesetzt, wird diese aufgerufen
    if ( objectp(res=QueryProp(P_UNWIELD_FUNC)) &&
         !(({int})res->UnwieldFunc(ME,silent,wielded_by)) ) 
    {
        // Eine Meldung muss die UnwieldFunc schon selbst ausgeben.
        return 1;
    }

    // Eine verfluchte Waffe kann man natuerlich nicht wegstecken - aber
    // auch da gibts Ausnahmefaelle, wie z.B. den Tod eines Spielers
    if ((res=QueryProp(P_CURSED)) && !(silent&M_NOCHECK)) 
    {
        if (stringp(res))
        {
            // Stand in P_CURSED ein String? Dann diesen ausgeben
            tell_object(wielded_by, 
                (res[<1]=='\n' ? res : break_string(res,78)));
        }
        else
        {
            // Sonst eine Standard-Meldung ausgeben
            tell_object(wielded_by, break_string(
                "Du kannst "+name(WEN)+" nicht wegstecken.",78));
        }
        return 1;
    }

    // Ok, jetzt ist alles klar, die (Parier)Waffe wird weggesteckt
    parry=QueryProp(P_PARRY);
    cl=symbol_function("SetProp",wielded_by);

    if (parry<PARRY_ONLY)
    {
        // Eintrag als Waffe im Spieler loeschen
        funcall(cl,P_WEAPON, 0);
    }
    if (parry>PARRY_NOT)
    {
        // Eintrag als Parierwaffe im Spieler loeschen
        funcall(cl,P_PARRY_WEAPON, 0);
    }

    // Im Spieler die Zeit setzen, zu der er zuletzt eine Waffe weggesteckt
    // hat.
    funcall(cl,P_UNWIELD_TIME,time());

    // Meldung ausgeben, wenn silent-Flag nicht gesetzt ist
    if (!silent) 
    {
        doUnwieldMessage(wielded_by);
    } 

    // Die Haende, die bisher von der Waffe benutzt wurden, freigeben
    ({int})wielded_by->FreeHands(ME);
    SetProp(P_WIELDED, 0);

    // Waffen koennen Attribute aendern/blockieren. Also muessen diese
    // nach dem Wegstecken aktualisiert werden
    ({void})wielded_by->deregister_modifier(ME);
    ({void})wielded_by->UpdateAttributes();

    // P_TOTAL_AC/P_TOTAL_WC im Spieler aktualisieren. Da dort Attribute
    // eingehen, kann das erst hier gemacht werden.
    if (parry<PARRY_ONLY)
    {
        ({int})wielded_by->QueryProp(P_TOTAL_WC);
    }
    if (parry>PARRY_NOT)
    {
        ({int})wielded_by->QueryProp(P_TOTAL_AC);
    }

    // Inform-Funktion aufrufen
    InformUnwield(wielded_by, silent);

    // Fertig mit dem Wegstecken
    return 1;
}

// Die Funktion, die das "zuecken"-Kommando auswertet
varargs int wield(string str, int silent) 
{
    if ( !stringp(str) ||
         (query_verb()[0..3]=="zieh" && sscanf(str,"%s hervor",str)!=1) )
    {
        return 0;
    }

    if (!id(str)) 
    {
        _notify_fail("Du hast sowas nicht.\n");
        return 0;
    }

    return DoWield(silent);
}

// Die Funktion, die das "wegstecken"-Kommando auswertet
int unwield(string str) 
{   int    parry;
    string dummy;

    // Erstmal die Eingabe auswerten. Ist dies wirklich ein Kommando
    // zum Wegstecken?
    if ( !stringp(str) || (sscanf(str,"%s weg",     dummy)!=1  && 
                           sscanf(str,"%s ein",     dummy)!=1  &&
                           sscanf(str,"%s zurueck", dummy)!=1 ) )
    {
        return 0;
    }

    str = dummy;
    parry=QueryProp(P_PARRY);

    // Ist wirklich diese Waffe gemeint?
    if ( !stringp(str) || !id(str) ||
         ((parry<PARRY_ONLY)&&(({object})PL->QueryProp(P_WEAPON)!=ME)) ||
         ((parry>PARRY_NOT)&&(({object})PL->QueryProp(P_PARRY_WEAPON)!=ME)) )
    {
        return 0;
    }

    // Man kann nur Waffen wegstecken, die man auch bei sich hat
    if (environment() != PL) 
    {
        _notify_fail("Diese Waffe gehoert Dir nicht!\n");
        return 0;
    }

    // Und natuerlich geht das auch nur, wenn die Waffe gezueckt ist.
    if (!QueryProp(P_WIELDED)) 
    {
        _notify_fail("Diese Waffe hast Du gar nicht gezueckt ...\n");
        return 0;
    }

    return DoUnwield();
}

// Die Funktion, die den Schaden berechnet, den die Waffe verursacht
int QueryDamage(object enemy)
{   int    dam;
    mixed  hit_func;
    object wielder;

    // Nur gezueckte Waffen machen Schaden
    if (!objectp(wielder=QueryProp(P_WIELDED)))
        return 0;

    // Den Basis-Schaden berechnen. Die Staerke des Benutzers wird
    // hier beruecksichtigt.
    dam = (2*QueryProp(P_WC)+10*(({int})wielder->QueryAttribute(A_STR)))/3;

    // Wie gut man getroffen hat, wird ueber ein random() simuliert
    dam = random(1+dam);

    // Ist eine HitFunc gesetzt, dann wird diese ausgewertet. Der 
    // Rueckgabe-Wert wird zum Schaden addiert
    if (!hit_cl || !get_type_info(hit_cl,2))
    {
        if (objectp(hit_func=QueryProp(P_HIT_FUNC)))
        {
            hit_cl=symbol_function("HitFunc",hit_func);
        }
    }
    if (hit_cl && get_type_info(hit_cl,2))
    {
        dam += funcall(hit_cl,enemy);
    }

    // Zeitpunkt der letzten Benutzung ausgeben
    SetProp(P_LAST_USE,time());

    // Berechneten Schaden zurueckgeben
    return dam;
}

// Die Funktion, die bei Parierwaffen den Schutzwert berechnet.
int QueryDefend(string* dam_type, mixed spell, object enemy) 
{   int     prot;
    mixed   def_func;
    object  pl;

    prot = 0;

    // Ruestungen schuetzen nur gegen physikalischen Schaden
    if (!spell || (mappingp(spell) && spell[SP_PHYSICAL_ATTACK]))
    {
        if (sizeof(filter(dam_type,PHYSICAL_DAMAGE_TYPES)))
        {
            prot = random(1+QueryProp(P_AC));
        }
    }

    // Ist eine DefendFunc gesetzt, wird diese ausgewertet
    if (!defend_cl || !get_type_info(defend_cl,2))
    {
        if (objectp(def_func=QueryProp(P_DEFEND_FUNC))) 
        {
            defend_cl=symbol_function("DefendFunc",def_func);
        }
    }
    //Bei Netztoten keine (zurueckschlagende) DefendFunc
    if (defend_cl && get_type_info(defend_cl,2) &&
        objectp(pl=QueryProp(P_WIELDED)) && (!query_once_interactive(pl) ||
        interactive(pl)) )
    {
        // Der Rueckgabewert der DefendFunc wird zum Schutz addiert
        prot += funcall(defend_cl, dam_type, spell, enemy);
    }

    // Zeitpunkt der letzten Benutzung ausgeben
    SetProp(P_LAST_USE,time());

    // Berechneten Schutz zurueckgeben
    return prot;
}

// Die Anzahl der von einer Waffe benoetigten Haende darf natuerlich nicht
// kleiner als 1 sein.
int _set_nr_hands(int arg)
{
    if (!intp(arg) || (arg<1) )
        return Query(P_NR_HANDS, F_VALUE);
    return Set(P_NR_HANDS, arg, F_VALUE);
}

// Der Schadenstyp einer Waffe darf zwar als string angegeben werden, wird
// intern aber immer als array gespeichert
mixed _set_dam_type(mixed arg)
{
    if (pointerp(arg))
    {
        return Set(P_DAM_TYPE, arg, F_VALUE);
    }
    else if (stringp(arg))
    {
        return Set(P_DAM_TYPE, ({ arg }), F_VALUE);
    }
    return Query(P_DAM_TYPE, F_VALUE);
}

// Objekte, die die Beschaedigung einer Waffe durch direktes Setzen von
// P_DAMAGED durchfuehren, werden im awmaster geloggt
mixed _set_item_damaged(mixed arg)
{
    if (arg && !intp(arg))
    {
        return Query(P_DAMAGED, F_VALUE);
    }

    if (previous_object(1))
      call_other("/secure/awmaster","RegisterDamager",
            previous_object(1),QueryProp(P_DAMAGED),arg);
    
    return Set(P_DAMAGED,arg,F_VALUE);
}

// Wird etwas an P_HIT_FUNC geaendert, muss die zugehoerige closure
// erstmal geloescht werden.
mixed _set_hit_func(mixed arg)
{
    hit_cl=0;
    return Set(P_HIT_FUNC, arg, F_VALUE);
}

// Wird etwas an P_DEFEND_FUNC geaendert, muss die zugehoerige closure
// erstmal geloescht werden.
mixed _set_defend_func(mixed arg)
{
    defend_cl=0;
    return Set(P_DEFEND_FUNC, arg, F_VALUE);
}

// Die maximale Waffenklasse einer Waffe berechnet sich natuerlich aus
// der aktuellen Waffenklasse und der Beschaedigung. Eine Ausnahme bilden
// hier Waffen, deren effektive Waffenklasse groesser ist als diese Summe
int  _query_max_wc() 
{   int a,b;

    a=QueryProp(P_WC)+QueryProp(P_DAMAGED);
    b=QueryProp(P_EFFECTIVE_WC);
    if (b>a)
        return b;
    return a;
}

// Will man eine Waffe beschaedigen oder reparieren, so macht man das
// am besten ueber die Funktion Damage(argument). Positive Argumente
// bedeuten eine Beschaedigung, negative eine Reparatur. Der Rueckgabewert
// ist die wirklich durchgefuehrte Aenderung des Beschaedigungswertes
int Damage(int new_dam)
{   int    wc,old_dam;
    object w;

    // Uebergebenes Argument pruefen
    if (!new_dam || !intp(new_dam))
    {
        return 0;
    }

    // Bei Waffen, die nicht ausschliesslich zur Parade eingesetzt werden,
    // geht die Beschaedigung auf die Kampfkraft, sprich: P_WC
    if (QueryProp(P_PARRY)<PARRY_ONLY)
    {
        if ((wc=QueryProp(P_WC))<=MIN_WEAPON_CLASS && new_dam>0)
        {
            // Sonst wuerde Beschaedigung zur Reparatur fuehren
            return 0;
        }

        // Min-WC und Max-WC beachten
        if ((wc-new_dam) < MIN_WEAPON_CLASS)
        {
            new_dam = wc-MIN_WEAPON_CLASS;
        }
        else if ((wc-new_dam) > MAX_WEAPON_CLASS)
        {
            new_dam = wc-MAX_WEAPON_CLASS;
        }

        // Nie mehr als 100% reparieren
        if (((old_dam=QueryProp(P_DAMAGED))<-new_dam) && new_dam<0)
        {
            new_dam=-old_dam;
        }

        // Aenderungen an der Waffenklasse und der dem Beschaedigungswert
        // durchfuehren
        SetProp(P_WC,(wc-new_dam));
        // Ausnahmeweise Set, um die loggende Setmethode zu umgehen.
	// TODO: SetProp, sobald direktes Beschaedigen raus ist.
	Set(P_DAMAGED, old_dam+new_dam, F_VALUE);

        // P_TOTAL_WC im Traeger updaten, so vorhanden
        if (objectp(w=QueryProp(P_WIELDED)))
            ({int})w->QueryProp(P_TOTAL_WC);

        // Rueckgabewert: Durchgefuehrte Aenderung an P_DAMAGE
        return new_dam;
    }

    // Bei reinen Parierwaffen geht die Beschaedigung auf die
    // Schutzwirkung, sprich: P_AC

    if ((wc=QueryProp(P_AC))<=0 && new_dam>0)
    {
        // Sonst wuerde Beschaedigung zur Reparatur fuehren
        return 0;
    }
    
    // Min-AC=0 und Max-AC beachten
    if ((wc-new_dam) < MIN_PARRY_CLASS)
    {
        new_dam = wc-MIN_PARRY_CLASS;
    }
    else if ((wc-new_dam) > MAX_PARRY_CLASS)
    {
        new_dam = wc-MAX_PARRY_CLASS;
    }
 
    // Nie mehr als 100% reparieren
    if (((old_dam=QueryProp(P_DAMAGED))<-new_dam) && new_dam<0)
    {
        new_dam=-old_dam;
    }

    // Aenderungen an der Ruestungsklasse und dem Beschaedigungswert
    // durchfuehren
    SetProp(P_AC,wc-new_dam);
    // Ausnahmeweise Set, um die loggende Setmethode zu umgehen.
    // TODO: SetProp, sobald direktes Beschaedigen raus ist.
    Set(P_DAMAGED,old_dam+new_dam, F_VALUE);

    // P_TOTAL_AC im Traeger updaten, so vorhanden
    if (objectp(w=QueryProp(P_WIELDED)))
        ({int})w->QueryProp(P_TOTAL_AC);
    
    // Rueckgabewert: Durchgefuehrte Aenderung an P_DAMAGE
    return new_dam;
}

// Wird die Waffe einer Belastung ausgesetzt (z.B. wenn man damit
// zuschlaegt), dann wird TakeFlaw() aufgerufen.
varargs void TakeFlaw(object enemy)
{   int c;

    // Flaw-Wert erhoehen
    flaw++;

    // Ist der Waffe eine Qualitaet gesetzt worden, so kann es zu einer
    // allmaehlichen Beschaedigung der Waffe kommen
    if ((c=QueryProp(P_QUALITY)) && !(flaw%c))
        Damage(1);

    // Zeitpunkt des ersten Aufrufes festhalten
    if (!ftime)
        ftime=time();
}

// Die Flaw-Daten koennen natuerlich auch abgerufen werden
mixed *QueryFlaw()
{
    return({flaw,ftime,dtime(ftime)});
}

