// MorgenGrauen MUDlib
//
// living/skill_utils -- some helper functions for manipulating skill data 
//                       needed in more than one program.
//
// $Id: skill_utils.c 6738 2008-02-19 18:46:14Z Humni $
#pragma strong_types
#pragma save_types
#pragma range_check
#pragma no_clone
#pragma pedantic

#include <new_skills.h>

protected void SkillResTransfer(mapping from_M, mapping to_M)
{
  if ( !mappingp(from_M) || !mappingp(to_M) )
    return;

  if ( member(from_M,SI_SKILLDAMAGE) )
    to_M[SI_SKILLDAMAGE] = to_int(from_M[SI_SKILLDAMAGE]);

  if ( member(from_M,SI_SKILLDAMAGE_MSG) )
    to_M[SI_SKILLDAMAGE_MSG] = to_string(from_M[SI_SKILLDAMAGE_MSG]);

  if ( member(from_M,SI_SKILLDAMAGE_MSG2) )
    to_M[SI_SKILLDAMAGE_MSG2] = to_string(from_M[SI_SKILLDAMAGE_MSG2]);

  if ( member(from_M,SI_SKILLDAMAGE_TYPE) )
    to_M[SI_SKILLDAMAGE_TYPE] = from_M[SI_SKILLDAMAGE_TYPE];

  if ( member(from_M,SI_SPELL) )
    to_M[SI_SPELL] = from_M[SI_SPELL];
}

