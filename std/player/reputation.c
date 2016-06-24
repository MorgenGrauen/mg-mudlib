#pragma strong_types
#pragma save_types
#pragma range_check
#pragma no_clone
#pragma pedantic

#include <wizlevels.h>
#include <reputation.h>

private mapping reputations = ([ ]);

/*
 * Argumente:
 * repid = Reputation-ID im Repmaster
 * value = Wert um den die Reputation geaendert werden soll,
 *         positiv oder negativ
 * silent = Keine Std-Meldung ausgeben
 *
 * Return:
 * REP_RET_WRONGARGS = Falsche Argumente
 * REP_RET_INACTIVE = Rep inaktiv (kann derzeit nicht geaendert werden)
 * REP_RET_INVALIDUID = Unzulaessie UID
 * REP_RET_ALREADYMAX = Rep bereits maximum / minimum
 * REP_RET_INVALIDREP = Reputation nicht vorhanden
 *
 * REP_RET_SUCCESS = Reputation wurde veraendert
 * REP_RET_SUCCESSCUT = Reputation wurde auf Min / Max veraendert
 */
public varargs int ChangeReputation(string repid, int value, 
                                          int silent) {
  string uid, changemsg; int newval; mapping rep;
  
  if(!intp(value) || !value || !stringp(repid) || !sizeof(repid)) 
    return REP_RET_WRONGARGS;
  if(!mappingp(rep = REPMASTER->GetReputationData(repid)))
    return REP_RET_INVALIDREP;
  if(!(rep["flags"] & REP_FLAG_ACTIVE))
    return REP_RET_INACTIVE;
  if(REPMASTER->CheckValidUid(repid, previous_object()) < 1)
    return REP_RET_INVALIDUID;
  if(reputations[repid] >= REP_MAXIMUM || reputations[repid] <= REP_MINIMUM) 
    return REP_RET_ALREADYMAX;

  if(reputations[repid] + value > REP_MAXIMUM) 
    newval = reputations[repid] + value - REP_MAXIMUM;
  else if(reputations[repid] - value < REP_MINIMUM)
    newval = reputations[repid] + value + REP_MINIMUM;

  if(!silent &&
     stringp(changemsg = REPMASTER->GetDefaultChangeMsg(repid, 
       newval || value)))
    tell_object(this_object(), changemsg);

  reputations[repid] += newval || value;

  return newval ? REP_RET_SUCCESSCUT : REP_RET_SUCCESS;
}

/*
 * Argumente:
 * repid = Reputation-ID im Repmaster
 *
 * Return:
 * 0 = Reputation noch nicht veraendert / enthalten 
 * !0 = Reputationswert
 */
public int GetReputation(string repid) { return reputations[repid]; }

/*
 * Return:
 * Mappingkopie aller gespeicherten Reputationswert
 */
public mapping GetReputations() { return copy(reputations); }
