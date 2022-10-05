#pragma no_shadow, no_inherit
#pragma strong_types,save_types,rtt_checks
#pragma pedantic,range_check,warn_deprecated
#pragma warn_empty_casts,warn_missing_return,warn_function_inconsistent

inherit "/std/secure_thing";

#include <properties.h>
#include <defines.h>

#define BS(x) break_string(x,78,0,BS_LEAVE_MY_LFS)
#define BSI(x, y) break_string(x, 78, y, BS_LEAVE_MY_LFS|BS_INDENT_ONCE)

protected void create()
{
  ::create();
  SetProp(P_NAME,"Twitter-Tool");
  SetProp(P_AUTOLOADOBJ,1);
  SetProp(P_NODROP,1);
  SetProp(P_NEVERDROP,1);
  SetProp(P_WEIGHT, 0);
  AddId(({"ttool","twittertool","twitter-tool"}));
  SetProp(P_SHORT,"Ein Twitter-Tool");
  SetProp(P_LONG,
    BS("Dies ist das Twitter-Tool zum Senden von Tweets aus dem MG.")+
    BS("\nEs versteht folgende Befehle:")+
    BSI("tweet <msg>:\nSendet den Text <msg> als Tweet des MG auf Twitter.","- ")+
    BS("Das Senden erfolgt asynchron, kann also einige Sekunden dauern und "
       "leider kann das Tool keinen Erfolg oder Misserfolg melden."));
  AddCmd("tweet",
    function int(string str)
    {
      notify_fail("Du solltest zumindest ein paar Buchstaben senden...\n");
      str = PL->_unparsed_args(0);
      if(!sizeof(str))
        return 0;

      // Der Twitter-Relay gibt Fehlermeldungen selbst aus, d.h. nur bei
      // Erfolg muessen wir selber etwas ausgeben.
      if ("/secure/misc/twitter"->twitter(str))
        this_interactive()->ReceiveMsg(
          BS(sprintf("Tweet \'%s\' wird gesendet.",str)),
          MT_NOTIFICATION|MSG_BS_LEAVE_LFS);
      return 1;
    });
}

protected void NotifyMove(object dest, object oldenv, int method)
{
  ::NotifyMove(dest, oldenv, method);
  // Nur die Personen erlauben, die auch im Twitter-Daemon erlaubt sind.
  if ("/secure/misc/twitter"->tool_allowed(dest) != 1)
  {
    remove(1);
  }
}

protected int PreventMove(object dest, object oldenv, int method)
{
  // Nur die Personen erlauben, die auch im Twitter-Daemon erlaubt sind.
  if ("/secure/misc/twitter"->tool_allowed(dest) != 1)
    return ME_NOT_ALLOWED;

  return ::PreventMove(dest, oldenv, method);
}

