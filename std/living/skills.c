// MorgenGrauen MUDlib
//
// living/skills.c -- Gilden-, Skill- und Spellfunktionen fuer Lebewesen
//
// $Id: skills.c 8755 2014-04-26 13:13:40Z Zesstra $
#pragma strict_types
#pragma save_types
#pragma range_check
#pragma no_clone
#pragma pedantic

inherit "/std/living/std_skills";

#define NEED_PROTOTYPES
#include <living/skills.h>
#include <living/skill_attributes.h>
#include <player/life.h>
#undef NEED_PROTOTYPES

#include <thing/properties.h>
#include <properties.h>
#include <new_skills.h>
#include <wizlevels.h>

// speichert die Spell-Fatigues (global, Spruchgruppen, Einzelsprueche)
private mapping spell_fatigues = ([]);

// Prototypen
private void expire_spell_fatigues();

protected void create()
{
  // mainly necessary for players, but there may be some NPC with savefiles.
  // Additionally, it simplifies expiration of old keys to have it here.
  call_out(#'expire_spell_fatigues, 4);
}


// Diese - hier scheinbar sinnlose - Funktion wird von /std/player/skills.c dann 
// ueberladen.
public int is_deactivated_skill(string sname, string gilde)
{
	return 0;
}



static string _query_visible_guild()
{ string res;

  if ( stringp(res=Query(P_VISIBLE_GUILD)) )
    return res;

  return QueryProp(P_GUILD);
}

static string _query_visible_subguild_title()
{ string res;

  if ( stringp(res=Query(P_VISIBLE_SUBGUILD_TITLE)) )
    return res;

  return QueryProp(P_SUBGUILD_TITLE);
}

static mixed _query_guild_prepareblock()
{ mapping res;
  string  gilde;

  if ( !stringp(gilde=QueryProp(P_GUILD)) )
    return 0;
  if ( !mappingp(res=Query(P_GUILD_PREPAREBLOCK)) 
      || !member(res,gilde) )
    return 0;
  return res[gilde];
}

static mixed _set_guild_prepareblock(mixed arg)
{ mapping res;
  string  gilde;

  if ( !stringp(gilde=QueryProp(P_GUILD)) )
    return 0;
  if ( !mappingp(res=Query(P_GUILD_PREPAREBLOCK)) )
    res=([]);

  res[gilde]=arg;
  Set(P_GUILD_PREPAREBLOCK,res);

  return arg;
}


private nosave int valid_setskills_override;
// Man sollte eigentlich ja nicht Parameter als globale Variablen
// uebergeben, aber hier ging es nicht anders
nomask private int valid_setskills(string gilde)
{ string fn;

  if ( !query_once_interactive(this_object()) )
    return 1; // Monster duerfen sich selber Skills setzen :)

  if ( QueryProp(P_TESTPLAYER) || IS_WIZARD(this_object()) )
      return 1; // Testspieler und Magier sind schutzlose Opfer ;-)

  if ( previous_object() )
  {
    if ( previous_object()==this_object()
        && this_interactive()==this_object() )
      return 1;

    fn=object_name(previous_object());
    if ( fn[0..7]=="/gilden/"
        || fn[0..11]=="/spellbooks/"
        || fn[0..7]=="/secure/"
        || fn[0..11]=="/p/zauberer/" )
      return 1; // Die sollten problemlos aendern duerfen

    if ( file_size("/gilden/access_rights")>0
       && call_other("/gilden/access_rights",
                    "access_rights",
                    getuid(previous_object()),
                    gilde+".c"))
      return 1; // Setzendes Objekt kommt vom Gildenprogrammierer

    if ( file_size("/gilden/"+gilde+".c")>0
        && call_other("/gilden/"+gilde,
                      "valid_setskills",
                        explode(fn,"#")[0]) )
      return 1; // Die Gilde selber kann Ausnahmen zulassen
  }

  if (valid_setskills_override)
  {
    valid_setskills_override=0;
    return 1; // Fuers Setzen der Closure
  }

  if ( this_interactive() )
  {
    if ( IS_ARCH(this_interactive()) )
      return 1; // Erzmagier duerfen immer aendern

    if ( call_other("/gilden/access_rights",
                    "access_rights",
                    getuid(this_interactive()),
                    gilde+".c"))
      return 1;  // Der Gildenprogrammierer selber auch
  }

  // Fuer die Waffenskills, die sollen sich selbst auch setzen duerfen
  if (!this_interactive() && this_object()==previous_object())
	  return 1;
  
  
  log_file("SETSKILLS",sprintf("*****\n%s PO:%O->TO:%O TI:%O\n GUILD:%s VERB_ARGS:'%s'\n",
            ctime(time())[4..15],
            previous_object(),
            this_object(),
            this_interactive(),
            gilde,
            ( this_interactive() ? query_verb() + " " +
                this_interactive()->_unparsed_args() : "") ));

  return 0;
}

// Nur interne Verwendung, value wird nicht weiter prueft, muss ok sein.
// Es wird keine Kopie von value gemacht, wenn es ins Mapping geschrieben
// wird!
private mapping internal_set_newskills(mapping value, string gilde) {
  mapping skills;

  // in der richtigen Gilde setzen.
  if ( !gilde && !(gilde=QueryProp(P_GUILD)) )
    gilde="ANY";

  // Query(), hier ist eine Kopie nicht sinnvoll.
  if ( !mappingp(skills=Query(P_NEWSKILLS,F_VALUE)) ) {
    skills=([]);
    Set(P_NEWSKILLS, skills, F_VALUE);
  }

  // Falls dies hier mal ausgewertet werden sollte, nicht vergessen, dass
  // einige Funktion hier im File die Prop evtl. via
  // internal_query_newskills() abrufen und direkt aendern...
  valid_setskills(gilde); // Sicherheitsueberpruefung
  
  // Skills setzen. Set() unnoetig, weil wir das von Query() gelieferte
  // Mapping aendern und das ja via Referenz bekommen haben.
  skills[gilde]=value;
  //Set(P_NEWSKILLS,skills);

  return(value);
}

// nur zur internen Verwendung, es wird keine Kopie des Skillmappings gemacht!
private mapping internal_query_newskills(string gilde) {
  mapping skills;

  // richtige Gilde abfragen.
  if ( !gilde && !(gilde=QueryProp(P_GUILD)) )
    gilde="ANY";

  skills=Query(P_NEWSKILLS);

  if (!mappingp(skills) || !mappingp(skills=skills[gilde]) )
    return ([]);

  return(skills);
}

// Eigentlich sollte man den _query-Funktionen keine Parameter geben...
static varargs mapping _query_newskills(string gilde) {

  // sonst Kopie des spellmappings liefern! Kostet zwar, aber verhindert
  // einige andere Bugs und versehentliche Aenderungen an den Skills!
  return(deep_copy(internal_query_newskills(gilde)));
}

// Eigentlich sollte man den _set-Funktionen keine weiteren Parameter geben
static varargs mapping _set_newskills(mapping value, string gilde) {

  // value auf Mappings normalisieren, ggf. Kopieren
  if ( !mappingp(value) )
      value=([]);
  else
      //zur Sicherheit, wer weiss, was der setzende noch damit macht...
      value=deep_copy(value);

  // und setzen...
  internal_set_newskills(value, gilde);

  // und noch ne Kopie von dem Liefern, was wir gesetzt haben (keine Referenz,
  // sonst koennte der Aufrufende ja noch im Nachhinein aendern).
  return(_query_newskills(gilde));
}

private mapping InternalQuerySkill(string sname, string gilde) {
  mixed skill, skills;
  // In is_any wird gespeichert, ob es ein gildenunabhaengier Skill ist,
  // fuer die is_deactivate_skill-Abfrage.
  int is_any;

  // Skills komplett abfragen, keine spez. Gilde
  if (!mappingp(skills=Query(P_NEWSKILLS,F_VALUE)))
      return 0;

  if (stringp(gilde) && sizeof(gilde)) {
    //bestimmte Gilde angegeben, gut, dort gucken.
    if (mappingp(skills[gilde]))
      skill=skills[gilde][sname]; 
  }
  else {
    gilde=QueryProp(P_GUILD); //reale Gilde holen
    if (gilde && mappingp(skills[gilde]) && 
	(skill=skills[gilde][sname])) {
      // gibt es den Spell in der Gilde des Spielers?
      // dann hier nix machen...
    }
    else if (mappingp(skills["ANY"])) {
     // Zum Schluss: Gibt es den Skill vielleicht Gildenunabhaengig?
      skill=skills["ANY"][sname];
      // wenn man hier reinkommt, dann spaeter mit is_deactivated_skill() 
      // pruefen!
      is_any=1;
    }
  }

  // wenn kein Skill gefunden, mit 0 direkt raus
  if (!skill) return 0;

  // Bei gildenunabhaengigen auch im Skillmapping vermerken
  if ( is_any ) {	
      skill+=([SI_GUILD:"ANY"]);	
      // Ist er vielleicht in der Gilde des Spielers deaktiviert? 		
      // Dies kann nur der Fall sein, wenn es kein Gildenskill ist.		
      if (is_deactivated_skill(sname,gilde)) {		    
	  return 0;		
      }
  }

  return(skill);
}

public varargs mapping QuerySkill(string sname, string gilde) {
 
    if (!stringp(sname) || !sizeof(sname))
	return 0;

    //Kopie zurueckliefern
    return(deep_copy(InternalQuerySkill(sname,gilde)));
}

#define SMUL(x,y) ((x<0 && y<0)?(-1*x*y):(x*y))
public varargs int QuerySkillAbility(string sname, string gilde)
{ mapping skill;
  string skill2;

  if ( !(skill=InternalQuerySkill(sname, gilde)) )
    return 0;

  int val=skill[SI_SKILLABILITY];

  if (skill2=skill[SI_INHERIT])
  {
    int val2;
    val2=QuerySkillAbility(skill2);
    val=(val*MAX_ABILITY+SMUL(val,val2))/(2*MAX_ABILITY);
  }

  return val;
}

protected varargs mixed LimitAbility(mapping sinfo, int diff)
{ mixed abil;
  int max,old,d2;

  abil=sinfo[SI_SKILLABILITY];

  if ( !intp(abil) )
    return sinfo;
  old=abil;

  // Beim Spieler eingetragene Schwierigkeit gilt vor angegebener.
  if ( (d2=sinfo[SI_DIFFICULTY]) )
    diff=d2;

  // diff <-100 soll nicht hemmen und macht keinen Sinn
  diff=(diff<(-100))?(-100):diff;
  
  max=MAX_ABILITY-(diff+100)*(35-QueryProp(P_LEVEL));

// diff|lvl 1:|   3:|	7:| 10:| 13:| 16:| 19:| 22:| 25:| 28:| 31:| 34:|
// ----+------+-----+-----+----+----+----+----+----+----+----+----+----+
//  -50|   83%|  84%|  86%| 87%| 89%| 90%| 92%| 93%| 95%| 96%| 98%| 99%|
//  -10|   69%|  72%|  74%| 77%| 80%| 82%| 85%| 88%| 91%| 93%| 96%| 99%|
//    0|   66%|  69%|  72%| 75%| 78%| 81%| 84%| 87%| 90%| 93%| 96%| 99%|
//   10|   62%|  65%|  69%| 72%| 75%| 79%| 82%| 85%| 89%| 92%| 95%| 98%|
//   20|   59%|  62%|  66%| 70%| 73%| 77%| 80%| 84%| 88%| 91%| 95%| 98%|
//   30|   55%|  59%|  63%| 67%| 71%| 75%| 79%| 83%| 87%| 90%| 94%| 98%|
//   40|   52%|  56%|  60%| 65%| 69%| 73%| 77%| 81%| 86%| 90%| 94%| 98%|
//   50|   49%|  53%|  58%| 62%| 67%| 71%| 76%| 80%| 85%| 89%| 94%| 98%|
//  100|   32%|  38%|  44%| 50%| 56%| 62%| 68%| 74%| 80%| 86%| 92%| 98%|
//  150|   15%|  22%|  30%| 37%| 45%| 52%| 60%| 67%| 75%| 82%| 90%| 97%|
//  200|   -2%|   7%|  16%| 25%| 34%| 43%| 52%| 61%| 70%| 79%| 88%| 97%|
//  250|  -19%|  -8%|	2%| 12%| 23%| 33%| 44%| 54%| 65%| 75%| 86%| 96%|
//  300|  -36%| -24%| -12%|  0%| 12%| 24%| 36%| 48%| 60%| 72%| 84%| 96%|
//  400|  -70%| -55%| -40%|-25%|-10%|  5%| 20%| 35%| 50%| 65%| 80%| 95%|
//  500| -104%| -86%| -68%|-50%|-32%|-14%|  4%| 22%| 40%| 58%| 76%| 94%|
//  600| -138%|-117%| -96%|-75%|-54%|-33%|-12%|  9%| 30%| 51%| 72%| 93%|

  if ( abil>max )
    abil=max;
  if ( abil>MAX_ABILITY
    ) abil=MAX_ABILITY;
  else if ( abil<-MAX_ABILITY )
    abil=-MAX_ABILITY;

  if ( old && !abil )
    abil=1;
  // Faehigkeiten sollen nicht durch die Begrenzung verschwinden

  sinfo[SI_SKILLABILITY]=abil;

  return sinfo;
}

public varargs void ModifySkill(string sname, mixed val, int diff, string gilde)
{ 
  mapping skills;
  mixed skill;

  if ( !stringp(sname) || !sizeof(sname) )
    return;

  // internal_query_newskills() macht keine Kopie
  skills=internal_query_newskills(gilde);

  // Skill ermitteln, wenn nicht existiert, wird er angelegt.
  if (!skill=skills[sname]) {
      skill=([]); 
  }
  
  // Zur Sicherheit mal das Mapping kopieren, wer weiss, was der	
  // Aufrufende dieser Funktion selber spaeter damit noch macht.
  // ist ok, wenn val kein Mapping ist, dann macht deep_copy nix.
  val=deep_copy(val);

  // Skill und val vereinigen
  if ( mappingp(val) )
    skill+=val;
  else if (intp(val))
    skill[SI_SKILLABILITY]=val;
  else
    raise_error(sprintf("Bad arg 2 to ModifySkill(): expected 'int', "
          "got %.10O.\n",val));

  // Lernen entsprechend SI_DIFFICULTY begrenzen.
  if(diff && !member(skill,SI_DIFFICULTY))
    skill[SI_DIFFICULTY]=diff;
  skill=LimitAbility(skill,diff || skill[SI_DIFFICULTY]);
  
  // schliesslich im Skillmapping vermerken. Im Normalfall ist der Skill jetzt
  // schon geaendert, nicht erst nach dem internal_set_newskills().
  skills[sname]=skill;

  // explizites Abspeichern fast ueberfluessig, weil wir oben eine Referenz
  // auf das Skillmapping gekriegt haben...
  // Aber es koennte sein, dass dies der erste Skill fuer diese Gilde ist,
  // dann ist es noetig. Zum anderen wird internal_set_newskills() nochmal
  // geloggt.
  internal_set_newskills(skills,gilde);
}

public varargs void LearnSkill(string sname, int add, int diff)
{ mapping skill;
  string skill2,gilde;
  int val;

  // Spieler sollen nur lernen, wenn sie interactive sind. Das soll
  // natuerlich nur fuer Spieler gelten.
  if (query_once_interactive(this_object()) && !interactive())
	  return;

  if ( add>MAX_SKILLEARN )
    add=MAX_SKILLEARN;
  else if ( add<1 )
    add=1;

  // Skillmapping ermitteln (hier kommt keine Kopie zurueck)
  skill=InternalQuerySkill(sname, 0);
  // wenn kein Skill, dann Abbruch
  if (!skill) return;

  val=skill[SI_SKILLABILITY];
  gilde=skill[SI_GUILD];
 
  val+=add;

  ModifySkill(sname,val,diff,gilde);
  if ( skill2=skill[SI_INHERIT] )
    LearnSkill(skill2,add/3,diff);
}

public varargs int UseSpell(string str, string spell)
{ string gilde,sbook;
  mapping sinfo;
  closure cl;
  
  if ( !spell && !(spell=query_verb()) )
    return 0;

  spell=lower_case(spell);

  // QuerySkill() liefert eine Kopie des Skillmappings.
  // wenn skill unbekannt oder Ability <= 0, ist der Spell nicht nutzbar.
  if ( !(sinfo=QuerySkill(spell,0))
        || sinfo[SI_SKILLABILITY] <= 0 )
    return 0;

  sinfo[SI_SKILLARG]=str; // Argument eintragen

  if ( !closurep(cl=sinfo[SI_CLOSURE]) )
  {
    // Wenn ein Spellbook angegeben ist wird der Spell direkt ausgefuehrt
    if ( stringp(sbook=sinfo[SI_SPELLBOOK]) )
      cl=symbol_function("UseSpell",SPELLBOOK_DIR+sbook);

    // Wenn der Spieler in einer Gilde ist, so weiss diese, in welchem
    // Spellbook der Spell zu finden ist...
    else if ( (gilde=QueryProp(P_GUILD)) && 
      ( find_object(GUILD_DIR+gilde) || file_size(GUILD_DIR+gilde+".c")>-1))
      cl=symbol_function("UseSpell",GUILD_DIR+gilde);
    else
      cl=function int () {return 0;};

    sinfo[SI_CLOSURE]=cl;
    valid_setskills_override=1;
    ModifySkill(spell,([SI_CLOSURE:cl]),0,sinfo[SI_GUILD]);
    valid_setskills_override=0;
  }
  return funcall(cl,this_object(),spell,sinfo);
}

public varargs mixed UseSkill(string skill, mapping args)
{ mapping sinfo;
  string gilde, func,skill2;
  mixed res;
  closure cl;
  
  if ( !skill ||
       QueryProp(P_GHOST))
    return 0;

  skill=capitalize(skill);
  // QuerySkill() liefert eine Kopie des Skillmappings
  if ( !(sinfo=QuerySkill(skill,0)) )
    return 0;

  if (args)
    sinfo+=args;

  if ( !closurep(cl=sinfo[SI_CLOSURE]) )
  {
    if ( !(func=sinfo[SI_SKILLFUNC])    // Keine Funktion angegeben?
        || !(gilde=QueryProp(P_GUILD))) // Keine Gilde angegeben?
    {
      // Dann Standard-Funktion nehmen, wenn es die nicht gibt, den
      // Ability-Wert zurueckliefern.
      if (!closurep(cl = symbol_function("StdSkill_"+skill,this_object())))
        cl=function int (object ob, string sname)
             {return QuerySkillAbility(sname);} ;
    }
    else
    {
      // Sonst diese Funktion im Gildenobjekt aufrufen
      cl=symbol_function(func,GUILD_DIR+gilde);
    }

    sinfo[SI_CLOSURE]=cl;
    valid_setskills_override=1;
    ModifySkill(skill,([SI_CLOSURE:cl]),0,sinfo[SI_GUILD]);
    valid_setskills_override=0;
  }

  res=funcall(cl,this_object(),skill,sinfo);
  if ( (skill2=sinfo[SI_INHERIT]) && mappingp(res) )
    res=UseSkill(skill2,res); // Fuer Skills, die von anderen abhaengen

  return res;
}

// ************** Spellfatigues ***************

/*  Prueft die Spellfatigue fuer Spruch(gruppe) <key>.
 *  <key> darf 0 sein (globale Spruchsperre).
 *  Liefert 0, wenn keine Sperre und die Ablaufzeit, wenn eine Sperre noch
 *  gueltig. ist.
 */
public varargs int CheckSpellFatigue(string key) {
  // key==0 is the (default) global spellfatigue.
  if (spell_fatigues[key] > time())
    return spell_fatigues[key]; // Ablaufzeit zurueckgeben.

  return 0; // ok, keine Sperre.
}

/** Speichert eine Spellfatigue von <duration> Sekunden fuer <key>.
 * <key> darf 0 sein und bezeichnet das globale Spellfatigue.
 * Rueckgabewert: Ablaufzeit der gesetzten Sperre
                  -1, wenn noch eine nicht-abgelaufene Sperre auf dem <key> lag.
                  0, wenn duration 0 ist.
 */
public varargs int SetSpellFatigue(int duration, string key) {
  // aktuelle Sperre abgelaufen?
  if (CheckSpellFatigue(key))
    return -1; // alte Sperre noch aktiv.

  duration += time();
  // 0 is OK for <key>, it is the key for global spell fatigues
  spell_fatigues[key] = duration;
  return duration;
}

/*  Prueft die Spellfatigue fuer Spruch(gruppe) <key>.
 *  <key> darf fuer diese Funktion 0 (globale Spruchsperre) sein, aber man
 *  darf das Argument nicht weglassen, damit nicht ein verpeilter Magier
 *  versehentlich die globale Spruchsperre nullt.
 */
public void DeleteSpellFatigue(string key) {
  // key==0 is the (default) global spellfatigue.
  m_delete(spell_fatigues, key);
}

/** Loescht abgelaufene Keys aus dem spell_fatigue mapping.
 */
private void expire_spell_fatigues() {
  foreach(string key, int endtime: spell_fatigues) {
    if (endtime <= time())
      m_delete(spell_fatigues, key);
  }
}

/** Setmethode fuer P_NEXT_SPELL_TIME.
  */
static int _set_next_spell(int fatigue) {
  return SetSpellFatigue(fatigue - time());
}
/** Querymethode fuer P_NEXT_SPELL_TIME.
  */
static int _query_next_spell() {
  return CheckSpellFatigue();
}

