// MorgenGrauen MUDlib
//
// /std/hook_provider.c  - Hooksystem
//
// $Id: hook_provider.c 9453 2016-02-04 21:22:54Z Zesstra $

#pragma strong_types
#pragma save_types
#pragma no_clone
#pragma range_check

#define NEED_PROTOTYPES
#include <hook.h>
#undef NEED_PROTOTYPES

// get the object fur closure <cl> (where the code is!)
#define GET_OBJECT(cl) get_type_info(cl,2)

// Struct describing one hook consumer. <cl> will be called when the hook is
// triggered. The lower <prio>, the earlier the consumer will be called.
// <type> is a consumertype, e.g. surveyor, listener etc. (see hook.h)
struct hook_entry_s {
  closure cl;
  int prio;
  int endtime;
  int type;
};

// Struct holding the consumers of one specific hook.
// the value arrays are guarenteed to exist, but may be empty.
struct hook_s {
  struct hook_entry_s *surveyors;
  struct hook_entry_s *hmods;
  struct hook_entry_s *dmods;
  struct hook_entry_s *listeners;
};

/* hook mapping
   the list of all offered hooks in the following structure:
   ([hookid: (<hook_s>), ...
   )]
*/
private nosave mapping hookMapping=([]);

protected int CleanHookMapping(int *hookids);

// Debugging - ggf. ueberschreiben
protected int h_dbg() {return 0;}

void HookTestOffer(int id, int stat){
  if(h_dbg()) {
      offerHook(id,stat);
  }
}

void HookTestTrigger(int id, mixed data){
  if(h_dbg()) {
      HookFlow(id,data);
  }
}

// NOTE: if you have the closure, you can call the lfun, even if it is
// private. Therefore access to this data should be restricted to
// this_object().
// These two functions should be used for debugging purposes.
protected mapping HCopyHookMapping(){
  return deep_copy(hookMapping);
}

protected mapping HCopyHookConsumers(int hookid){
  if(member(hookMapping,hookid)) {
      CleanHookMapping(({hookid}));
      return deep_copy(hookMapping[hookid]);
  }
  return 0;
}

// Ggf. zum Ueberschreiben.
// Prueft, ob <consumer> den Hooktyp <type> im Hook <hook> benutzen darf.
protected int HConsumerTypeIsAllowed(int hookid, int type, object consumer)
{
  return 1;
}

// Prueft, ob <consumer> den Hooktyp <type> mit der Prioritaet <prio> im
// Hook <hook> benutzen darf.
protected int HPriorityIsAllowed(int hookid, int type, int prio,
                                 object consumer)
{
  return 1;
}

// clean internal hook data structures of stale hook consumers.
// returns the number of valid consumers left.
protected int CleanHookMapping(int *hookids)
{
  // hooks enthaelt die aufzuraeumenden Hooks. Wenn kein Array -> alle Hooks
  if (!pointerp(hookids))
    hookids=m_indices(hookMapping);

  int count;
  foreach(int hookid : hookids)
  { // alle Hooks
    struct hook_s hooks = hookMapping[hookid];
    if (!structp(hooks))
      continue;
    // ueber alle Consumertypen laufen
    foreach (string consumertype: H_CONSUMERNAMES)
    {
      // Yeah - compute struct lookup at runtime... ;-)
      struct hook_entry_s *consumers = hooks->(consumertype);
      // Hookeintraege / Consumer
      foreach(struct hook_entry_s h : &consumers)
      {
        // alle abgelaufenen Eintraege oder solche mit zerstoerten Objekten
        // nullen und die anderen/gueltigen zaehlen.
        if (!h->cl || h->endtime < time() )
            h = 0;
        else
            ++count;
      }
      // 0 noch rauswerfen.
      hooks->(consumertype) -= ({0});
    }
  }
  return count;
}

// Returns the number of valid consumers for the given hooks (or all, if
// hooks==0).
// Side effect: Cleans the internal structures and removes any stale
// consumers.
public varargs int HHasConsumers(int *hookids)
{
  return CleanHookMapping(hookids);
}

protected void offerHook(int hookid, int offerstate)
{
  H_DMSG(sprintf("offerHook hookid %d offerstate %d\n",hookid,offerstate));
  if (hookid>0)
  {
      if (offerstate) {
          if (!member(hookMapping,hookid)) {
              struct hook_s hook = (<hook_s>
                  surveyors: ({}),
                  hmods: ({}),
                  dmods: ({}),
                  listeners: ({}) );
              hookMapping[hookid] = hook;
          }
      }
      else {
          if (member(hookMapping,hookid)) {
            m_delete(hookMapping,hookid);
          }
      }
  }
  H_DMSG(sprintf("  result %O\n",hookMapping));
}

// hookConsumerInfo() liefert Array von hook_entry_s zurueck. D.h. bei Abfrage
// von Objekten alle Closures dieses Objekts und jede davon erzeugt ein
// Element hook_entry_s im Ergebnisarray. Bei Abfrage von Closures hat das
// Array immer genau 1 oder kein Element.
// WARNING: whoever has a hook_entry_s can change/delete the hook!
//          NEVER return the original to an external caller!
// NOTE: whoever has the cl from hook_entry_s can call it, even if the lfun
//       is private (and this object the only one knowing it).
protected mixed * hookConsumerInfo(int hookid, object|closure consumer)
{
  closure filter_cl;

  if (!member(hookMapping,hookid))
    return ({});

  // Closure zum Filtern bestimmen - je nachdem, was gesucht wird.
  if (closurep(consumer))
  {
    filter_cl = function int (struct hook_entry_s h)
                { return h->cl == consumer
                         && h->endtime >= time(); };
  }
  else if (objectp(consumer))
  {
    filter_cl = function int (struct hook_entry_s h)
                { return GET_OBJECT(h->cl) == consumer
                         && h->endtime >= time(); };
  }
  else
  {
    return ({});
  }

  struct hook_s hook = hookMapping[hookid];
  struct hook_entry_s *result = ({});
  foreach (string consumertype: H_CONSUMERNAMES )
  {
    result += filter(hook->(consumertype), filter_cl);
  }
  return result;
}

int HIsHookConsumer(int hookid, mixed consumer) {
  return sizeof(hookConsumerInfo(hookid,consumer)) != 0;
}

int* HListHooks() {
  return m_indices(hookMapping);
}

int HUnregisterFromHook(int hookid, mixed consumer) {

  H_DMSG(sprintf("HUnregisterFromHook hookid %d consumer %O\n",hookid,consumer));
  if (objectp(consumer))
    consumer = symbol_function("HookCallback", consumer);
  if (!closurep(consumer))
    return 0;

  struct hook_entry_s *info = hookConsumerInfo(hookid,consumer);

  // it should never happen that a closure is registered more than once, i.e.
  // the result contains more than one element.
  if (sizeof(info)) {
      struct hook_entry_s h = info[0];
      h->cl = 0;
      H_DMSG(sprintf("  result %O\n", hookMapping));
      // the now invalid h will be cleaned up later.
      return 1;
  }
  return 0;
}

// surveyors are asked for registration permittance
protected int askSurveyorsForRegistrationAllowance(
                  struct hook_entry_s *surveyors, object consumer,int hookid,
                  int hookprio,int consumertype)
{
  H_DMSG(sprintf("askSurveyorsForRegistrationAllowance surveyors %O, "
        "consumer %O, hookid %d, hookprio %d, consumertype %d\n",
        surveyors,consumer,hookid,hookprio,consumertype));

  foreach(struct hook_entry_s surveyor : surveyors) {
    if (closurep(surveyor->cl) && surveyor->endtime >= time())
    {
      // surveyor hook gueltig.
      object sob = GET_OBJECT(surveyor->cl);
      if (!sob->HookRegistrationCallback(consumer, hookid,
                        this_object(), hookprio, consumertype))
        return 0;
    }
  }
  return 1;
}

int HRegisterToHook(int hookid, mixed consumer, int hookprio,
                       int consumertype, int timeInSeconds) {
  int ret, regtime;

  if (!closurep(consumer) && !objectp(consumer))
    raise_error(sprintf("Wrong argument %.50O to HRegisterToHook(): consumer "
          "must be closure or object.\n",consumer));

  if (!member(hookMapping, hookid))
    return -1;

  if (objectp(consumer)) {
    consumer = symbol_function("HookCallback", consumer);
    if (!closurep(consumer))
      return -2;
  }

  if (timeInSeconds > 0) {
      regtime=time() + timeInSeconds;
  }
  else {
      regtime=__INT_MAX__;
  }

  H_DMSG(sprintf("HRegisterToHook hookid %d consumer %O\n hookprio %d "
        "consumertype %d\n",hookid,consumer,hookprio,consumertype));

  CleanHookMapping(({hookid})); // entfernt ungueltige/abgelaufene Eintraege

  // nur einmal pro closure registrieren!
  if (HIsHookConsumer(hookid, consumer))
    return -3;

  // Consumertyp erlaubt?
  if (H_CONSUMERCHECK(consumertype) == -1
      || !HConsumerTypeIsAllowed(hookid, consumertype,GET_OBJECT(consumer)))
    return -4;

  // Prioritaet erlaubt?
  if (H_HOOK_VALIDPRIO(hookprio) == -1
      || !HPriorityIsAllowed(hookid, consumertype, hookprio,
                             GET_OBJECT(consumer)))
    return -5;

  struct hook_s hook = hookMapping[hookid];

  // Und surveyors erlauben die Registierung?
  if (!askSurveyorsForRegistrationAllowance(hook->surveyors,
                    GET_OBJECT(consumer),hookid,
                    hookprio,consumertype))
    return -6;

  string ctypename = H_CONSUMERNAMES[consumertype];

  // get the consumer array
  struct hook_entry_s *consumers = hook->(ctypename);

  // assemble new hook consumer struct
  struct hook_entry_s newconsumer = (<hook_entry_s>
                      cl : consumer,
                      prio : hookprio,
                      endtime : regtime,
                      type : consumertype );

  // consumers enthaelt die Hookeintraege
  if (sizeof(consumers) < MAX_HOOK_COUNTS[consumertype]) {
    // max. Anzahl an Eintraegen fuer diesen Typ noch nicht
    // erreicht, direkt anhaengen.
    consumers += ({ newconsumer });
    hook->(ctypename) = consumers;
    ret=1;
  }
  else {
    // gibt es einen Eintrag mit hoeherem Priowert (niedrigere
    // Prioritaet), den man ersetzen koennte?
    // Das Array ist sortiert, mit hoechsten Priowerten am
    // Ende. Ersetzt werden soll der Eintrag mit dem hoechsten
    // Zahlenwert, falls der neue Eintrag einen niedrigeren Wert
    // hat, d.h. es muss nur er letzte Consumer im Array geprueft werden.
    // Pruefung auf Closureexistenz, falls der Surveyor Objekte
    // zerstoert (hat)...
    struct hook_entry_s oh = consumers[<1];
    if (!oh->cl || oh->prio > newconsumer->prio) {
      // Found superseedable entry - replace it, but inform the object.
      H_DMSG("Found superseedable entry\n");
      consumers[<1] = newconsumer;
      GET_OBJECT(oh->cl)->superseededHook(hookid, this_object());
      ret = 1;
      // nicht noetig, consumers wieder in sein hook_s zu haenngen wegen Array
      // -> Referenz
    }
  }

  // wenn ein Eintrag hinzugefuegt wurde, muss neu sortiert werden
  if (ret) {
    hook->(ctypename) = sort_array(consumers,
        function int (struct hook_entry_s a, struct hook_entry_s b) {
            return a->prio > b->prio; } );
  }

  H_DMSG(sprintf("  result %O\n",hookMapping));

  // -7, wenn kein Eintrag mehr frei / zuviele Hooks
  return (ret > 0 ? 1 : -7);
}

// Conveniences wrapper for simple listener hooks
int HRegisterListener(int hookid, mixed consumer)
{
  return HRegisterToHook(hookid, consumer, H_HOOK_OTHERPRIO(2), H_LISTENER, 0);
}

// Cnveniences wrapper for simple modificator hooks
int HRegisterModifier(int hookid, mixed consumer)
{
  return HRegisterToHook(hookid, consumer, H_HOOK_OTHERPRIO(2),
                         H_HOOK_MODIFICATOR, 0);
}

// surveyors are asked for cancellation permittance
protected int askSurveyorsForCancelAllowance(mixed surveyors,
  object modifiyingOb,mixed data,int hookid,int prio,object hookOb){

  foreach(struct hook_entry_s surveyor : surveyors) {
    if (closurep(surveyor->cl) && surveyor->endtime >= time())
    {
      // surveyor hook gueltig.
      object sob = GET_OBJECT(surveyor->cl);
      if (!sob->HookCancelAllowanceCallback(modifiyingOb, hookid,
                        hookOb, prio, data))
        return 0;
    }
  }
  return 1;
}

// surveyors are asked for data change permittance
protected int askSurveyorsForModificationAllowance(mixed surveyors,
  object modifiyingOb,mixed data,int hookid,int prio,object hookOb){

  foreach(struct hook_entry_s surveyor : surveyors) {
    if (closurep(surveyor->cl) && surveyor->endtime >= time())
    {
      // surveyor hook gueltig.
      object sob = GET_OBJECT(surveyor->cl);
      if (!sob->HookModificationAllowanceCallback(modifiyingOb,
                        hookid, hookOb, prio, data))
        return 0;
    }
  }
  return 1;
}

protected mixed HookFlow(int hookid, mixed hookdata){
  mixed tmp, ret;

  ret=({H_NO_MOD,hookdata});

  H_DMSG(sprintf("HookFlow hookid %d hookdata %O\n",hookid,hookdata));

  if (!member(hookMapping, hookid))
    return ret;

  struct hook_s hook = hookMapping[hookid];

  // notify surveyors
  foreach(struct hook_entry_s h : hook->surveyors) {
    if (closurep(h->cl) && h->endtime >= time())
    {
      // Hook gueltig
      tmp = funcall(h->cl, this_object(), hookid, ret[H_RETDATA]);
      if(tmp[H_RETCODE]==H_CANCELLED) {
        ret[H_RETCODE]=H_CANCELLED;
        return ret;  // und weg...
      }
      else if(tmp[H_RETCODE]==H_ALTERED){
        ret[H_RETCODE]=H_ALTERED;
        ret[H_RETDATA]=tmp[H_RETDATA];
      }
    }
    // ungueltige/abgelaufene Eintraege -> Aufraeumen, aber nicht jetzt.
    else if (find_call_out(#'CleanHookMapping) == -1) {
      call_out(#'CleanHookMapping, 0, ({hookid}));
    }
  } // surveyors fertig

  // notify hmods
  foreach(struct hook_entry_s h : hook->hmods) {
    if (closurep(h->cl) && h->endtime >= time())
    {
      // Hook gueltig
      tmp = funcall(h->cl, this_object(), hookid, ret[H_RETDATA]);
      if(tmp[H_RETCODE]==H_CANCELLED) {
        // ask allowance in surveyors
        if(h->cl &&
           askSurveyorsForCancelAllowance(hook->surveyors,
              GET_OBJECT(h->cl), hookdata, hookid,
              h->prio, this_object()))
        {
            ret[H_RETCODE] = H_CANCELLED;
            return ret; // und raus...
        }
      }
      else if(tmp[H_RETCODE]==H_ALTERED) {
        // ask allowance in surveyors
        if(h->cl &&
           askSurveyorsForModificationAllowance(hook->surveyors,
              GET_OBJECT(h->cl),hookdata, hookid,
              h->prio,this_object()))
        {
            ret[H_RETCODE] = H_ALTERED;
            ret[H_RETDATA] = tmp[H_RETDATA];
        }
      }
    }
    // ungueltige/abgelaufene Eintraege -> Aufraeumen, aber nicht jetzt.
    else if (find_call_out(#'CleanHookMapping) == -1) {
      call_out(#'CleanHookMapping, 0, ({hookid}));
    }
  } // hmods fertig

  // notify dmods
  foreach(struct hook_entry_s h : hook->dmods) {
    if (closurep(h->cl) && h->endtime >= time())
    {
      // Hook gueltig
      tmp = funcall(h->cl, this_object(), hookid, ret[H_RETDATA]);
      if(tmp[H_RETCODE]==H_ALTERED) {
        // ask allowance in surveyors
        if (h->cl &&
            askSurveyorsForModificationAllowance(hook->surveyors,
              GET_OBJECT(h->cl),hookdata, hookid,
              h->prio,this_object()))
        {
            ret[H_RETCODE] = H_ALTERED;
            ret[H_RETDATA] = tmp[H_RETDATA];
        }
      }
    }
    // ungueltige/abgelaufene Eintraege -> Aufraeumen, aber nicht jetzt.
    else if (find_call_out(#'CleanHookMapping) == -1) {
      call_out(#'CleanHookMapping, 0, ({hookid}));
    }
  } // dmods fertig

  // notify listener
  foreach(struct hook_entry_s h : hook->listeners) {
    if (closurep(h->cl) && h->endtime >= time())
    {
      // Hook gueltig
      funcall(h->cl, this_object(), hookid, ret[H_RETDATA]);
    }
    // ungueltige/abgelaufene Eintraege -> Aufraeumen, aber nicht jetzt.
    else if (find_call_out(#'CleanHookMapping) == -1) {
      call_out(#'CleanHookMapping, 0, ({hookid}));
    }
  } // listener fertig

  return ret;
}

