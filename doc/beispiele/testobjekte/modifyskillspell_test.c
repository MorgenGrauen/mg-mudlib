// ModifySkill-Beispiel fuer einem Spell. Erlaeuterung auf man-Page
// und im Objekt selbst
inherit "/std/thing";
#include <new_skills.h>
#include <properties.h>
#include <wizlevels.h>
#include <break_string.h>

static int skillset(string str);
static int skillunset(string str);

void create() {
  if(!clonep(this_object())) {
    set_next_reset(-1);
    return;
  }

  ::create();
  set_next_reset(7200);

  SetProp(P_NAME, "Skillbeispiel");
  SetProp(P_SHORT, "Eine Skillbeispiel");
  SetProp(P_LONG, break_string(
    "Mit \"skillset\" kann man sich den Skill (eigentlich Spell) "
    "\"fnrx\" setzen.\n"+
    "Durch \"skillunset\" wird der Skill geloescht.\n"
    "Das zaehlt deshalb als Spell, weil er kleingeschrieben und "
    "damit direkt vom Spieler (also dir) als Kommando via UseSpell() "
    "ausfuehrbar ist.", 78, 0, BS_LEAVE_MY_LFS));
  SetProp(P_GENDER, NEUTER);
  AddId(({"beispiel", "skillbeispiel", "spellbeispiel",
          "skill", "spell"}));
  
  AddCmd("skillset", #'skillset);
  AddCmd("skillunset", #'skillunset);
}

// Testfunktion, weil im Code dazu aufgefordert wird, das Objekt auch
// mal zu wegzulegen. Spieler sollten nicht an sich herumfummeln.
private static int _checkLearner(object pl) {
  if(!IS_LEARNER(this_player())) {
    notify_fail("Du bist kein Magier, deshalb geht das nicht.\n");
    call_out(#'remove, 1);
    return 0;
  }
  return 1;
}

static int skillset(string str) {
  if(!_checkLearner(this_player()))
    return 0;

  if(this_player()->QuerySkill("fnrx")) {
    notify_fail("Du kannst den Skill schon.\n");
    return 0;
  }

  this_player()->ModifySkill("fnrx",
    ([SI_CLOSURE: function int (object caster, string skill, mapping sinf) {
            caster->LearnSkill("fnrx", 1);
            tell_object(caster, "Peng! Dein Skillwert steigt auf "+
                                caster->QuerySkillAbility("fnrx")+".\n");
                    return 1;
                  },
      SI_SKILLABILITY: 8432]), 100, "ANY");
  tell_object(this_player(), break_string(
    "Der Skill ist gesetzt. Tipp doch ein paar Mal \"fnrx\" und schau den "
    "Code unter "+load_name(this_object())+" an, damit du siehst, dass der "
    "Skill jetzt nur aus einem Eintrag in dir und einer ueber UseSpell() "
    "gerufenen Funktion in diesem Objekt besteht. Der Skill ist kein "
    "Kommando in einem anderen Objekt.\n\n"
    "Wenn du dieses Objekt weglegst, funktioniert also alles weiter. "
    "Zerstoerst du aber das Objekt (oder dich mit \"ende\", funktioniert "
    "die Skillfunktion nicht mehr.\n"
    "Schau dir auch xeval this_player()->QuerySkill(\"fnrx\") an.", 78, 0,
    BS_LEAVE_MY_LFS));
  return 1;
}

// Here be dragons!
// Bitte benutzt die folgende Art von Manipulation der Skills nur dann,
// wenn ihr genau wisst, was ihr tut. In anderen Worten: NICHT MACHEN!
// Und wenn, dann NUR AM EIGENEN MAGIEROBJEKT, NICHT AN SPIELERN!
static int skillunset(string str) {
  if(!_checkLearner(this_player()))
    return 0;

  // per Query() das Mapping an der _query_*-Fun vorbei direkt holen
  mapping skills = this_player()->Query(P_NEWSKILLS);
  // ... und manipulieren (wirkt sich wegen Referenz direkt aus)
  if(mappingp(skills) && mappingp(skills["ANY"]) &&
     member(skills["ANY"], "fnrx")) {
    efun::m_delete(skills["ANY"], "fnrx");
    tell_object(this_player(), "Erledigt.\n");
  } else
    tell_object(this_player(), "Nichts zu erledigen.\n");
  
  return 1;
}

void reset() {
  remove();
}
