// MorgenGrauen MUDlib
//
// spellbook.h -- Funktions-Prototypen fuer das /std/spellbook.h
//
// $Id: spellbook.h 8822 2014-05-17 09:49:49Z Zesstra $

#ifndef __SPELLBOOK_H__
#define __SPELLBOOK_H__

// no defines

#endif // __SPELLBOOK_H__

#ifdef NEED_PROTOTYPES

#ifndef __SPELLBOOK_H_PROTO__
#define __SPELLBOOK_H_PROTO__

varargs int      AddSpell(string verb, int kosten, mixed ski);
        int      CanTrySpell(object caster, mapping sinfo);
        int      SpellSuccess(object caster, mapping sinfo);
        int      TryAttackSpell(object victim, int damage, mixed dtypes,
                                mixed is_spell, object caster,
                                mapping sinfo);
varargs int      TryDefaultAttackSpell(object victim, object caster,
                                       mapping sinfo, mixed str);
varargs int      TryGlobalAttackSpell(object caster, mapping sinfo, int suc,
                                      int damage, mixed dt, mixed is_spell,
                                      int dist, int depth, int width);
varargs int      UseSpell(object caster, string spell, mapping sinfo);
        mapping  QuerySpell(string spell);
varargs mixed    FindDistantGroups(object pl, int dist, int dy, int dx);
varargs object   FindDistantEnemyVictim(string wen, object pl, string msg,
                                        int dist, int dy);
varargs object  *FindDistantGroup(object pl, int who, int dist, int dy,
                                  int dx);
varargs object   FindEnemyVictim(string wen, object pl, string msg);
varargs object   FindFarEnemyVictim(string wen, object pl, string msg,
                                    int min, int max);
        object  *FindGroup(object pl, int who);
        object  *FindGroupN(object pl, int who, int n);
        object  *FindGroupP(object pl, int who, int pr);
varargs object   FindLivingVictim(string wen, object pl, string msg);
varargs object   FindNearEnemyVictim(string wen, object pl, string msg);
static
varargs object   find_victim(string wen, object pl);
varargs object   FindVictim(string wen, object pl, string msg);
        string   SelectSpell(string spell, mapping sinfo);
        void     Erfolg(object caster, string spell, mapping sinfo);
        void     Learn(object caster, string spell, mapping sinfo);
        void     Misserfolg(object caster, string spell, mapping sinfo);
varargs void     prepare_spell(object caster, string spell, mapping sinfo);

#endif // __SPELLBOOK_H_PROTO__

#endif // // NEED_PROTOYPES
