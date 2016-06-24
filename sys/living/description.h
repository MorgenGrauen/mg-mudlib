#ifndef __LIVING_DESCRIPTION_H__
#define __LIVING_DESCRIPTION_H__

// properties

#define P_GUILD                   "guild"
#define P_VISIBLE_GUILD           "visible_guild"

#define P_EXTRA_LOOK              "extralook"
#define P_INTERNAL_EXTRA_LOOK     "internal_extralook"

#define P_PLAYER_LIGHT            "player_light"
#define P_LIGHT_MODIFIER          "light_modifier"

#define P_LEVEL                   "level"

#define P_RACE                    "race"
#define P_REAL_RACE               "real_race"

// bitte auch in /secure/master.h aendern, falls das mal geaendert wird.
#define P_WEAPON                  "weapon"

#define P_PARA                    "para"
#define P_SIZE                    "size"

#define P_DIE_MSG                 "die_msg"

#define P_WIMPY                   "wimpy"
#define P_WIMPY_DIRECTION         "wimpy_dir"

#define P_AVATAR_URI              "p_lib_avataruri"

#endif // __LIVING_DESCRIPTION_H__


#ifdef NEED_PROTOTYPES

#ifndef __LIVING_DESCRIPTION_H_PROTO__
#define __LIVING_DESCRIPTION_H_PROTO__

varargs int         CannotSee(int silent);
public varargs int  AddExtraLook(string look, int duration, string key, 
                                 string lookende, object ob);
public int          RemoveExtraLook(string key);

#endif // __LIVING_DESCRIPTION_H_PROTO__

#endif // NEED_PROTOYPES
