// MorgenGrauen MUDlib
//
// player/exploration.c -- exploration point management
//
// $Id: exploration.c 9142 2015-02-04 22:17:29Z Zesstra $
#pragma strong_types
#pragma save_types
#pragma range_check
#pragma no_clone
#pragma pedantic

#define NEED_PROTOTYPES
#include <player/life.h>
#include <player/base.h>
#include <thing/properties.h>
#undef NEED_PROTOTYPES

#include <exploration.h>
#include <scoremaster.h>
#include <properties.h>
#include <new_skills.h>

private string given_scores;

private nosave mixed epnum;

void create() {
  Set(P_LEP, SECURED|SAVE, F_MODE_AS);

  given_scores = "";
}

string Forschung()
{
  return EPMASTER->QueryForschung();
}

static string _query_given_scores()
{
  return given_scores;
}

// Hier kommen Funktionen fuer die Levelpunkte

#define XP_FAC ([1:10,2:40,3:150,4:600,5:2250,6:9000,7:35000,8:140000,9:500000])

//#define DEBUG(x,y) printf(x,y)
#define DEBUG(x,y)

int AddScore(int contributor)
{
  mixed info;
  object po;
  int drin;
  
  if (!pointerp(info = SCOREMASTER->QueryNPCbyNumber(contributor)))
    return -1;

  if ((po = previous_object()) && (object_name(po) == SCOREMASTER))
    po = previous_object(1);

  if (!po || old_explode(object_name(po),"#")[0] != info[SCORE_KEY])
    return -2;

  if (!stringp(given_scores))
    given_scores = " ";

  if (catch(drin = test_bit(given_scores, contributor);publish))
    return -3;

  if (!drin) {
    given_scores = set_bit(given_scores, contributor);
    Set(P_LEP, Query(P_LEP) + info[SCORE_SCORE]);
    force_save();
    return info[SCORE_SCORE];
  }
  return 0;
}

int TestScore(int contributor)
{
  int ret;
  
  if (!previous_object() || (object_name(previous_object()) != SCOREMASTER))
    return 0;

  catch(ret = test_bit(given_scores, contributor);publish);

  return ret;
}

int SetScoreBit(int contributor)
{
  int drin;
  
  if (!previous_object() || (object_name(previous_object()) != SCOREMASTER))
    return -1;

  if (catch(drin = test_bit(given_scores, contributor);publish))
    return -2;

  if (drin) return -3;

  given_scores = set_bit(given_scores, contributor);
  force_save();
  return 1;
}

int ClearScoreBit(int contributor)
{
  int drin;
  
  if (!previous_object() || (object_name(previous_object()) != SCOREMASTER))
    return -1;

  if (catch(drin = test_bit(given_scores, contributor);publish))
    return -2;

  if (!drin) return -3;

  given_scores = clear_bit(given_scores, contributor);
  force_save();
  return 1;
}

