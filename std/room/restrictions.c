// MorgenGrauen MUDlib
//
// room/restrictions.c -- weight property handling for rooms
//
// $Id: restrictions.c 9020 2015-01-10 21:49:41Z Zesstra $

inherit "std/container/restrictions";
#pragma strong_types
#pragma save_types
#pragma pedantic
#pragma range_check
#pragma no_clone

//#define NEED_PROTOTYPES

#include <thing/properties.h>
#include <properties.h>

static int _set_weight(int w);
static int _query_weight();
static int _set_total_weight(int w);
static int _query_total_weight();

void create()
{
  Set(P_WEIGHT,PROTECTED,F_MODE);
  Set(P_TOTAL_WEIGHT,PROTECTED,F_MODE);
}

int MayAddWeight(int w)
{
  return 0;
}

int MayAddObject(object ob)
{  return 1;  }

int PreventInsert(object ob)
{
  return 0;
}

static int _set_weight(int w)
{
  return 0;
}

static int _query_weight()
{
  return 0;
}

static int _set_total_weight(int w)
{
  return 0;
}

static int _query_total_weight()
{
  return 0;
}
