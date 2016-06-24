// MorgenGrauen MUDlib
//
// living/team.h -- team header
//
// $Id: team.h 8449 2013-03-04 21:06:38Z Zesstra $

#ifndef __LIVING_TEAM_H__
#define __LIVING_TEAM_H__

#define P_TEAM                    "team"
#define P_TEAM_NEWMEMBER          "potential_team_member"
#define P_TEAM_ATTACK_CMD         "team_attack_cmd"
#define P_TEAM_AUTOFOLLOW         "team_autofollow"
#define P_TEAM_WANTED_ROW         "team_wanted_row"
#define P_TEAM_WIMPY_ROW          "team_wimpy_row"
#define P_TEAM_LEADER             "team_leader"
#define P_TEAM_ASSOC_MEMBERS      "team_assoc_members"
#define P_TEAM_COLORS             "team_colors"

#define TEAM_OBJECT "/obj/team"
#define TEAM_MASTER "/p/daemon/teammaster"
#define EMPTY_TEAMARRAY ({({}),({}),({}),({}),({})})
#define MAX_TEAMROWS 5
#define MAX_TEAM_ROWLEN 6
#define MAX_TEAM_MEMBERS 12

#endif // __LIVING_TEAM_H__

#ifdef NEED_PROTOTYPES
#ifndef __LIVING_TEAM_H_PROTO__
#define __LIVING_TEAM_H_PROTO__

string TeamPrefix();
object IsTeamLeader();
object IsTeamMove();
object *TeamMembers();
varargs int PresentPosition(mixed pmap);
varargs mapping PresentTeamPositions(mixed pres_rows);
mixed *PresentTeamRows();
varargs mixed *PresentEnemyRows(object *here);
varargs object SelectNearEnemy(object *here, int forcefrom);
varargs object SelectFarEnemy(object *here, int min, int max, int forcefrom);
varargs void InsertEnemyTeam(mixed ens, int rek);
int AssocMember(object npc);
int DeAssocMember(object npc);
int TeamFlee();
varargs void InformRowChange(int from, int to, object caster);
 
#endif // __LIVING_TEAM_H_PROTO__
#endif // // NEED_PROTOYPES
