// MorgenGrauen MUDlib
//
// skills.h -- Defines and prototypes for /std/living/skills.c
//
// $Id: skills.h 8694 2014-02-20 23:06:50Z Zesstra $

#ifndef _LIVING_SKILLS_H_
#define _LIVING_SKILLS_H_

#endif // _LIVING_SKILLS_H_

#ifdef NEED_PROTOTYPES
#ifndef _LIVING_SKILLS_H_PROTO_
#define _LIVING_SKILLS_H_PROTO_
public    varargs int     UseSpell(string str, string spell);

public    varargs mapping QuerySkill(string sname, string gilde);
public    varargs int     QuerySkillAbility(mixed sname, mixed argl);

protected varargs mixed   LimitAbility(mixed sinfo, int diff);
public    varargs mixed   UseSkill(string skill, mapping args);

public    varargs void    LearnSkill(string sname, int add, int diff);
public    varargs void    ModifySkill(string sname, mixed val, int diff, 
                                      string gilde);
public    varargs int     SetSpellFatigue(int duration, string key);
public    varargs int     CheckSpellFatigue(string key);
public            void    DeleteSpellFatigue(string key);

#endif // _LIVING_SKILLS_H_PROTO_

#endif // NEED_PROTOTYPES

