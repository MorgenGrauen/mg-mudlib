#include "moebel.h"

#define SCHRANKMASTER2 "/players/swift/workroom"

#include <language.h>
#include <defines.h>
#include <combat.h>
#include <properties.h>
#include <wizlevels.h>
#include <moving.h>
#include <rooms.h>

#define BS   break_string
#define ENV(x)  environment(x)
#define ETO environment(this_object())
#define ETP environment(this_player())
#define TP   this_player()
#define TO   this_object()

#define SP SetProp
#define QP QueryProp
#define AD AddDetail
#define ARD AddReadDetail

#define SUID seteuid(getuid(this_object()))

//#define CAP(x) capitalize(x)
#define LOWER(x) lower_case(x)

#define CLONEME(x) clone_object(x)->move(ME)
#define CLONEOB(x) clone_object(x)
#define CLONE(x,y) clone_object(x)->move(y)
#define IST_ZAHL(x) (string)to_int(x)==x

#define RIGHT(x,y) (y>sizeof(x)?x:extract(x,sizeof(x)-y,sizeof(x)-1))
#define LEFT(x,y) (y>sizeof(x)?x:extract(x,0,y-1))
